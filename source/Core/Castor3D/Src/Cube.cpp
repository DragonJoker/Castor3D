#include "Cube.hpp"
#include "Submesh.hpp"
#include "Vertex.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	Cube::Cube()
		: MeshGenerator( eMESH_TYPE_CUBE )
		, m_width( 0 )
		, m_height( 0 )
		, m_depth( 0 )
	{
	}

	Cube::~Cube()
	{
	}

	MeshGeneratorSPtr Cube::Create()
	{
		return std::make_shared< Cube >();
	}

	void Cube::DoGenerate( Mesh & p_mesh, UIntArray const & p_faces, RealArray const & p_dimensions )
	{
		m_width = p_dimensions[0];
		m_height = p_dimensions[1];
		m_depth = p_dimensions[2];
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

		BufferElementGroupSPtr l_vertex;
		SubmeshSPtr l_pSubmesh1 = p_mesh.CreateSubmesh();
		SubmeshSPtr l_pSubmesh2 = p_mesh.CreateSubmesh();
		SubmeshSPtr l_pSubmesh3 = p_mesh.CreateSubmesh();
		SubmeshSPtr l_pSubmesh4 = p_mesh.CreateSubmesh();
		SubmeshSPtr l_pSubmesh5 = p_mesh.CreateSubmesh();
		SubmeshSPtr l_pSubmesh6 = p_mesh.CreateSubmesh();
		//Calcul des coordonnées des 8 sommets du pavé
		l_vertex = l_pSubmesh1->AddPoint( m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 1.0, 0.0 );
		Vertex::SetNormal( l_vertex, 0.0, 0.0, 1.0 );
		l_vertex = l_pSubmesh1->AddPoint( -m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 0.0, 0.0 );
		Vertex::SetNormal( l_vertex, 0.0, 0.0, 1.0 );
		l_vertex = l_pSubmesh1->AddPoint( -m_width / 2,  m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 0.0, 1.0 );
		Vertex::SetNormal( l_vertex, 0.0, 0.0, 1.0 );
		l_vertex = l_pSubmesh1->AddPoint( m_width / 2,  m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 1.0, 1.0 );
		Vertex::SetNormal( l_vertex, 0.0, 0.0, 1.0 );
		l_vertex = l_pSubmesh2->AddPoint( -m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 1.0, 0.0 );
		Vertex::SetNormal( l_vertex, 0.0, 0.0, -1.0 );
		l_vertex = l_pSubmesh2->AddPoint( m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 0.0, 0.0 );
		Vertex::SetNormal( l_vertex, 0.0, 0.0, -1.0 );
		l_vertex = l_pSubmesh2->AddPoint( m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 0.0, 1.0 );
		Vertex::SetNormal( l_vertex, 0.0, 0.0, -1.0 );
		l_vertex = l_pSubmesh2->AddPoint( -m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 1.0, 1.0 );
		Vertex::SetNormal( l_vertex, 0.0, 0.0, -1.0 );
		l_vertex = l_pSubmesh3->AddPoint( -m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 1.0, 0.0 );
		Vertex::SetNormal( l_vertex, -1.0, 0.0, 0.0 );
		l_vertex = l_pSubmesh3->AddPoint( -m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 0.0, 0.0 );
		Vertex::SetNormal( l_vertex, -1.0, 0.0, 0.0 );
		l_vertex = l_pSubmesh3->AddPoint( -m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 0.0, 1.0 );
		Vertex::SetNormal( l_vertex, -1.0, 0.0, 0.0 );
		l_vertex = l_pSubmesh3->AddPoint( -m_width / 2,  m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 1.0, 1.0 );
		Vertex::SetNormal( l_vertex, -1.0, 0.0, 0.0 );
		l_vertex = l_pSubmesh4->AddPoint( m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 1.0, 0.0 );
		Vertex::SetNormal( l_vertex, 1.0, 0.0, 0.0 );
		l_vertex = l_pSubmesh4->AddPoint( m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 0.0, 0.0 );
		Vertex::SetNormal( l_vertex, 1.0, 0.0, 0.0 );
		l_vertex = l_pSubmesh4->AddPoint( m_width / 2,  m_height / 2,  m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 0.0, 1.0 );
		Vertex::SetNormal( l_vertex, 1.0, 0.0, 0.0 );
		l_vertex = l_pSubmesh4->AddPoint( m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 1.0, 1.0 );
		Vertex::SetNormal( l_vertex, 1.0, 0.0, 0.0 );
		l_vertex = l_pSubmesh5->AddPoint( -m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 1.0, 0.0 );
		Vertex::SetNormal( l_vertex, 0.0, -1.0, 0.0 );
		l_vertex = l_pSubmesh5->AddPoint( m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 0.0, 0.0 );
		Vertex::SetNormal( l_vertex, 0.0, -1.0, 0.0 );
		l_vertex = l_pSubmesh5->AddPoint( m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 0.0, 1.0 );
		Vertex::SetNormal( l_vertex, 0.0, -1.0, 0.0 );
		l_vertex = l_pSubmesh5->AddPoint( -m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 1.0, 1.0 );
		Vertex::SetNormal( l_vertex, 0.0, -1.0, 0.0 );
		l_vertex = l_pSubmesh6->AddPoint( -m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 1.0, 0.0 );
		Vertex::SetNormal( l_vertex, 0.0, 1.0, 0.0 );
		l_vertex = l_pSubmesh6->AddPoint( m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 0.0, 0.0 );
		Vertex::SetNormal( l_vertex, 0.0, 1.0, 0.0 );
		l_vertex = l_pSubmesh6->AddPoint( m_width / 2,  m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 0.0, 1.0 );
		Vertex::SetNormal( l_vertex, 0.0, 1.0, 0.0 );
		l_vertex = l_pSubmesh6->AddPoint( -m_width / 2,  m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( l_vertex, 1.0, 1.0 );
		Vertex::SetNormal( l_vertex, 0.0, 1.0, 0.0 );

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
		p_mesh.ComputeContainers();
	}
}
