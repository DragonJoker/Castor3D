#include "Plane.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Miscellaneous/Parameter.hpp"

using namespace castor3d;
using namespace castor;

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

MeshGeneratorSPtr Plane::create()
{
	return std::make_shared< Plane >();
}

void Plane::doGenerate( Mesh & p_mesh, Parameters const & p_parameters )
{
	String param;

	if ( p_parameters.get( cuT( "width_subdiv" ), param ) )
	{
		m_subDivisionsW = string::toUInt( param );
	}

	if ( p_parameters.get( cuT( "depth_subdiv" ), param ) )
	{
		m_subDivisionsD = string::toUInt( param );
	}

	if ( p_parameters.get( cuT( "width" ), param ) )
	{
		m_width = string::toFloat( param );
	}

	if ( p_parameters.get( cuT( "depth" ), param ) )
	{
		m_depth = string::toFloat( param );
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
	SubmeshSPtr submesh = p_mesh.createSubmesh();

	for ( uint32_t i = 0; i < nbVertexW; i++ )
	{
		for ( uint32_t j = 0; j < nbVertexH; j++ )
		{
			vertex = submesh->addPoint( offsetW + ( i * gapW ), offsetH + ( j * gapH ), 0.0 );
			Vertex::setTexCoord( vertex, ( i * gapW / m_width ), ( j * gapH / m_depth ) );
			Vertex::setNormal( vertex, 0.0, 0.0, 1.0 );
		}
	}

	auto indexMapping = std::make_shared< TriFaceMapping >( *submesh );

	for ( uint32_t i = 0; i < m_subDivisionsW + 1; i++ )
	{
		for ( uint32_t j = i * ( m_subDivisionsD + 1 ); j < ( i + 1 ) * ( m_subDivisionsD + 1 ); j++ )
		{
			indexMapping->addFace( j + i, j + m_subDivisionsW + 2 + i, j + m_subDivisionsW + 3 + i );
			indexMapping->addFace( j + m_subDivisionsW + 3 + i, j + i + 1, j + i );
		}
	}

	indexMapping->computeTangentsFromNormals();
	submesh->setIndexMapping( indexMapping );
	p_mesh.computeContainers();
}
