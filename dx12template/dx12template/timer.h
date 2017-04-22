#pragma once
#include <ctime>

class CTimer
{
private:
	long m_startTime;
	long m_lastDelta;
	long m_lastTime;
	float m_gameScale;

public:
	CTimer()
		: m_lastDelta(0)
		, m_gameScale(1.f)
	{
		m_lastTime = clock();
		m_startTime = clock();
	}

	void SetGameScale(float const scale)
	{
		m_gameScale = scale;
	}

	void Tick()
	{
		m_lastDelta = clock() - m_lastTime;
		m_lastTime = clock();
	}

	long TimeFromStart() const
	{
		return clock() - m_startTime;
	}

	long LastDelta() const
	{
		return m_lastDelta;
	}

	float Delta() const
	{
		return static_cast<float>(m_lastDelta) / static_cast<float>(CLOCKS_PER_SEC);
	}

	float GameDelta() const
	{
		return m_gameScale * static_cast<float>(m_lastDelta) / static_cast<float>(CLOCKS_PER_SEC);
	}

	static float GetSeconds( long const time ) 
	{ 
		return static_cast<float>(time) / static_cast<float>(CLOCKS_PER_SEC);
	}
};

extern CTimer GTimer;