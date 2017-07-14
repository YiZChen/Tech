#pragma once

#include <nclgl\Mesh.h>

#include <ncltech\Scene.h>
#include <ncltech\NCLDebug.h>
#include <ncltech\CommonUtils.h>
#include <ncltech\SceneManager.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\NetworkBase.h>
#include <ncltech\DistanceConstraint.h>

#include <sstream>

class Scene_Main : public Scene
{
public:
	Scene_Main(const std::string& friendly_name) :
		Scene(friendly_name),
		m_AccumTime(0.0f),
		m_Rotating(true) {};

	virtual ~Scene_Main() {};

	virtual void OnInitializeScene() override;
	virtual void OnUpdateScene(float dt) override;

	//Network
	virtual void OnCleanupScene() override;
	void ProcessNetworkEvent(const ENetEvent& evnt);
	NetworkBase	GetNetwork() { return m_Network; }

	//Get Moon Position for gravity calculation
	Vector3 GetMoonPosition() { return m_MoonPos; }

	//Calculate score for hitting the target
	void CalculateScore(int n) { this->m_Score = n; }
	int GetScorePoint() { return m_Score; }

	void SetAIHeight(float h) { this->m_AIHight = h; }
	float GetAIHeight() { return m_AIHight; }

	void SetAISpeed(float v) { this->m_AISpeed; }
	float GetAISpeed() { return m_AISpeed; }

	void SetAITrack(float t) { this->m_AITrack = t; }
	float GetAITrack() { return m_AITrack; }

	void DrawBoundingBox();
	void DrawIdleObject();

	void SendDataToServer();

	void AIPerformance();

protected:
	int*					sco_all;
	int						num_score;

	float					m_AccumTime;
	const Vector3			EarthPos = Vector3(0.0f, 0.0f, 0.0f);

	bool					m_Rotating;
	Vector3					m_MoonPos;

	int						m_Score;

	std::vector<Object*>	m_Cubes;
	std::vector<Object*>	m_Spheres;

	//NetWork
	NetworkBase				m_Network;
	ENetPeer*				m_pServerConnection;
	bool					can_send_information;

	//Debug draw count
	int						m_DebugViewCount;

	float					m_AIHight;
	float					m_AISpeed;
	float					m_AITrack;
};