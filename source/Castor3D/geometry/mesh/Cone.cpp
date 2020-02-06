#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/geometry/mesh/Cone.h"
#include "Castor3D/geometry/mesh/Submesh.h"
#include "Castor3D/geometry/basic/Face.h"
#include "Castor3D/geometry/basic/Vertex.h"
#include "Castor3D/main/Root.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/render_system/Buffer.h"


using namespace Castor::Math;
using namespace Castor3D;

Cone :: Cone( MeshManager * p_pManager, real p_rHheight, real p_rRadius, unsigned int p_uiNbFaces,
						const String & p_name)
	:	Mesh( p_pManager, p_name),
		m_nbFaces( p_uiNbFaces),
		m_height( p_rHheight),
		m_radius( p_rRadius)
{
	m_meshType = eCone;
	GeneratePoints();
}

Cone :: ~Cone()
{
}

void Cone :: GeneratePoints()
{
	real angleRotation = real( Angle::PiMult2) / m_nbFaces;

	SubmeshPtr l_submesh = CreateSubmesh( 2);

	unsigned int cpt = 0;
	//etape 1 : On calcule les coordonn�es des points
	for (real dAlphaI = 0 ; dAlphaI < Angle::PiMult2 && cpt < m_nbFaces ; dAlphaI += angleRotation)
	{
		l_submesh->AddPoint( m_radius * cos(dAlphaI), 0.0, m_radius * sin(dAlphaI));
		cpt++;
	}

	IdPoint3rPtr l_ptBottomCenter = l_submesh->AddPoint( 0.0, 0.0, 0.0);
	IdPoint3rPtr l_ptTopCenter = l_submesh->AddPoint( 0.0f, m_height, 0.0f);

	//etape 2 : on reconstitue les faces
	unsigned int i;
	size_t l_nbFaces = 0;
	real l_currentTexIndex = 1.0;
	real l_previousTexIndex = 1.0;

	if (m_height < 0)
	{
		m_height = -m_height;
	}

	const IdPoint3rPtrArray & l_vertex = l_submesh->GetPoints();
	//Composition des faces du bas
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		FacePtr l_faceA = l_submesh->AddFace( i + 1, i, l_ptBottomCenter->GetIndex(), 0);
		l_faceA->SetVertexTexCoords( 0, (1.0f + (*l_vertex[i + 1])[0] / m_radius) / 2.0f, (1.0f + (*l_vertex[i + 1])[2] / m_radius) / 2.0f);
		l_faceA->SetVertexTexCoords( 1, (1.0f + (*l_vertex[i])[0] / m_radius) / 2.0f, (1.0f + (*l_vertex[i])[2] / m_radius) / 2.0f);
		l_faceA->SetVertexTexCoords( 2, 0.5f, 0.5f);
		l_nbFaces += 1;
	}

	FacePtr l_faceA = l_submesh->AddFace( 0, m_nbFaces - 1, l_ptBottomCenter->GetIndex(), 0);
	l_faceA->SetVertexTexCoords( 0, (1.0f + (*l_vertex[0])[0] / m_radius) / 2.0f, (1.0f + (*l_vertex[0])[2] / m_radius) / 2.0f);
	l_faceA->SetVertexTexCoords( 1, (1.0f + (*l_vertex[m_nbFaces - 1])[0] / m_radius) / 2.0f, (1.0f + (*l_vertex[m_nbFaces - 1])[2] / m_radius) / 2.0f);
	l_faceA->SetVertexTexCoords( 2, 0.5, 0.5);
	l_nbFaces += 1;

	//Composition des faces des c�t�s
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		l_currentTexIndex -= 1.0f / ((real)m_nbFaces);
		FacePtr l_faceA = l_submesh->AddFace( i, i + 1, l_ptTopCenter->GetIndex(), 1);
		l_faceA->SetVertexTexCoords( 0, l_previousTexIndex, 0.0f);
		l_faceA->SetVertexTexCoords( 1, l_currentTexIndex, 0.0f);
		l_faceA->SetVertexTexCoords( 2, l_currentTexIndex, 1.0f);

		l_previousTexIndex = l_currentTexIndex;
		l_nbFaces += 1;
	}

	FacePtr l_faceC = l_submesh->AddFace( i, 0, l_ptTopCenter->GetIndex(), 1);
	l_faceC->SetVertexTexCoords( 0, l_currentTexIndex, 0.0);
	l_faceC->SetVertexTexCoords( 1, 0.0, 0.0);
	l_faceC->SetVertexTexCoords( 2, 0.0, 1.0);

	l_nbFaces += 1;

	l_submesh->GenerateBuffers();

	Logger::LogMessage( CU_T( "Cone - %s - Nb Vertex : %d, Nb Faces : %d"), GetName().char_str(), l_vertex.size(), l_nbFaces);
}

void Cone :: Modify( real p_radius, real p_height, unsigned int p_nbFaces)
{
	m_nbFaces = p_nbFaces;
	m_radius = p_radius;
	m_height = p_height;
	Cleanup();
	GeneratePoints();
}