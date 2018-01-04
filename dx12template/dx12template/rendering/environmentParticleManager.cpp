#include "render.h"
#include "../timer.h"

CEnvironmentParticleManager::CEnvironmentParticleManager()
	: m_particlesGPU( nullptr )
	, m_particleCA( nullptr )
	, m_particleCL( nullptr )
	, m_particleRS( nullptr )
	, m_particlesNum( 0 )
{}

void CEnvironmentParticleManager::Init( UINT const initParticleNum, int const boxesNumX, int const boxesNumY, int const boxesNumZ, float const boxesSizeX, float const boxesSizeY, float const boxesSizeZ )
{
	m_boxesSize.Set( boxesSizeX, boxesSizeY, boxesSizeZ );
	m_boxesNum.Set( boxesNumX, boxesNumY, boxesNumZ );

	m_boxMatrix.m_a00 = m_boxesSize.x;
	m_boxMatrix.m_a11 = -m_boxesSize.y;
	m_boxMatrix.m_a22 = m_boxesSize.z;

	ID3D12Device* const device = GRender.GetDevice();

	D3D12_ROOT_PARAMETER rootParameters[2];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor = {0, 0};
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
	rootParameters[1].Descriptor = { 0, 0 };
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_DESC descRootSignature;
	descRootSignature.NumParameters = ARRAYSIZE(rootParameters);
	descRootSignature.pParameters = rootParameters;
	descRootSignature.NumStaticSamplers = 0;
	descRootSignature.pStaticSamplers = nullptr;
	descRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	ID3DBlob* signature;
	ID3DBlob* error = nullptr;
	CheckResult(D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error), error);
	CheckResult(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_particleRS)));
	signature->Release();

	CheckResult( device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS( &m_particleCA ) ) );
	CheckResult( device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE, m_particleCA, nullptr, IID_PPV_ARGS( &m_particleCL ) ) );
	CheckResult(m_particleCL->Close());

	m_particleRS->SetName( L"Environment Particle Root Signature" );
	m_particleCA->SetName( L"Environment Particle Command Allocator" );
	m_particleCL->SetName( L"Environment Particle Command List" );

	m_particleShaderUpdate.InitComputeShader( L"../shaders/environmentParticleUpdate.hlsl", m_particleRS );
	m_particleShaderInit.InitComputeShader( L"../shaders/environmentParticleInit.hlsl", m_particleRS );

	m_particlesNum = initParticleNum;
	AllocateBuffers();
}

void CEnvironmentParticleManager::AllocateBuffers()
{
	if ( m_particlesGPU )
	{
		m_particlesGPU->Release();
	}

	ID3D12Device* const device = GRender.GetDevice();

	D3D12_RESOURCE_DESC particleResDesc = {};
	particleResDesc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
	particleResDesc.Width = m_particlesNum * 9 * sizeof( float ); //position + velocity
	particleResDesc.Height = 1;
	particleResDesc.DepthOrArraySize = 1;
	particleResDesc.MipLevels = 1;
	particleResDesc.Format = DXGI_FORMAT_UNKNOWN;
	particleResDesc.SampleDesc.Count = 1;
	particleResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	particleResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	CheckResult( device->CreateCommittedResource( &GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &particleResDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS( &m_particlesGPU ) ) );

	InitParticles();
}

void CEnvironmentParticleManager::InitParticles()
{
	struct CBuffer
	{
		float m_velocity[3];
		float m_padding;
		float m_initSize[2];
		float m_sizeRand[2];
		float m_velocityOffsetRand[2];
		float m_speedRand[2];
		UINT m_seed;
		UINT m_particleNum;
	} cbuffer;

	cbuffer.m_velocity[ 0 ] = 4.f;
	cbuffer.m_velocity[ 1 ] = 3.f;
	cbuffer.m_velocity[ 2 ] = 1.f;
	cbuffer.m_initSize[ 0 ] = .5f;
	cbuffer.m_initSize[ 1 ] = 8.f;
	cbuffer.m_sizeRand[ 0 ] = 0.005f;
	cbuffer.m_sizeRand[ 1 ] = 0.01f;
	cbuffer.m_velocityOffsetRand[ 0 ] = -.2f;
	cbuffer.m_velocityOffsetRand[ 1 ] = .2f;
	cbuffer.m_speedRand[ 0 ] = 2.f;
	cbuffer.m_speedRand[ 1 ] = 3.f;
	cbuffer.m_seed = rand();
	cbuffer.m_particleNum = m_particlesNum;
	( ( Vec3* )( &cbuffer.m_velocity[ 0 ] ) )->Normalize();

	D3D12_GPU_VIRTUAL_ADDRESS constBufferAddress;
	GRender.SetConstBuffer( constBufferAddress, (Byte*)&cbuffer, sizeof( cbuffer ) );

	m_particleCA->Reset();
	m_particleCL->Reset( m_particleCA, m_particleShaderInit.GetPSO() );

	m_particleCL->SetComputeRootSignature( m_particleRS );
	m_particleCL->SetComputeRootConstantBufferView( 0, constBufferAddress );
	m_particleCL->SetComputeRootSignature( m_particleRS );
	m_particleCL->SetComputeRootUnorderedAccessView( 1, m_particlesGPU->GetGPUVirtualAddress() );

	m_particleCL->Dispatch( ( m_particlesNum + 63 ) & ~63, 1, 1 );

	m_particleCL->Close();

	GRender.ExecuteComputeQueue( 1, (ID3D12CommandList* const*)(&m_particleCL) );
	GRender.WaitForComputeQueue();
}

