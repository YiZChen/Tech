#include "Scene_Main.h"

const int num = 3;
bool show_rankingList = false;
bool isDebugDraw = false;

void Scene_Main::OnInitializeScene()
{
	SceneManager::Instance()->GetCamera()->SetPosition(Vector3(30.0f, 0.0f, 30.0f));
	SceneManager::Instance()->GetCamera()->SetYaw(45.0f);
	SceneManager::Instance()->GetCamera()->SetPitch(0.0f);
	PhysicsEngine::Instance()->SetGravity(Vector3(0.f, 0.f, 0.f));

	m_AccumTime = 0.0f;
	m_Rotating = true;
	m_MoonPos = Vector3(30.0f, 0.0f, 0.0f);
	m_Score = 0;
	m_DebugViewCount = 0;
	m_AIHight = 0;
	m_AISpeed = 4.0f;
	m_AITrack = 30.0f;

	//Initialize Client Network
	if (m_Network.Initialize(0))
	{
		NCLDebug::Log("Network: Initialized!");

		m_pServerConnection = m_Network.ConnectPeer(127, 0, 0, 1, 1234);
		NCLDebug::Log("Network: Attempting to connet to server!");
	}

	Scene::OnInitializeScene();

	//Creating earth
	Object* P1 = CommonUtils::BuildPlanetObject("Death Star",
		EarthPos,						//Position leading to 0.25 meter overlap between spheres
		10.0f,							//Radius
		true,							//Has Physics Object
		0.0f,							//Infinite Mass
		true,							//Has Collision Shape
		false,							//Dragable by the user
		Vector4(1.0, 1.0, 1.0, 1.0));	//Color
	this->AddGameObject(P1);

	//Creating TargetBoard_Earth
	Object* target_Earth = CommonUtils::BuildCuboidObject("Target_Earth",
		EarthPos + Vector3(0, 11, 0),
		Vector3(1.5, 1.5, 0.1),
		true,
		0.0f,
		true,
		false,
		Vector4(1.0, 1.0, 1.0, 1.0));
	this->AddGameObject(target_Earth);
	target_Earth->Physics()->SetScoreState(true);

	//Creating moon
	Object* Starship = CommonUtils::BuildBattleShip("StarDestroyer",
		m_MoonPos,
		Vector3(1, 1, 1),
		true,
		0.0f,
		true,
		false,
		Vector4(1.0, 1.0, 1.0, 1.0));
	this->AddGameObject(Starship);

	DrawBoundingBox();
	DrawIdleObject();

	//Create Springs
	{
		Object* Spring[6][4];
		for (int i = 0;i < 6; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				Spring[i][j] = CommonUtils::BuildSpringSphereObject("",
					EarthPos + Vector3(0, 15 + 2 * i, -4 + 2 * j),	//Position leading to 0.25 meter overlap between spheres
					1.0f,							//Radius
					true,							//Has Physics Object
					2.0f,							//Infinite Mass
					true,							//Has Collision Shape
					false,							//Dragable by the user
					Vector4(1.0, 1.0, 1.0, 1.0));	//Color
				this->AddGameObject(Spring[i][j]);
				Spring[i][j]->Physics()->SetSpringSphere(true);
			}
		}

		for (int j = 0; j < 3; ++j)
		{
			for (int i = 0;i < 6; ++i)
			{
				DistanceConstraint* constraint = new DistanceConstraint(
					Spring[i][j]->Physics(),						//Physics Object A
					Spring[i][j + 1]->Physics(),					//Physics Object B
					Spring[i][j]->Physics()->GetPosition(),			//Attachment Position on Object A	-> Currently the centre
					Spring[i][j + 1]->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  
				PhysicsEngine::Instance()->AddConstraint(constraint);
			}
		}

		for (int i = 0;i < 5; ++i)
		{
			for (int j = 0;j < 4; ++j)
			{
				DistanceConstraint* constraint = new DistanceConstraint(
					Spring[i][j]->Physics(),						//Physics Object A
					Spring[i + 1][j]->Physics(),						//Physics Object B
					Spring[i][j]->Physics()->GetPosition(),			//Attachment Position on Object A	-> Currently the centre
					Spring[i + 1][j]->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  
				PhysicsEngine::Instance()->AddConstraint(constraint);
			}
		}

		for (int a = 0; a < 4; ++a)
		{
			Spring[5][a]->Physics()->SetInverseMass(0.0f);
			Spring[0][a]->Physics()->SetInverseMass(0.0f);
			
		}
	}
}

void Scene_Main::OnUpdateScene(float dt)
{
	Object* planet_Main = this->FindGameObject("Death Star");

	Object* Board_Target = this->FindGameObject("Target_Earth");

	Object* starshipmain = this->FindGameObject("StarDestroyer");

	m_MoonPos = starshipmain->Physics()->GetPosition();

	Scene::OnUpdateScene(dt);

	if (m_Rotating)
	{
		m_AccumTime += dt;

		if (planet_Main != NULL)
		{
			//earth self spin
			planet_Main->Physics()->SetAngularVelocity(Vector3(0, 0.25, 0));
		}

		//Set the Target position and rotation
		if (Board_Target != NULL)
		{
			Board_Target->Physics()->SetAngularVelocity(planet_Main->Physics()->GetAngularVelocity());

			//See if the target is hitted by a missle or a ball
			if (Board_Target->Physics()->IsColliding() && Board_Target->Physics()->IsScore())
			{
				m_Score += Board_Target->Physics()->GetScorePoint();
			}
		}

		if (starshipmain != NULL)
		{
			starshipmain->Physics()->SetAngularVelocity(Vector3(0, 15.0, 0));
			starshipmain->Physics()->SetPosition(Vector3(
				EarthPos.x + m_AITrack * cos(DegToRad(m_AccumTime * m_AISpeed) * 2.0f) * 0.75f,
				EarthPos.y + m_AIHight,
				EarthPos.z + m_AITrack * sin(DegToRad(m_AccumTime * m_AISpeed) * 2.0f) * 0.75f));
		}
	}

	//Press M to Draw collision shap for debug
	uint drawFlags = PhysicsEngine::Instance()->GetDebugDrawFlags();

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_M))
	{
		switch (m_DebugViewCount % 3)
		{
		case 0: 
		{
			drawFlags = DEBUGDRAW_FLAGS_COLLISIONVOLUMES | DEBUGDRAW_FLAGS_CONSTRAINT;
			SceneManager::Instance()->SetDrawingEnabled(false);		
		}
		break;
			
		case 1:
		{
			drawFlags = DEBUGDRAW_FLAGS_COLLISIONVOLUMES;
			SceneManager::Instance()->SetDrawingEnabled(true);	
		}
		break;

		case 2:
		{
			drawFlags = NULL;
			SceneManager::Instance()->SetDrawingEnabled(true);
		}	
		break;
		}
		m_DebugViewCount++;
		//isDebugDraw = !isDebugDraw;
		
	}
	PhysicsEngine::Instance()->SetDebugDrawFlags(drawFlags);

	float fireSpeed = 10.0f;

	//Press J to fire sphere
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_J))
	{
		static int n = 0;

		//Build an sphere every time
		ostringstream projectileName;
		projectileName << "bullet" << n;
		Object* sphere = CommonUtils::BuildBallObject(projectileName.str(),
			SceneManager::Instance()->GetCamera()->GetPosition(),	//Position leading to 0.25 meter overlap between spheres
			0.35f,													//Radius
			true,													//Has Physics Object
			0.1f,													//Infinite Mass
			true,													//Has Collision Shape
			false,													//Dragable by the user
			Vector4(1.0f, 1.0f, 1.0f, 1.0f));						//Color
		sphere->Physics()->SetInverseMass(1.0f);
		this->AddGameObject(sphere);
		sphere->Physics()->SetScoreState(true);

		Object* bullet = this->FindGameObject(projectileName.str());


		if (bullet != NULL)
		{
			//Set the fire orientation
			Camera* cam = SceneManager::Instance()->GetCamera();

			//using the viewMatrix to get the Camera's facing side
			Matrix4& viewRot = SceneManager::Instance()->GetViewMatrix();
			Vector3 forward = -Vector3(viewRot[2], viewRot[6], viewRot[10]);

			//Draw the ball trail
			Vector3 initialVelocity = forward * fireSpeed;
			bullet->Physics()->SetLinearVelocity(Vector3(initialVelocity));
		}
		n++;
	}

	//Press K to fire rocket
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_K))
	{
		fireSpeed = 3 * fireSpeed;
		static int i = 0;
		ostringstream projectileName;
		projectileName << "rocket" << i;
		Object* rocket = CommonUtils::BuildRocketObject(projectileName.str(),
			SceneManager::Instance()->GetCamera()->GetPosition(),	//Position leading to 0.25 meter overlap between spheres
			Vector3(0.25, 0.25, 0.5),									//Radius
			true,													//Has Physics Object
			0.1f,													//Infinite Mass
			true,													//Has Collision Shape
			false,													//Dragable by the user
			Vector4(1.0f, 1.0f, 1.0f, 1.0f));						//Color
		rocket->Physics()->SetInverseMass(1.0f);
		this->AddGameObject(rocket);
		rocket->Physics()->SetScoreState(true);

		Object* roc = this->FindGameObject(projectileName.str());

		if (roc != NULL)
		{
			Camera* cam = SceneManager::Instance()->GetCamera();
			Matrix4 viewMtx = cam->BuildViewMatrix();
			roc->Physics()->SetOrientation(Quaternion::FromMatrix(viewMtx));

			//using the viewMatrix to get the Camera's facing side
			Matrix4& viewRot = SceneManager::Instance()->GetViewMatrix();
			Vector3 forward = -Vector3(viewRot[2], viewRot[6], viewRot[10]);

			//Draw the ball trail
			Vector3 initialVelocity = forward * fireSpeed;
			roc->Physics()->SetLinearVelocity(Vector3(initialVelocity));
		}
		i++;
	}

	//Press L to show the ranking list
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_L))
	{
		show_rankingList = !show_rankingList;
	}
	
	//Update NetWork
	auto callback = std::bind(
		&Scene_Main::ProcessNetworkEvent,
		this,
		std::placeholders::_1);
	m_Network.ServiceNetwork(dt, callback);

	uint8_t ip1 = m_pServerConnection->address.host & 0xFF;
	uint8_t ip2 = (m_pServerConnection->address.host >> 8) & 0xFF;
	uint8_t ip3 = (m_pServerConnection->address.host >> 16) & 0xFF;
	uint8_t ip4 = (m_pServerConnection->address.host >> 24) & 0xFF;

	NCLDebug::DrawTextWs(planet_Main->Physics()->GetPosition() + Vector3(0.f, 0.5f, 0.f), 14.f, TEXTALIGN_CENTRE, Vector4(),
		"%u.%u.%u.%u:%u", ip1, ip2, ip3, ip4, m_pServerConnection->address.port);

	if (show_rankingList)
	{
		NCLDebug::AddStatusEntry(Vector4(0.0, 1.0, 0.0, 1.0), "<------Ranking Top 10------>");

		for (int i = 0; i < num_score; i++)
		{
			ostringstream scorArr;
			scorArr << "No." << i + 1 << "-------------------" << sco_all[i];
			NCLDebug::AddStatusEntry(Vector4(0.0, 1.0, 0.0, 1.0), scorArr.str());
		}

		NCLDebug::AddStatusEntry(Vector4(0.0, 1.0, 0.0, 1.0), "<------------------------->");
	}
	else
	{
		NCLDebug::AddStatusEntry(Vector4(0.0, 1.0, 0.0, 1.0), "Press L to show the Ranking List");
	}
}

