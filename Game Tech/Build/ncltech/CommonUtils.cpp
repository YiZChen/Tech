#include "CommonUtils.h"

#include "ObjectMesh.h"
#include "ObjectMeshDragable.h"
#include "SphereCollisionShape.h"
#include "CuboidCollisionShape.h"
#include "CommonMeshes.h"

Vector4 CommonUtils::GenColour(float scalar, float alpha)
{
	Vector4 c;
	c.w = alpha;

	float t;
	c.x = abs(modf(scalar + 1.0f, &t) * 6.0f - 3.0f) - 1.0f;
	c.y = abs(modf(scalar + 2.0f / 3.0f, &t) * 6.0f - 3.0f) - 1.0f;
	c.z = abs(modf(scalar + 1.0f / 3.0f, &t) * 6.0f - 3.0f) - 1.0f;

	c.x = min(max(c.x, 0.0f), 1.0f);
	c.y = min(max(c.y, 0.0f), 1.0f);
	c.z = min(max(c.z, 0.0f), 1.0f);

	c.x = c.y = c.z = scalar;

	return c;
}

Object* CommonUtils::BuildSphereObject(
	const std::string& name,
	const Vector3& pos,
	float radius,
	bool physics_enabled,
	float inverse_mass,
	bool collidable,
	bool dragable,
	const Vector4& color)
{
	ObjectMesh* pSphere = dragable
		? new ObjectMeshDragable(name)
		: new ObjectMesh(name);

	pSphere->SetMesh(CommonMeshes::Sphere(), false);
	pSphere->SetTexture(CommonMeshes::Planet_SubTex(), false);
	//pSphere->SetTextureBump(x);
	pSphere->SetLocalTransform(Matrix4::Scale(Vector3(radius, radius, radius)));
	pSphere->SetColour(color);
	pSphere->SetBoundingRadius(radius);

	if (!physics_enabled)
	{
		//If no physics object is present, just set the local transform (modelMatrix) directly
		pSphere->SetLocalTransform(Matrix4::Translation(pos) * pSphere->GetLocalTransform());
	}
	else
	{
		//Otherwise create a physics object, and set it's position etc
		pSphere->CreatePhysicsNode();

		pSphere->Physics()->SetPosition(pos);
		pSphere->Physics()->SetInverseMass(inverse_mass);
		
		if (!collidable)
		{
			//Even without a collision shape, the inertia matrix for rotation has to be derived from the objects shape
			pSphere->Physics()->SetInverseInertia(SphereCollisionShape(radius).BuildInverseInertia(inverse_mass));
		}
		else
		{
			CollisionShape* pColshape = new SphereCollisionShape(radius);
			pSphere->Physics()->SetCollisionShape(pColshape);
			pSphere->Physics()->SetInverseInertia(pColshape->BuildInverseInertia(inverse_mass));
		}			
	}

	return pSphere;
}

Object* CommonUtils::BuildIdleSphereObject(
	const std::string& name,
	const Vector3& pos,
	float radius,
	bool physics_enabled,
	float inverse_mass,
	bool collidable,
	bool dragable,
	const Vector4& color)
{
	ObjectMesh* pSphere = dragable
		? new ObjectMeshDragable(name)
		: new ObjectMesh(name);

	pSphere->SetMesh(CommonMeshes::Sphere(), false);
	pSphere->SetTexture(CommonMeshes::SphereTex(), false);
	pSphere->SetLocalTransform(Matrix4::Scale(Vector3(radius, radius, radius)));
	pSphere->SetColour(color);
	pSphere->SetBoundingRadius(radius);

	if (!physics_enabled)
	{
		//If no physics object is present, just set the local transform (modelMatrix) directly
		pSphere->SetLocalTransform(Matrix4::Translation(pos) * pSphere->GetLocalTransform());
	}
	else
	{
		//Otherwise create a physics object, and set it's position etc
		pSphere->CreatePhysicsNode();

		pSphere->Physics()->SetPosition(pos);
		pSphere->Physics()->SetInverseMass(inverse_mass);

		if (!collidable)
		{
			//Even without a collision shape, the inertia matrix for rotation has to be derived from the objects shape
			pSphere->Physics()->SetInverseInertia(SphereCollisionShape(radius).BuildInverseInertia(inverse_mass));
		}
		else
		{
			CollisionShape* pColshape = new SphereCollisionShape(radius);
			pSphere->Physics()->SetCollisionShape(pColshape);
			pSphere->Physics()->SetInverseInertia(pColshape->BuildInverseInertia(inverse_mass));
		}
	}

	return pSphere;
}