void CEnvironmentParticleManager::UpdateParticles()
{
	struct CBuffer
	{
		float m_deltaTime;
		UINT m_particleNum;
	} cbuffer;
	cbuffer.m_deltaTime = GTimer.GameDelta();
	cbuffer.m_particleNum = m_particlesNum;
	D3D12_GPU_VIRTUAL_ADDRESS constBufferAddress;
	GRender.SetConstBuffer( constBufferAddress, (Byte*)&cbuffer, sizeof( cbuffer ) );

	m_particleCA->Reset();
	m_particleCL->Reset( m_particleCA, m_particleShaderUpdate.GetPSO() );

	m_particleCL->SetComputeRootSignature( m_particleRS );
	m_particleCL->SetComputeRootConstantBufferView( 0, constBufferAddress );
	m_particleCL->SetComputeRootUnorderedAccessView( 1, m_particlesGPU->GetGPUVirtualAddress() );

	m_particleCL->Dispatch( ( m_particlesNum + 63 ) & ~63, 1, 1 );

	m_particleCL->Close();

	GRender.AddComputeCommandList( m_particleCL );
}
void CEnvironmentParticleManager::FillRenderData()
{
	Vec3 const cameraPosition = GComponentCameraManager.GetMainCameraPosition();
	Vec3 position = cameraPosition;
	position = Math::Snap( position, m_boxesSize ) - ( Vec3( 0.5f, -0.5f, 0.5f ) * m_boxesSize );

	SRenderData renderData;
	renderData.m_verticesStart = 0;
	renderData.m_indicesStart = 0;
	renderData.m_indicesNum = 6 * m_particlesNum;
	renderData.m_instancesNum = 1;

	renderData.m_topology = D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderData.m_textureID[ 0 ] = T_RAIN_DROP;
	renderData.m_shaderID = EShaderType::ST_ENV_PARTICLE;
	renderData.m_drawType = SRenderData::EDrawType::DrawInstanced;

	Matrix4x4 const worldToScreen = GViewObject.m_camera.m_worldToScreen;
	Matrix4x4 objectToWorld = m_boxMatrix;

	Vec4 const color( 0.1f, 0.1f, 0.1f, 1.f );

	for ( int x = -m_boxesNum.x; x <= m_boxesNum.x; ++x )
	{
		for ( int y = -m_boxesNum.y; y <= m_boxesNum.y; ++y )
		{
			for ( int z = -m_boxesNum.z; z <= m_boxesNum.z; ++z )
			{
				Vec3 const boxPosition = Vec3( float( x ), float( y ), float( z ) ) * m_boxesSize + position;

				objectToWorld.m_a30 = boxPosition.x;
				objectToWorld.m_a31 = boxPosition.y;
				objectToWorld.m_a32 = boxPosition.z;

				Matrix4x4 tObjectToWorld = objectToWorld;
				Matrix4x4 tWorldToScreen = worldToScreen;

				tObjectToWorld.Transpose();
				tWorldToScreen.Transpose();

				CConstBufferCtx const cbCtx = GRender.GetConstBufferCtx( renderData.m_cbOffset, renderData.m_shaderID );
				cbCtx.SetParam( reinterpret_cast<Byte const*>( &tWorldToScreen ), sizeof( tWorldToScreen ), EShaderParameters::WorldToScreen );
				cbCtx.SetParam( reinterpret_cast<Byte const*>( &tObjectToWorld ), 3 * sizeof( Vec4 ), EShaderParameters::ObjectToWorld );
				cbCtx.SetParam( reinterpret_cast<Byte const*>( &color ), sizeof( color ), EShaderParameters::Color );
				cbCtx.SetParam( reinterpret_cast<Byte const*>( &cameraPosition ), sizeof( cameraPosition ), EShaderParameters::CameraPositionWS );

				GViewObject.m_renderData[ERenderLayer::RL_TRANSLUCENT].Add(renderData);
			}
		}
	}
}
void CEnvironmentParticleManager::Release()
{
	m_particlesGPU->Release();

	m_particleCA->Release();
	m_particleCL->Release();

	m_particleRS->Release();
	m_particleShaderUpdate.Release();
	m_particleShaderInit.Release();
}

D3D12_GPU_VIRTUAL_ADDRESS CEnvironmentParticleManager::GetParticlesBufferAddress() const
{
	return m_particlesGPU->GetGPUVirtualAddress();
}

CEnvironmentParticleManager GEnvironmentParticleManager;