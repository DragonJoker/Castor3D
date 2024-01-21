#include "Castor3D/Model/Mesh/Generator/Cube.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"

CU_ImplementSmartPtr( castor3d, Cube )

namespace castor3d
{
	Cube::Cube()
		: MeshGenerator( cuT( "cube" ) )
	{
	}

	MeshGeneratorUPtr Cube::create()
	{
		return castor::makeUniqueDerived< MeshGenerator, Cube >();
	}

	void Cube::doGenerate( Mesh & mesh, Parameters const & parameters )
	{
		castor::String param;
		float height{};
		float width{};
		float depth{};

		if ( parameters.get( cuT( "width" ), param ) )
		{
			width = castor::string::toFloat( param );
		}

		if ( parameters.get( cuT( "height" ), param ) )
		{
			height = castor::string::toFloat( param );
		}

		if ( parameters.get( cuT( "depth" ), param ) )
		{
			depth = castor::string::toFloat( param );
		}

		int CptNegatif = 0;

		if ( width < 0 )
		{
			CptNegatif++;
		}

		if ( height < 0 )
		{
			CptNegatif++;
		}

		if ( depth < 0 )
		{
			CptNegatif++;
		}

		auto submesh1 = mesh.createDefaultSubmesh();
		auto submesh2 = mesh.createDefaultSubmesh();
		auto submesh3 = mesh.createDefaultSubmesh();
		auto submesh4 = mesh.createDefaultSubmesh();
		auto submesh5 = mesh.createDefaultSubmesh();
		auto submesh6 = mesh.createDefaultSubmesh();
		static castor::Point4f const zero;

		// Face avant
		submesh1->addPoints( {
			InterleavedVertex{ castor::Point3f{ +width / 2, -height / 2, +depth / 2 }, castor::Point3f{ 0.0, 0.0, 1.0 }, zero, castor::Point3f{ 1.0, 0.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ -width / 2, -height / 2, +depth / 2 }, castor::Point3f{ 0.0, 0.0, 1.0 }, zero, castor::Point3f{ 0.0, 0.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ -width / 2, +height / 2, +depth / 2 }, castor::Point3f{ 0.0, 0.0, 1.0 }, zero, castor::Point3f{ 0.0, 1.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ +width / 2, +height / 2, +depth / 2 }, castor::Point3f{ 0.0, 0.0, 1.0 }, zero, castor::Point3f{ 1.0, 1.0, 0.0 } },
		} );

		// Face arrière
		submesh2->addPoints( {
			InterleavedVertex{ castor::Point3f{ -width / 2, -height / 2, -depth / 2 }, castor::Point3f{ 0.0, 0.0, -1.0 }, zero, castor::Point3f{ 1.0, 0.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ +width / 2, -height / 2, -depth / 2 }, castor::Point3f{ 0.0, 0.0, -1.0 }, zero, castor::Point3f{ 0.0, 0.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ +width / 2, +height / 2, -depth / 2 }, castor::Point3f{ 0.0, 0.0, -1.0 }, zero, castor::Point3f{ 0.0, 1.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ -width / 2, +height / 2, -depth / 2 }, castor::Point3f{ 0.0, 0.0, -1.0 }, zero, castor::Point3f{ 1.0, 1.0, 0.0 } },
		} );

		// Face gauche
		submesh3->addPoints( {
			InterleavedVertex{ castor::Point3f{ -width / 2, -height / 2, +depth / 2 }, castor::Point3f{ -1.0, 0.0, 0.0 }, zero, castor::Point3f{ 1.0, 0.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ -width / 2, -height / 2, -depth / 2 }, castor::Point3f{ -1.0, 0.0, 0.0 }, zero, castor::Point3f{ 0.0, 0.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ -width / 2, +height / 2, -depth / 2 }, castor::Point3f{ -1.0, 0.0, 0.0 }, zero, castor::Point3f{ 0.0, 1.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ -width / 2, +height / 2, +depth / 2 }, castor::Point3f{ -1.0, 0.0, 0.0 }, zero, castor::Point3f{ 1.0, 1.0, 0.0 } },
		} );

		// Face droite
		submesh4->addPoints( {
			InterleavedVertex{ castor::Point3f{ +width / 2, -height / 2, -depth / 2 }, castor::Point3f{ 1.0, 0.0, 0.0 }, zero, castor::Point3f{ 1.0, 0.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ +width / 2, -height / 2, +depth / 2 }, castor::Point3f{ 1.0, 0.0, 0.0 }, zero, castor::Point3f{ 0.0, 0.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ +width / 2, +height / 2, +depth / 2 }, castor::Point3f{ 1.0, 0.0, 0.0 }, zero, castor::Point3f{ 0.0, 1.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ +width / 2, +height / 2, -depth / 2 }, castor::Point3f{ 1.0, 0.0, 0.0 }, zero, castor::Point3f{ 1.0, 1.0, 0.0 } },
		} );

		// Face bas
		submesh5->addPoints( {
			InterleavedVertex{ castor::Point3f{ -width / 2, -height / 2, +depth / 2 }, castor::Point3f{ 0.0, -1.0, 0.0 }, zero, castor::Point3f{ 1.0, 0.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ +width / 2, -height / 2, +depth / 2 }, castor::Point3f{ 0.0, -1.0, 0.0 }, zero, castor::Point3f{ 0.0, 0.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ +width / 2, -height / 2, -depth / 2 }, castor::Point3f{ 0.0, -1.0, 0.0 }, zero, castor::Point3f{ 0.0, 1.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ -width / 2, -height / 2, -depth / 2 }, castor::Point3f{ 0.0, -1.0, 0.0 }, zero, castor::Point3f{ 1.0, 1.0, 0.0 } },
		} );

		// Face haut
		submesh6->addPoints( {
			InterleavedVertex{ castor::Point3f{ -width / 2, height / 2, -depth / 2 }, castor::Point3f{ 0.0, 1.0, 0.0 }, zero, castor::Point3f{ 1.0, 0.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ +width / 2, height / 2, -depth / 2 }, castor::Point3f{ 0.0, 1.0, 0.0 }, zero, castor::Point3f{ 0.0, 0.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ +width / 2, height / 2, +depth / 2 }, castor::Point3f{ 0.0, 1.0, 0.0 }, zero, castor::Point3f{ 0.0, 1.0, 0.0 } },
			InterleavedVertex{ castor::Point3f{ -width / 2, height / 2, +depth / 2 }, castor::Point3f{ 0.0, 1.0, 0.0 }, zero, castor::Point3f{ 1.0, 1.0, 0.0 } },
		} );

		auto mapping1 = submesh1->createComponent< TriFaceMapping >();
		auto mapping2 = submesh2->createComponent< TriFaceMapping >();
		auto mapping3 = submesh3->createComponent< TriFaceMapping >();
		auto mapping4 = submesh4->createComponent< TriFaceMapping >();
		auto mapping5 = submesh5->createComponent< TriFaceMapping >();
		auto mapping6 = submesh6->createComponent< TriFaceMapping >();
		auto & mapping1Data = mapping1->getData();
		auto & mapping2Data = mapping2->getData();
		auto & mapping3Data = mapping3->getData();
		auto & mapping4Data = mapping4->getData();
		auto & mapping5Data = mapping5->getData();
		auto & mapping6Data = mapping6->getData();

		if ( CptNegatif == 1 || CptNegatif == 3 )
		{
			// Faces du bas
			mapping1Data.addFace( 0, 2, 1 );
			mapping1Data.addFace( 2, 0, 3 );
			// Faces du haut
			mapping2Data.addFace( 0, 2, 1 );
			mapping2Data.addFace( 2, 0, 3 );
			// Faces de derrière
			mapping3Data.addFace( 0, 2, 1 );
			mapping3Data.addFace( 2, 0, 3 );
			// Faces de devant
			mapping4Data.addFace( 0, 2, 1 );
			mapping4Data.addFace( 2, 0, 3 );
			// Faces de droite
			mapping5Data.addFace( 0, 2, 1 );
			mapping5Data.addFace( 2, 0, 3 );
			// Faces de gauche
			mapping6Data.addFace( 0, 2, 1 );
			mapping6Data.addFace( 2, 0, 3 );
		}
		else
		{
			// Faces du bas
			mapping1Data.addFace( 3, 1, 2 );
			mapping1Data.addFace( 1, 3, 0 );
			// Faces du haut
			mapping2Data.addFace( 3, 1, 2 );
			mapping2Data.addFace( 1, 3, 0 );
			// Faces de derrière
			mapping3Data.addFace( 3, 1, 2 );
			mapping3Data.addFace( 1, 3, 0 );
			// Faces de devant
			mapping4Data.addFace( 3, 1, 2 );
			mapping4Data.addFace( 1, 3, 0 );
			// Faces de droite
			mapping5Data.addFace( 3, 1, 2 );
			mapping5Data.addFace( 1, 3, 0 );
			// Faces de gauche
			mapping6Data.addFace( 3, 1, 2 );
			mapping6Data.addFace( 1, 3, 0 );
		}

		mapping1->computeTangents();
		mapping2->computeTangents();
		mapping3->computeTangents();
		mapping4->computeTangents();
		mapping5->computeTangents();
		mapping6->computeTangents();
	}
}
