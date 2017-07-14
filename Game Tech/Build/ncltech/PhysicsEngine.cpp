#include "PhysicsEngine.h"
#include "Object.h"
#include "CollisionDetectionSAT.h"
#include "NCLDebug.h"
#include <nclgl\Window.h>
#include <omp.h>

#include "../GameTech_Coursework/Scene_Main.h"
#include "SceneManager.h"
void PhysicsEngine::SetDefaults()
{
	m_DebugDrawFlags = NULL;
	m_IsPaused = false;
	m_IsShieldUp = false;
	m_IsUseForce = false;
	m_IsDrawTexture = true;
	m_UpdateTimestep = 1.0f / 60.f;
	m_UpdateAccum = 0.0f;
	m_Gravity = Vector3(0.0f, -9.81f, 0.0f);
	m_DampingFactor = 0.999f;
}

PhysicsEngine::PhysicsEngine()
{
	SetDefaults();
}

PhysicsEngine::~PhysicsEngine()
{
	RemoveAllPhysicsObjects();
}

void PhysicsEngine::AddPhysicsObject(PhysicsObject* obj)
{
	m_PhysicsObjects.push_back(obj);
}

void PhysicsEngine::RemovePhysicsObject(PhysicsObject* obj)
{
	//Lookup the object in question
	auto found_loc = std::find(m_PhysicsObjects.begin(), m_PhysicsObjects.end(), obj);

	//If found, remove it from the list
	if (found_loc != m_PhysicsObjects.end())
	{
		m_PhysicsObjects.erase(found_loc);
	}
}

void PhysicsEngine::RemoveAllPhysicsObjects()
{
	//Delete and remove all constraints/collision manifolds
	for (Constraint* c : m_vpConstraints)
	{
		delete c;
	}
	m_vpConstraints.clear();

	for (Manifold* m : m_vpManifolds)
	{
		delete m;
	}
	m_vpManifolds.clear();


	//Delete and remove all physics objects
	// - we also need to inform the (possible) associated game-object
	//   that the physics object no longer exists
	for (PhysicsObject* obj : m_PhysicsObjects)
	{
		if (obj->m_pParent != NULL) obj->m_pParent->m_pPhysicsObject = NULL;
		delete obj;
	}
	m_PhysicsObjects.clear();
}


void PhysicsEngine::Update(float deltaTime)
{
	const int max_updates_per_frame = 1;

	if (!m_IsPaused)
	{
		m_UpdateAccum += deltaTime;
		for (int i = 0; (m_UpdateAccum >= m_UpdateTimestep) && i < max_updates_per_frame; ++i)
		{
			m_UpdateAccum -= m_UpdateTimestep;
			if (!m_IsPaused) UpdatePhysics(); //Additional check here incase physics was paused mid-update and the contents of the physics need to be displayed
		}

		//if (m_UpdateAccum >= m_UpdateTimestep)
		//{
		//	//Drop Time in the hope that it can continue to run in real-time
		//	NCLERROR("Physics too slow to run in real time!");
		//	m_UpdateAccum = 0.0f;
		//}
	}
}


void PhysicsEngine::UpdatePhysics()
{
	for (Manifold* m : m_vpManifolds)
	{
		delete m;
	}
	m_vpManifolds.clear();

	//No colliding for default state
	for (auto* obj : m_PhysicsObjects)
	{
		obj->m_IsColliding = false;
	}

	//Check for collisions
	BroadPhaseCollisions();
	NarrowPhaseCollisions();

	//Solve collision constraints
	SolveConstraints();

	//Update movement
	for (PhysicsObject* obj : m_PhysicsObjects)
	{
		if (!obj->IsResting())
			UpdatePhysicsObject(obj);
	}
}


