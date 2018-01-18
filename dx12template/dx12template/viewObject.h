#pragma once
#include "rendering\renderData.h"

struct SCameraMatrices
{
	Matrix4x4 m_viewToScreen;
	Matrix4x4 m_screenToView;
	Matrix4x4 m_worldToView;
	Matrix4x4 m_viewToWorld;
	Matrix4x4 m_worldToScreen;
	Matrix4x4 m_screenToWorld;
};

enum EViews
{
	SCENE,
	RAIN_SHADOW,

	MAX
};

struct SViewObject
{
	SCameraMatrices m_camera;
};