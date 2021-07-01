
#include "AddInNative.h"

Elvis::Elvis() : loc_iMemoryManager(nullptr)
{
	jClass = nullptr;
	jObClass = nullptr;
};

Elvis::~Elvis()
{

	JNIEnv* je = getJniEnv();
	if (jClass)
	{
		je->DeleteGlobalRef(jClass);
	}

	if (jObClass)
	{
		je->DeleteGlobalRef(jObClass);
	}

}

void Elvis::Initialize(IAddInDefBaseEx *cnn, IMemoryManager *in_iMemoryManager)	//В процедуру передается указатель на объект платформы 1с.
{
	loc_iMemoryManager = in_iMemoryManager;

	if (!jObClass)
	{
		IAndroidComponentHelper* helper = (IAndroidComponentHelper*)cnn->GetInterface(eIAndroidComponentHelper);		
		//1с предоставляет нам некий интерфейс, который позволяет найти наш пакет, загрузить в память все его классы и запустить под него свой поток.

		if (helper)
		{
			WCHAR_T* WCHART = nullptr;
			convToShortWchar(&WCHART, DREAMCATCHER_CLASS_NAME);			//Все интерфейсы, предоставляемые 1с работают со строками из двухбайтных символов.
			jclass locClass = helper->FindClass(WCHART);				//Получаем указатель на класс в пакете.

			delete[] WCHART;						//Процедура convToShortWchar выделяет память в хипе для новой строки - память нужно освободить, иначе будет утечка.
			WCHART = nullptr;

			if (locClass)
			{
				JNIEnv* env = getJniEnv();			//Получаем указатель на JNI для вызова функций JNI

				jClass = static_cast<jclass>(env->NewGlobalRef(locClass));			//Локальный указатель на класс надо преобразовать в глобальный.
				env->DeleteLocalRef(locClass);										//И удалить локальный указатель (так рекомендует документация по JNI).

				jobject activivty = helper->GetActivity();							//Получаем локальный указатель на активити 1с.
				if (activivty)
				{
					jmethodID jmID = env->GetMethodID(jClass, "<init>", "(Landroid/app/Activity;J)V");	//Ищем в классе процедуру инициализации.
					//Поиск происходит по описанию прототипа. Прототип описывается так:
					//Имя процедуры (имя предопределенной процедуры init всегда указывается в квадратных скобках), 
					//третим аргументом передается список ТИПОВ параметров функции - в скобках, а за скобками - тип возвращаемого параметра (V - void - процедура не возвращает значение).

					jobject locJO = env->NewObject(jClass, jmID, activivty, (jlong)cnn);	
					//Так в JNI инициализируется новый объект из класса. Последние 2 параметра - прикладные. Т.е. мы туда можем передать что хотим.
					//Фактически - на стороне андроид-приложения вызывается процедура "init"
					//Получаем локальный указатель на загруженный в память, инициализированный и готовый к работе класс "DreamCatcher"

					env->DeleteLocalRef(activivty);				//Гасим локальный указатель.

					if (locJO)
					{
						jObClass = static_cast<jobject>(env->NewGlobalRef(locJO));		
						env->DeleteLocalRef(locJO);										//Удаляем локальный указатель

						//На стороне приложения ищем и вызываем метод "show".
						jmID = env->GetMethodID(jClass, "show", "()V");
						env->CallVoidMethod(jObClass, jmID);
					}
				}
			}
		}
	}
}

void Elvis::StartBroadcast(wstring *std_wstr_filter, IAddInDefBaseEx* cnn)
{
	if (jObClass)
	{
		JNIEnv* env = getJniEnv();				//получаем указатель на JNI
		jmethodID jmID =  env->GetMethodID(jClass, "start", "(Ljava/lang/String;)V");		//В созданном при инициализации классе "Dreamcatcher" ищем метод "старт"
		jstring jstring_ = ToJniString(std_wstr_filter);									//JNI работает со строками jstring - конвертируем wstring в jstring.
		env->CallVoidMethod(jObClass, jmID, jstring_);										//Вызываем метод на стороне Андроид-приложения.
		env->DeleteLocalRef(jstring_);														//Обязательно удаляем локальную ссылку.
	}
}

