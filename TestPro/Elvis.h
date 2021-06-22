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
	
public:
	Elvis();
	~Elvis();
	void Initialize(IAddInDefBaseEx* cnn);
	void StartBroadcast(wstring* std_wstr_filter, IAddInDefBaseEx* cnn);
	void StopBroadcast();
};

