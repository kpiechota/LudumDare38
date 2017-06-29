#pragma once
#pragma comment( lib, "../lib/FreeImage" )
#pragma comment( lib, "../lib/irrKlang" )
#pragma comment( lib, "d3d12" )
#pragma comment( lib, "dxgi" )
#pragma comment( lib, "d3dcompiler" )

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
#include "constantBuffer.h"

#include "gameObject.h"

#include "dynamicGeometryManager.h"
#include "textRenderManager.h"

extern int GWidth;
extern int GHeight;

extern float const GIslandSize;

void CheckResult(HRESULT result);
void CheckResult(HRESULT result, ID3DBlob* errorBlob);

#ifdef _DEBUG
#define ASSERT( condition ) assert( condition )
#define ASSERT_STR( condition, msg ) assert( condition && msg )
#else
#define ASSERT( condition ) {}
#define ASSERT_STR( condition, msg ) {}
#endif

#define FORCE_INLINE __forceinline