void Elvis::StopBroadcast()
{
	if (jObClass)
	{
		JNIEnv* env = getJniEnv();
		jmethodID jmID = env->GetMethodID(jClass, "stop", "()V");
		env->CallVoidMethod(jObClass, jmID);
	}
}


#pragma region http
void Elvis::StartHTTP(tVariant* pvarRetValue, int PortNumber)
{
	JNIEnv* env = getJniEnv();
	jmethodID methID = env->GetMethodID(jClass, "StartHTTP", "(I)Ljava/lang/String;");
	jstring stringObject = static_cast<jstring>(env->CallObjectMethod(jClass, methID, PortNumber));
	wstring std_wstr = ToWStringJni(stringObject);
	env->DeleteLocalRef(stringObject);
	ToV8String(std_wstr.c_str(), pvarRetValue, loc_iMemoryManager);

	return;
}

void Elvis::StopHTTP(tVariant* pvarRetValue)
{
	JNIEnv* env = getJniEnv();
	jmethodID methID = env->GetMethodID(jClass, "StopHTTP", "()Ljava/lang/String;");
	jstring stringObject = static_cast<jstring>(env->CallObjectMethod(jObClass, methID));
	wstring std_wstr = ToWStringJni(stringObject);
	env->DeleteLocalRef(stringObject);
	ToV8String(std_wstr.c_str(), pvarRetValue, loc_iMemoryManager);

	return;
}

void Elvis::StopHTTP()
{
	JNIEnv* env = getJniEnv();
	jmethodID methID = env->GetMethodID(jClass, "StopHTTP", "()Ljava/lang/String;");
	env->CallObjectMethod(jObClass, methID);
	return;
}

#pragma endregion

static const wchar_t g_EventSource[] = L"TestPro";
static const wchar_t g_EventName[] = L"BroadcastCatched";
static WcharWrapper s_EventSource(g_EventSource);		//Это обертка, которая делает из wchar_t WCHAR_T (двухбайтную строку из четырехбайтной).
static WcharWrapper s_EventName(g_EventName);		//Это обертка, которая делает из wchar_t WCHAR_T (двухбайтную строку из четырехбайтной).

//https://www3.ntu.edu.sg/home/ehchua/programming/java/javanativeinterface.html
// name of function built according to Java native call
//
extern "C" JNIEXPORT void JNICALL Java_ru_companyname_testpro_BroadcastReceiverJavaClass_OnBroadcastCatched(JNIEnv * env, jclass jClass, jlong pObject, jstring inJNIStr)
{
	wstring std_wstring = ToWStringJni(inJNIStr);					//jstring to wstring
	WCHAR_T* WCHART = nullptr;
	convToShortWchar(&WCHART, std_wstring.c_str());					//wstring to WCHAR_T

	IAddInDefBaseEx* pAddIn = (IAddInDefBaseEx*)pObject;			//преобразуем jlong в указатель на IAddInDefBaseEx
	pAddIn->ExternalEvent(s_EventSource, s_EventName, WCHART);		//Вызываем метод интерфейса IAddInDefBaseEx - это инициирует внешнее событие в платформе 1с.

	delete[] WCHART;					//Освобождаем память, т.к. в функции convToShortWchar она выделяется из хипа потока методом new.

}

static const wchar_t g_EventName_http[] = L"http_request";
static WcharWrapper s_EventName_http(g_EventName_http);

extern "C" JNIEXPORT void JNICALL Java_ru_companyname_testpro_catcher_OnHttpServerServ(JNIEnv * env, jclass jClass, jlong pObject, jstring inReq)
{
	wstring std_wstring = ToWStringJni(inReq);
	WCHAR_T* WCHART = nullptr;
	convToShortWchar(&WCHART, std_wstring.c_str());

	IAddInDefBaseEx* pAddIn = (IAddInDefBaseEx*)pObject;
	pAddIn->ExternalEvent(s_EventSource, s_EventName_http, WCHART);

	delete[] WCHART;

}