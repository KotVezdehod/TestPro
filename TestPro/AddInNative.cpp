
#include "AddInNative.h"


static const wchar_t g_ComponentNameAddIn[] = L"TestPro";
static WcharWrapper s_ComponentClass(g_ComponentNameAddIn);

// This component supports 1.0 version
const long g_VersionAddIn = 1000;

static AppCapabilities g_capabilities = eAppCapabilitiesInvalid;


//---------------------------------------------------------------------------//
long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface)
{
    if(!*pInterface)
    {
        *pInterface = new AddInNative();
        return (long)*pInterface;
    }
    return 0;
}
    
//---------------------------------------------------------------------------//
AppCapabilities SetPlatformCapabilities(const AppCapabilities capabilities)
{
    g_capabilities = capabilities;
    return eAppCapabilitiesLast;
}
    
//---------------------------------------------------------------------------//
long DestroyObject(IComponentBase** pInterface)
{
    if(!*pInterface)
        return -1;

    delete *pInterface;
    *pInterface = 0;
    return 0;
}
    
//---------------------------------------------------------------------------//
const WCHAR_T* GetClassNames()
{
    return s_ComponentClass;
}
    
//---------------------------------------------------------------------------//
// AddInNative

AddInNative::AddInNative(): m_iMemory(nullptr), m_iConnect(nullptr)
{

}

//---------------------------------------------------------------------------//
AddInNative::~AddInNative()
{
}

/////////////////////////////////////////////////////////////////////////////
// IInitDoneBase
//---------------------------------------------------------------------------//
bool AddInNative::Init(void* pConnection)
{
    m_iConnect = (IAddInDefBaseEx*)pConnection;
    return m_iConnect != nullptr;
}

//---------------------------------------------------------------------------//
bool AddInNative::setMemManager(void* mem)
{
    m_iMemory = (IMemoryManager*)mem;
    return m_iMemory != nullptr;
}

//---------------------------------------------------------------------------//
long AddInNative::GetInfo()
{ 
    return g_VersionAddIn;
}
//---------------------------------------------------------------------------//
void AddInNative::Done()
{
    m_iMemory = nullptr;
    m_iConnect = nullptr;
}

/////////////////////////////////////////////////////////////////////////////
// ILanguageExtenderBase
//---------------------------------------------------------------------------//
bool AddInNative::RegisterExtensionAs(WCHAR_T** wsExtensionName)
{ 
    const wchar_t* wsExtension = g_ComponentNameAddIn;
    uint32_t iActualSize = static_cast<uint32_t>(::wcslen(wsExtension) + 1);

    if (m_iMemory)
    {
        if (m_iMemory->AllocMemory((void**)wsExtensionName, iActualSize * sizeof(WCHAR_T)))
        {
            convToShortWchar(wsExtensionName, wsExtension, iActualSize);
            return true;
        }
    }
    return false; 
}
//---------------------------------------------------------------------------//
long AddInNative::GetNProps()
{ 
    return eLastProp;
}
//---------------------------------------------------------------------------//
long AddInNative::FindProp(const WCHAR_T* wsPropName)
{ 
    return -1;
}
//---------------------------------------------------------------------------//
const WCHAR_T* AddInNative::GetPropName(long lPropNum, long lPropAlias)
{ 
    return 0;
}
//---------------------------------------------------------------------------//
bool AddInNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{ 
    return false;
}
//---------------------------------------------------------------------------//
bool AddInNative::SetPropVal(const long lPropNum, tVariant* varPropVal)
{ 
    return false;
}
//---------------------------------------------------------------------------//
bool AddInNative::IsPropReadable(const long lPropNum)
{ 
    return false;
}
//---------------------------------------------------------------------------//
bool AddInNative::IsPropWritable(const long lPropNum)
{
    return false;
}
//---------------------------------------------------------------------------//
long AddInNative::GetNMethods()
{ 
    return eMethLast;
}
//---------------------------------------------------------------------------//
long AddInNative::FindMethod(const WCHAR_T* wsMethodName)
{ 
    long plMethodNum = -1;
    wchar_t* name = 0;
    convFromShortWchar(&name, wsMethodName);

    plMethodNum = findName(g_MethodNames, name, eMethLast);

    if (plMethodNum == -1)
        plMethodNum = findName(g_MethodNamesRu, name, eMethLast);

    delete[] name;

    return plMethodNum;
}

//---------------------------------------------------------------------------//
long AddInNative::findName(const wchar_t* names[], const wchar_t* name, const uint32_t size) const
{
    long ret = -1;
    for (uint32_t i = 0; i < size; i++)
    {
        if (!wcscmp(names[i], name))
        {
            ret = i;
            break;
        }
    }
    return ret;
}

