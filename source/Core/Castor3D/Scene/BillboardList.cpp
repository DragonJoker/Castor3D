#include "Castor3D/Scene/BillboardList.hpp"

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Scene/Scene.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	BillboardList::TextWriter::TextWriter( String const & tabs )
		: MovableObject::TextWriter{ tabs }
	{
	}

	bool BillboardList::TextWriter::operator()( BillboardList const & obj, castor::TextFile & file )
	{
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "billboard \"" ) + obj.getName() + cuT( "\"\n" ) ) > 0
						&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		MovableObject::TextWriter::checkError( result, "BillboardList name" );

		if ( result )
		{
			result = MovableObject::TextWriter{ m_tabs + cuT( "\t" ) }( obj, file );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tmaterial \"" ) + obj.getMaterial()->getName() + cuT( "\"\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "BillboardList material" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tdimensions " )
				+ string::toString( obj.getDimensions()[0], std::locale{ "C" } ) + cuT( " " )
				+ string::toString( obj.getDimensions()[1], std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "BillboardList dimensions" );
		}

		if ( result && obj.getCount() )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\tpositions\n" ) ) > 0
					   && file.writeText( m_tabs + cuT( "\t{\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "BillboardList positions" );

			for ( auto const & point : obj )
			{
				result = file.writeText( m_tabs + cuT( "\t\tpos " )
					+ string::toString( point[0], std::locale{ "C" } ) + cuT( " " )
					+ string::toString( point[1], std::locale{ "C" } ) + cuT( " " )
					+ string::toString( point[2], std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				MovableObject::TextWriter::checkError( result, "BillboardList position" );
			}

			result = result && ( file.writeText( m_tabs + cuT( "\t}\n" ) ) > 0 );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*************************************************************************************************

	BillboardBase::BillboardBase( Scene & scene
		, SceneNode * node
		, ashes::PipelineVertexInputStateCreateInfoPtr vertexLayout
		, uint32_t vertexStride
		, ashes::VertexBufferBasePtr vertexBuffer )
		: m_vertexLayout{ std::move( vertexLayout ) }
		, m_vertexStride{ vertexStride }
		, m_vertexBuffer{ vertexBuffer ? std::move( vertexBuffer ) : nullptr }
		, m_scene{ scene }
		, m_node{ node }
	{
	}

	BillboardBase::~BillboardBase()
	{
	}

	bool BillboardBase::initialise( uint32_t count )
	{
		if ( !m_initialised )
		{
			m_count = count;
			Quad vertices
			{
				Vertex{ castor::Point3f{ -0.5f, -0.5f, 1.0f }, castor::Point2f{ 0.0f, 0.0f } },
				Vertex{ castor::Point3f{ -0.5f, +0.5f, 1.0f }, castor::Point2f{ 0.0f, 1.0f } },
				Vertex{ castor::Point3f{ +0.5f, -0.5f, 1.0f }, castor::Point2f{ 1.0f, 0.0f } },
				Vertex{ castor::Point3f{ +0.5f, +0.5f, 1.0f }, castor::Point2f{ 1.0f, 1.0f } },
			};
			auto & device = getCurrentRenderDevice( m_scene );
			m_quadBuffer = makeVertexBuffer< Quad >( device
				, 1u
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "BillboardQuad" );

			if ( auto buffer = m_quadBuffer->lock( 0u, 1u, 0u ) )
			{
				*buffer = vertices;
				m_quadBuffer->flush( 0u, 1u );
				m_quadBuffer->unlock();
			}

			m_quadLayout = std::make_unique< ashes::PipelineVertexInputStateCreateInfo >( 0u
				, ashes::VkVertexInputBindingDescriptionArray
				{
					{ 0u, sizeof( Vertex ), VK_VERTEX_INPUT_RATE_VERTEX },
				}
				, ashes::VkVertexInputAttributeDescriptionArray
				{
					{ 0u, 0u, VK_FORMAT_R32G32B32_SFLOAT, offsetof( Vertex, position ) },
					{ 1u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( Vertex, uv ) },
				} );

			ashes::BufferCRefArray buffers;
			std::vector< uint64_t > offsets;
			ashes::PipelineVertexInputStateCreateInfoCRefArray layouts;
			doGatherBuffers( buffers, offsets, layouts );

			m_geometryBuffers.vbo = buffers;
			m_geometryBuffers.vboOffsets = offsets;
			m_geometryBuffers.layouts = layouts;
			m_geometryBuffers.vtxCount = 4u;
			m_initialised = true;
		}

		return m_initialised;
	}

	void BillboardBase::cleanup()
	{
		if ( m_initialised )
		{
			m_initialised = false;
			m_geometryBuffers.vbo.clear();
			m_geometryBuffers.vboOffsets.clear();
			m_geometryBuffers.layouts.clear();
			m_geometryBuffers.ibo = nullptr;
			m_geometryBuffers.iboOffset = 0u;
			m_geometryBuffers.idxCount = 0u;
			m_geometryBuffers.vtxCount = 0u;
			m_quadLayout.reset();
			m_quadBuffer.reset();
			m_vertexLayout.reset();
			m_vertexBuffer.reset();
		}
	}

	void BillboardBase::sortByDistance( castor::Point3f const & cameraPosition )
	{
		m_needUpdate = m_cameraPosition != cameraPosition;
		m_cameraPosition = cameraPosition;
	}

	void BillboardBase::update()
	{
		if ( m_count )
		{
			auto & device = getCurrentRenderDevice( getParentScene() );
			auto mappedSize = ashes::getAlignedSize( VkDeviceSize( m_count ) * m_vertexStride
				, device.properties.limits.nonCoherentAtomSize );

			if ( auto gpuBuffer = m_vertexBuffer->getBuffer().lock( 0
				, mappedSize
				, 0u ) )
			{
				struct Element
				{
					uint8_t * m_buffer;
					castor::Coords3f m_position;
					uint32_t m_stride;

					Element( uint8_t * buffer
						, uint32_t offset
						, uint32_t stride )
						: m_buffer{ buffer }
						, m_position{ reinterpret_cast< float * >( buffer + offset ) }
						, m_stride{ stride }
					{
					}

					Element( Element const & rhs )
						: m_buffer{ rhs.m_buffer }
						, m_position{ rhs.m_position }
						, m_stride{ rhs.m_stride }
					{
					}

					Element( Element && rhs )noexcept
						: m_buffer{ rhs.m_buffer }
						, m_position{ std::move( rhs.m_position ) }
						, m_stride{ rhs.m_stride }
					{
						rhs.m_buffer = nullptr;
					}

					Element & operator=( Element const & rhs )
					{
						std::memcpy( m_buffer, rhs.m_buffer, m_stride );
						return *this;
					}

					Element & operator=( Element && rhs )noexcept
					{
						if ( &rhs != this )
						{
							m_buffer = rhs.m_buffer;
							m_position = std::move( rhs.m_position );
							m_stride = std::move( rhs.m_stride );
							rhs.m_buffer = nullptr;
						}
						return *this;
					}
				};

				ByteArray copy{ gpuBuffer, gpuBuffer + ( size_t( m_vertexStride ) * m_count ) };
				std::vector< Element > elements;
				auto buffer = copy.data();
				elements.reserve( m_count );

				for ( uint32_t i = 0u; i < m_count; ++i )
				{
					elements.emplace_back( buffer, m_centerOffset, m_vertexStride );
					buffer += m_vertexStride;
				}

				try
				{
					std::sort( elements.begin()
						, elements.end()
						, [this]( Element const & p_a
						, Element const & p_b )
						{
							return point::lengthSquared( p_a.m_position - m_cameraPosition )
								> point::lengthSquared( p_b.m_position - m_cameraPosition );
						} );

					for ( auto & element : elements )
					{
						std::memcpy( gpuBuffer, element.m_buffer, m_vertexStride );
						gpuBuffer += m_vertexStride;
					}
				}
				catch ( Exception const & p_exc )
				{
					log::error << "Submesh::SortFaces - Error: " << p_exc.what() << std::endl;
				}

				m_vertexBuffer->getBuffer().flush( 0u, mappedSize );
				m_vertexBuffer->getBuffer().unlock();
			}
		}
	}

	ProgramFlags BillboardBase::getProgramFlags()const
	{
		ProgramFlags result = ProgramFlag::eBillboards;

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

	void BillboardBase::setMaterial( MaterialSPtr value )
	{
		MaterialSPtr oldMaterial = getMaterial();

		if ( oldMaterial != value )
		{
			m_material = value;

			if ( oldMaterial )
			{
				onMaterialChanged( *this, oldMaterial, value );
			}
		}
	}

	void BillboardBase::setCount( uint32_t value )
	{
		if ( m_count != value )
		{
			m_count = value;
			getParentScene().setChanged();
		}
	}

	void BillboardBase::doGatherBuffers( ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts )
	{
		buffers.emplace_back( m_quadBuffer->getBuffer() );
		offsets.emplace_back( 0u );
		layouts.emplace_back( *m_quadLayout );
		buffers.emplace_back( m_vertexBuffer->getBuffer() );
		offsets.emplace_back( 0u );
		layouts.emplace_back( *m_vertexLayout );
	}

	//*************************************************************************************************

	BillboardList::BillboardList( String const & name
		, Scene & scene
		, SceneNode & node )
		: MovableObject( name
			, scene
			, MovableType::eBillboard
			, node )
		, BillboardBase{ scene
			, &node
			, std::make_unique< ashes::PipelineVertexInputStateCreateInfo >( 0u
				, ashes::VkVertexInputBindingDescriptionArray
				{
					{ 1u, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_INSTANCE },
				}
				, ashes::VkVertexInputAttributeDescriptionArray
				{
					{ 2u, 1u, VK_FORMAT_R32G32B32_SFLOAT, 0u },
				} )
			, sizeof( castor::Point3f ) }
	{
	}
	
	BillboardList::BillboardList( String const & name
		, Scene & scene )
		: MovableObject( name
			, scene
			, MovableType::eBillboard )
		, BillboardBase{ scene
			, nullptr
			, std::make_unique< ashes::PipelineVertexInputStateCreateInfo >( 0u
				, ashes::VkVertexInputBindingDescriptionArray
				{
					{ 1u, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_INSTANCE },
				}
				, ashes::VkVertexInputAttributeDescriptionArray
				{
					{ 2u, 1u, VK_FORMAT_R32G32B32_SFLOAT, 0u },
				} )
			, sizeof( castor::Point3f ) }
	{
	}

	BillboardList::~BillboardList()
	{
	}

	bool BillboardList::initialise()
	{
		if ( !m_vertexLayout
			|| !m_vertexBuffer
			|| m_arrayPositions.size() != m_vertexBuffer->getSize() )
		{
			auto & device = getCurrentRenderDevice( m_scene );
			m_vertexBuffer = makeVertexBuffer< castor::Point3f >( device
				, uint32_t( m_arrayPositions.size() )
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, getName() + "Billboard" );

			if ( auto * buffer = m_vertexBuffer->getBuffer().lock( 0u
				, uint32_t( m_vertexStride * m_arrayPositions.size() )
				, 0u ) )
			{
				for ( auto & pos : m_arrayPositions )
				{
					std::memcpy( buffer, pos.constPtr(), m_vertexStride );
					buffer += m_vertexStride;
				}

				m_vertexBuffer->getBuffer().flush( 0u, uint32_t( m_vertexStride * m_arrayPositions.size() ) );
				m_vertexBuffer->getBuffer().unlock();
			}
		}

		return BillboardBase::initialise( uint32_t( m_arrayPositions.size() ) );
	}

	void BillboardList::removePoint( uint32_t index )
	{
		if ( index < m_arrayPositions.size() )
		{
			m_arrayPositions.erase( m_arrayPositions.begin() + index );
			m_needUpdate = true;
		}
	}

	void BillboardList::addPoint( castor::Point3f const & position )
	{
		m_arrayPositions.push_back( position );
		m_needUpdate = true;
	}

	void BillboardList::addPoints( castor::Point3fArray const & positions )
	{
		m_arrayPositions.insert( m_arrayPositions.end(), positions.begin(), positions.end() );
		m_needUpdate = true;
	}

	void BillboardList::attachTo( SceneNode & node )
	{
		MovableObject::attachTo( node );
		setNode( node );
	}

	//*************************************************************************************************
}
