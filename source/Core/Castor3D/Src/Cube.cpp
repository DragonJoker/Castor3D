#include "Cube.hpp"
#include "Submesh.hpp"
#include "Vertex.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	Cube::Cube()
		:	MeshCategory( eMESH_TYPE_CUBE )
		,	m_width( 0 )
		,	m_height( 0 )
		,	m_depth( 0 )
	{
	}

	Cube::~Cube()
	{
	}

	MeshCategorySPtr Cube::Create()
	{
		return std::make_shared< Cube >();
	}

	void Cube::Generate()
	{
		int CptNegatif = 0;

		if ( m_width < 0 )
		{
			CptNegatif++;
		}

		if ( m_height < 0 )
		{
			CptNegatif++;
		}

		if ( m_depth < 0 )
		{
			CptNegatif++;
		}

		BufferElementGroupSPtr l_pVertex;
		SubmeshSPtr l_pSubmesh1 = GetMesh()->CreateSubmesh();
		SubmeshSPtr l_pSubmesh2 = GetMesh()->CreateSubmesh();
		SubmeshSPtr l_pSubmesh3 = GetMesh()->CreateSubmesh();
		SubmeshSPtr l_pSubmesh4 = GetMesh()->CreateSubmesh();
		SubmeshSPtr l_pSubmesh5 = GetMesh()->CreateSubmesh();
		SubmeshSPtr l_pSubmesh6 = GetMesh()->CreateSubmesh();
		//Calcul des coordonnées des 8 sommets du pavé
		l_pVertex = l_pSubmesh1->AddPoint( m_width / 2,	-m_height / 2,	m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 1.0, 0.0 );
		Vertex::SetNormal(	l_pVertex, 0.0, 0.0, 1.0 );
		l_pVertex = l_pSubmesh1->AddPoint( -m_width / 2,	-m_height / 2,	m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 0.0, 0.0 );
		Vertex::SetNormal(	l_pVertex, 0.0, 0.0, 1.0 );
		l_pVertex = l_pSubmesh1->AddPoint( -m_width / 2,	 m_height / 2,	m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 0.0, 1.0 );
		Vertex::SetNormal(	l_pVertex, 0.0, 0.0, 1.0 );
		l_pVertex = l_pSubmesh1->AddPoint( m_width / 2,	 m_height / 2,	m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 1.0, 1.0 );
		Vertex::SetNormal(	l_pVertex, 0.0, 0.0, 1.0 );
		l_pVertex = l_pSubmesh2->AddPoint( -m_width / 2,	-m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 1.0, 0.0 );
		Vertex::SetNormal(	l_pVertex, 0.0, 0.0, -1.0 );
		l_pVertex = l_pSubmesh2->AddPoint( m_width / 2,	-m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 0.0, 0.0 );
		Vertex::SetNormal(	l_pVertex, 0.0, 0.0, -1.0 );
		l_pVertex = l_pSubmesh2->AddPoint( m_width / 2,	 m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 0.0, 1.0 );
		Vertex::SetNormal(	l_pVertex, 0.0, 0.0, -1.0 );
		l_pVertex = l_pSubmesh2->AddPoint( -m_width / 2,	 m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 1.0, 1.0 );
		Vertex::SetNormal(	l_pVertex, 0.0, 0.0, -1.0 );
		l_pVertex = l_pSubmesh3->AddPoint( -m_width / 2,	-m_height / 2,	m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 1.0, 0.0 );
		Vertex::SetNormal(	l_pVertex, -1.0, 0.0, 0.0 );
		l_pVertex = l_pSubmesh3->AddPoint( -m_width / 2,	-m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 0.0, 0.0 );
		Vertex::SetNormal(	l_pVertex, -1.0, 0.0, 0.0 );
		l_pVertex = l_pSubmesh3->AddPoint( -m_width / 2,	 m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 0.0, 1.0 );
		Vertex::SetNormal(	l_pVertex, -1.0, 0.0, 0.0 );
		l_pVertex = l_pSubmesh3->AddPoint( -m_width / 2,	 m_height / 2,	m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 1.0, 1.0 );
		Vertex::SetNormal(	l_pVertex, -1.0, 0.0, 0.0 );
		l_pVertex = l_pSubmesh4->AddPoint( m_width / 2,	-m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 1.0, 0.0 );
		Vertex::SetNormal(	l_pVertex, 1.0, 0.0, 0.0 );
		l_pVertex = l_pSubmesh4->AddPoint( m_width / 2,	-m_height / 2,	m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 0.0, 0.0 );
		Vertex::SetNormal(	l_pVertex, 1.0, 0.0, 0.0 );
		l_pVertex = l_pSubmesh4->AddPoint( m_width / 2,	 m_height / 2,  m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 0.0, 1.0 );
		Vertex::SetNormal(	l_pVertex, 1.0, 0.0, 0.0 );
		l_pVertex = l_pSubmesh4->AddPoint( m_width / 2,	 m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 1.0, 1.0 );
		Vertex::SetNormal(	l_pVertex, 1.0, 0.0, 0.0 );
		l_pVertex = l_pSubmesh5->AddPoint( -m_width / 2,	-m_height / 2,	m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 1.0, 0.0 );
		Vertex::SetNormal(	l_pVertex, 0.0, -1.0, 0.0 );
		l_pVertex = l_pSubmesh5->AddPoint( m_width / 2,	-m_height / 2,	m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 0.0, 0.0 );
		Vertex::SetNormal(	l_pVertex, 0.0, -1.0, 0.0 );
		l_pVertex = l_pSubmesh5->AddPoint( m_width / 2,	-m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 0.0, 1.0 );
		Vertex::SetNormal(	l_pVertex, 0.0, -1.0, 0.0 );
		l_pVertex = l_pSubmesh5->AddPoint( -m_width / 2,	-m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 1.0, 1.0 );
		Vertex::SetNormal(	l_pVertex, 0.0, -1.0, 0.0 );
		l_pVertex = l_pSubmesh6->AddPoint( -m_width / 2,	 m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 1.0, 0.0 );
		Vertex::SetNormal(	l_pVertex, 0.0, 1.0, 0.0 );
		l_pVertex = l_pSubmesh6->AddPoint( m_width / 2,	 m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 0.0, 0.0 );
		Vertex::SetNormal(	l_pVertex, 0.0, 1.0, 0.0 );
		l_pVertex = l_pSubmesh6->AddPoint( m_width / 2,	 m_height / 2,	m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 0.0, 1.0 );
		Vertex::SetNormal(	l_pVertex, 0.0, 1.0, 0.0 );
		l_pVertex = l_pSubmesh6->AddPoint( -m_width / 2,	 m_height / 2,	m_depth / 2 );
		Vertex::SetTexCoord(	l_pVertex, 1.0, 1.0 );
		Vertex::SetNormal(	l_pVertex, 0.0, 1.0, 0.0 );

		//CONSTRUCTION FACES /!\ Pour OpenGL le Z est inversé

		if ( CptNegatif == 1 || CptNegatif == 3 )
		{
			// Faces du bas
			l_pSubmesh1->AddFace( 0, 1, 2 );
			l_pSubmesh1->AddFace( 2, 3, 0 );
			// Face du haut
			l_pSubmesh2->AddFace( 0, 1, 2 );
			l_pSubmesh2->AddFace( 2, 3, 0 );
			// Face de derrière
			l_pSubmesh3->AddFace( 0, 1, 2 );
			l_pSubmesh3->AddFace( 2, 3, 0 );
			// Face de devant
			l_pSubmesh4->AddFace( 0, 1, 2 );
			l_pSubmesh4->AddFace( 2, 3, 0 );
			// Faces de droite
			l_pSubmesh5->AddFace( 0, 1, 2 );
			l_pSubmesh5->AddFace( 2, 3, 0 );
			// Face de gauche
			l_pSubmesh6->AddFace( 0, 1, 2 );
			l_pSubmesh6->AddFace( 2, 3, 0 );
		}
		else
		{
			// Faces du bas
			l_pSubmesh1->AddFace( 3, 2, 1 );
			l_pSubmesh1->AddFace( 1, 0, 3 );
			// Face du haut
			l_pSubmesh2->AddFace( 3, 2, 1 );
			l_pSubmesh2->AddFace( 1, 0, 3 );
			// Face de derrière
			l_pSubmesh3->AddFace( 3, 2, 1 );
			l_pSubmesh3->AddFace( 1, 0, 3 );
			// Face de devant
			l_pSubmesh4->AddFace( 3, 2, 1 );
			l_pSubmesh4->AddFace( 1, 0, 3 );
			// Faces de droite
			l_pSubmesh5->AddFace( 3, 2, 1 );
			l_pSubmesh5->AddFace( 1, 0, 3 );
			// Face de gauche
			l_pSubmesh6->AddFace( 3, 2, 1 );
			l_pSubmesh6->AddFace( 1, 0, 3 );
		}

		l_pSubmesh1->ComputeTangentsFromNormals();
		l_pSubmesh2->ComputeTangentsFromNormals();
		l_pSubmesh3->ComputeTangentsFromNormals();
		l_pSubmesh4->ComputeTangentsFromNormals();
		l_pSubmesh5->ComputeTangentsFromNormals();
		l_pSubmesh6->ComputeTangentsFromNormals();
		GetMesh()->ComputeContainers();
	}

	void Cube::Initialise( UIntArray const & CU_PARAM_UNUSED( p_arrayFaces ), RealArray const & p_arrayDimensions )
	{
		m_width = p_arrayDimensions[0];
		m_height = p_arrayDimensions[1];
		m_depth = p_arrayDimensions[2];
		GetMesh()->Cleanup();
		Generate();
	}
}
