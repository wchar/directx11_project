#pragma once

#pragma warning(disable: 4005)
#include <d3d11.h>
#include <d3dx11.h>
//#include <d3d11shader.h>
#include <D3Dcompiler.h>
#include <xnamath.h>
#include <DxErr.h>
#pragma warning(default: 4005)

#define SAFE_DELETE(p)			\
	if(p)						\
	{ delete (p); (p)=NULL; }


#define SAFE_RELEASE(p)			\
	if(p)						\
	{ (p)->Release(); (p)=NULL; }

#define SAFE_DELETE_ARRAY(p)	\
	if(p)						\
	{ delete[] (p); (p)=NULL; }

#ifndef V
#define V(x) { if (FAILED(x)) { MessageBox(NULL, L#x, L"Error", 0x00020000L); } }	
#endif

#ifdef SIMPLEDLL_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

#define NS_WE_BEGAN		namespace WE\
						{
#define NS_WE_END		}
#define USING_WE		using namespace WE;

#define NS_NONE_BEGAN	namespace\
						{
#define NS_NONE_END		}

#include <vector>
#include <list>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <string>

using namespace std;

#ifdef DEBUG
#define WLOG(x) wcout<<"log: "<<x<<endl;
#else
#define WLOG(x) do witch)(0)
#endif

NS_WE_BEGAN

struct size
{
	UINT width;
	UINT height;

	size() = default;
	size(UINT w, UINT h) : width(w), height(h) { NULL; }
};

template<class type>
class auto_release_ptr
{
public:
	auto_release_ptr(type *com)
		: _com(com)
	{}
	~auto_release_ptr()
	{
		SAFE_RELEASE(_com);
	}
private:
	type *_com;
};

#define STATIC_AUTO_RELEASE(x) { static auto_release_ptr<IUnknown> s_auto_ptr(x); }

NS_WE_END
