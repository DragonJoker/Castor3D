#include "Castor3D/Model/Mesh/Generator/Icosahedron.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"

#include <CastorUtils/Math/SphericalVertex.hpp>

#pragma GCC diagnostic ignored "-Wuseless-cast"

namespace castor3d
{
	Icosahedron::Icosahedron()
		: MeshGenerator( cuT( "icosahedron" ) )
		, m_radius( 0 )
		, m_nbFaces( 0 )
	{
	}

	MeshGeneratorUPtr Icosahedron::create()
	{
		return std::make_unique< Icosahedron >();
	}

	void Icosahedron::doGenerate( Mesh & mesh, Parameters const & parameters )
	{
		castor::String param;

		if ( parameters.get( cuT( "radius" ), param ) )
		{
			m_radius = castor::string::toFloat( param );
		}

		if ( m_radius < 0 )
		{
			m_radius = -m_radius;
		}

		auto submesh = mesh.createSubmesh( SubmeshFlag::ePosNmlTanTex );
		auto normals = submesh->getComponent< NormalsComponent >();
		auto texcoords = submesh->getComponent< Texcoords0Component >();

		// Construction de l'icosaèdre
		std::vector< InterleavedVertex > vertices{ 12 };

		// on crée les 12 points le composant
		uint32_t index{ 0u };
		castor::Point3f vertex;
		auto phi = float( ( 1.0f + sqrt( 5.0f ) ) / 2.0f );
		auto X = float( m_radius / sqrt( phi * sqrt( 5.0f ) ) );
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
		indexMapping->addFace( 1, 0, 4 );
		indexMapping->addFace( 0, 9, 4 );
		indexMapping->addFace( 4, 9, 5 );
		indexMapping->addFace( 8, 4, 5 );
		indexMapping->addFace( 1, 4, 8 );
		indexMapping->addFace( 10, 1, 8 );
		indexMapping->addFace( 8, 3, 10 );
		indexMapping->addFace( 5, 3, 8 );
		indexMapping->addFace( 5, 2, 3 );
		indexMapping->addFace( 2, 7, 3 );
		indexMapping->addFace( 10, 3, 7 );
		indexMapping->addFace( 7, 6, 10 );
		indexMapping->addFace( 11, 6, 7 );
		indexMapping->addFace( 6, 11, 0 );
		indexMapping->addFace( 0, 1, 6 );
		indexMapping->addFace( 6, 1, 10 );
		indexMapping->addFace( 9, 0, 11 );
		indexMapping->addFace( 9, 11, 2 );
		indexMapping->addFace( 5, 9, 2 );
		indexMapping->addFace( 7, 2, 11 );

		for ( uint32_t i = 0u; i < submesh->getPointsCount(); ++i )
		{
			castor::SphericalVertex vsVertex1( castor::Point3f{ normals->getData()[i] } );
			texcoords->getData()[i] = castor::Point3f{ vsVertex1.m_phi, vsVertex1.m_theta, 0 };
		}

		indexMapping->computeTangents();
	}
}
