#include "headers.h"

void CheckResult(HRESULT result)
{
	if (FAILED(result))
	{
		_com_error err(result);
		::OutputDebugString(err.ErrorMessage());
		__debugbreak();
	}
}
void CheckResult(HRESULT result, ID3DBlob* errorBlob)
{
	if (FAILED(result))
	{
		_com_error err(result);
		::OutputDebugString(err.ErrorMessage());
		::OutputDebugStringA("\n");
		::OutputDebugStringA((LPCSTR)errorBlob->GetBufferPointer());
		ASSERT( false );
	}
	if (errorBlob && 0 < errorBlob->GetBufferSize())
	{
		::OutputDebugString((LPCWSTR)errorBlob->GetBufferPointer());
		ASSERT( false );
	}
}