Object* CommonUtils::BuildShieldObject(
	const std::string& name,
	const Vector3& pos,
	float radius,
	bool physics_enabled,
	float inverse_mass,
	bool collidable,
	bool dragable,
	const Vector4& color)
{
	ObjectMesh* pSphere = dragable
		? new ObjectMeshDragable(name)
		: new ObjectMesh(name);

	pSphere->SetMesh(CommonMeshes::Sphere(), false);
	pSphere->SetTexture(CommonMeshes::SheildTex(), false);
	pSphere->SetLocalTransform(Matrix4::Scale(Vector3(radius, radius, radius)));
	pSphere->SetColour(color);
	pSphere->SetBoundingRadius(radius);

	if (!physics_enabled)
	{
		//If no physics object is present, just set the local transform (modelMatrix) directly
		pSphere->SetLocalTransform(Matrix4::Translation(pos) * pSphere->GetLocalTransform());
	}
	else
	{
		//Otherwise create a physics object, and set it's position etc
		pSphere->CreatePhysicsNode();

		pSphere->Physics()->SetPosition(pos);
		pSphere->Physics()->SetInverseMass(inverse_mass);

		if (!collidable)
		{
			//Even without a collision shape, the inertia matrix for rotation has to be derived from the objects shape
			pSphere->Physics()->SetInverseInertia(SphereCollisionShape(radius).BuildInverseInertia(inverse_mass));
		}
		else
		{
			CollisionShape* pColshape = new SphereCollisionShape(radius);
			pSphere->Physics()->SetCollisionShape(pColshape);
			pSphere->Physics()->SetInverseInertia(pColshape->BuildInverseInertia(inverse_mass));
		}
	}

	return pSphere;
}

Object* CommonUtils::BuildCuboidObject(
	const std::string& name,
	const Vector3& pos,
	const Vector3& halfdims,
	bool physics_enabled,
	float inverse_mass,
	bool collidable,
	bool dragable,
	const Vector4& color)
{
	ObjectMesh* pCuboid = dragable
		? new ObjectMeshDragable(name)
		: new ObjectMesh(name);

	pCuboid->SetMesh(CommonMeshes::Cube(), false);
	pCuboid->SetTexture(CommonMeshes::TargetBoardTex(), false);
	pCuboid->SetLocalTransform(Matrix4::Scale(halfdims));
	pCuboid->SetColour(color);
	pCuboid->SetBoundingRadius(halfdims.Length());

	if (!physics_enabled)
	{
		//If no physics object is present, just set the local transform (modelMatrix) directly
		pCuboid->SetLocalTransform(Matrix4::Translation(pos) * pCuboid->GetLocalTransform());
	}
	else
	{
		//Otherwise create a physics object, and set it's position etc
		pCuboid->CreatePhysicsNode();
		pCuboid->Physics()->SetPosition(pos);
		pCuboid->Physics()->SetInverseMass(inverse_mass);

		if (!collidable)
		{
			//Even without a collision shape, the inertia matrix for rotation has to be derived from the objects shape
			pCuboid->Physics()->SetInverseInertia(CuboidCollisionShape(halfdims).BuildInverseInertia(inverse_mass));
		}
		else
		{
			CollisionShape* pColshape = new CuboidCollisionShape(halfdims);
			pCuboid->Physics()->SetCollisionShape(pColshape);
			pCuboid->Physics()->SetInverseInertia(pColshape->BuildInverseInertia(inverse_mass));
		}
	}

	return pCuboid;
}