void PhysicsEngine::SolveConstraints()
{
	//Optional step to allow constraints to 
	// precompute values based off current velocities 
	// before they are updated in the main loop below.
	for (Manifold* m : m_vpManifolds)
	{
	   m->PreSolverStep(m_UpdateTimestep);
	}
	
	for (Constraint* c : m_vpConstraints)
	{
	   c->PreSolverStep(m_UpdateTimestep);
	}
	
	for (size_t i = 0; i < SOLVER_ITERATIONS; ++i)
	{
	   for (Manifold* m : m_vpManifolds)
	       {
	           m->ApplyImpulse();
	       }
	
	       for (Constraint* c : m_vpConstraints)
	       {
	           c->ApplyImpulse();
	       }
	}
}


void PhysicsEngine::UpdatePhysicsObject(PhysicsObject* obj)
{
	//Change the gravity towards to the center of the planet
	
	Scene* scene = SceneManager::Instance()->GetCurrentScene();
	Scene_Main* scenemain = dynamic_cast<Scene_Main*>(scene);

	Vector3 gravity = Vector3(0.f, 0.f, 0.f);

	Vector3 gE = Vector3(0, 0, 0) - obj->GetPosition();
	Vector3 moonPos = scenemain->GetMoonPosition();
	Vector3 gM = moonPos - obj->GetPosition();

	float disToEarth = gE.Length();
	float disToMoon = gM.Length();

	if (scenemain != NULL)
	{
		const float G = 0.08f;
		
		gM.Normalise();
		gE.Normalise();

		if (m_IsUseForce)
		{
			if (disToEarth > 10 && disToEarth < 15)
			{
				gravity -= gE * 400;
			}
		}
		else
		{
			if ((disToEarth - 10) < (disToMoon - 2.5) * 0.5)
			{
				gravity += gE * G / (disToEarth * disToEarth * 0.0001f);
			}
			else if ((disToEarth - 10) * 0.3 > (disToMoon - 2.5))
			{
				gravity += gM * G / (disToMoon * disToMoon * 0.001f);
			}
			else
			{
				gravity += gE * G / (disToEarth * disToEarth * 0.0001f);
				gravity += gM * G / (disToMoon * disToMoon * 0.001f);
			}
		}
	}

	//Apply  Gravity
	//  Technically  gravity  here is  calculated  by  formula:
	// ( m_Gravity / invMass * invMass * dt )
	if (obj->m_InvMass  > 0.0f)
		obj->m_LinearVelocity += (m_Gravity + gravity) * m_UpdateTimestep;
	Vector3 tempVelocity;
	tempVelocity = obj->m_LinearVelocity;
	//1. velocity in next time step
	obj->m_LinearVelocity += obj->m_Force * obj->m_InvMass * m_UpdateTimestep;
	
	if (m_IsShieldUp)
	{
		if (disToEarth > 10 && disToEarth < 15)
		{
			obj->m_LinearVelocity = obj->m_LinearVelocity * m_DampingFactor * 0.85f;
		}
		else
		{
			obj->m_LinearVelocity = obj->m_LinearVelocity * m_DampingFactor;
		}
	}
	else
	{
		obj->m_LinearVelocity = obj->m_LinearVelocity * m_DampingFactor;
	}

	//2. position in next time step
	//obj->m_Position += obj->m_LinearVelocity * m_UpdateTimestep;
	obj->m_Position += (obj->m_LinearVelocity + tempVelocity) * m_UpdateTimestep *0.5;

	if (obj->m_isSpringSphere)
	{
		obj->m_AngularVelocity = Vector3(0, 0, 0);
	}
	else
	{
		//Midpoint Method (angular displacement)
		tempVelocity = obj->m_AngularVelocity;
		//1. velocity
		obj->m_AngularVelocity += obj->m_InvInertia * obj->m_Torque * m_UpdateTimestep;
		obj->m_AngularVelocity = obj->m_AngularVelocity * m_DampingFactor;
	}
	
	//2. position
	obj->m_Orientation = obj->m_Orientation + obj->m_Orientation * (obj->m_AngularVelocity * m_UpdateTimestep * 0.5f);
	//obj->m_Orientation = obj->m_Orientation + obj->m_Orientation * ((obj->m_AngularVelocity + tempVelocity) * m_UpdateTimestep * 0.5f);
	obj->m_Orientation.Normalise();

	obj->m_wsTransformInvalidated = true;


	float newCombinedVel = obj->m_LinearVelocity.LengthSquared() + obj->m_AngularVelocity.LengthSquared();
	obj->m_AvgVelocity = (obj->m_AvgVelocity + newCombinedVel);

	if (obj->m_AvgVelocity < 0.2f)
	{
		if (obj->m_AngularVelocity.LengthSquared() < 0.05f)
		{
			obj->m_Resting = true;
		}
	}

	if (m_IsDrawTexture == true)
	{
		obj->GetAssociatedObject()->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else
	{
		obj->GetAssociatedObject()->SetColour(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	}

}


void PhysicsEngine::BroadPhaseCollisions()
{
	m_BroadphaseCollisionPairs.clear();

	PhysicsObject *m_pObj1, *m_pObj2;

	//	The broadphase needs to build a list of all potentially colliding objects in the world,
	//	which then get accurately assesed in narrowphase. If this is too coarse then the system slows down with
	//	the complexity of narrowphase collision checking, if this is too fine then collisions may be missed.

	//	Brute force approach.
	//  - For every object A, assume it could collide with every other object.. 
	//    even if they are on the opposite sides of the world.
	if (m_PhysicsObjects.size() > 0)
	{
		for (size_t i = 0; i < m_PhysicsObjects.size() - 1; ++i)
		{
			for (size_t j = i + 1; j < m_PhysicsObjects.size(); ++j)
			{
				m_pObj1 = m_PhysicsObjects[i];
				m_pObj2 = m_PhysicsObjects[j];

				if (!m_pObj1->IsResting() || !m_pObj2->IsResting())
				{
					//Check they both atleast have collision shapes
					if (m_pObj1->GetCollisionShape() != NULL
						&& m_pObj2->GetCollisionShape() != NULL)
					{
						CollisionPair cp;
						cp.pObjectA = m_pObj1;
						cp.pObjectB = m_pObj2;
						m_BroadphaseCollisionPairs.push_back(cp);
					}
				}
			}
		}
	}
}

void PhysicsEngine::NarrowPhaseCollisions()
{
	if (m_BroadphaseCollisionPairs.size() > 0)
	{
		//Collision data to pass between detection and manifold generation stages.
		CollisionData colData;				

		//Collision Detection Algorithm to use
		CollisionDetectionSAT colDetect;	

		// Iterate over all possible collision pairs and perform accurate collision detection
		for (size_t i = 0; i < m_BroadphaseCollisionPairs.size(); ++i)
		{
			CollisionPair& cp = m_BroadphaseCollisionPairs[i];

			CollisionShape *shapeA = cp.pObjectA->GetCollisionShape();
			CollisionShape *shapeB = cp.pObjectB->GetCollisionShape();

			colDetect.BeginNewPair(
				cp.pObjectA,
				cp.pObjectB,
				cp.pObjectA->GetCollisionShape(),
				cp.pObjectB->GetCollisionShape());

			//--TUTORIAL 4 CODE--
			// Detects if the objects are colliding - Seperating Axis Theorem
			if (colDetect.AreColliding(&colData))
			{
				cp.pObjectA->m_IsColliding = true;
				cp.pObjectB->m_IsColliding = true;
				//Draw collision data to the window if requested
				// - Have to do this here as colData is only temporary. 
				if (m_DebugDrawFlags & DEBUGDRAW_FLAGS_COLLISIONNORMALS)
				{
					NCLDebug::DrawPointNDT(colData._pointOnPlane, 0.1f, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
					NCLDebug::DrawThickLine(colData._pointOnPlane, colData._pointOnPlane - colData._normal * colData._penetration, 0.15f, Vector4(0.0f, 1.0f, 0.0f, 0.0f));
				}

				//Check to see if any of the objects have collision callbacks that dont want the objects to physically collide
				bool okA = cp.pObjectA->FireOnCollisionEvent(cp.pObjectA, cp.pObjectB);
				bool okB = cp.pObjectB->FireOnCollisionEvent(cp.pObjectB, cp.pObjectA);

				if (okA && okB)
				{
					cp.pObjectA->WakeUp();
					cp.pObjectB->WakeUp();

					if (cp.pObjectA->m_isScoreObj && cp.pObjectB->m_isScoreObj)
					{
						cp.pObjectA->AddScore(true);
						cp.pObjectB->AddScore(true);

						if ((cp.pObjectA->m_Position - cp.pObjectB->m_Position).LengthSquared() < 1.7)
						{
							if ((cp.pObjectA->m_Position - cp.pObjectB->m_Position).LengthSquared() < 1.0)
							{
								if ((cp.pObjectA->m_Position - cp.pObjectB->m_Position).LengthSquared() < 0.5)
								{
									if ((cp.pObjectA->m_Position - cp.pObjectB->m_Position).LengthSquared() < 0.1)
									{
										cp.pObjectA->SetScorePoint(100);
										cp.pObjectB->SetScorePoint(100);
									}
									else
									{
										cp.pObjectA->SetScorePoint(60);
										cp.pObjectB->SetScorePoint(60);
									}
								}
								else
								{
									cp.pObjectA->SetScorePoint(30);
									cp.pObjectB->SetScorePoint(30);
								}
							}
							else
							{
								cp.pObjectA->SetScorePoint(10);
								cp.pObjectB->SetScorePoint(10);
							}
						}
						else
						{
							cp.pObjectA->SetScorePoint(0);
							cp.pObjectB->SetScorePoint(0);
						}
					}
					else
					{
						cp.pObjectA->AddScore(false);
						cp.pObjectB->AddScore(false);
					}

					//trying to add some rocket
					/*if (cp.pObjectA->m_isExplosive)
					{
						cp.pObjectA->m_isDestroyed = true;
					}
					else if(cp.pObjectB->m_isExplosive)
					{
						cp.pObjectB->m_isDestroyed = true;
					}
					else
					{
						cp.pObjectA->m_isDestroyed = false;
						cp.pObjectB->m_isDestroyed = false;
					}*/

					//-- TUTORIAL 5 CODE --
					// Build full collision manifold that will also handle the collision response between the two objects in the solver stage
					Manifold* manifold = new Manifold();
					manifold->Initiate(cp.pObjectA, cp.pObjectB);

					// Construct contact points that form the perimeter of the collision manifold
					colDetect.GenContactPoints(manifold);

					// Add to list of manifolds that need solving
					m_vpManifolds.push_back(manifold);
				}
			}
		}
	}
}

void PhysicsEngine::DebugRender()
{
	// Draw all collision manifolds
	if (m_DebugDrawFlags & DEBUGDRAW_FLAGS_MANIFOLD)
	{
		for (Manifold* m : m_vpManifolds)
		{
			m->DebugDraw();
		}
	}

	// Draw all constraints
	if (m_DebugDrawFlags & DEBUGDRAW_FLAGS_CONSTRAINT)
	{
		for (Constraint* c : m_vpConstraints)
		{
			c->DebugDraw();
		}
	}

	// Draw all associated collision shapes
	if (m_DebugDrawFlags & DEBUGDRAW_FLAGS_COLLISIONVOLUMES)
	{
		for (PhysicsObject* obj : m_PhysicsObjects)
		{
			if (obj->GetCollisionShape() != NULL)
			{
				obj->GetCollisionShape()->DebugDraw(obj);
			}
		}
	}
}