void Scene_Main::DrawBoundingBox()
{
	//Creating Cages
	Object* CageBoard1 = CommonUtils::BuildQuadObject("B1",
		EarthPos + Vector3(2, -15, 0),
		Vector3(2.02, 2.02, 0.0),
		true,
		0.0f,
		true,
		false,
		Vector4(1.0, 1.0, 1.0, 1.0));
	this->AddGameObject(CageBoard1);
	CageBoard1->Physics()->SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 90, 0));

	Object* CageBoard2 = CommonUtils::BuildQuadObject("B2",
		EarthPos + Vector3(-2, -15, 0),
		Vector3(2.02, 2.02, 0.0),
		true,
		0.0f,
		true,
		false,
		Vector4(1.0, 1.0, 1.0, 1.0));
	this->AddGameObject(CageBoard2);
	CageBoard2->Physics()->SetOrientation(Quaternion::EulerAnglesToQuaternion(0, -90, 0));

	Object* CageBoard3 = CommonUtils::BuildQuadObject("B3",
		EarthPos + Vector3(0, -15, 2),
		Vector3(2.02, 2.02, 0.0),
		true,
		0.0f,
		true,
		false,
		Vector4(1.0, 1.0, 1.0, 1.0));
	this->AddGameObject(CageBoard3);

	Object* CageBoard4 = CommonUtils::BuildQuadObject("B4",
		EarthPos + Vector3(0, -15, -2),
		Vector3(2.02, 2.02, 0.0),
		true,
		0.0f,
		true,
		false,
		Vector4(1.0, 1.0, 1.0, 1.0));
	this->AddGameObject(CageBoard4);
	CageBoard4->Physics()->SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 180, 0));

	Object* CageBoard5 = CommonUtils::BuildQuadObject("B5",
		EarthPos + Vector3(0, -13, 0),
		Vector3(2.02, 2.02, 0.0),
		true,
		0.0f,
		true,
		false,
		Vector4(1.0, 1.0, 1.0, 1.0));
	this->AddGameObject(CageBoard5);
	CageBoard5->Physics()->SetOrientation(Quaternion::EulerAnglesToQuaternion(-90, 0, 0));

	Object* CageBoard6 = CommonUtils::BuildQuadObject("B6",
		EarthPos + Vector3(0, -17, 0),
		Vector3(2.02, 2.02, 0.0),
		true,
		0.0f,
		true,
		false,
		Vector4(1.0, 1.0, 1.0, 1.0));
	this->AddGameObject(CageBoard6);
	CageBoard6->Physics()->SetOrientation(Quaternion::EulerAnglesToQuaternion(90, 0, 0));
}

