#include "BillboardList.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Material/Material.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderProgram.hpp"

#include <Design/ArrayView.hpp>

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	BillboardList::TextWriter::TextWriter( String const & p_tabs )
		: MovableObject::TextWriter{ p_tabs }
	{
	}

	bool BillboardList::TextWriter::operator()( BillboardList const & p_obj, castor::TextFile & p_file )
	{
		bool result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "billboard \"" ) + p_obj.getName() + cuT( "\"\n" ) ) > 0
						&& p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		MovableObject::TextWriter::checkError( result, "BillboardList name" );

		if ( result )
		{
			result = MovableObject::TextWriter{ m_tabs + cuT( "\t" ) }( p_obj, p_file );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tmaterial \"" ) + p_obj.getMaterial()->getName() + cuT( "\"\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "BillboardList material" );
		}

		if ( result )
		{
			result = p_file.print( 256, cuT( "%s\tdimensions %d %d\n" ), m_tabs.c_str(), p_obj.getDimensions().getWidth(), p_obj.getDimensions().getHeight() ) > 0;
			MovableObject::TextWriter::checkError( result, "BillboardList dimensions" );
		}

		if ( result && p_obj.getCount() )
		{
			result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "\tpositions\n" ) ) > 0
					   && p_file.writeText( m_tabs + cuT( "\t{\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "BillboardList positions" );

			for ( auto const & point : p_obj )
			{
				result &= p_file.print( 256, cuT( "%s\t\tpos %f %f %f" ), m_tabs.c_str(), point[0], point[1], point[2] ) > 0;
				MovableObject::TextWriter::checkError( result, "BillboardList position" );
			}

			result &= p_file.writeText( m_tabs + cuT( "\t}\n" ) ) > 0;
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*************************************************************************************************

	BillboardBase::BillboardBase( Scene & p_scene
								  , SceneNodeSPtr p_node
								  , VertexBufferSPtr p_vertexBuffer )
		: m_vertexBuffer{ p_vertexBuffer }
		, m_scene{ p_scene }
		, m_node{ p_node }
		, m_quad( std::make_unique< VertexBuffer >( *p_scene.getEngine(), BufferDeclaration
		{
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec3 ),
				BufferElementDeclaration( ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 ),
			}
		} ) )
	{
	}

	BillboardBase::~BillboardBase()
	{
	}

	bool BillboardBase::initialise( uint32_t p_count )
	{
		if ( !m_initialised )
		{
			m_count = p_count;
			uint32_t stride = m_vertexBuffer->getDeclaration().stride();

			if ( m_vertexBuffer->getSize() < uint32_t( p_count * stride ) )
			{
				m_vertexBuffer->resize( uint32_t( p_count * stride ) );
			}

			m_initialised = m_vertexBuffer->initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );

			if ( m_initialised )
			{
				std::array< std::array< float, 5 >, 4 > vertices
				{
					{
						std::array< float, 5 >{ { -0.5f, 0.5f, 1.0f, 0.0f, 1.0f } },
						std::array< float, 5 >{ { -0.5f, -0.5f, 1.0f, 0.0f, 0.0f } },
						std::array< float, 5 >{ { 0.5f, -0.5f, 1.0f, 1.0f, 0.0f } },
						std::array< float, 5 >{ { 0.5f, 0.5f, 1.0f, 1.0f, 1.0f } },
					}
				};

				stride = m_quad->getDeclaration().stride();
				m_quad->resize( 4 * stride );
				auto buffer = m_quad->getData();

				for ( auto & vertex : vertices )
				{
					std::memcpy( buffer, vertex.data(), stride );
					buffer += stride;
				}

				m_initialised = m_quad->initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
			}
		}

		return m_initialised;
	}

	void BillboardBase::cleanup()
	{
		if ( m_initialised )
		{
			m_initialised = false;

			for ( auto buffers : m_geometryBuffers )
			{
				buffers->cleanup();
			}

			m_geometryBuffers.clear();
			m_quad->cleanup();
			m_vertexBuffer->cleanup();
		}
	}

	void BillboardBase::draw( GeometryBuffers const & p_geometryBuffers )
	{
		if ( m_needUpdate )
		{
			update();
			m_needUpdate = false;
		}

		p_geometryBuffers.drawInstanced( 4, 0u, m_count );
	}

	GeometryBuffersSPtr BillboardBase::getGeometryBuffers( ShaderProgram const & p_program )
	{
		GeometryBuffersSPtr buffers;
		auto it = std::find_if( std::begin( m_geometryBuffers ), std::end( m_geometryBuffers ), [&p_program]( GeometryBuffersSPtr p_buffers )
		{
			return &p_buffers->getProgram() == &p_program;
		} );

		if ( it == m_geometryBuffers.end() )
		{
			buffers = getParentScene().getEngine()->getRenderSystem()->createGeometryBuffers( Topology::eTriangleFan, p_program );

			getParentScene().getListener().postEvent( makeFunctorEvent( EventType::ePreRender, [this, buffers]()
			{
				buffers->initialise( { *m_quad, *m_vertexBuffer }, nullptr );
			} ) );
			m_geometryBuffers.push_back( buffers );
		}
		else
		{
			buffers = *it;
		}

		return buffers;
	}

	void BillboardBase::sortByDistance( Point3r const & p_cameraPosition )
	{
		m_needUpdate = m_cameraPosition != p_cameraPosition;
		m_cameraPosition = p_cameraPosition;
	}

	void BillboardBase::update()
	{
		if ( m_count )
		{
			m_vertexBuffer->bind();
			uint32_t stride = m_vertexBuffer->getDeclaration().stride();
			auto gpuBuffer = m_vertexBuffer->lock( 0, m_count * stride, AccessType::eRead | AccessType::eWrite );

			if ( gpuBuffer )
			{
				struct Element
				{
					uint8_t * m_buffer;
					Coords3r m_position;
					uint32_t m_stride;

					Element( uint8_t * p_buffer
						, uint32_t p_offset
						, uint32_t p_stride )
						: m_buffer{ p_buffer }
						  , m_position{ reinterpret_cast< real * >( p_buffer + p_offset ) }
						  , m_stride{ p_stride }
					{
					}

					Element( Element const & p_rhs )
						: m_buffer{ p_rhs.m_buffer }
						  , m_position{ p_rhs.m_position }
						  , m_stride{ p_rhs.m_stride }
					{
					}

					Element( Element && p_rhs )
						: m_buffer{ p_rhs.m_buffer }
						  , m_position{ std::move( p_rhs.m_position ) }
						  , m_stride{ p_rhs.m_stride }
					{
						p_rhs.m_buffer = nullptr;
					}

					Element & operator=( Element const & p_rhs )
					{
						std::memcpy( m_buffer, p_rhs.m_buffer, m_stride );
						return *this;
					}

					Element & operator=( Element && p_rhs )
					{
						if ( &p_rhs != this )
						{
							m_buffer = p_rhs.m_buffer;
							m_position = std::move( p_rhs.m_position );
							p_rhs.m_buffer = nullptr;
						}
						return *this;
					}
				};

				ByteArray copy{ gpuBuffer, gpuBuffer + ( stride * m_count ) };
				std::vector< Element > elements;
				auto buffer = copy.data();
				elements.reserve( m_count );

				for ( uint32_t i = 0u; i < m_count; ++i )
				{
					elements.emplace_back( buffer, m_centerOffset, stride );
					buffer += stride;
				}

				try
				{
					std::sort( elements.begin(), elements.end(), [this]( Element const & p_a
						, Element const & p_b )
						{
							return point::lengthSquared( p_a.m_position - m_cameraPosition )
								   > point::lengthSquared( p_b.m_position - m_cameraPosition );
						} );

					for ( auto & element : elements )
					{
						std::memcpy( gpuBuffer, element.m_buffer, stride );
						gpuBuffer += stride;
					}
				}
				catch ( Exception const & p_exc )
				{
					Logger::logError( std::stringstream() << "Submesh::SortFaces - Error: " << p_exc.what());
				}

				m_vertexBuffer->unlock();
			}

			m_vertexBuffer->unbind();
		}
	}

	ProgramFlags BillboardBase::getProgramFlags()const
	{
		ProgramFlags result = uint32_t( ProgramFlag::eBillboards );

		if ( m_billboardType == BillboardType::eSpherical )
		{
			addFlag( result, ProgramFlag::eSpherical );
		}

		if ( m_billboardSize == BillboardSize::eFixed )
		{
			addFlag( result, ProgramFlag::eFixedSize );
		}

		return result;
	}

	//*************************************************************************************************

	BillboardList::BillboardList( String const & p_name
								  , Scene & p_scene
								  , SceneNodeSPtr p_parent )
		: MovableObject(
			p_name,
			p_scene,
			MovableType::eBillboard,
			p_parent )
		, BillboardBase( 
			p_scene,
			p_parent,
			std::make_shared< VertexBuffer >(
				*p_scene.getEngine(),
				BufferDeclaration(
					{ BufferElementDeclaration( cuT( "center" ), uint32_t( 0u ), ElementType::eVec3, 0u, 1u ) }
				) ) )
	{
	}

	BillboardList::~BillboardList()
	{
	}

	bool BillboardList::initialise()
	{
		uint32_t stride = m_vertexBuffer->getDeclaration().stride();
		m_vertexBuffer->resize( uint32_t( m_arrayPositions.size() * stride ) );
		uint8_t * buffer = m_vertexBuffer->getData();

		for ( auto & pos : m_arrayPositions )
		{
			std::memcpy( buffer, pos.constPtr(), stride );
			buffer += stride;
		}

		return BillboardBase::initialise( uint32_t( m_arrayPositions.size() ) );
	}

	void BillboardList::RemovePoint( uint32_t p_index )
	{
		if ( p_index < m_arrayPositions.size() )
		{
			m_arrayPositions.erase( m_arrayPositions.begin() + p_index );
			m_needUpdate = true;
		}
	}

	void BillboardList::addPoint( castor::Point3r const & p_position )
	{
		m_arrayPositions.push_back( p_position );
		m_needUpdate = true;
	}

	void BillboardList::addPoints( castor::Point3rArray const & p_ptPositions )
	{
		m_arrayPositions.insert( m_arrayPositions.end(), p_ptPositions.begin(), p_ptPositions.end() );
		m_needUpdate = true;
	}

	void BillboardList::attachTo( SceneNodeSPtr p_node )
	{
		MovableObject::attachTo( p_node );
		setNode( p_node );
	}

	//*************************************************************************************************
}
