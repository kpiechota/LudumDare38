#pragma once
#ifdef _DEBUG
#pragma comment(lib, "../lib/DirectXTex_d.lib")
#else
#pragma comment(lib, "../lib/DirectXTex.lib")
#endif
#pragma comment( lib, "../lib/irrKlang" )
#pragma comment( lib, "../lib/libfbxsdk")
#pragma comment( lib, "d3d12" )
#pragma comment( lib, "dxgi" )
#pragma comment( lib, "d3dcompiler" )

#ifdef _DEBUG
#define ASSERT( condition ) assert( condition )
#define ASSERT_STR( condition, msg ) assert( condition && msg )
#define CT_ASSERT( condition ) static_assert( condition )
#else
#define ASSERT( condition ) {}
#define ASSERT_STR( condition, msg ) {}
#define CT_ASSERT( condition ) {}
#endif

#define FORCE_INLINE __forceinline
#define FLAG( f ) (1 << f)

#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>

#include <string>
#include <vector>
#include <comdef.h>
#include <intrin.h>
#include <cstdlib>
#include <assert.h>
#include <stdarg.h>

#include "types.h"
#include "math.h"
#include "texture.h"

#include "viewObject.h"
#include "gameObject.h"
#include "staticObject.h"
#include "lightObject.h"

#include "dynamicGeometryManager.h"
#include "textRenderManager.h"

extern int GWidth;
extern int GHeight;

extern float const GIslandSize;

void CheckResult(HRESULT result);
void CheckResult(HRESULT result, ID3DBlob* errorBlob);