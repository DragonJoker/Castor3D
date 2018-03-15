#include "Icosahedron.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Miscellaneous/Parameter.hpp"

#include <Math/SphericalVertex.hpp>

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
	Point3r vertex;
	real phi = ( 1.0f + sqrt( 5.0f ) ) / 2.0f;
	real X = m_radius / sqrt( phi * sqrt( 5.0f ) );
	real Z = X * phi;
	vertex = Point3r( -X,  0,  Z );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( X,  0,  Z );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( -X,  0, -Z );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( X,  0, -Z );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( 0,  Z,  X );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( 0,  Z, -X );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( 0, -Z,  X );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( 0, -Z, -X );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( Z,  X,  0 );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( -Z,  X,  0 );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( Z, -X,  0 );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( -Z, -X,  0 );
	memcpy( vertices[index].pos.ptr(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].nml.ptr(), point::getNormalised( vertex ).ptr(), 3 * sizeof( real ) );
	submesh->addPoints( vertices );

	// on construit toutes les faces de l'icosaèdre
	auto indexMapping = std::make_shared< TriFaceMapping >( *submesh );

	Face faces[20]
	{
		indexMapping->addFace( 0, 1, 4 ),
		indexMapping->addFace( 9, 0, 4 ),
		indexMapping->addFace( 9, 4, 5 ),
		indexMapping->addFace( 4, 8, 5 ),
		indexMapping->addFace( 4, 1, 8 ),
		indexMapping->addFace( 1, 10, 8 ),
		indexMapping->addFace( 3, 8, 10 ),
		indexMapping->addFace( 3, 5, 8 ),
		indexMapping->addFace( 2, 5, 3 ),
		indexMapping->addFace( 7, 2, 3 ),
		indexMapping->addFace( 3, 10, 7 ),
		indexMapping->addFace( 6, 7, 10 ),
		indexMapping->addFace( 6, 11, 7 ),
		indexMapping->addFace( 11, 6, 0 ),
		indexMapping->addFace( 1, 0, 6 ),
		indexMapping->addFace( 1, 6, 10 ),
		indexMapping->addFace( 0, 9, 11 ),
		indexMapping->addFace( 11, 9, 2 ),
		indexMapping->addFace( 9, 5, 2 ),
		indexMapping->addFace( 2, 7, 11 ),
	};

	for ( auto const & face : faces )
	{
		auto posA = submesh->getPoint( face[0] ).pos;
		auto posB = submesh->getPoint( face[1] ).pos;
		auto posC = submesh->getPoint( face[2] ).pos;
		real u = real( 0.5 ) * ( real( 1.0 ) + real( atan2( posA[2], posA[0] ) * ( 1 / Angle::Pi ) ) );
		real v = real( acos( posA[1] ) * ( 1 / Angle::Pi ) );
		submesh->getPoint( face[0] ).tex = Point3f{ u, v, 0 };
		u = real( 0.5 ) * ( real( 1.0 ) + real( atan2( posB[2], posB[0] ) * ( 1 / Angle::Pi ) ) );
		v = real( acos( posB[1] ) * ( 1 / Angle::Pi ) );
		submesh->getPoint( face[1] ).tex = Point3f{ u, v, 0 };
		u = real( 0.5 ) * ( real( 1.0 ) + real( atan2( posC[2], posC[0] ) * ( 1 / Angle::Pi ) ) );
		v = real( acos( posC[1] ) * ( 1 / Angle::Pi ) );
		submesh->getPoint( face[2] ).tex = Point3f{ u, v, 0 };
	}

	for ( auto & vertex : submesh->getPoints() )
	{
		SphericalVertex vsVertex1( vertex.nml );
		vertex.tex = Point3f{ vsVertex1.m_phi, vsVertex1.m_theta, 0 };
	}

	indexMapping->computeTangentsFromNormals();
	submesh->setIndexMapping( indexMapping );
	p_mesh.computeContainers();
}
