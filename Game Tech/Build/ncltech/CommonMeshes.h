/******************************************************************************
Class: CommonMeshes
Implements:
Author: Pieran Marris <p.marris@newcastle.ac.uk>
Description: 
A quick and dirty library of common meshes, to save loading the same common meshes over and over again.

These are loaded when the scene is first initialised and released when it is deleted, so will 
be globally availible for the entirity of the program. 

If they are being used within an ObjectMesh instance, remember to set the 'deleteOnCleanup' flag to 
false in order to prevent them being deleted when the ObjectMesh instance is deleted.


		(\_/)
		( '_')
	 /""""""""""""\=========     -----D
	/"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*//////////////////////////////////////////////////////////////////////////////

#pragma once
#include <nclgl\Mesh.h>
#include <stdio.h>
#include <sstream>

class Scene;

class CommonMeshes
{
	friend class SceneRenderer; //Initializes/Destroys the given meshes within it's own lifecycle

public:

	//Quad
	static Mesh* Plane()			{ return m_pPlane; }

	//Cube
	static Mesh* Cube()				{ return m_pCube; }

	//Sphere
	static Mesh* Sphere()			{ return m_pSphere; }

	//Planet
	static Mesh* Planet()			{ return m_Planet; }

	//Rocket
	static Mesh* Rocket()			{ return m_Rocket; }

	//Star Destroyer
	static Mesh* BattleShip() { return m_StarDestroyer; }

	
	//PhysicsEngine Checkerboard - Hidden here for reasons of laziness
	static GLuint Planet_SubTex() { return m_SubPlanetTex; }

	static GLuint Planet_MainTex() { return m_MainPlanetTex; }

	static GLuint Planet_MainBumpTex() { return m_MainPlanetBumpTex; }

	static GLuint FireBallTex(int n) { return gTex[n]; }

	static GLuint TargetBoardTex() { return m_TargetTex; }

	static GLuint CubeTex() { return m_CubeUnwakeTex; }

	static GLuint SphereTex() { return m_SphereUnWakeTex; }

	static GLuint RocketTex() { return m_RocketTex; }

	static GLuint SheildTex() { return m_ShieldTex; }

	static GLuint HullTex() { return m_Hull; }

	static GLuint CageTex() { return m_CageTex; }

	static GLuint SpringSphereTex() { return m_SpringSphere; }

protected:
	//Called by SceneRenderer
	static void InitializeMeshes();
	static void ReleaseMeshes();

protected:
	static Mesh* m_pCube;
	static Mesh* m_pSphere;
	static Mesh* m_pPlane;
	static Mesh* m_Planet;
	static Mesh* m_Rocket;
	static Mesh* m_StarDestroyer;

	static GLuint m_SubPlanetTex;
	static GLuint m_MainPlanetTex;
	static GLuint m_TargetTex;
	static GLuint m_MainPlanetBumpTex;
	
	static GLuint m_ballTex1;
	static GLuint m_ballTex2;
	static GLuint m_ballTex3;
	static GLuint m_ballTex4;
	static GLuint m_ballTex5;
	static GLuint m_ballTex6;

	static GLuint gTex[6];

	static GLuint m_CubeUnwakeTex;
	static GLuint m_SphereUnWakeTex;
	static GLuint m_RocketTex;
	static GLuint m_ShieldTex;
	static GLuint m_Hull;

	static GLuint m_CageTex;
	static GLuint m_SpringSphere;
};