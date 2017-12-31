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

		auto mapping1 = std::make_shared< TriFaceMapping >( *pSubmesh1 );
		auto mapping2 = std::make_shared< TriFaceMapping >( *pSubmesh2 );
		auto mapping3 = std::make_shared< TriFaceMapping >( *pSubmesh3 );
		auto mapping4 = std::make_shared< TriFaceMapping >( *pSubmesh4 );
		auto mapping5 = std::make_shared< TriFaceMapping >( *pSubmesh5 );
		auto mapping6 = std::make_shared< TriFaceMapping >( *pSubmesh6 );

		if ( CptNegatif == 1 || CptNegatif == 3 )
		{
			// Faces du bas
			mapping1->addFace( 0, 1, 2 );
			mapping1->addFace( 2, 3, 0 );
			// Faces du haut
			mapping2->addFace( 0, 1, 2 );
			mapping2->addFace( 2, 3, 0 );
			// Faces de derrière
			mapping3->addFace( 0, 1, 2 );
			mapping3->addFace( 2, 3, 0 );
			// Faces de devant
			mapping4->addFace( 0, 1, 2 );
			mapping4->addFace( 2, 3, 0 );
			// Faces de droite
			mapping5->addFace( 0, 1, 2 );
			mapping5->addFace( 2, 3, 0 );
			// Faces de gauche
			mapping6->addFace( 0, 1, 2 );
			mapping6->addFace( 2, 3, 0 );
		}
		else
		{
			// Faces du bas
			mapping1->addFace( 3, 2, 1 );
			mapping1->addFace( 1, 0, 3 );
			// Faces du haut
			mapping2->addFace( 3, 2, 1 );
			mapping2->addFace( 1, 0, 3 );
			// Faces de derrière
			mapping3->addFace( 3, 2, 1 );
			mapping3->addFace( 1, 0, 3 );
			// Faces de devant
			mapping4->addFace( 3, 2, 1 );
			mapping4->addFace( 1, 0, 3 );
			// Faces de droite
			mapping5->addFace( 3, 2, 1 );
			mapping5->addFace( 1, 0, 3 );
			// Faces de gauche
			mapping6->addFace( 3, 2, 1 );
			mapping6->addFace( 1, 0, 3 );
		}

		mapping1->computeTangentsFromNormals();
		mapping2->computeTangentsFromNormals();
		mapping3->computeTangentsFromNormals();
		mapping4->computeTangentsFromNormals();
		mapping5->computeTangentsFromNormals();
		mapping6->computeTangentsFromNormals();

		pSubmesh1->setIndexMapping( mapping1 );
		pSubmesh2->setIndexMapping( mapping2 );
		pSubmesh3->setIndexMapping( mapping3 );
		pSubmesh4->setIndexMapping( mapping4 );
		pSubmesh5->setIndexMapping( mapping5 );
		pSubmesh6->setIndexMapping( mapping6 );

		p_mesh.computeContainers();
	}
}
