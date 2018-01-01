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
	UINT m_particlesNum;

private:
	void AllocateBuffers();
	void InitParticles();

public:
	CEnvironmentParticleManager();

	void Init( UINT const initParticleNum );
	void UpdateParticles();
	void FillRenderData();

	void Release();

	D3D12_GPU_VIRTUAL_ADDRESS GetParticlesBufferAddress() const;
};

extern CEnvironmentParticleManager GEnvironmentParticleManager;