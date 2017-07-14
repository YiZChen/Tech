#include "ObjectMesh.h"

ObjectMesh::ObjectMesh(const std::string& name)
	: Object(name)
	, m_DeleteMeshOnCleanup(false)
	, m_DeleteTexOnCleanup(false)
	, m_DeleteTexOnCleanupBump(false)
	, m_pMesh(NULL)
	, m_Texture(0)
	, m_TextureBump(0)
{
}

ObjectMesh::~ObjectMesh()
{
	if (m_DeleteMeshOnCleanup && m_pMesh)
	{
		delete m_pMesh;
		m_pMesh = NULL;
	}

	if (m_DeleteTexOnCleanup && m_Texture)
	{
		glDeleteTextures(1, &m_Texture);
		m_Texture = 0;
	}

	if (m_DeleteTexOnCleanupBump && m_TextureBump)
	{
		glDeleteTextures(1, &m_TextureBump);
		m_TextureBump = 0;
	}
}

void ObjectMesh::SetMesh(Mesh* mesh, bool deleteOnCleanup)
{
	m_pMesh = mesh;
	m_DeleteMeshOnCleanup = deleteOnCleanup;

	if (!m_Texture)
	{
		m_Texture = m_pMesh->GetTexture();
	}
}

void ObjectMesh::SetTexture(GLuint tex, bool deleteOnCleanup)
{
	m_Texture = tex;
	m_DeleteTexOnCleanup = deleteOnCleanup;
}
void ObjectMesh::SetTextureBump(GLuint tex, bool deleteOnCleanup)
{
	m_TextureBump = tex;
	m_DeleteTexOnCleanupBump = deleteOnCleanup;
}

void ObjectMesh::OnRenderObject()
{
	if (m_Texture)
	{
		m_pMesh->SetTexture(m_Texture);
	}
	if (m_TextureBump)
	{
		m_pMesh->SetBumpMap(m_TextureBump);
	}
	else
	{
		m_pMesh->SetBumpMap(0);
	}

	m_pMesh->Draw();
}