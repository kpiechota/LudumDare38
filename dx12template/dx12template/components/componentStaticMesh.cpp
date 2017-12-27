#include "../headers.h"
#include "../render.h"

void CComponentStaticMeshManager::FillRenderData() const
{
	SRenderData renderData;
	renderData.m_topology = D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	Matrix4x4 const worldToScreen = GViewObject.m_camera.m_worldToScreen;
	UINT const renderComponentsNum = m_renderComponents.Size();
	for ( UINT i = 0; i < renderComponentsNum; ++i )
	{
		SComponentTransform const transform = GComponentTransformManager.GetComponentNoCheck( m_renderComponents[ i ].m_transformID );
		SComponentStaticMesh const staticMesh = GetComponentNoCheck( m_renderComponents[ i ].m_staticMeshID );

		Matrix4x4 tObjectToWorld = Matrix4x4::GetTranslateRotationSize( transform.m_position, transform.m_rotation, transform.m_scale );
		Matrix4x4 tObjectToScreen = Math::Mul( tObjectToWorld, worldToScreen );
		tObjectToWorld.Transpose();
		tObjectToScreen.Transpose();

		renderData.m_dataNum = GGeometryInfo[ staticMesh.m_geometryInfoID ].m_indicesNum;
		renderData.m_dataNum = GGeometryInfo[ staticMesh.m_geometryInfoID ].m_indicesNum;
		renderData.m_geometryID = GGeometryInfo[ staticMesh.m_geometryInfoID ].m_geometryID;
		renderData.m_shaderID = staticMesh.m_shaderID;

		for ( UINT texture = 0; texture < ARRAYSIZE( staticMesh.m_textureID ); ++texture )
		{
			renderData.m_textureID[ texture ] = staticMesh.m_textureID[ texture ];
		}

		CConstBufferCtx const cbCtx = GRender.GetConstBufferCtx( renderData.m_cbOffset, staticMesh.m_shaderID );
		cbCtx.SetParam( reinterpret_cast<Byte const*>( &tObjectToWorld ), sizeof( tObjectToWorld ), EShaderParameters::ObjectToWorld );
		cbCtx.SetParam( reinterpret_cast<Byte const*>( &tObjectToScreen ), sizeof( tObjectToScreen ), EShaderParameters::ObjectToScreen );

		GViewObject.m_renderData[staticMesh.m_layer].Add(renderData);
	}
}