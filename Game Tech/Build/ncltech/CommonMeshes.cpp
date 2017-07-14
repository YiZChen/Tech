#include "CommonMeshes.h"
#include <nclgl\OBJMesh.h>

Mesh* CommonMeshes::m_pPlane	= NULL;
Mesh* CommonMeshes::m_pCube		= NULL;
Mesh* CommonMeshes::m_pSphere	= NULL;
Mesh* CommonMeshes::m_Planet	= NULL;
Mesh* CommonMeshes::m_Rocket	= NULL;
Mesh* CommonMeshes::m_StarDestroyer = NULL;

GLuint CommonMeshes::m_SubPlanetTex = 0;
GLuint CommonMeshes::m_MainPlanetTex = 0;
GLuint CommonMeshes::m_TargetTex = 0;
GLuint CommonMeshes::m_CubeUnwakeTex = 0;
GLuint CommonMeshes::m_SphereUnWakeTex = 0;
GLuint CommonMeshes::m_RocketTex = 0;
GLuint CommonMeshes::m_ShieldTex = 0;
GLuint CommonMeshes::m_MainPlanetBumpTex = 0;

GLuint CommonMeshes::m_ballTex1 = 0;
GLuint CommonMeshes::m_ballTex2 = 0;
GLuint CommonMeshes::m_ballTex3 = 0;
GLuint CommonMeshes::m_ballTex4 = 0;
GLuint CommonMeshes::m_ballTex5 = 0;
GLuint CommonMeshes::m_ballTex6 = 0;

GLuint CommonMeshes::gTex[6] = {0};
GLuint CommonMeshes::m_Hull = 0;
GLuint CommonMeshes::m_CageTex = 0;
GLuint CommonMeshes::m_SpringSphere = 0;

void CommonMeshes::InitializeMeshes()
{
	if (m_pPlane == NULL)
	{
		//Set texture
		m_SubPlanetTex = SOIL_load_OGL_texture(TEXTUREDIR"Planet_Sub.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		m_MainPlanetTex = SOIL_load_OGL_texture(TEXTUREDIR"DeathStar.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
		m_MainPlanetBumpTex = SOIL_load_OGL_texture(TEXTUREDIR"DeathStarNormalMap.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
		m_Hull = SOIL_load_OGL_texture(TEXTUREDIR"Shield.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);

		m_CubeUnwakeTex = SOIL_load_OGL_texture(TEXTUREDIR"IdleCube.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
		m_SphereUnWakeTex = SOIL_load_OGL_texture(TEXTUREDIR"Idlesphere.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
		m_RocketTex = SOIL_load_OGL_texture(TEXTUREDIR"missle.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
		m_ShieldTex = m_TargetTex = SOIL_load_OGL_texture(TEXTUREDIR"Shield.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);

		m_TargetTex = SOIL_load_OGL_texture(TEXTUREDIR"TargetEarth.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
		m_CageTex = SOIL_load_OGL_texture(TEXTUREDIR"cage.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);

		m_SpringSphere = SOIL_load_OGL_texture(TEXTUREDIR"SpringSphere.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);

		m_ballTex1 = SOIL_load_OGL_texture(TEXTUREDIR"Ball1.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
		m_ballTex2 = SOIL_load_OGL_texture(TEXTUREDIR"Ball2.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
		m_ballTex3 = SOIL_load_OGL_texture(TEXTUREDIR"Ball3.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
		m_ballTex4 = SOIL_load_OGL_texture(TEXTUREDIR"Ball4.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
		m_ballTex5 = SOIL_load_OGL_texture(TEXTUREDIR"Ball5.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
		m_ballTex6 = SOIL_load_OGL_texture(TEXTUREDIR"Ball6.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
		
		gTex[0] = m_ballTex1;
		gTex[1] = m_ballTex2;
		gTex[2] = m_ballTex3;
		gTex[3] = m_ballTex4;
		gTex[4] = m_ballTex5;
		gTex[5] = m_ballTex6;

		m_pPlane = Mesh::GenerateQuadTexCoordCol(Vector2(1.f, 1.f), Vector2(0.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pCube = new OBJMesh(MESHDIR"cube.obj");
		m_pSphere = new OBJMesh(MESHDIR"sphere.obj");
		m_Planet = new OBJMesh(MESHDIR"sphere.obj");
		m_Rocket = new OBJMesh(MESHDIR"rocket.obj");
		m_StarDestroyer = new OBJMesh(MESHDIR"OutPost.obj");
	}
}

void CommonMeshes::ReleaseMeshes()
{
	if (m_pPlane != NULL)
	{
		glDeleteTextures(1, &m_SubPlanetTex);
		delete m_pPlane;
		delete m_pCube;
		delete m_pSphere;
		delete m_Planet;
		delete m_Rocket;
	}

	m_pPlane = NULL;
}