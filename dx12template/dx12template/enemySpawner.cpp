#include "enemySpawner.h"
#include "enemy.h"
#include "timer.h"
#include "playerObject.h"

extern CTimer GTimer;
extern CPlayerObject* GPlayer;

CEnemySpawner::CEnemySpawner()
{}

void CEnemySpawner::Init()
{
	m_aliveEnemy = 0;
	m_maxEnemySpawn = 5;
	m_maxAliveEnemy = 100;
	m_spawnTime = 5.f;
	m_lastSpawn = m_spawnTime;
	m_spawnIncTime = 20.f;
	m_lastSpawnInc = m_spawnIncTime;
}

void CEnemySpawner::Update()
{
	m_lastSpawn -= GTimer.GameDelta();
	m_lastSpawnInc -= GTimer.GameDelta();

	if (m_lastSpawnInc < 0.f)
	{
		m_maxEnemySpawn += 5;
		m_lastSpawnInc = m_spawnIncTime;
	}

	if (m_lastSpawn < 0.f && m_aliveEnemy < m_maxAliveEnemy)
	{
		unsigned int const enemyToSpawn = rand() % min(m_maxEnemySpawn, m_maxAliveEnemy - m_aliveEnemy);
		
		for (unsigned int enemyID = 0; enemyID < enemyToSpawn; ++enemyID)
		{
			Vec2 const spawnPosition = Vec2::GetRandomInCircle() * (GIslandSize - 12.f);

			CEnemyObject* pEnemy = new CEnemyObject();
			pEnemy->SetPosition(spawnPosition);
			GGameObjectsToSpawn.push_back(pEnemy);

			++m_aliveEnemy;
			m_lastSpawn = m_spawnTime;
		}
	}
}

void CEnemySpawner::EnemyKilled()
{
	--m_aliveEnemy;
	GPlayer->AddEnergy();
}
