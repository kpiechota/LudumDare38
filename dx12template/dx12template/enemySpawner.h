#pragma once
#include "headers.h"

class CEnemySpawner
{
private:
	unsigned int m_aliveEnemy;
	unsigned int m_maxEnemySpawn;
	unsigned int m_maxAliveEnemy;

	float m_spawnTime;
	float m_lastSpawn;

	float m_spawnIncTime;
	float m_lastSpawnInc;

public:
	CEnemySpawner();

	void Init();
	void Update();
	void EnemyKilled();
};
