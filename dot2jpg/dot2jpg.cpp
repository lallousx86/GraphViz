#ifdef UNICODE
#error cannot compile in unicode yet
#endif

#define _CRT_SECURE_NO_DEPRECATE
#include <atlbase.h>
#include <iostream>
#include <string>
#include <stdio.h>

#include "WinGraphviz.h"

using namespace std;

bool read_to_str(const char *fn, std::string &str)
{
  FILE *fp = fopen(fn, "rb");
  if (fp == NULL)
    return false;

  fseek(fp, 0, SEEK_END);

  long sz = ftell(fp);

  fseek(fp, 0, SEEK_SET);

  char *buf = new char[sz+2];

  fread(buf, 1, sz, fp);

  buf[sz] = 0;

  str = buf;

  delete [] buf;

  fclose(fp);

  return true;
}

HRESULT __stdcall MyCoCreateInstance(
  LPCTSTR szDllName,
  IN REFCLSID rclsid, 
  IUnknown* pUnkOuter, 
  IN REFIID riid, 
  OUT LPVOID FAR* ppv)
{
  HRESULT hr = REGDB_E_KEYMISSING;

  HMODULE hDll = ::LoadLibrary(szDllName);
  if (hDll == 0)
    return hr;

  typedef HRESULT (__stdcall *pDllGetClassObject)(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv);

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

int main(int argc, char* argv[])
{
	USES_CONVERSION;
	HRESULT hr;
	IDOT * pIDOT;
	CComBSTR result;

  if (argc < 3)
  {
    cout << "Usage: dot_to_jpg FileName.dot FileName.Jpg" << endl;
    return -1;
  }
	hr = CoInitialize(NULL);

	if (FAILED(hr)) 
  {
		cout << "CoInitialize Failed: " << hr << "\n\n";
		exit(1);
	}
		
	hr = CoCreateInstance(CLSID_DOT, NULL, CLSCTX_ALL, 
	  IID_IDOT, (LPVOID *)&pIDOT);

	if (hr == REGDB_E_CLASSNOTREG)
  {
    hr = MyCoCreateInstance(_T("WinGraphViz.dll"), CLSID_DOT, NULL, IID_IDOT, (LPVOID *)&pIDOT);
  }

  if (FAILED(hr)) 
  {
	  cout << "CoCreateInstance Failed: " << hr << "\n\n";
    return -1;
	}
		
  std::string s;
  VARIANT_BOOL vBool;

  if (!read_to_str(argv[1], s))
  {
    //s = "digraph G {A [label=\"hi\"];A -> B -> C -> A;}";
    cout << "Could not open input file: " << argv[1] << endl;
    return -2;
  }

  BSTR bstr = A2BSTR(s.c_str());

  if (FAILED(pIDOT->Validate(bstr, &vBool)) || vBool == FALSE)
  {
    cout << "DOT file syntax error!\n";
    pIDOT->Release();
    return -4;
  }

  IBinaryImage *ib = 0;
  pIDOT->ToJPEG(bstr, &ib);
	
  CComBSTR bstrOut = argv[2];
  if (FAILED(ib->Save((BSTR)bstrOut, &vBool)) || vBool == FALSE)
  {
    cout << "Failed to save!\n";
  }
  ib->Release();

  pIDOT->Release();

	CoUninitialize();
	return 0;
};
	