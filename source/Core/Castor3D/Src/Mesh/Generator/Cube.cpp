#include "Cube.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Miscellaneous/Parameter.hpp"

using namespace Castor;

namespace Castor3D
{
	Cube::Cube()
		: MeshGenerator( cuT( "cube" ) )
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

	void Cube::DoGenerate( Mesh & p_mesh, Parameters const & p_parameters )
	{
		String param;

		if ( p_parameters.Get( cuT( "width" ), param ) )
		{
			m_width = string::to_float( param );
		}

		if ( p_parameters.Get( cuT( "height" ), param ) )
		{
			m_height = string::to_float( param );
		}

		if ( p_parameters.Get( cuT( "depth" ), param ) )
		{
			m_depth = string::to_float( param );
		}

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

		BufferElementGroupSPtr vertex;
		SubmeshSPtr pSubmesh1 = p_mesh.CreateSubmesh();
		SubmeshSPtr pSubmesh2 = p_mesh.CreateSubmesh();
		SubmeshSPtr pSubmesh3 = p_mesh.CreateSubmesh();
		SubmeshSPtr pSubmesh4 = p_mesh.CreateSubmesh();
		SubmeshSPtr pSubmesh5 = p_mesh.CreateSubmesh();
		SubmeshSPtr pSubmesh6 = p_mesh.CreateSubmesh();
		//Calcul des coordonnées des 8 sommets du pavé
		vertex = pSubmesh1->AddPoint( m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( vertex, 1.0, 0.0 );
		Vertex::SetNormal( vertex, 0.0, 0.0, 1.0 );
		vertex = pSubmesh1->AddPoint( -m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( vertex, 0.0, 0.0 );
		Vertex::SetNormal( vertex, 0.0, 0.0, 1.0 );
		vertex = pSubmesh1->AddPoint( -m_width / 2,  m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( vertex, 0.0, 1.0 );
		Vertex::SetNormal( vertex, 0.0, 0.0, 1.0 );
		vertex = pSubmesh1->AddPoint( m_width / 2,  m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( vertex, 1.0, 1.0 );
		Vertex::SetNormal( vertex, 0.0, 0.0, 1.0 );
		vertex = pSubmesh2->AddPoint( -m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( vertex, 1.0, 0.0 );
		Vertex::SetNormal( vertex, 0.0, 0.0, -1.0 );
		vertex = pSubmesh2->AddPoint( m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( vertex, 0.0, 0.0 );
		Vertex::SetNormal( vertex, 0.0, 0.0, -1.0 );
		vertex = pSubmesh2->AddPoint( m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( vertex, 0.0, 1.0 );
		Vertex::SetNormal( vertex, 0.0, 0.0, -1.0 );
		vertex = pSubmesh2->AddPoint( -m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( vertex, 1.0, 1.0 );
		Vertex::SetNormal( vertex, 0.0, 0.0, -1.0 );
		vertex = pSubmesh3->AddPoint( -m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( vertex, 1.0, 0.0 );
		Vertex::SetNormal( vertex, -1.0, 0.0, 0.0 );
		vertex = pSubmesh3->AddPoint( -m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( vertex, 0.0, 0.0 );
		Vertex::SetNormal( vertex, -1.0, 0.0, 0.0 );
		vertex = pSubmesh3->AddPoint( -m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( vertex, 0.0, 1.0 );
		Vertex::SetNormal( vertex, -1.0, 0.0, 0.0 );
		vertex = pSubmesh3->AddPoint( -m_width / 2,  m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( vertex, 1.0, 1.0 );
		Vertex::SetNormal( vertex, -1.0, 0.0, 0.0 );
		vertex = pSubmesh4->AddPoint( m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( vertex, 1.0, 0.0 );
		Vertex::SetNormal( vertex, 1.0, 0.0, 0.0 );
		vertex = pSubmesh4->AddPoint( m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( vertex, 0.0, 0.0 );
		Vertex::SetNormal( vertex, 1.0, 0.0, 0.0 );
		vertex = pSubmesh4->AddPoint( m_width / 2,  m_height / 2,  m_depth / 2 );
		Vertex::SetTexCoord( vertex, 0.0, 1.0 );
		Vertex::SetNormal( vertex, 1.0, 0.0, 0.0 );
		vertex = pSubmesh4->AddPoint( m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( vertex, 1.0, 1.0 );
		Vertex::SetNormal( vertex, 1.0, 0.0, 0.0 );
		vertex = pSubmesh5->AddPoint( -m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( vertex, 1.0, 0.0 );
		Vertex::SetNormal( vertex, 0.0, -1.0, 0.0 );
		vertex = pSubmesh5->AddPoint( m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( vertex, 0.0, 0.0 );
		Vertex::SetNormal( vertex, 0.0, -1.0, 0.0 );
		vertex = pSubmesh5->AddPoint( m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( vertex, 0.0, 1.0 );
		Vertex::SetNormal( vertex, 0.0, -1.0, 0.0 );
		vertex = pSubmesh5->AddPoint( -m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( vertex, 1.0, 1.0 );
		Vertex::SetNormal( vertex, 0.0, -1.0, 0.0 );
		vertex = pSubmesh6->AddPoint( -m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( vertex, 1.0, 0.0 );
		Vertex::SetNormal( vertex, 0.0, 1.0, 0.0 );
		vertex = pSubmesh6->AddPoint( m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::SetTexCoord( vertex, 0.0, 0.0 );
		Vertex::SetNormal( vertex, 0.0, 1.0, 0.0 );
		vertex = pSubmesh6->AddPoint( m_width / 2,  m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( vertex, 0.0, 1.0 );
		Vertex::SetNormal( vertex, 0.0, 1.0, 0.0 );
		vertex = pSubmesh6->AddPoint( -m_width / 2,  m_height / 2, m_depth / 2 );
		Vertex::SetTexCoord( vertex, 1.0, 1.0 );
		Vertex::SetNormal( vertex, 0.0, 1.0, 0.0 );

		//CONSTRUCTION FACES /!\ Pour OpenGL le Z est inversé

		if ( CptNegatif == 1 || CptNegatif == 3 )
		{
			// Faces du bas
			pSubmesh1->AddFace( 0, 1, 2 );
			pSubmesh1->AddFace( 2, 3, 0 );
			// Face du haut
			pSubmesh2->AddFace( 0, 1, 2 );
			pSubmesh2->AddFace( 2, 3, 0 );
			// Face de derrière
			pSubmesh3->AddFace( 0, 1, 2 );
			pSubmesh3->AddFace( 2, 3, 0 );
			// Face de devant
			pSubmesh4->AddFace( 0, 1, 2 );
			pSubmesh4->AddFace( 2, 3, 0 );
			// Faces de droite
			pSubmesh5->AddFace( 0, 1, 2 );
			pSubmesh5->AddFace( 2, 3, 0 );
			// Face de gauche
			pSubmesh6->AddFace( 0, 1, 2 );
			pSubmesh6->AddFace( 2, 3, 0 );
		}
		else
		{
			// Faces du bas
			pSubmesh1->AddFace( 3, 2, 1 );
			pSubmesh1->AddFace( 1, 0, 3 );
			// Face du haut
			pSubmesh2->AddFace( 3, 2, 1 );
			pSubmesh2->AddFace( 1, 0, 3 );
			// Face de derrière
			pSubmesh3->AddFace( 3, 2, 1 );
			pSubmesh3->AddFace( 1, 0, 3 );
			// Face de devant
			pSubmesh4->AddFace( 3, 2, 1 );
			pSubmesh4->AddFace( 1, 0, 3 );
			// Faces de droite
			pSubmesh5->AddFace( 3, 2, 1 );
			pSubmesh5->AddFace( 1, 0, 3 );
			// Face de gauche
			pSubmesh6->AddFace( 3, 2, 1 );
			pSubmesh6->AddFace( 1, 0, 3 );
		}

		pSubmesh1->ComputeTangentsFromNormals();
		pSubmesh2->ComputeTangentsFromNormals();
		pSubmesh3->ComputeTangentsFromNormals();
		pSubmesh4->ComputeTangentsFromNormals();
		pSubmesh5->ComputeTangentsFromNormals();
		pSubmesh6->ComputeTangentsFromNormals();
		p_mesh.ComputeContainers();
	}
}
