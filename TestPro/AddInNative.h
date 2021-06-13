﻿
#ifndef __ADDINNATIVE_H__
#define __ADDINNATIVE_H__

#include "../include/ComponentBase.h"
#include "../include/AddInDefBase.h"
#include "../include/IMemoryManager.h"
#include "../include/mobile.h"

static const wchar_t* g_PropNames[] =
{
    
};

static const wchar_t* g_PropNamesRu[] =
{
    L"ОписаниеУстройства",
    L"ТекущиеБроадкастФильтры_ч",
    L"ПследниеДанныеИзПерехватчикаБроадкаста_ч"
};

static const wchar_t* g_MethodNames[] =
{
    L"StartBroadcastsInterception",
    L"StopBroadcastsInterception"
};

static const wchar_t* g_MethodNamesRu[] =
{
    L"ЗапуститьПерехватБроадкастов",
    L"ОстановитьПерехватБроадкастов"
};

///////////////////////////////////////////////////////////////////////////////
// class AddInNative
class AddInNative : public IComponentBase
{
public:
    enum Props
    {
        eLastProp      // Always last
    };

    enum Methods
    {
        eMethStartBroadcastsInterception,
        eMethStopBroadcastsInterception,
        eMethLast       // Always last
    };

    AddInNative(void);
    virtual ~AddInNative();
    // IInitDoneBase
    virtual bool ADDIN_API Init(void*);
    virtual bool ADDIN_API setMemManager(void* mem);
    virtual long ADDIN_API GetInfo();
    virtual void ADDIN_API Done();
    // ILanguageExtenderBase
    virtual bool ADDIN_API RegisterExtensionAs(WCHAR_T**);
    virtual long ADDIN_API GetNProps();
    virtual long ADDIN_API FindProp(const WCHAR_T* wsPropName);
    virtual const WCHAR_T* ADDIN_API GetPropName(long lPropNum, long lPropAlias);
    virtual bool ADDIN_API GetPropVal(const long lPropNum, tVariant* pvarPropVal);
    virtual bool ADDIN_API SetPropVal(const long lPropNum, tVariant* varPropVal);
    virtual bool ADDIN_API IsPropReadable(const long lPropNum);
    virtual bool ADDIN_API IsPropWritable(const long lPropNum);
    virtual long ADDIN_API GetNMethods();
    virtual long ADDIN_API FindMethod(const WCHAR_T* wsMethodName);
    virtual const WCHAR_T* ADDIN_API GetMethodName(const long lMethodNum, 
                            const long lMethodAlias);
    virtual long ADDIN_API GetNParams(const long lMethodNum);
    virtual bool ADDIN_API GetParamDefValue(const long lMethodNum, const long lParamNum,
                            tVariant *pvarParamDefValue);   
    virtual bool ADDIN_API HasRetVal(const long lMethodNum);
    virtual bool ADDIN_API CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray);
    virtual bool ADDIN_API CallAsFunc(const long lMethodNum,
                tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
    // ILocaleBase
    virtual void ADDIN_API SetLocale(const WCHAR_T* loc);

    
private:
    long findName(const wchar_t* names[], const wchar_t* name, const uint32_t size) const;
    IMemoryManager* m_iMemory;
    IAddInDefBaseEx* m_iConnect;
};

#endif //__ADDINNATIVE_H__
