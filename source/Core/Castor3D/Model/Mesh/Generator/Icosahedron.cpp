#include "Castor3D/Model/Mesh/Generator/Icosahedron.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"

#include <CastorUtils/Math/SphericalVertex.hpp>

using namespace castor3d;
using namespace castor;

Icosahedron::Icosahedron()
	: MeshGenerator( cuT( "icosahedron" ) )
	, m_radius( 0 )
	, m_nbFaces( 0 )
{
}

Icosahedron::~Icosahedron()
{
}

MeshGeneratorSPtr Icosahedron::create()
{
	return std::make_shared< Icosahedron >();
}

void Icosahedron::doGenerate( Mesh & p_mesh, Parameters const & p_parameters )
{
	String param;

	if ( p_parameters.get( cuT( "radius" ), param ) )
	{
		m_radius = string::toFloat( param );
	}

	if ( m_radius < 0 )
	{
		m_radius = -m_radius;
	}

	SubmeshSPtr submesh = p_mesh.createSubmesh();

	// Construction de l'icosaèdre
	std::vector< InterleavedVertex > vertices{ 12 };

	// on crée les 12 points le composant
	uint32_t index{ 0u };
	castor::Point3f vertex;
	float phi = ( 1.0f + sqrt( 5.0f ) ) / 2.0f;
	float X = m_radius / sqrt( phi * sqrt( 5.0f ) );
	float Z = X * phi;
	vertex = castor::Point3f( -X,  0,  Z );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( float ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( float ) );
	++index;
	vertex = castor::Point3f( X,  0,  Z );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( float ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( float ) );
	++index;
	vertex = castor::Point3f( -X,  0, -Z );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( float ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( float ) );
	++index;
	vertex = castor::Point3f( X,  0, -Z );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( float ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( float ) );
	++index;
	vertex = castor::Point3f( 0,  Z,  X );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( float ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( float ) );
	++index;
	vertex = castor::Point3f( 0,  Z, -X );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( float ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( float ) );
	++index;
	vertex = castor::Point3f( 0, -Z,  X );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( float ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( float ) );
	++index;
	vertex = castor::Point3f( 0, -Z, -X );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( float ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( float ) );
	++index;
	vertex = castor::Point3f( Z,  X,  0 );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( float ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( float ) );
	++index;
	vertex = castor::Point3f( -Z,  X,  0 );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( float ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( float ) );
	++index;
	vertex = castor::Point3f( Z, -X,  0 );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( float ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( float ) );
	++index;
	vertex = castor::Point3f( -Z, -X,  0 );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( float ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( float ) );
	submesh->addPoints( vertices );

	// on construit toutes les faces de l'icosaèdre
	auto indexMapping = std::make_shared< TriFaceMapping >( *submesh );

	Face faces[20]
	{
		indexMapping->addFace(  1,  0,  4 ),
		indexMapping->addFace(  0,  9,  4 ),
		indexMapping->addFace(  4,  9,  5 ),
		indexMapping->addFace(  8,  4,  5 ),
		indexMapping->addFace(  1,  4,  8 ),
		indexMapping->addFace( 10,  1,  8 ),
		indexMapping->addFace(  8,  3, 10 ),
		indexMapping->addFace(  5,  3,  8 ),
		indexMapping->addFace(  5,  2,  3 ),
		indexMapping->addFace(  2,  7,  3 ),
		indexMapping->addFace( 10,  3,  7 ),
		indexMapping->addFace(  7,  6, 10 ),
		indexMapping->addFace( 11,  6,  7 ),
		indexMapping->addFace(  6, 11,  0 ),
		indexMapping->addFace(  0,  1,  6 ),
		indexMapping->addFace(  6,  1, 10 ),
		indexMapping->addFace(  9,  0, 11 ),
		indexMapping->addFace(  9, 11,  2 ),
		indexMapping->addFace(  5,  9,  2 ),
		indexMapping->addFace(  7,  2, 11 ),
	};

	for ( auto const & face : faces )
	{
		auto posA = submesh->getPoint( face[0] ).pos;
		auto posB = submesh->getPoint( face[1] ).pos;
		auto posC = submesh->getPoint( face[2] ).pos;
		float u = 0.5f * ( 1.0f + float( atan2( posA[2], posA[0] ) * ( 1 / Pi< float > ) ) );
		float v = float( acos( posA[1] ) * ( 1 / Pi< float > ) );
		submesh->getPoint( face[0] ).tex = Point3f{ u, v, 0 };
		u = 0.5f * ( 1.0f + float( atan2( posB[2], posB[0] ) * ( 1 / Pi< float > ) ) );
		v = float( acos( posB[1] ) * ( 1 / Pi< float > ) );
		submesh->getPoint( face[1] ).tex = Point3f{ u, v, 0 };
		u = 0.5f * ( 1.0f + float( atan2( posC[2], posC[0] ) * ( 1 / Pi< float > ) ) );
		v = float( acos( posC[1] ) * ( 1 / Pi< float > ) );
		submesh->getPoint( face[2] ).tex = Point3f{ u, v, 0 };
	}

	for ( auto & curVertex : submesh->getPoints() )
	{
		SphericalVertex vsVertex1( curVertex.nml );
		curVertex.tex = Point3f{ vsVertex1.m_phi, vsVertex1.m_theta, 0 };
	}

	indexMapping->computeTangentsFromNormals();
	submesh->setIndexMapping( indexMapping );
	p_mesh.computeContainers();
}
