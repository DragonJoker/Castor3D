#include "Icosahedron.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Miscellaneous/Parameter.hpp"

#include <Math/SphericalVertex.hpp>

using namespace Castor3D;
using namespace Castor;

Icosahedron::Icosahedron()
	: MeshGenerator( cuT( "icosahedron" ) )
	, m_radius( 0 )
	, m_nbFaces( 0 )
{
}

Icosahedron::~Icosahedron()
{
}

MeshGeneratorSPtr Icosahedron::Create()
{
	return std::make_shared< Icosahedron >();
}

void Icosahedron::DoGenerate( Mesh & p_mesh, Parameters const & p_parameters )
{
	String param;

	if ( p_parameters.Get( cuT( "radius" ), param ) )
	{
		m_radius = string::to_float( param );
	}

	if ( m_radius < 0 )
	{
		m_radius = -m_radius;
	}

	SubmeshSPtr submesh = p_mesh.CreateSubmesh();

	// Construction de l'icosaèdre
	std::vector< InterleavedVertex > vertices{ 12 };

	// on crée les 12 points le composant
	uint32_t index{ 0u };
	Point3r vertex;
	real phi = ( 1.0f + sqrt( 5.0f ) ) / 2.0f;
	real X = m_radius / sqrt( phi * sqrt( 5.0f ) );
	real Z = X * phi;
	vertex = Point3r( -X,  0,  Z );
	memcpy( vertices[index].m_pos.data(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].m_nml.data(), point::get_normalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( X,  0,  Z );
	memcpy( vertices[index].m_pos.data(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].m_nml.data(), point::get_normalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( -X,  0, -Z );
	memcpy( vertices[index].m_pos.data(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].m_nml.data(), point::get_normalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( X,  0, -Z );
	memcpy( vertices[index].m_pos.data(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].m_nml.data(), point::get_normalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( 0,  Z,  X );
	memcpy( vertices[index].m_pos.data(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].m_nml.data(), point::get_normalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( 0,  Z, -X );
	memcpy( vertices[index].m_pos.data(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].m_nml.data(), point::get_normalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( 0, -Z,  X );
	memcpy( vertices[index].m_pos.data(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].m_nml.data(), point::get_normalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( 0, -Z, -X );
	memcpy( vertices[index].m_pos.data(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].m_nml.data(), point::get_normalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( Z,  X,  0 );
	memcpy( vertices[index].m_pos.data(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].m_nml.data(), point::get_normalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( -Z,  X,  0 );
	memcpy( vertices[index].m_pos.data(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].m_nml.data(), point::get_normalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( Z, -X,  0 );
	memcpy( vertices[index].m_pos.data(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].m_nml.data(), point::get_normalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	vertex = Point3r( -Z, -X,  0 );
	memcpy( vertices[index].m_pos.data(), vertex.ptr(), 3 * sizeof( real ) );
	memcpy( vertices[index].m_nml.data(), point::get_normalised( vertex ).ptr(), 3 * sizeof( real ) );
	++index;
	submesh->AddPoints( vertices );

	// on construit toutes les faces de l'icosaèdre
	Face faces[20]
	{
		submesh->AddFace( 0, 1, 4 ),
		submesh->AddFace( 9, 0, 4 ),
		submesh->AddFace( 9, 4, 5 ),
		submesh->AddFace( 4, 8, 5 ),
		submesh->AddFace( 4, 1, 8 ),
		submesh->AddFace( 1, 10, 8 ),
		submesh->AddFace( 3, 8, 10 ),
		submesh->AddFace( 3, 5, 8 ),
		submesh->AddFace( 2, 5, 3 ),
		submesh->AddFace( 7, 2, 3 ),
		submesh->AddFace( 3, 10, 7 ),
		submesh->AddFace( 6, 7, 10 ),
		submesh->AddFace( 6, 11, 7 ),
		submesh->AddFace( 11, 6, 0 ),
		submesh->AddFace( 1, 0, 6 ),
		submesh->AddFace( 1, 6, 10 ),
		submesh->AddFace( 0, 9, 11 ),
		submesh->AddFace( 11, 9, 2 ),
		submesh->AddFace( 9, 5, 2 ),
		submesh->AddFace( 2, 7, 11 ),
	};

	for ( auto const & face : faces )
	{
		Point3r ptCoordsA;
		Point3r ptCoordsB;
		Point3r ptCoordsC;
		Vertex::GetPosition( submesh->GetPoint( face[0] ), ptCoordsA );
		Vertex::GetPosition( submesh->GetPoint( face[1] ), ptCoordsB );
		Vertex::GetPosition( submesh->GetPoint( face[2] ), ptCoordsC );
		real u = real( 0.5 ) * ( real( 1.0 ) + real( atan2( ptCoordsA[2], ptCoordsA[0] ) * ( 1 / Angle::Pi ) ) );
		real v = real( acos( ptCoordsA[1] ) * ( 1 / Angle::Pi ) );
		Vertex::SetTexCoord( submesh->GetPoint( face[0] ), u, v );
		u = real( 0.5 ) * ( real( 1.0 ) + real( atan2( ptCoordsB[2], ptCoordsB[0] ) * ( 1 / Angle::Pi ) ) );
		v = real( acos( ptCoordsB[1] ) * ( 1 / Angle::Pi ) );
		Vertex::SetTexCoord( submesh->GetPoint( face[1] ), u, v );
		u = real( 0.5 ) * ( real( 1.0 ) + real( atan2( ptCoordsC[2], ptCoordsC[0] ) * ( 1 / Angle::Pi ) ) );
		v = real( acos( ptCoordsC[1] ) * ( 1 / Angle::Pi ) );
		Vertex::SetTexCoord( submesh->GetPoint( face[2] ), u, v );
	}

	for ( auto & vertex : submesh->GetPoints() )
	{
		Point3r ptNml;
		Vertex::GetNormal( vertex, ptNml );
		SphericalVertex vsVertex1( ptNml );
		Vertex::SetTexCoord( vertex, vsVertex1.m_phi, vsVertex1.m_theta );
	}

	submesh->ComputeTangentsFromNormals();
	p_mesh.ComputeContainers();
}
