#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Cylinder.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Pattern.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/Root.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Buffer.hpp"

using namespace Castor3D;

Cylinder :: Cylinder()
	:	MeshCategory( eMESH_TYPE_CYLINDER)
	,	m_height( 0)
	,	m_radius( 0)
	,	m_nbFaces( 0)
{
}

Cylinder :: ~Cylinder()
{
}

MeshCategoryPtr Cylinder :: Clone()
{
	return MeshCategoryPtr( new Cylinder);
}

void Cylinder :: GeneratePoints()
{
	if (m_nbFaces < 2)
	{
		return;
	}

	SubmeshPtr l_submesh = GetMesh()->CreateSubmesh( 3);

	//CALCUL DE LA POSITION DES POINTS
	real angleRotation = Angle::PiMult2 / m_nbFaces;
	unsigned int i = 0;

	IdPoint3rPattern l_bottomArc;
	IdPoint3rPattern l_topArc;

	for (real dAlphaI = 0 ; dAlphaI < Angle::PiMult2 && i < m_nbFaces ; dAlphaI += angleRotation) 
	{
		IdPoint3rPtr l_vTmp = l_submesh->AddPoint( m_radius * cos(dAlphaI), 0.0, m_radius * sin(dAlphaI));
		l_bottomArc.AddElement( IdPoint3r( * l_vTmp), -1);

		IdPoint3rPtr l_vTmp2 = l_submesh->AddPoint( m_radius * cos(dAlphaI), m_height, m_radius * sin(dAlphaI));
		l_topArc.AddElement( IdPoint3r( * l_vTmp2), -1);
		i++;
	}

	IdPoint3rPtr l_ptTopCenter = l_submesh->AddPoint( 0.0, 0.0, 0.0);
	(*l_ptTopCenter)[1] += m_height;
	IdPoint3rPtr l_ptBottomCenter = l_submesh->AddPoint( 0.0, 0.0, 0.0);

	//RECONSTITION DES FACES
	if (m_height < 0)
	{
		m_height = -m_height;
	}

	//Composition des extrémités
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		//Composition du bas
		FacePtr l_faceA = l_submesh->AddFace( l_bottomArc[i + 1].GetIndex(), l_bottomArc[i].GetIndex(), l_ptBottomCenter->GetIndex(), 0);
		l_faceA->SetVertexTexCoords( 0, (1.0f + l_bottomArc[i + 1][0] / m_radius) / 2.0f,	(1.0f + l_bottomArc[i + 1][2] / m_radius) / 2.0f);
		l_faceA->SetVertexTexCoords( 1, (1.0f + l_bottomArc[i][0] / m_radius) / 2.0f,		(1.0f + l_bottomArc[i][2] / m_radius) / 2.0f);
		l_faceA->SetVertexTexCoords( 2, 0.5f,												0.5f);
		//Composition du dessus
		FacePtr l_faceB = l_submesh->AddFace( l_ptTopCenter->GetIndex(), l_topArc[i].GetIndex(), l_topArc[i + 1].GetIndex(), 2);
		l_faceB->SetVertexTexCoords( 0, 0.5,													0.5);
		l_faceB->SetVertexTexCoords( 1, 1.0f - (1.0f + l_topArc[i][0] / m_radius) / 2.0f,		(1.0f + l_topArc[i][2] / m_radius) / 2.0f);
		l_faceB->SetVertexTexCoords( 2, 1.0f - (1.0f + l_topArc[i + 1][0] / m_radius) / 2.0f,	(1.0f + l_topArc[i + 1][2] / m_radius) / 2.0f);
	}

	//Composition du bas
	FacePtr l_faceA = l_submesh->AddFace( l_bottomArc[0].GetIndex(), l_bottomArc[m_nbFaces - 1].GetIndex(), l_ptBottomCenter->GetIndex(), 0);
	l_faceA->SetVertexTexCoords( 0, (1.0f + l_bottomArc[0][0] / m_radius) / 2.0f,				(1.0f + l_bottomArc[0][2] / m_radius) / 2.0f);
	l_faceA->SetVertexTexCoords( 1, (1.0f + l_bottomArc[m_nbFaces - 1][0] / m_radius) / 2.0f,	(1.0f + l_bottomArc[m_nbFaces - 1][2] / m_radius) / 2.0f);
	l_faceA->SetVertexTexCoords( 2, 0.5,														 0.5);
	//Composition du dessus
	FacePtr l_faceB = l_submesh->AddFace( l_ptTopCenter->GetIndex(), l_topArc[m_nbFaces - 1].GetIndex(), l_topArc[0].GetIndex(), 2);
	l_faceB->SetVertexTexCoords( 0, 0.5,															0.5);
	l_faceB->SetVertexTexCoords( 1, 1.0f - (1.0f + l_topArc[m_nbFaces - 1][0] / m_radius) / 2.0f,	(1.0f + l_topArc[m_nbFaces - 1][2] / m_radius) / 2.0f);
	l_faceB->SetVertexTexCoords( 2, 1.0f - (1.0f + l_topArc[0][0] / m_radius) / 2.0f,				(1.0f + l_topArc[0][2] / m_radius) / 2.0f);

	//Composition des côtés
	for (i = 0 ; i < m_nbFaces - 1 ; i++)
	{
		l_submesh->AddQuadFace( l_bottomArc[i].GetIndex(), l_bottomArc[i + 1].GetIndex(), l_topArc[i + 1].GetIndex(), l_topArc[i].GetIndex(),
								1, Point3r( 1.0f - real( i) / real( m_nbFaces), 0.0f, 0.0f), Point3r( 1.0f - real( i + 1) / real( m_nbFaces), 1.0f, 0.0f));
	}

	l_submesh->AddQuadFace( l_bottomArc[i].GetIndex(), l_bottomArc[0].GetIndex(), l_topArc[0].GetIndex(), l_topArc[i].GetIndex(), 
							1, Point3r( 1.0f - real( i) / real( m_nbFaces), 0.0f, 0.0f), Point3r( 0.0, 1.0, 0.0));

	l_submesh->GenerateBuffers();

//	delete l_bottomArc;
//	delete l_topArc;
	Logger::LogMessage( cuT( "Cylindre - NbVertex : %d"), l_submesh->GetNbPoints());
}

void Cylinder :: Initialise( const UIntArray & p_arrayFaces, const FloatArray & p_arrayDimensions)
{
	m_nbFaces = p_arrayFaces[0];
	m_radius = p_arrayDimensions[0];
	m_height = p_arrayDimensions[1];
	GetMesh()->Cleanup();
	GeneratePoints();
}