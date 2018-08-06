#ifdef UNICODE
    #error cannot compile in unicode yet
#endif

#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "dot2jpg.h"
#include "WinGraphviz.h"

/*
Sample digraph
------------------

digraph G 
{
    A [label="hi"];
    A -> B -> C -> A;
}
*/

//-------------------------------------------------------------------------
static bool read_to_str(const char *fn, mybstr_t &str)
{
    FILE *fp = fopen(fn, "rb");
    if (fp == NULL)
        return false;

    fseek(fp, 0, SEEK_END);

    long sz = ftell(fp);

    fseek(fp, 0, SEEK_SET);

    char *buf = new char[sz + 2];

    fread(buf, 1, sz, fp);

    buf[sz] = 0;

    // Create BSTR
    mybstr_t s(buf);

    delete[] buf;

    fclose(fp);

    if (!s)
        return false;

    s.detach(str);

    return true;
}

//-------------------------------------------------------------------------
static HRESULT __stdcall MyCoCreateInstance(
    LPCTSTR szDllName,
    IN REFCLSID rclsid,
    IUnknown* pUnkOuter,
    IN REFIID riid,
    OUT LPVOID FAR* ppv)
{
    HRESULT hr = S_FALSE;

    HMODULE hDll = ::LoadLibrary(szDllName);
    if (hDll == NULL)
        return hr;

    typedef HRESULT(__stdcall *pDllGetClassObject)(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv);

    pDllGetClassObject GetClassObject = (pDllGetClassObject)::GetProcAddress(hDll, "DllGetClassObject");
    if (GetClassObject == 0)
    {
        ::FreeLibrary(hDll);
        return hr;
    }

    IClassFactory *pIFactory;

    hr = GetClassObject(rclsid, IID_IClassFactory, (LPVOID *)&pIFactory);

    if (!SUCCEEDED(hr))
        return hr;

    hr = pIFactory->CreateInstance(pUnkOuter, riid, ppv);

    pIFactory->Release();

    return hr;
}


//-------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: dot_to_jpg FileName.dot FileName.Jpg\n");
        return 0;
    }

    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr))
    {
        printf("CoInitialize Failed: %08X\n", hr);
        return -1;
    }

    IDOT *pIDOT = nullptr;
    hr = CoCreateInstance(
        CLSID_DOT, 
        nullptr,
        CLSCTX_ALL,
        IID_IDOT, (LPVOID *)&pIDOT);

    if (hr == REGDB_E_CLASSNOTREG)
        hr = MyCoCreateInstance("WinGraphViz.dll", CLSID_DOT, NULL, IID_IDOT, (LPVOID *)&pIDOT);

    if (FAILED(hr))
    {
        printf("CoCreateInstance Failed: %08X\n", hr);
        return -1;
    }

    if (pIDOT == nullptr)
    {
        printf("No instance!\n");
        return -2;
    }

    mybstr_t dot_script;
    VARIANT_BOOL vBool;

    if (!read_to_str(argv[1], dot_script))
    {
        printf("Could not open input file: %s\n", argv[1]);
        return -2;
    }

    bool ok = false;
    IBinaryImage *ib = nullptr;

    do
    {
        hr = pIDOT->Validate(dot_script, &vBool);
        if (FAILED(hr) || vBool == FALSE)
        {
            printf("DOT file syntax error!\n");
            break;
        }

        hr = pIDOT->ToJPEG(dot_script, &ib);
        if (FAILED(hr))
        {
            printf("Failed to convert to JPG\n");
            break;
        }

        mybstr_t fn(argv[2]);
        
        hr = ib->Save(fn, &vBool);
        if (FAILED(hr) || vBool == FALSE)
        {
            printf("Failed to save!\n");
            break;
        }
        printf("Written %s...\n", argv[2]);
    } while (false);


    if (ib != nullptr)
        ib->Release();

    if (pIDOT != nullptr)
        pIDOT->Release();

    CoUninitialize();

    return 0;
}