Object* CommonUtils::BuildIdleCuboidObject(
	const std::string& name,
	const Vector3& pos,
	const Vector3& halfdims,
	bool physics_enabled,
	float inverse_mass,
	bool collidable,
	bool dragable,
	const Vector4& color)
{
	ObjectMesh* pCuboid = dragable
		? new ObjectMeshDragable(name)
		: new ObjectMesh(name);

	pCuboid->SetMesh(CommonMeshes::Cube(), false);
	pCuboid->SetTexture(CommonMeshes::CubeTex(), false);
	pCuboid->SetLocalTransform(Matrix4::Scale(halfdims));
	pCuboid->SetColour(color);
	pCuboid->SetBoundingRadius(halfdims.Length());

	if (!physics_enabled)
	{
		//If no physics object is present, just set the local transform (modelMatrix) directly
		pCuboid->SetLocalTransform(Matrix4::Translation(pos) * pCuboid->GetLocalTransform());
	}
	else
	{
		//Otherwise create a physics object, and set it's position etc
		pCuboid->CreatePhysicsNode();
		pCuboid->Physics()->SetPosition(pos);
		pCuboid->Physics()->SetInverseMass(inverse_mass);

		if (!collidable)
		{
			//Even without a collision shape, the inertia matrix for rotation has to be derived from the objects shape
			pCuboid->Physics()->SetInverseInertia(CuboidCollisionShape(halfdims).BuildInverseInertia(inverse_mass));
		}
		else
		{
			CollisionShape* pColshape = new CuboidCollisionShape(halfdims);
			pCuboid->Physics()->SetCollisionShape(pColshape);
			pCuboid->Physics()->SetInverseInertia(pColshape->BuildInverseInertia(inverse_mass));
		}
	}

	return pCuboid;
}

Object* CommonUtils::BuildPlanetObject(
	const std::string& name,
	const Vector3& pos,
	float radius,
	bool physics_enabled,
	float inverse_mass,
	bool collidable,
	bool dragable,
	const Vector4& color)
{
	ObjectMesh* Planet = dragable
		? new ObjectMeshDragable(name)
		: new ObjectMesh(name);

	Planet->SetMesh(CommonMeshes::Planet(), false);
	Planet->SetTexture(CommonMeshes::Planet_MainTex(), false);
	Planet->SetLocalTransform(Matrix4::Scale(Vector3(radius, radius, radius)));
	Planet->SetColour(color);
	Planet->SetBoundingRadius(radius);

	if (!physics_enabled)
	{
		//If no physics object is present, just set the local transform (modelMatrix) directly
		Planet->SetLocalTransform(Matrix4::Translation(pos) * Planet->GetLocalTransform());
	}
	else
	{
		//Otherwise create a physics object, and set it's position etc
		Planet->CreatePhysicsNode();

		Planet->Physics()->SetPosition(pos);
		Planet->Physics()->SetInverseMass(inverse_mass);

		if (!collidable)
		{
			//Even without a collision shape, the inertia matrix for rotation has to be derived from the objects shape
			Planet->Physics()->SetInverseInertia(SphereCollisionShape(radius).BuildInverseInertia(inverse_mass));
		}
		else
		{
			CollisionShape* pColshape = new SphereCollisionShape(radius);
			Planet->Physics()->SetCollisionShape(pColshape);
			Planet->Physics()->SetInverseInertia(pColshape->BuildInverseInertia(inverse_mass));
		}
	}


	return Planet;
}

