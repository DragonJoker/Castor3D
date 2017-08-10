#include "Cube.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Miscellaneous/Parameter.hpp"

using namespace castor;

namespace castor3d
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

	MeshGeneratorSPtr Cube::create()
	{
		return std::make_shared< Cube >();
	}

	void Cube::doGenerate( Mesh & p_mesh, Parameters const & p_parameters )
	{
		String param;

		if ( p_parameters.get( cuT( "width" ), param ) )
		{
			m_width = string::toFloat( param );
		}

		if ( p_parameters.get( cuT( "height" ), param ) )
		{
			m_height = string::toFloat( param );
		}

		if ( p_parameters.get( cuT( "depth" ), param ) )
		{
			m_depth = string::toFloat( param );
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
		SubmeshSPtr pSubmesh1 = p_mesh.createSubmesh();
		SubmeshSPtr pSubmesh2 = p_mesh.createSubmesh();
		SubmeshSPtr pSubmesh3 = p_mesh.createSubmesh();
		SubmeshSPtr pSubmesh4 = p_mesh.createSubmesh();
		SubmeshSPtr pSubmesh5 = p_mesh.createSubmesh();
		SubmeshSPtr pSubmesh6 = p_mesh.createSubmesh();
		//Calcul des coordonnées des 8 sommets du pavé
		vertex = pSubmesh1->addPoint( m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::setTexCoord( vertex, 1.0, 0.0 );
		Vertex::setNormal( vertex, 0.0, 0.0, 1.0 );
		vertex = pSubmesh1->addPoint( -m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::setTexCoord( vertex, 0.0, 0.0 );
		Vertex::setNormal( vertex, 0.0, 0.0, 1.0 );
		vertex = pSubmesh1->addPoint( -m_width / 2,  m_height / 2, m_depth / 2 );
		Vertex::setTexCoord( vertex, 0.0, 1.0 );
		Vertex::setNormal( vertex, 0.0, 0.0, 1.0 );
		vertex = pSubmesh1->addPoint( m_width / 2,  m_height / 2, m_depth / 2 );
		Vertex::setTexCoord( vertex, 1.0, 1.0 );
		Vertex::setNormal( vertex, 0.0, 0.0, 1.0 );
		vertex = pSubmesh2->addPoint( -m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::setTexCoord( vertex, 1.0, 0.0 );
		Vertex::setNormal( vertex, 0.0, 0.0, -1.0 );
		vertex = pSubmesh2->addPoint( m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::setTexCoord( vertex, 0.0, 0.0 );
		Vertex::setNormal( vertex, 0.0, 0.0, -1.0 );
		vertex = pSubmesh2->addPoint( m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::setTexCoord( vertex, 0.0, 1.0 );
		Vertex::setNormal( vertex, 0.0, 0.0, -1.0 );
		vertex = pSubmesh2->addPoint( -m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::setTexCoord( vertex, 1.0, 1.0 );
		Vertex::setNormal( vertex, 0.0, 0.0, -1.0 );
		vertex = pSubmesh3->addPoint( -m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::setTexCoord( vertex, 1.0, 0.0 );
		Vertex::setNormal( vertex, -1.0, 0.0, 0.0 );
		vertex = pSubmesh3->addPoint( -m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::setTexCoord( vertex, 0.0, 0.0 );
		Vertex::setNormal( vertex, -1.0, 0.0, 0.0 );
		vertex = pSubmesh3->addPoint( -m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::setTexCoord( vertex, 0.0, 1.0 );
		Vertex::setNormal( vertex, -1.0, 0.0, 0.0 );
		vertex = pSubmesh3->addPoint( -m_width / 2,  m_height / 2, m_depth / 2 );
		Vertex::setTexCoord( vertex, 1.0, 1.0 );
		Vertex::setNormal( vertex, -1.0, 0.0, 0.0 );
		vertex = pSubmesh4->addPoint( m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::setTexCoord( vertex, 1.0, 0.0 );
		Vertex::setNormal( vertex, 1.0, 0.0, 0.0 );
		vertex = pSubmesh4->addPoint( m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::setTexCoord( vertex, 0.0, 0.0 );
		Vertex::setNormal( vertex, 1.0, 0.0, 0.0 );
		vertex = pSubmesh4->addPoint( m_width / 2,  m_height / 2,  m_depth / 2 );
		Vertex::setTexCoord( vertex, 0.0, 1.0 );
		Vertex::setNormal( vertex, 1.0, 0.0, 0.0 );
		vertex = pSubmesh4->addPoint( m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::setTexCoord( vertex, 1.0, 1.0 );
		Vertex::setNormal( vertex, 1.0, 0.0, 0.0 );
		vertex = pSubmesh5->addPoint( -m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::setTexCoord( vertex, 1.0, 0.0 );
		Vertex::setNormal( vertex, 0.0, -1.0, 0.0 );
		vertex = pSubmesh5->addPoint( m_width / 2, -m_height / 2, m_depth / 2 );
		Vertex::setTexCoord( vertex, 0.0, 0.0 );
		Vertex::setNormal( vertex, 0.0, -1.0, 0.0 );
		vertex = pSubmesh5->addPoint( m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::setTexCoord( vertex, 0.0, 1.0 );
		Vertex::setNormal( vertex, 0.0, -1.0, 0.0 );
		vertex = pSubmesh5->addPoint( -m_width / 2, -m_height / 2, -m_depth / 2 );
		Vertex::setTexCoord( vertex, 1.0, 1.0 );
		Vertex::setNormal( vertex, 0.0, -1.0, 0.0 );
		vertex = pSubmesh6->addPoint( -m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::setTexCoord( vertex, 1.0, 0.0 );
		Vertex::setNormal( vertex, 0.0, 1.0, 0.0 );
		vertex = pSubmesh6->addPoint( m_width / 2,  m_height / 2, -m_depth / 2 );
		Vertex::setTexCoord( vertex, 0.0, 0.0 );
		Vertex::setNormal( vertex, 0.0, 1.0, 0.0 );
		vertex = pSubmesh6->addPoint( m_width / 2,  m_height / 2, m_depth / 2 );
		Vertex::setTexCoord( vertex, 0.0, 1.0 );
		Vertex::setNormal( vertex, 0.0, 1.0, 0.0 );
		vertex = pSubmesh6->addPoint( -m_width / 2,  m_height / 2, m_depth / 2 );
		Vertex::setTexCoord( vertex, 1.0, 1.0 );
		Vertex::setNormal( vertex, 0.0, 1.0, 0.0 );

		//CONSTRUCTION FACES /!\ Pour OpenGL le Z est inversé

		if ( CptNegatif == 1 || CptNegatif == 3 )
		{
			// Faces du bas
			pSubmesh1->addFace( 0, 1, 2 );
			pSubmesh1->addFace( 2, 3, 0 );
			// Face du haut
			pSubmesh2->addFace( 0, 1, 2 );
			pSubmesh2->addFace( 2, 3, 0 );
			// Face de derrière
			pSubmesh3->addFace( 0, 1, 2 );
			pSubmesh3->addFace( 2, 3, 0 );
			// Face de devant
			pSubmesh4->addFace( 0, 1, 2 );
			pSubmesh4->addFace( 2, 3, 0 );
			// Faces de droite
			pSubmesh5->addFace( 0, 1, 2 );
			pSubmesh5->addFace( 2, 3, 0 );
			// Face de gauche
			pSubmesh6->addFace( 0, 1, 2 );
			pSubmesh6->addFace( 2, 3, 0 );
		}
		else
		{
			// Faces du bas
			pSubmesh1->addFace( 3, 2, 1 );
			pSubmesh1->addFace( 1, 0, 3 );
			// Face du haut
			pSubmesh2->addFace( 3, 2, 1 );
			pSubmesh2->addFace( 1, 0, 3 );
			// Face de derrière
			pSubmesh3->addFace( 3, 2, 1 );
			pSubmesh3->addFace( 1, 0, 3 );
			// Face de devant
			pSubmesh4->addFace( 3, 2, 1 );
			pSubmesh4->addFace( 1, 0, 3 );
			// Faces de droite
			pSubmesh5->addFace( 3, 2, 1 );
			pSubmesh5->addFace( 1, 0, 3 );
			// Face de gauche
			pSubmesh6->addFace( 3, 2, 1 );
			pSubmesh6->addFace( 1, 0, 3 );
		}

		pSubmesh1->computeTangentsFromNormals();
		pSubmesh2->computeTangentsFromNormals();
		pSubmesh3->computeTangentsFromNormals();
		pSubmesh4->computeTangentsFromNormals();
		pSubmesh5->computeTangentsFromNormals();
		pSubmesh6->computeTangentsFromNormals();
		p_mesh.computeContainers();
	}
}
