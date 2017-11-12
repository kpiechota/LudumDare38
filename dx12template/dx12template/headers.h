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

#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>

#include <string>
#include <comdef.h>
#include <intrin.h>
#include <cstdlib>
#include <assert.h>
#include <stdarg.h>

#include "core.h"
#include "texture.h"

#include "viewObject.h"
#include "components\components.h"
#include "entity.h"
#include "entityManager.h"

#include "dynamicGeometryManager.h"
#include "textRenderManager.h"

extern int GWidth;
extern int GHeight;

extern float const GIslandSize;

void CheckResult(HRESULT result);
void CheckResult(HRESULT result, ID3DBlob* errorBlob);