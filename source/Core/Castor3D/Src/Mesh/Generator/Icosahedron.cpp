#include "Icosahedron.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"

#include <Math/SphericalVertex.hpp>

using namespace Castor3D;
using namespace Castor;

Icosahedron::Icosahedron()
	: MeshGenerator( MeshType::eIcosahedron )
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

void Icosahedron::DoGenerate( Mesh & p_mesh, UIntArray const & p_faces, RealArray const & p_dimensions )
{
	m_radius = p_dimensions[0];

	if ( m_radius < 0 )
	{
		m_radius = -m_radius;
	}

	SubmeshSPtr l_submesh = p_mesh.CreateSubmesh();

	// Construction de l'icosaèdre
	std::vector< InterleavedVertex > l_vertices{ 12 };

	// on crée les 12 points le composant
	uint32_t l_index{ 0u };
	Point3r l_vertex;
	real phi = ( 1.0f + sqrt( 5.0f ) ) / 2.0f;
	real X = m_radius / sqrt( phi * sqrt( 5.0f ) );
	real Z = X * phi;
	l_vertex = Point3r( -X,  0,  Z );
	memcpy( l_vertices[l_index].m_pos.data(), l_vertex.ptr(), 3 * sizeof( real ) );
	memcpy( l_vertices[l_index].m_nml.data(), point::get_normalised( l_vertex ).ptr(), 3 * sizeof( real ) );
	++l_index;
	l_vertex = Point3r( X,  0,  Z );
	memcpy( l_vertices[l_index].m_pos.data(), l_vertex.ptr(), 3 * sizeof( real ) );
	memcpy( l_vertices[l_index].m_nml.data(), point::get_normalised( l_vertex ).ptr(), 3 * sizeof( real ) );
	++l_index;
	l_vertex = Point3r( -X,  0, -Z );
	memcpy( l_vertices[l_index].m_pos.data(), l_vertex.ptr(), 3 * sizeof( real ) );
	memcpy( l_vertices[l_index].m_nml.data(), point::get_normalised( l_vertex ).ptr(), 3 * sizeof( real ) );
	++l_index;
	l_vertex = Point3r( X,  0, -Z );
	memcpy( l_vertices[l_index].m_pos.data(), l_vertex.ptr(), 3 * sizeof( real ) );
	memcpy( l_vertices[l_index].m_nml.data(), point::get_normalised( l_vertex ).ptr(), 3 * sizeof( real ) );
	++l_index;
	l_vertex = Point3r( 0,  Z,  X );
	memcpy( l_vertices[l_index].m_pos.data(), l_vertex.ptr(), 3 * sizeof( real ) );
	memcpy( l_vertices[l_index].m_nml.data(), point::get_normalised( l_vertex ).ptr(), 3 * sizeof( real ) );
	++l_index;
	l_vertex = Point3r( 0,  Z, -X );
	memcpy( l_vertices[l_index].m_pos.data(), l_vertex.ptr(), 3 * sizeof( real ) );
	memcpy( l_vertices[l_index].m_nml.data(), point::get_normalised( l_vertex ).ptr(), 3 * sizeof( real ) );
	++l_index;
	l_vertex = Point3r( 0, -Z,  X );
	memcpy( l_vertices[l_index].m_pos.data(), l_vertex.ptr(), 3 * sizeof( real ) );
	memcpy( l_vertices[l_index].m_nml.data(), point::get_normalised( l_vertex ).ptr(), 3 * sizeof( real ) );
	++l_index;
	l_vertex = Point3r( 0, -Z, -X );
	memcpy( l_vertices[l_index].m_pos.data(), l_vertex.ptr(), 3 * sizeof( real ) );
	memcpy( l_vertices[l_index].m_nml.data(), point::get_normalised( l_vertex ).ptr(), 3 * sizeof( real ) );
	++l_index;
	l_vertex = Point3r( Z,  X,  0 );
	memcpy( l_vertices[l_index].m_pos.data(), l_vertex.ptr(), 3 * sizeof( real ) );
	memcpy( l_vertices[l_index].m_nml.data(), point::get_normalised( l_vertex ).ptr(), 3 * sizeof( real ) );
	++l_index;
	l_vertex = Point3r( -Z,  X,  0 );
	memcpy( l_vertices[l_index].m_pos.data(), l_vertex.ptr(), 3 * sizeof( real ) );
	memcpy( l_vertices[l_index].m_nml.data(), point::get_normalised( l_vertex ).ptr(), 3 * sizeof( real ) );
	++l_index;
	l_vertex = Point3r( Z, -X,  0 );
	memcpy( l_vertices[l_index].m_pos.data(), l_vertex.ptr(), 3 * sizeof( real ) );
	memcpy( l_vertices[l_index].m_nml.data(), point::get_normalised( l_vertex ).ptr(), 3 * sizeof( real ) );
	++l_index;
	l_vertex = Point3r( -Z, -X,  0 );
	memcpy( l_vertices[l_index].m_pos.data(), l_vertex.ptr(), 3 * sizeof( real ) );
	memcpy( l_vertices[l_index].m_nml.data(), point::get_normalised( l_vertex ).ptr(), 3 * sizeof( real ) );
	++l_index;
	l_submesh->AddPoints( l_vertices );

	// on construit toutes les faces de l'icosaèdre
	Face l_faces[20]
	{
		l_submesh->AddFace( 0, 1, 4 ),
		l_submesh->AddFace( 9, 0, 4 ),
		l_submesh->AddFace( 9, 4, 5 ),
		l_submesh->AddFace( 4, 8, 5 ),
		l_submesh->AddFace( 4, 1, 8 ),
		l_submesh->AddFace( 1, 10, 8 ),
		l_submesh->AddFace( 3, 8, 10 ),
		l_submesh->AddFace( 3, 5, 8 ),
		l_submesh->AddFace( 2, 5, 3 ),
		l_submesh->AddFace( 7, 2, 3 ),
		l_submesh->AddFace( 3, 10, 7 ),
		l_submesh->AddFace( 6, 7, 10 ),
		l_submesh->AddFace( 6, 11, 7 ),
		l_submesh->AddFace( 11, 6, 0 ),
		l_submesh->AddFace( 1, 0, 6 ),
		l_submesh->AddFace( 1, 6, 10 ),
		l_submesh->AddFace( 0, 9, 11 ),
		l_submesh->AddFace( 11, 9, 2 ),
		l_submesh->AddFace( 9, 5, 2 ),
		l_submesh->AddFace( 2, 7, 11 ),
	};

	for ( auto const & l_face : l_faces )
	{
		Point3r l_ptCoordsA;
		Point3r l_ptCoordsB;
		Point3r l_ptCoordsC;
		Vertex::GetPosition( l_submesh->GetPoint( l_face[0] ), l_ptCoordsA );
		Vertex::GetPosition( l_submesh->GetPoint( l_face[1] ), l_ptCoordsB );
		Vertex::GetPosition( l_submesh->GetPoint( l_face[2] ), l_ptCoordsC );
		real u = real( 0.5 ) * ( real( 1.0 ) + real( atan2( l_ptCoordsA[2], l_ptCoordsA[0] ) * ( 1 / Angle::Pi ) ) );
		real v = real( acos( l_ptCoordsA[1] ) * ( 1 / Angle::Pi ) );
		Vertex::SetTexCoord( l_submesh->GetPoint( l_face[0] ), u, v );
		u = real( 0.5 ) * ( real( 1.0 ) + real( atan2( l_ptCoordsB[2], l_ptCoordsB[0] ) * ( 1 / Angle::Pi ) ) );
		v = real( acos( l_ptCoordsB[1] ) * ( 1 / Angle::Pi ) );
		Vertex::SetTexCoord( l_submesh->GetPoint( l_face[1] ), u, v );
		u = real( 0.5 ) * ( real( 1.0 ) + real( atan2( l_ptCoordsC[2], l_ptCoordsC[0] ) * ( 1 / Angle::Pi ) ) );
		v = real( acos( l_ptCoordsC[1] ) * ( 1 / Angle::Pi ) );
		Vertex::SetTexCoord( l_submesh->GetPoint( l_face[2] ), u, v );
	}

	for ( auto & l_vertex : l_submesh->GetPoints() )
	{
		Point3r l_ptNml;
		Vertex::GetNormal( l_vertex, l_ptNml );
		SphericalVertex l_vsVertex1( l_ptNml );
		Vertex::SetTexCoord( l_vertex, l_vsVertex1.m_rPhi, l_vsVertex1.m_rTheta );
	}

	l_submesh->ComputeTangentsFromNormals();
	p_mesh.ComputeContainers();
}