void Scene_Main::DrawIdleObject()
{
	//Creating Unwaken Cubes
	const int total = num * num * num;
	m_Cubes.resize(total);
	for (int a = 0; a < num; a++)
	{
		for (int b = 0; b < num; b++)
		{
			for (int c = 0; c < num; c++)
			{
				int idx = (a * num + b) * num + c;
				m_Cubes[idx] = CommonUtils::BuildIdleCuboidObject("Cubes",
					EarthPos + Vector3(26 + a, b, c),
					Vector3(0.5, 0.5, 0.5),
					true,
					1.0f,
					true,
					false,
					Vector4(0.6, 0.6, 0.6, 1.0));
				this->AddGameObject(m_Cubes[idx]);
			}
		}
	}

	//Creating Unwaken Spheres
	m_Spheres.resize(total);
	for (int a = 0; a < num; a++)
	{
		for (int b = 0; b < num; b++)
		{
			for (int c = 0; c < num; c++)
			{
				int idx = (a * num + b) * num + c;
				m_Spheres[idx] = CommonUtils::BuildIdleSphereObject("Spheres",
					EarthPos + Vector3(a, b, 26 + c),
					0.5f,
					true,
					1.0f,
					true,
					false,
					Vector4(0.6, 0.6, 0.6, 1.0));
				this->AddGameObject(m_Spheres[idx]);
			}
		}
	}
}

