#pragma once

class CEnvironmentParticleManager
{
private:
	CShaderRes	m_particleShaderUpdate;
	CShaderRes	m_particleShaderInit;

	ID3D12Resource* m_particlesGPU;

	ID3D12CommandAllocator*		m_particleCA;
	ID3D12GraphicsCommandList*	m_particleCL;

	ID3D12RootSignature*		m_particleRS;

	Matrix4x4 m_boxMatrix;
	Vec3i m_boxesNum;
	Vec3 m_boxesSize;
	UINT m_particlesNum;

private:
	void AllocateBuffers();
	void InitParticles();

public:
	CEnvironmentParticleManager();

	void Init( UINT const initParticleNum, int const boxesNumX, int const boxesNumY, int const boxesNumZ, float const boxesSizeX, float const boxesSizeY, float const boxesSizeZ );
	void UpdateParticles();
	void FillRenderData();

	void Release();

	D3D12_GPU_VIRTUAL_ADDRESS GetParticlesBufferAddress() const;
};

extern CEnvironmentParticleManager GEnvironmentParticleManager;