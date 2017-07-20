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
	String param;

	if ( p_parameters.Get( cuT( "width_subdiv" ), param ) )
	{
		m_subDivisionsW = string::to_uint( param );
	}

	if ( p_parameters.Get( cuT( "depth_subdiv" ), param ) )
	{
		m_subDivisionsD = string::to_uint( param );
	}

	if ( p_parameters.Get( cuT( "width" ), param ) )
	{
		m_width = string::to_float( param );
	}

	if ( p_parameters.Get( cuT( "depth" ), param ) )
	{
		m_depth = string::to_float( param );
	}

	uint32_t nbVertexW = m_subDivisionsW + 2;
	uint32_t nbVertexH = m_subDivisionsD + 2;
	real offsetW = -m_width / 2;
	real offsetH = -m_depth / 2;
	real gapW = m_width / ( m_subDivisionsW + 1 );
	real gapH = m_depth / ( m_subDivisionsD + 1 );
	Point3r ptCurrentUV;
	Point3r ptPreviousUV;
	BufferElementGroupSPtr vertex;
	Point3r ptNormal( 0.0, 1.0, 0.0 );
	Point3r ptTangent;
	Point2r ptUv;
	SubmeshSPtr submesh = p_mesh.CreateSubmesh();

	for ( uint32_t i = 0; i < nbVertexW; i++ )
	{
		for ( uint32_t j = 0; j < nbVertexH; j++ )
		{
			vertex = submesh->AddPoint( offsetW + ( i * gapW ), offsetH + ( j * gapH ), 0.0 );
			Vertex::SetTexCoord( vertex, ( i * gapW / m_width ), ( j * gapH / m_depth ) );
			Vertex::SetNormal( vertex, 0.0, 0.0, 1.0 );
		}
	}

	for ( uint32_t i = 0; i < m_subDivisionsW + 1; i++ )
	{
		for ( uint32_t j = i * ( m_subDivisionsD + 1 ); j < ( i + 1 ) * ( m_subDivisionsD + 1 ); j++ )
		{
			submesh->AddFace( j + i, j + m_subDivisionsW + 2 + i, j + m_subDivisionsW + 3 + i );
			submesh->AddFace( j + m_subDivisionsW + 3 + i, j + i + 1, j + i );
		}
	}

	//ComputeNormals();
	submesh->ComputeTangentsFromNormals();
	p_mesh.ComputeContainers();
}