Object* CommonUtils::BuildBallObject(
	const std::string& name,
	const Vector3& pos,
	float radius,
	bool physics_enabled,
	float inverse_mass,
	bool collidable,
	bool dragable,
	const Vector4& color)
{
	ObjectMesh* bullet = dragable
		? new ObjectMeshDragable(name)
		: new ObjectMesh(name);

	bullet->SetMesh(CommonMeshes::Sphere(), false);
	bullet->SetTexture(CommonMeshes::FireBallTex(rand() % 6), false);
	bullet->SetLocalTransform(Matrix4::Scale(Vector3(radius, radius, radius)));
	bullet->SetColour(color);
	bullet->SetBoundingRadius(radius);

	if (!physics_enabled)
	{
		//If no physics object is present, just set the local transform (modelMatrix) directly
		bullet->SetLocalTransform(Matrix4::Translation(pos) * bullet->GetLocalTransform());
	}
	else
	{
		//Otherwise create a physics object, and set it's position etc
		bullet->CreatePhysicsNode();

		bullet->Physics()->SetPosition(pos);
		bullet->Physics()->SetInverseMass(inverse_mass);

		if (!collidable)
		{
			//Even without a collision shape, the inertia matrix for rotation has to be derived from the objects shape
			bullet->Physics()->SetInverseInertia(SphereCollisionShape(radius).BuildInverseInertia(inverse_mass));
		}
		else
		{
			CollisionShape* pColshape = new SphereCollisionShape(radius);
			bullet->Physics()->SetCollisionShape(pColshape);
			bullet->Physics()->SetInverseInertia(pColshape->BuildInverseInertia(inverse_mass));
		}
	}

	return bullet;
}

Object* CommonUtils::BuildRocketObject(
	const std::string& name,
	const Vector3& pos,
	const Vector3& halfdims,
	bool physics_enabled,
	float inverse_mass,
	bool collidable,
	bool dragable,
	const Vector4& color)
{
	ObjectMesh* rocket = dragable
		? new ObjectMeshDragable(name)
		: new ObjectMesh(name);

	rocket->SetMesh(CommonMeshes::Rocket(), false);
	rocket->SetTexture(CommonMeshes::RocketTex(), false);
	rocket->SetLocalTransform(Matrix4::Scale(halfdims));
	rocket->SetColour(color);
	rocket->SetBoundingRadius(halfdims.Length());

	if (!physics_enabled)
	{
		//If no physics object is present, just set the local transform (modelMatrix) directly
		rocket->SetLocalTransform(Matrix4::Translation(pos) * rocket->GetLocalTransform());
	}
	else
	{
		//Otherwise create a physics object, and set it's position etc
		rocket->CreatePhysicsNode();

		rocket->Physics()->SetPosition(pos);
		rocket->Physics()->SetInverseMass(inverse_mass);

		if (!collidable)
		{
			//Even without a collision shape, the inertia matrix for rotation has to be derived from the objects shape
			rocket->Physics()->SetInverseInertia(CuboidCollisionShape(halfdims).BuildInverseInertia(inverse_mass));
		}
		else
		{
			CollisionShape* pColshape = new CuboidCollisionShape(halfdims);
			rocket->Physics()->SetCollisionShape(pColshape);
			rocket->Physics()->SetInverseInertia(pColshape->BuildInverseInertia(inverse_mass));
		}
	}

	return rocket;
}

Object* CommonUtils::BuildBattleShip(
	const std::string& name,
	const Vector3& pos,
	const Vector3& halfdims,
	bool physics_enabled,
	float inverse_mass,
	bool collidable,
	bool dragable,
	const Vector4& color)
{
	ObjectMesh* BattleShip = dragable
		? new ObjectMeshDragable(name)
		: new ObjectMesh(name);

	BattleShip->SetMesh(CommonMeshes::BattleShip(), false);
	BattleShip->SetTexture(CommonMeshes::HullTex(), false);
	BattleShip->SetLocalTransform(Matrix4::Scale(halfdims));
	BattleShip->SetColour(color);
	BattleShip->SetBoundingRadius(halfdims.Length());

	if (!physics_enabled)
	{
		//If no physics object is present, just set the local transform (modelMatrix) directly
		BattleShip->SetLocalTransform(Matrix4::Translation(pos) * BattleShip->GetLocalTransform());
	}
	else
	{
		//Otherwise create a physics object, and set it's position etc
		BattleShip->CreatePhysicsNode();

		BattleShip->Physics()->SetPosition(pos);
		BattleShip->Physics()->SetInverseMass(inverse_mass);

		if (!collidable)
		{
			//Even without a collision shape, the inertia matrix for rotation has to be derived from the objects shape
			BattleShip->Physics()->SetInverseInertia(CuboidCollisionShape(halfdims).BuildInverseInertia(inverse_mass));
		}
		else
		{
			CollisionShape* pColshape = new CuboidCollisionShape(halfdims);
			BattleShip->Physics()->SetCollisionShape(pColshape);
			BattleShip->Physics()->SetInverseInertia(pColshape->BuildInverseInertia(inverse_mass));
		}
	}

	return BattleShip;
}

