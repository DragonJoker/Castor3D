#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Cone.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/Root.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Buffer.hpp"


using namespace Castor::Math;
using namespace Castor3D;

Cone :: Cone()
	:	MeshCategory( eMESH_TYPE_CONE)
	,	m_nbFaces( 0)
	,	m_height( 0)
	,	m_radius( 0)
{
}

Cone :: ~Cone()
{
}

MeshCategoryPtr Cone :: Clone()
{
	return MeshCategoryPtr( new Cone);
}

void Cone :: GeneratePoints()
{
	real angleRotation = real( Angle::PiMult2) / m_nbFaces;

	SubmeshPtr l_pSubmesh = GetMesh()->CreateSubmesh( 2);

	unsigned int cpt = 0;
	//etape 1 : On calcule les coordonnées des points
	for (real dAlphaI = 0 ; dAlphaI < Angle::PiMult2 && cpt < m_nbFaces ; dAlphaI += angleRotation)
	{
		l_pSubmesh->AddPoint( m_radius * cos(dAlphaI), 0.0, m_radius * sin(dAlphaI));
		cpt++;
	}

	IdPoint3rPtr l_ptBottomCenter = l_pSubmesh->AddPoint( 0.0, 0.0, 0.0);
	IdPoint3rPtr l_ptTopCenter = l_pSubmesh->AddPoint( 0.0f, m_height, 0.0f);

	//etape 2 : on reconstitue les faces
	unsigned int i;
	size_t l_nbFaces = 0;
	real l_currentTexIndex = 1.0;
	real l_previousTexIndex = 1.0;

	if (m_height < 0)
	{
		m_height = -m_height;
	}

	const IdPoint3rPtrArray & l_vertex = l_pSubmesh->GetPoints();
	//Composition des faces du bas
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		FacePtr l_faceA = l_pSubmesh->AddFace( i + 1, i, l_ptBottomCenter->GetIndex(), 0);
		l_faceA->SetVertexTexCoords( 0, (1.0f + (*l_vertex[i + 1])[0] / m_radius) / 2.0f, (1.0f + (*l_vertex[i + 1])[2] / m_radius) / 2.0f);
		l_faceA->SetVertexTexCoords( 1, (1.0f + (*l_vertex[i])[0] / m_radius) / 2.0f, (1.0f + (*l_vertex[i])[2] / m_radius) / 2.0f);
		l_faceA->SetVertexTexCoords( 2, 0.5f, 0.5f);
		l_nbFaces += 1;
	}

	FacePtr l_faceA = l_pSubmesh->AddFace( 0, m_nbFaces - 1, l_ptBottomCenter->GetIndex(), 0);
	l_faceA->SetVertexTexCoords( 0, (1.0f + (*l_vertex[0])[0] / m_radius) / 2.0f, (1.0f + (*l_vertex[0])[2] / m_radius) / 2.0f);
	l_faceA->SetVertexTexCoords( 1, (1.0f + (*l_vertex[m_nbFaces - 1])[0] / m_radius) / 2.0f, (1.0f + (*l_vertex[m_nbFaces - 1])[2] / m_radius) / 2.0f);
	l_faceA->SetVertexTexCoords( 2, 0.5, 0.5);
	l_nbFaces += 1;

	//Composition des faces des côtés
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		l_currentTexIndex -= 1.0f / ((real)m_nbFaces);
		FacePtr l_faceA = l_pSubmesh->AddFace( i, i + 1, l_ptTopCenter->GetIndex(), 1);
		l_faceA->SetVertexTexCoords( 0, l_previousTexIndex, 0.0f);
		l_faceA->SetVertexTexCoords( 1, l_currentTexIndex, 0.0f);
		l_faceA->SetVertexTexCoords( 2, l_currentTexIndex, 1.0f);

		l_previousTexIndex = l_currentTexIndex;
		l_nbFaces += 1;
	}

	FacePtr l_faceC = l_pSubmesh->AddFace( i, 0, l_ptTopCenter->GetIndex(), 1);
	l_faceC->SetVertexTexCoords( 0, l_currentTexIndex, 0.0);
	l_faceC->SetVertexTexCoords( 1, 0.0, 0.0);
	l_faceC->SetVertexTexCoords( 2, 0.0, 1.0);

	l_nbFaces += 1;

	l_pSubmesh->GenerateBuffers();

	Logger::LogMessage( cuT( "Cone - Nb Vertex : %d, Nb Faces : %d"), l_vertex.size(), l_nbFaces);
}

void Cone :: Initialise( const UIntArray & p_arrayFaces, const FloatArray & p_arrayDimensions)
{
	m_nbFaces = p_arrayFaces[0];
	m_radius = p_arrayDimensions[0];
	m_height = p_arrayDimensions[1];
	GetMesh()->Cleanup();
	GeneratePoints();
}