void Scene_Main::OnCleanupScene()
{
	Scene::OnCleanupScene();
	//m_pObj = NULL; // Deleted in above function

	//Send one final packet telling the server we are disconnecting
	// - We are not waiting to resend this, so if it fails to arrive
	//   the server will have to wait until we time out naturally
	enet_peer_disconnect_now(m_pServerConnection, 0);

	//Release network and all associated data/peer connections
	m_Network.Release();
	m_pServerConnection = NULL;
}

void Scene_Main::ProcessNetworkEvent(const ENetEvent& evnt)
{
	Object* target = this->FindGameObject("target");

	switch (evnt.type)
	{
		//New connection request or an existing peer accepted our connection request
	case ENET_EVENT_TYPE_CONNECT:
	{
		if (evnt.peer == m_pServerConnection)
		{
			NCLDebug::Log("Network: Successfully connected to server!");
		}
	}
	break;

	//Server has sent us a new packet
	case ENET_EVENT_TYPE_RECEIVE:
	{
		sco_all = reinterpret_cast<int *>(evnt.packet->data);
		num_score = evnt.packet->dataLength / sizeof(int);
	}
	break;

	//Server has disconnected
	case ENET_EVENT_TYPE_DISCONNECT:
	{
		NCLDebug::Log("Network: Server has disconnected!");
	}
	break;
	}
}

void Scene_Main::SendDataToServer()
{
	ostringstream score_text;
	score_text << m_Score;
	ENetPacket* packet = enet_packet_create(
		score_text.str().c_str(),
		strlen(score_text.str().c_str()) + 1,
		ENET_PACKET_FLAG_RELIABLE
		);
	enet_peer_send(m_pServerConnection, 0, packet);
}