//---------------------------------------------------------------------------//
const WCHAR_T* AddInNative::GetMethodName(const long lMethodNum, 
                            const long lMethodAlias)
{ 
    if (lMethodNum >= eMethLast)
        return NULL;

    wchar_t* wsCurrentName = NULL;
    WCHAR_T* wsMethodName = NULL;

    switch (lMethodAlias)
    {
    case 0: // First language (english)
        wsCurrentName = (wchar_t*)g_MethodNames[lMethodNum];
        break;
    case 1: // Second language (local)
        wsCurrentName = (wchar_t*)g_MethodNamesRu[lMethodNum];
        break;
    default:
        return 0;
    }

    uint32_t iActualSize = static_cast<uint32_t>(wcslen(wsCurrentName) + 1);

    if (m_iMemory && wsCurrentName)
    {
        if (m_iMemory->AllocMemory((void**)&wsMethodName, iActualSize * sizeof(WCHAR_T)))
            convToShortWchar(&wsMethodName, wsCurrentName, iActualSize);
    }

    return wsMethodName;
}
//---------------------------------------------------------------------------//
long AddInNative::GetNParams(const long lMethodNum)
{ 
    switch (lMethodNum)
    {
    case eMethStartBroadcastsInterception:
        return 1;

    case eMethStopBroadcastsInterception:
        return 0;

    default:
        break;
    }

    return 0;
}
//---------------------------------------------------------------------------//
bool AddInNative::GetParamDefValue(const long lMethodNum, const long lParamNum,
                          tVariant *pvarParamDefValue)
{ 

    return false;
} 
//---------------------------------------------------------------------------//
bool AddInNative::HasRetVal(const long lMethodNum)
{ 
    switch (lMethodNum)
    {
    case eMethStartBroadcastsInterception:
        return true;

    case eMethStopBroadcastsInterception:
        return true;

    default:
        break;
    }
    return false;
}
//---------------------------------------------------------------------------//
bool AddInNative::CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray)
{
    return false;
}
//---------------------------------------------------------------------------//
bool AddInNative::CallAsFunc(const long lMethodNum,
                tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{ 

	switch (lMethodNum)
	{
	case eMethStartBroadcastsInterception:              //Если вызвана функция "ЗапуститьПерехватБроадкастов"
	{
        if (!lSizeArray || !paParams)           //Если массив параметров нулевого размера или указатель пустой, то вызываем ошибку (значит параметров не хватает). 
                                                //В этом случае на стороне 1с вызывается исключение.
            return false;

        if (!isBKStarted)                   //Если броадкаст не запущен, то запускаем.  (в заголовочном файле объявили приватную переменную)
        {
            wstring std_wstr(paParams->pwstrVal, paParams->pwstrVal + paParams->strLen);        
            //инициализируем новую строку wstring. Используется такой конструктор: указатель на память с началом строки wchar_t, указатель на память, где конец строки wchar_t.

            //В заголовочном файле объявили приватную переменную (m_Elvis). 
            //При инициализации класса AddInNative сразу инициализируется и переменная "m_Elvis" - вызывается конструктор класса "Elvis".
            //После этого класс в памяти гарантировано и мы можем дергать его за методы.
            m_Elvis.Initialize(m_iConnect);
            m_Elvis.StartBroadcast(&std_wstr, m_iConnect);
            
            //Эта процедура идет в примере к внешней компоненте. Она сразу преобразует wchar_t в tVariant и устанавливает ему 1сный тип "строка". Сладко...
            ToV8String(L"ok_en", pvarRetValue, m_iMemory);              
            
            isBKStarted = true;
        }
        else                //Если броадкаст уже запущен, то возвращаемся без запуска, а в 1с приходит результат функции - строка, в которой содержится описание ошибки.
        {
            ToV8String(L"Перехватчик уже запущен.", pvarRetValue, m_iMemory);
            return true;
        }

	}
	break;

	case eMethStopBroadcastsInterception:              //Если вызвана функция "ОстановитьПерехватБроадкастов"
	{
        if (isBKStarted)
        {
            m_Elvis.StopBroadcast();
            ToV8String(L"ok_en", pvarRetValue, m_iMemory);
            isBKStarted = false;
            return true;
        }
        else
        {
            ToV8String(L"Перехватчик не запущен.", pvarRetValue, m_iMemory);
            return true;
        }

	}
	break;

	default:
		break;
	}

	return true;
}
//---------------------------------------------------------------------------//
void AddInNative::SetLocale(const WCHAR_T* loc)
{
}

//---------------------------------------------------------------------------//
jstring ToJniString(wstring* in_std_wstring)
{
    JNIEnv* env = getJniEnv();
    WCHAR_T* WCHART = nullptr;
    convToShortWchar(&WCHART, in_std_wstring->c_str());
    int len = getLenShortWcharStr(WCHART);

    jstring retJString = env->NewString(WCHART, len);
    delete[] WCHART;

    return retJString;
}

wstring ToWStringJni(jstring jstr)
{
    wstring ret;
    if (jstr)
    {
        JNIEnv* env = getJniEnv();
        const jchar* jChars = env->GetStringChars(jstr, NULL);
        jsize jLen = env->GetStringLength(jstr);
        ret.assign(jChars, jChars + jLen);
        env->ReleaseStringChars(jstr, jChars);
    }
    return ret;
}

void ToV8String(const wchar_t* wstr, tVariant* par, IMemoryManager* m_iMemory)
{
    if (wstr)
    {
        ULONG len = wcslen(wstr);
        m_iMemory->AllocMemory((void**)&par->pwstrVal, (len + 1) * sizeof(WCHAR_T));
        convToShortWchar(&par->pwstrVal, wstr);
        par->vt = VTYPE_PWSTR;
        par->wstrLen = len;
    }
    else
        par->vt = VTYPE_EMPTY;
}


