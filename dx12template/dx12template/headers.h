#pragma once
#pragma comment( lib, "../lib/FreeImage" )
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

void CheckResult(HRESULT result);
void CheckResult(HRESULT result, ID3DBlob* errorBlob);