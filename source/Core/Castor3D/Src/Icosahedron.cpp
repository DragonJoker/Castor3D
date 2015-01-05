#include "Icosahedron.hpp"
#include "Submesh.hpp"
#include "Vertex.hpp"
#include "Face.hpp"

#include <Angle.hpp>
#include <SphericalVertex.hpp>

using namespace Castor3D;
using namespace Castor;

Icosahedron::Icosahedron()
	:	MeshCategory( eMESH_TYPE_ICOSAHEDRON )
	,	m_radius( 0 )
	,	m_nbFaces( 0 )
{
}

Icosahedron::~Icosahedron()
{
}

MeshCategorySPtr Icosahedron::Create()
{
	return std::make_shared< Icosahedron >();
}

void Icosahedron::Generate()
{
	if ( m_radius < 0 )
	{
		m_radius = -m_radius;
	}

	SubmeshSPtr l_pSubmesh = GetMesh()->CreateSubmesh();
	// Dessin de l'icosaèdre
	real phi = ( 1.0f + sqrt( 5.0f ) ) / 2.0f;
	real X = m_radius / sqrt( phi * sqrt( 5.0f ) );
	real Z = X * phi;
	real u, v;
	FaceSPtr l_pFaces[20];
	int l_iIndex = 0;
	Point3r l_ptCoordsA;
	Point3r l_ptCoordsB;
	Point3r l_ptCoordsC;
	BufferElementGroupSPtr l_pVertex;
	Point3r l_ptNml;
	// on crée les 12 points le composant
	l_pVertex = l_pSubmesh->AddPoint( -X,  0,  Z );
	Vertex::SetNormal( l_pVertex, point::get_normalised( Vertex::GetPosition( l_pVertex, l_ptCoordsA ) ) );
	l_pVertex = l_pSubmesh->AddPoint( X,  0,  Z );
	Vertex::SetNormal( l_pVertex, point::get_normalised( Vertex::GetPosition( l_pVertex, l_ptCoordsA ) ) );
	l_pVertex = l_pSubmesh->AddPoint( -X,  0, -Z );
	Vertex::SetNormal( l_pVertex, point::get_normalised( Vertex::GetPosition( l_pVertex, l_ptCoordsA ) ) );
	l_pVertex = l_pSubmesh->AddPoint( X,  0, -Z );
	Vertex::SetNormal( l_pVertex, point::get_normalised( Vertex::GetPosition( l_pVertex, l_ptCoordsA ) ) );
	l_pVertex = l_pSubmesh->AddPoint( 0,  Z,  X );
	Vertex::SetNormal( l_pVertex, point::get_normalised( Vertex::GetPosition( l_pVertex, l_ptCoordsA ) ) );
	l_pVertex = l_pSubmesh->AddPoint( 0,  Z, -X );
	Vertex::SetNormal( l_pVertex, point::get_normalised( Vertex::GetPosition( l_pVertex, l_ptCoordsA ) ) );
	l_pVertex = l_pSubmesh->AddPoint( 0, -Z,  X );
	Vertex::SetNormal( l_pVertex, point::get_normalised( Vertex::GetPosition( l_pVertex, l_ptCoordsA ) ) );
	l_pVertex = l_pSubmesh->AddPoint( 0, -Z, -X );
	Vertex::SetNormal( l_pVertex, point::get_normalised( Vertex::GetPosition( l_pVertex, l_ptCoordsA ) ) );
	l_pVertex = l_pSubmesh->AddPoint( Z,  X,  0 );
	Vertex::SetNormal( l_pVertex, point::get_normalised( Vertex::GetPosition( l_pVertex, l_ptCoordsA ) ) );
	l_pVertex = l_pSubmesh->AddPoint( -Z,  X,  0 );
	Vertex::SetNormal( l_pVertex, point::get_normalised( Vertex::GetPosition( l_pVertex, l_ptCoordsA ) ) );
	l_pVertex = l_pSubmesh->AddPoint( Z, -X,  0 );
	Vertex::SetNormal( l_pVertex, point::get_normalised( Vertex::GetPosition( l_pVertex, l_ptCoordsA ) ) );
	l_pVertex = l_pSubmesh->AddPoint( -Z, -X,  0 );
	// on construit toutes les faces de l'isocaèdre
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 0,  1,  4 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 9,  0,  4 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 9,  4,  5 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 4,  8,  5 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 4,  1,  8 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 1, 10,  8 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 3,  8, 10 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 3,  5,  8 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 2,  5,  3 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 7,  2,  3 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 3, 10,  7 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 6,  7, 10 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 6, 11,  7 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 11,  6,  0 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 1,  0,  6 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 1,  6, 10 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 0,  9, 11 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 11,  9,  2 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 9,  5,  2 );
	l_pFaces[l_iIndex++] = l_pSubmesh->AddFace( 2,  7, 11 );

	for ( int i = 0; i < 20; i++ )
	{
		Vertex::GetPosition( l_pSubmesh->GetPoint( l_pFaces[i]->GetVertexIndex( 0 ) ), l_ptCoordsA );
		Vertex::GetPosition( l_pSubmesh->GetPoint( l_pFaces[i]->GetVertexIndex( 1 ) ), l_ptCoordsB );
		Vertex::GetPosition( l_pSubmesh->GetPoint( l_pFaces[i]->GetVertexIndex( 2 ) ), l_ptCoordsC );
		u = real( 0.5 ) * ( real( 1.0 ) + atan2( l_ptCoordsA[2], l_ptCoordsA[0] ) * ( 1 / Angle::Pi ) );
		v = acos( l_ptCoordsA[1] ) * ( 1 / Angle::Pi );
		Vertex::SetTexCoord( l_pSubmesh->GetPoint( l_pFaces[i]->GetVertexIndex( 0 ) ), u, v );
		u = real( 0.5 ) * ( real( 1.0 ) + atan2( l_ptCoordsB[2], l_ptCoordsB[0] ) * ( 1 / Angle::Pi ) );
		v = acos( l_ptCoordsB[1] ) * ( 1 / Angle::Pi );
		Vertex::SetTexCoord( l_pSubmesh->GetPoint( l_pFaces[i]->GetVertexIndex( 1 ) ), u, v );
		u = real( 0.5 ) * ( real( 1.0 ) + atan2( l_ptCoordsC[2], l_ptCoordsC[0] ) * ( 1 / Angle::Pi ) );
		v = acos( l_ptCoordsC[1] ) * ( 1 / Angle::Pi );
		Vertex::SetTexCoord( l_pSubmesh->GetPoint( l_pFaces[i]->GetVertexIndex( 2 ) ), u, v );
	}

	for ( VertexPtrArrayIt l_it = l_pSubmesh->VerticesBegin(); l_it != l_pSubmesh->VerticesEnd(); ++l_it )
	{
		Vertex::GetNormal( ( *l_it ), l_ptNml );
		SphericalVertex l_vsVertex1( l_ptNml );
		Vertex::SetTexCoord( ( *l_it ), l_vsVertex1.m_rPhi, l_vsVertex1.m_rTheta );
	}

	ComputeNormals( true );
	GetMesh()->ComputeContainers();
}

void Icosahedron::Initialise( UIntArray const & p_arrayFaces, RealArray const & p_arrayDimensions )
{
//	m_nbFaces = p_arrayFaces[0];
	m_radius = p_arrayDimensions[0];
	GetMesh()->Cleanup();
	Generate();
}
