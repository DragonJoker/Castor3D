#include "Castor3D/Model/Mesh/Generator/Cube.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"

namespace castor3d
{
	Cube::Cube()
		: MeshGenerator( cuT( "cube" ) )
		, m_height( 0 )
		, m_width( 0 )
		, m_depth( 0 )
	{
	}

	MeshGeneratorUPtr Cube::create()
	{
		return std::make_unique< Cube >();
	}

	void Cube::doGenerate( Mesh & mesh, Parameters const & parameters )
	{
		castor::String param;

		if ( parameters.get( cuT( "width" ), param ) )
		{
			m_width = castor::string::toFloat( param );
		}

		if ( parameters.get( cuT( "height" ), param ) )
		{
			m_height = castor::string::toFloat( param );
		}

		if ( parameters.get( cuT( "depth" ), param ) )
		{
			m_depth = castor::string::toFloat( param );
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

		SubmeshSPtr submesh1 = mesh.createSubmesh( SubmeshFlag::ePosNmlTanTex );
		SubmeshSPtr submesh2 = mesh.createSubmesh( SubmeshFlag::ePosNmlTanTex );
		SubmeshSPtr submesh3 = mesh.createSubmesh( SubmeshFlag::ePosNmlTanTex );
		SubmeshSPtr submesh4 = mesh.createSubmesh( SubmeshFlag::ePosNmlTanTex );
		SubmeshSPtr submesh5 = mesh.createSubmesh( SubmeshFlag::ePosNmlTanTex );
		SubmeshSPtr submesh6 = mesh.createSubmesh( SubmeshFlag::ePosNmlTanTex );
		static castor::Point3f const zero;

		// Face avant
		submesh1->addPoints( {
			{ castor::Point3f{ +m_width / 2, -m_height / 2, +m_depth / 2 }, castor::Point3f{ 0.0, 0.0, 1.0 }, zero, castor::Point3f{ 1.0, 0.0, 0.0 } },
			{ castor::Point3f{ -m_width / 2, -m_height / 2, +m_depth / 2 }, castor::Point3f{ 0.0, 0.0, 1.0 }, zero, castor::Point3f{ 0.0, 0.0, 0.0 } },
			{ castor::Point3f{ -m_width / 2, +m_height / 2, +m_depth / 2 }, castor::Point3f{ 0.0, 0.0, 1.0 }, zero, castor::Point3f{ 0.0, 1.0, 0.0 } },
			{ castor::Point3f{ +m_width / 2, +m_height / 2, +m_depth / 2 }, castor::Point3f{ 0.0, 0.0, 1.0 }, zero, castor::Point3f{ 1.0, 1.0, 0.0 } },
		} );

		// Face arrière
		submesh2->addPoints( {
			{ castor::Point3f{ -m_width / 2, -m_height / 2, -m_depth / 2 }, castor::Point3f{ 0.0, 0.0, -1.0 }, zero, castor::Point3f{ 1.0, 0.0, 0.0 } },
			{ castor::Point3f{ +m_width / 2, -m_height / 2, -m_depth / 2 }, castor::Point3f{ 0.0, 0.0, -1.0 }, zero, castor::Point3f{ 0.0, 0.0, 0.0 } },
			{ castor::Point3f{ +m_width / 2, +m_height / 2, -m_depth / 2 }, castor::Point3f{ 0.0, 0.0, -1.0 }, zero, castor::Point3f{ 0.0, 1.0, 0.0 } },
			{ castor::Point3f{ -m_width / 2, +m_height / 2, -m_depth / 2 }, castor::Point3f{ 0.0, 0.0, -1.0 }, zero, castor::Point3f{ 1.0, 1.0, 0.0 } },
		} );

		// Face gauche
		submesh3->addPoints( {
			{ castor::Point3f{ -m_width / 2, -m_height / 2, +m_depth / 2 }, castor::Point3f{ -1.0, 0.0, 0.0 }, zero, castor::Point3f{ 1.0, 0.0, 0.0 } },
			{ castor::Point3f{ -m_width / 2, -m_height / 2, -m_depth / 2 }, castor::Point3f{ -1.0, 0.0, 0.0 }, zero, castor::Point3f{ 0.0, 0.0, 0.0 } },
			{ castor::Point3f{ -m_width / 2, +m_height / 2, -m_depth / 2 }, castor::Point3f{ -1.0, 0.0, 0.0 }, zero, castor::Point3f{ 0.0, 1.0, 0.0 } },
			{ castor::Point3f{ -m_width / 2, +m_height / 2, +m_depth / 2 }, castor::Point3f{ -1.0, 0.0, 0.0 }, zero, castor::Point3f{ 1.0, 1.0, 0.0 } },
		} );

		// Face droite
		submesh4->addPoints( {
			{ castor::Point3f{ +m_width / 2, -m_height / 2, -m_depth / 2 }, castor::Point3f{ 1.0, 0.0, 0.0 }, zero, castor::Point3f{ 1.0, 0.0, 0.0 } },
			{ castor::Point3f{ +m_width / 2, -m_height / 2, +m_depth / 2 }, castor::Point3f{ 1.0, 0.0, 0.0 }, zero, castor::Point3f{ 0.0, 0.0, 0.0 } },
			{ castor::Point3f{ +m_width / 2, +m_height / 2, +m_depth / 2 }, castor::Point3f{ 1.0, 0.0, 0.0 }, zero, castor::Point3f{ 0.0, 1.0, 0.0 } },
			{ castor::Point3f{ +m_width / 2, +m_height / 2, -m_depth / 2 }, castor::Point3f{ 1.0, 0.0, 0.0 }, zero, castor::Point3f{ 1.0, 1.0, 0.0 } },
		} );

		// Face bas
		submesh5->addPoints( {
			{ castor::Point3f{ -m_width / 2, -m_height / 2, +m_depth / 2 }, castor::Point3f{ 0.0, -1.0, 0.0 }, zero, castor::Point3f{ 1.0, 0.0, 0.0 } },
			{ castor::Point3f{ +m_width / 2, -m_height / 2, +m_depth / 2 }, castor::Point3f{ 0.0, -1.0, 0.0 }, zero, castor::Point3f{ 0.0, 0.0, 0.0 } },
			{ castor::Point3f{ +m_width / 2, -m_height / 2, -m_depth / 2 }, castor::Point3f{ 0.0, -1.0, 0.0 }, zero, castor::Point3f{ 0.0, 1.0, 0.0 } },
			{ castor::Point3f{ -m_width / 2, -m_height / 2, -m_depth / 2 }, castor::Point3f{ 0.0, -1.0, 0.0 }, zero, castor::Point3f{ 1.0, 1.0, 0.0 } },
		} );

		// Face haut
		submesh6->addPoints( {
			{ castor::Point3f{ -m_width / 2, m_height / 2, -m_depth / 2 }, castor::Point3f{ 0.0, 1.0, 0.0 }, zero, castor::Point3f{ 1.0, 0.0, 0.0 } },
			{ castor::Point3f{ +m_width / 2, m_height / 2, -m_depth / 2 }, castor::Point3f{ 0.0, 1.0, 0.0 }, zero, castor::Point3f{ 0.0, 0.0, 0.0 } },
			{ castor::Point3f{ +m_width / 2, m_height / 2, +m_depth / 2 }, castor::Point3f{ 0.0, 1.0, 0.0 }, zero, castor::Point3f{ 0.0, 1.0, 0.0 } },
			{ castor::Point3f{ -m_width / 2, m_height / 2, +m_depth / 2 }, castor::Point3f{ 0.0, 1.0, 0.0 }, zero, castor::Point3f{ 1.0, 1.0, 0.0 } },
		} );

		auto mapping1 = submesh1->createComponent< TriFaceMapping >();
		auto mapping2 = submesh2->createComponent< TriFaceMapping >();
		auto mapping3 = submesh3->createComponent< TriFaceMapping >();
		auto mapping4 = submesh4->createComponent< TriFaceMapping >();
		auto mapping5 = submesh5->createComponent< TriFaceMapping >();
		auto mapping6 = submesh6->createComponent< TriFaceMapping >();

		if ( CptNegatif == 1 || CptNegatif == 3 )
		{
			// Faces du bas
			mapping1->addFace( 0, 2, 1 );
			mapping1->addFace( 2, 0, 3 );
			// Faces du haut
			mapping2->addFace( 0, 2, 1 );
			mapping2->addFace( 2, 0, 3 );
			// Faces de derrière
			mapping3->addFace( 0, 2, 1 );
			mapping3->addFace( 2, 0, 3 );
			// Faces de devant
			mapping4->addFace( 0, 2, 1 );
			mapping4->addFace( 2, 0, 3 );
			// Faces de droite
			mapping5->addFace( 0, 2, 1 );
			mapping5->addFace( 2, 0, 3 );
			// Faces de gauche
			mapping6->addFace( 0, 2, 1 );
			mapping6->addFace( 2, 0, 3 );
		}
		else
		{
			// Faces du bas
			mapping1->addFace( 3, 1, 2 );
			mapping1->addFace( 1, 3, 0 );
			// Faces du haut
			mapping2->addFace( 3, 1, 2 );
			mapping2->addFace( 1, 3, 0 );
			// Faces de derrière
			mapping3->addFace( 3, 1, 2 );
			mapping3->addFace( 1, 3, 0 );
			// Faces de devant
			mapping4->addFace( 3, 1, 2 );
			mapping4->addFace( 1, 3, 0 );
			// Faces de droite
			mapping5->addFace( 3, 1, 2 );
			mapping5->addFace( 1, 3, 0 );
			// Faces de gauche
			mapping6->addFace( 3, 1, 2 );
			mapping6->addFace( 1, 3, 0 );
		}

		mapping1->computeTangents();
		mapping2->computeTangents();
		mapping3->computeTangents();
		mapping4->computeTangents();
		mapping5->computeTangents();
		mapping6->computeTangents();
	}
}
