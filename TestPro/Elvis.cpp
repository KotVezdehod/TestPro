
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

void Elvis::Initialize(IAddInDefBaseEx *cnn, IMemoryManager *in_iMemoryManager)	//� ��������� ���������� ��������� �� ������ ��������� 1�.
{
	loc_iMemoryManager = in_iMemoryManager;

	if (!jObClass)
	{
		IAndroidComponentHelper* helper = (IAndroidComponentHelper*)cnn->GetInterface(eIAndroidComponentHelper);		
		//1� ������������� ��� ����� ���������, ������� ��������� ����� ��� �����, ��������� � ������ ��� ��� ������ � ��������� ��� ���� ���� �����.

		if (helper)
		{
			WCHAR_T* WCHART = nullptr;
			convToShortWchar(&WCHART, DREAMCATCHER_CLASS_NAME);			//��� ����������, ��������������� 1� �������� �� �������� �� ����������� ��������.
			jclass locClass = helper->FindClass(WCHART);				//�������� ��������� �� ����� � ������.

			delete[] WCHART;						//��������� convToShortWchar �������� ������ � ���� ��� ����� ������ - ������ ����� ����������, ����� ����� ������.
			WCHART = nullptr;

			if (locClass)
			{
				JNIEnv* env = getJniEnv();			//�������� ��������� �� JNI ��� ������ ������� JNI

				jClass = static_cast<jclass>(env->NewGlobalRef(locClass));			//��������� ��������� �� ����� ���� ������������� � ����������.
				env->DeleteLocalRef(locClass);										//� ������� ��������� ��������� (��� ����������� ������������ �� JNI).

				jobject activivty = helper->GetActivity();							//�������� ��������� ��������� �� �������� 1�.
				if (activivty)
				{
					jmethodID jmID = env->GetMethodID(jClass, "<init>", "(Landroid/app/Activity;J)V");	//���� � ������ ��������� �������������.
					//����� ���������� �� �������� ���������. �������� ����������� ���:
					//��� ��������� (��� ���������������� ��������� init ������ ����������� � ���������� �������), 
					//������ ���������� ���������� ������ ����� ���������� ������� - � �������, � �� �������� - ��� ������������� ��������� (V - void - ��������� �� ���������� ��������).

					jobject locJO = env->NewObject(jClass, jmID, activivty, (jlong)cnn);	
					//��� � JNI ���������������� ����� ������ �� ������. ��������� 2 ��������� - ����������. �.�. �� ���� ����� �������� ��� �����.
					//���������� - �� ������� �������-���������� ���������� ��������� "init"
					//�������� ��������� ��������� �� ����������� � ������, ������������������ � ������� � ������ ����� "DreamCatcher"

					env->DeleteLocalRef(activivty);				//����� ��������� ���������.

					if (locJO)
					{
						jObClass = static_cast<jobject>(env->NewGlobalRef(locJO));		
						env->DeleteLocalRef(locJO);										//������� ��������� ���������

						//�� ������� ���������� ���� � �������� ����� "show".
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
		JNIEnv* env = getJniEnv();				//�������� ��������� �� JNI
		jmethodID jmID =  env->GetMethodID(jClass, "start", "(Ljava/lang/String;)V");		//� ��������� ��� ������������� ������ "Dreamcatcher" ���� ����� "�����"
		jstring jstring_ = ToJniString(std_wstr_filter);									//JNI �������� �� �������� jstring - ������������ wstring � jstring.
		env->CallVoidMethod(jObClass, jmID, jstring_);										//�������� ����� �� ������� �������-����������.
		env->DeleteLocalRef(jstring_);														//����������� ������� ��������� ������.
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
static WcharWrapper s_EventSource(g_EventSource);		//��� �������, ������� ������ �� wchar_t WCHAR_T (����������� ������ �� ��������������).
static WcharWrapper s_EventName(g_EventName);		//��� �������, ������� ������ �� wchar_t WCHAR_T (����������� ������ �� ��������������).

//https://www3.ntu.edu.sg/home/ehchua/programming/java/javanativeinterface.html
// name of function built according to Java native call
//
extern "C" JNIEXPORT void JNICALL Java_ru_companyname_testpro_BroadcastReceiverJavaClass_OnBroadcastCatched(JNIEnv * env, jclass jClass, jlong pObject, jstring inJNIStr)
{
	wstring std_wstring = ToWStringJni(inJNIStr);					//jstring to wstring
	WCHAR_T* WCHART = nullptr;
	convToShortWchar(&WCHART, std_wstring.c_str());					//wstring to WCHAR_T

	IAddInDefBaseEx* pAddIn = (IAddInDefBaseEx*)pObject;			//����������� jlong � ��������� �� IAddInDefBaseEx
	pAddIn->ExternalEvent(s_EventSource, s_EventName, WCHART);		//�������� ����� ���������� IAddInDefBaseEx - ��� ���������� ������� ������� � ��������� 1�.

	delete[] WCHART;					//����������� ������, �.�. � ������� convToShortWchar ��� ���������� �� ���� ������ ������� new.

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