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
	Matrix4x4 m_projectionMatrix;
	Vec3 m_positionOffset;
	UINT m_boxesNum;
	float m_boxesSize;
	UINT m_particlesNum;

private:
	void AllocateBuffers();
	void InitParticles(UINT const initParticleNum, UINT const boxesNum, float const boxesSize);
	FORCE_INLINE void InitProjectionMatrix( Vec3 const forward );

public:
	CEnvironmentParticleManager();

	void Init( UINT const initParticleNum, UINT const boxesNum, float const boxesSize );
	void UpdateParticles();
	void FillRenderData();

	Matrix4x4 GetProjectionMatrix() const { return m_projectionMatrix; }
	Vec3 GetPositionOffset() const { return m_positionOffset; }

	void Release();

	D3D12_GPU_VIRTUAL_ADDRESS GetParticlesBufferAddress() const;
};

extern CEnvironmentParticleManager GEnvironmentParticleManager;