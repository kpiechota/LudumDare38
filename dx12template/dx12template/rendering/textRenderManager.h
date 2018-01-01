#pragma once
#include "dynamicGeometryManager.h"
class CTextRenderManager
{
private:
	enum
	{
		CHAR_MAX_NUM = 512
	};

	tDynGeometryID m_dynGeometryID;

public:
	void Init();
	void Print( Vec4 const color, Vec2 position, float const size, char const* msg );
};

extern CTextRenderManager GTextRenderManager;