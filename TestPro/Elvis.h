#pragma once
#include "pch.h"
#include <iostream>
#include "../jni/jnienv.h"
#include "../include/AddInDefBase.h"
#include "../include/IAndroidComponentHelper.h"
#include "ConversionWchar.h"


using namespace std;

class Elvis
{
private:
	jobject jObClass;
	jclass jClass;
	const wchar_t *DREAMCATCHER_CLASS_NAME = L"ru.companyname.testpro.BroadcastReceiverJavaClass";

	IMemoryManager * loc_iMemoryManager;
	
public:
	Elvis();
	~Elvis();
	void Initialize(IAddInDefBaseEx* cnn, IMemoryManager* in_iMemory);
	void StartBroadcast(wstring* std_wstr_filter, IAddInDefBaseEx* cnn);
	void StopBroadcast();
	void StartHTTP(tVariant* pvarRetValue, int PortNumber);
	void StopHTTP(tVariant* pvarRetValue);
	void StopHTTP();
	void FSScan(tVariant* pvarRetValue);
};