Object* CommonUtils::BuildQuadObject(
	const std::string& name,
	const Vector3& pos,
	const Vector3& halfdims,
	bool physics_enabled,
	float inverse_mass,
	bool collidable,
	bool dragable,
	const Vector4& color)
{
	ObjectMesh* quad = dragable
		? new ObjectMeshDragable(name)
		: new ObjectMesh(name);

	quad->SetMesh(CommonMeshes::Plane(), false);
	quad->SetTexture(CommonMeshes::CageTex(), false);
	quad->SetLocalTransform(Matrix4::Scale(halfdims * 2.0f));
	quad->SetLocalTransform(Matrix4::Translation(
		Vector3(-1.0f * halfdims.x + halfdims.x * 0.01f, -1.0f * halfdims.y + halfdims.y * 0.01f, halfdims.z)
		) * quad->GetLocalTransform());
	quad->SetColour(color);
	quad->SetBoundingRadius(halfdims.Length());

	if (!physics_enabled)
	{
		//If no physics object is present, just set the local transform (modelMatrix) directly
		quad->SetLocalTransform(Matrix4::Translation(pos) * quad->GetLocalTransform());
	}
	else
	{
		//Otherwise create a physics object, and set it's position etc
		quad->CreatePhysicsNode();

		quad->Physics()->SetPosition(pos);
		quad->Physics()->SetInverseMass(inverse_mass);

		if (!collidable)
		{
			//Even without a collision shape, the inertia matrix for rotation has to be derived from the objects shape
			quad->Physics()->SetInverseInertia(CuboidCollisionShape(halfdims).BuildInverseInertia(inverse_mass));
		}
		else
		{
			CollisionShape* pColshape = new CuboidCollisionShape(halfdims);
			quad->Physics()->SetCollisionShape(pColshape);
			quad->Physics()->SetInverseInertia(pColshape->BuildInverseInertia(inverse_mass));
		}
	}

	return quad;
}

Object* CommonUtils::BuildSpringSphereObject(
	const std::string& name,
	const Vector3& pos,
	float radius,
	bool physics_enabled,
	float inverse_mass,
	bool collidable,
	bool dragable,
	const Vector4& color)
{
	ObjectMesh* bullet = dragable
		? new ObjectMeshDragable(name)
		: new ObjectMesh(name);

	bullet->SetMesh(CommonMeshes::Sphere(), false);
	bullet->SetTexture(CommonMeshes::SpringSphereTex(), false);
	bullet->SetLocalTransform(Matrix4::Scale(Vector3(radius, radius, radius)));
	bullet->SetColour(color);
	bullet->SetBoundingRadius(radius);

	if (!physics_enabled)
	{
		//If no physics object is present, just set the local transform (modelMatrix) directly
		bullet->SetLocalTransform(Matrix4::Translation(pos) * bullet->GetLocalTransform());
	}
	else
	{
		//Otherwise create a physics object, and set it's position etc
		bullet->CreatePhysicsNode();

		bullet->Physics()->SetPosition(pos);
		bullet->Physics()->SetInverseMass(inverse_mass);

		if (!collidable)
		{
			//Even without a collision shape, the inertia matrix for rotation has to be derived from the objects shape
			bullet->Physics()->SetInverseInertia(SphereCollisionShape(radius).BuildInverseInertia(inverse_mass));
		}
		else
		{
			CollisionShape* pColshape = new SphereCollisionShape(radius);
			bullet->Physics()->SetCollisionShape(pColshape);
			bullet->Physics()->SetInverseInertia(pColshape->BuildInverseInertia(inverse_mass));
		}
	}

	return bullet;
}