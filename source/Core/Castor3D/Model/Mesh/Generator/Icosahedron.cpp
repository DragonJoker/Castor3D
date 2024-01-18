#include "Castor3D/Model/Mesh/Generator/Icosahedron.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"

#include <CastorUtils/Math/SphericalVertex.hpp>

#pragma GCC diagnostic ignored "-Wuseless-cast"

CU_ImplementSmartPtr( castor3d, Icosahedron )

namespace castor3d
{
	Icosahedron::Icosahedron()
		: MeshGenerator( cuT( "icosahedron" ) )
	{
	}

	MeshGeneratorUPtr Icosahedron::create()
	{
		return castor::makeUniqueDerived< MeshGenerator, Icosahedron >();
	}

	void Icosahedron::doGenerate( Mesh & mesh, Parameters const & parameters )
	{
		castor::String param;
		float radius{};

		if ( parameters.get( cuT( "radius" ), param ) )
		{
			radius = castor::string::toFloat( param );
		}

		if ( radius < 0 )
		{
			radius = -radius;
		}

		auto submesh = mesh.createDefaultSubmesh();
		auto normals = submesh->getComponent< NormalsComponent >();
		auto & normalsData = normals->getData();
		auto texcoords = submesh->getComponent< Texcoords0Component >();
		auto & texcoordsData = texcoords->getData();

		// Construction de l'icosaèdre
		std::vector< InterleavedVertex > vertices{ 12 };

		// on crée les 12 points le composant
		uint32_t index{ 0u };
		castor::Point3f vertex;
		auto phi = float( ( 1.0f + sqrt( 5.0f ) ) / 2.0f );
		auto X = float( radius / sqrt( phi * sqrt( 5.0f ) ) );
		float Z = X * phi;
		vertex = castor::Point3f( -X, 0, Z );
		std::copy( vertex.begin(), vertex.end(), vertices[index].pos.begin() );
		vertex = castor::point::getNormalised( vertex );
		std::copy( vertex.begin(), vertex.end(), vertices[index].nml.begin() );
		++index;
		vertex = castor::Point3f( X, 0, Z );
		std::copy( vertex.begin(), vertex.end(), vertices[index].pos.begin() );
		vertex = castor::point::getNormalised( vertex );
		std::copy( vertex.begin(), vertex.end(), vertices[index].nml.begin() );
		++index;
		vertex = castor::Point3f( -X, 0, -Z );
		std::copy( vertex.begin(), vertex.end(), vertices[index].pos.begin() );
		vertex = castor::point::getNormalised( vertex );
		std::copy( vertex.begin(), vertex.end(), vertices[index].nml.begin() );
		++index;
		vertex = castor::Point3f( X, 0, -Z );
		std::copy( vertex.begin(), vertex.end(), vertices[index].pos.begin() );
		vertex = castor::point::getNormalised( vertex );
		std::copy( vertex.begin(), vertex.end(), vertices[index].nml.begin() );
		++index;
		vertex = castor::Point3f( 0, Z, X );
		std::copy( vertex.begin(), vertex.end(), vertices[index].pos.begin() );
		vertex = castor::point::getNormalised( vertex );
		std::copy( vertex.begin(), vertex.end(), vertices[index].nml.begin() );
		++index;
		vertex = castor::Point3f( 0, Z, -X );
		std::copy( vertex.begin(), vertex.end(), vertices[index].pos.begin() );
		vertex = castor::point::getNormalised( vertex );
		std::copy( vertex.begin(), vertex.end(), vertices[index].nml.begin() );
		++index;
		vertex = castor::Point3f( 0, -Z, X );
		std::copy( vertex.begin(), vertex.end(), vertices[index].pos.begin() );
		vertex = castor::point::getNormalised( vertex );
		std::copy( vertex.begin(), vertex.end(), vertices[index].nml.begin() );
		++index;
		vertex = castor::Point3f( 0, -Z, -X );
		std::copy( vertex.begin(), vertex.end(), vertices[index].pos.begin() );
		vertex = castor::point::getNormalised( vertex );
		std::copy( vertex.begin(), vertex.end(), vertices[index].nml.begin() );
		++index;
		vertex = castor::Point3f( Z, X, 0 );
		std::copy( vertex.begin(), vertex.end(), vertices[index].pos.begin() );
		vertex = castor::point::getNormalised( vertex );
		std::copy( vertex.begin(), vertex.end(), vertices[index].nml.begin() );
		++index;
		vertex = castor::Point3f( -Z, X, 0 );
		std::copy( vertex.begin(), vertex.end(), vertices[index].pos.begin() );
		vertex = castor::point::getNormalised( vertex );
		std::copy( vertex.begin(), vertex.end(), vertices[index].nml.begin() );
		++index;
		vertex = castor::Point3f( Z, -X, 0 );
		std::copy( vertex.begin(), vertex.end(), vertices[index].pos.begin() );
		vertex = castor::point::getNormalised( vertex );
		std::copy( vertex.begin(), vertex.end(), vertices[index].nml.begin() );
		++index;
		vertex = castor::Point3f( -Z, -X, 0 );
		std::copy( vertex.begin(), vertex.end(), vertices[index].pos.begin() );
		vertex = castor::point::getNormalised( vertex );
		std::copy( vertex.begin(), vertex.end(), vertices[index].nml.begin() );
		submesh->addPoints( vertices );

		// on construit toutes les faces de l'icosaèdre
		auto indexMapping = submesh->createComponent< TriFaceMapping >();
		auto & indexMappingData = indexMapping->getData();
		indexMappingData.addFace( 1, 0, 4 );
		indexMappingData.addFace( 0, 9, 4 );
		indexMappingData.addFace( 4, 9, 5 );
		indexMappingData.addFace( 8, 4, 5 );
		indexMappingData.addFace( 1, 4, 8 );
		indexMappingData.addFace( 10, 1, 8 );
		indexMappingData.addFace( 8, 3, 10 );
		indexMappingData.addFace( 5, 3, 8 );
		indexMappingData.addFace( 5, 2, 3 );
		indexMappingData.addFace( 2, 7, 3 );
		indexMappingData.addFace( 10, 3, 7 );
		indexMappingData.addFace( 7, 6, 10 );
		indexMappingData.addFace( 11, 6, 7 );
		indexMappingData.addFace( 6, 11, 0 );
		indexMappingData.addFace( 0, 1, 6 );
		indexMappingData.addFace( 6, 1, 10 );
		indexMappingData.addFace( 9, 0, 11 );
		indexMappingData.addFace( 9, 11, 2 );
		indexMappingData.addFace( 5, 9, 2 );
		indexMappingData.addFace( 7, 2, 11 );

		for ( uint32_t i = 0u; i < submesh->getPointsCount(); ++i )
		{
			castor::SphericalVertex vsVertex1( castor::Point3f{ normalsData.getData()[i] } );
			texcoordsData.getData()[i] = castor::Point3f{ vsVertex1.m_phi, vsVertex1.m_theta, 0 };
		}

		indexMapping->computeTangents();
	}
}
