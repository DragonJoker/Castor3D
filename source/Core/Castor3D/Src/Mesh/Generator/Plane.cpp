#include "Plane.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Miscellaneous/Parameter.hpp"

using namespace Castor3D;
using namespace Castor;

Plane::Plane()
	: MeshGenerator( cuT( "plane" ) )
	, m_depth( 0 )
	, m_width( 0 )
	, m_subDivisionsW( 0 )
	, m_subDivisionsD( 0 )
{
}

Plane::~Plane()
{
}

MeshGeneratorSPtr Plane::Create()
{
	return std::make_shared< Plane >();
}

void Plane::DoGenerate( Mesh & p_mesh, Parameters const & p_parameters )
{
	String l_param;

	if ( p_parameters.Get( cuT( "width_subdiv" ), l_param ) )
	{
		m_subDivisionsW = string::to_uint( l_param );
	}

	if ( p_parameters.Get( cuT( "depth_subdiv" ), l_param ) )
	{
		m_subDivisionsD = string::to_uint( l_param );
	}

	if ( p_parameters.Get( cuT( "width" ), l_param ) )
	{
		m_width = string::to_float( l_param );
	}

	if ( p_parameters.Get( cuT( "depth" ), l_param ) )
	{
		m_depth = string::to_float( l_param );
	}

	uint32_t l_nbVertexW = m_subDivisionsW + 2;
	uint32_t l_nbVertexH = m_subDivisionsD + 2;
	real l_offsetW = -m_width / 2;
	real l_offsetH = -m_depth / 2;
	real l_gapW = m_width / ( m_subDivisionsW + 1 );
	real l_gapH = m_depth / ( m_subDivisionsD + 1 );
	Point3r l_ptCurrentUV;
	Point3r l_ptPreviousUV;
	BufferElementGroupSPtr l_vertex;
	Point3r l_ptNormal( 0.0, 1.0, 0.0 );
	Point3r l_ptTangent;
	Point2r l_ptUv;
	SubmeshSPtr l_submesh = p_mesh.CreateSubmesh();

	for ( uint32_t i = 0; i < l_nbVertexW; i++ )
	{
		for ( uint32_t j = 0; j < l_nbVertexH; j++ )
		{
			l_vertex = l_submesh->AddPoint( l_offsetW + ( i * l_gapW ), l_offsetH + ( j * l_gapH ), 0.0 );
			Vertex::SetTexCoord( l_vertex, ( i * l_gapW / m_width ), ( j * l_gapH / m_depth ) );
			Vertex::SetNormal( l_vertex, 0.0, 0.0, 1.0 );
		}
	}

	for ( uint32_t i = 0; i < m_subDivisionsW + 1; i++ )
	{
		for ( uint32_t j = i * ( m_subDivisionsD + 1 ); j < ( i + 1 ) * ( m_subDivisionsD + 1 ); j++ )
		{
			l_submesh->AddFace( j + i, j + m_subDivisionsW + 2 + i, j + m_subDivisionsW + 3 + i );
			l_submesh->AddFace( j + m_subDivisionsW + 3 + i, j + i + 1, j + i );
		}
	}

	//ComputeNormals();
	l_submesh->ComputeTangentsFromNormals();
	p_mesh.ComputeContainers();
}
