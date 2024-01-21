#include "Castor3D/Scene/BillboardList.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/DirectUploadData.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/InstantUploadData.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Scene/Scene.hpp"

CU_ImplementSmartPtr( castor3d, BillboardBase )
CU_ImplementSmartPtr( castor3d, BillboardList )

namespace castor3d
{
	//*************************************************************************************************

	namespace billboard
	{
		struct Element
		{
			uint8_t * m_buffer;
			castor::Coords3f m_position;
			uint32_t m_stride;

			~Element()noexcept = default;

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
					m_stride = rhs.m_stride;
					rhs.m_buffer = nullptr;
				}
				return *this;
			}
		};

		static castor::Point4fArray convert( castor::Point3fArray const & src )
		{
			castor::Point4fArray result;
			result.reserve( src.size() );

			for ( auto & value : src )
			{
				result.push_back( castor::Point4f{ value->x, value->y, value->z, 1.0f } );
			}

			return result;
		}
	}

	//*************************************************************************************************

	BillboardBase::BillboardBase( Scene & scene
		, SceneNode * node
		, ashes::PipelineVertexInputStateCreateInfoPtr vertexLayout
		, uint32_t vertexStride
		, GpuBufferOffsetT< uint8_t > vertexBuffer )
		: m_scene{ scene }
		, m_node{ node }
		, m_vertexBuffer{ std::move( vertexBuffer ) }
		, m_vertexLayout{ std::move( vertexLayout ) }
		, m_vertexStride{ vertexStride }
		, m_proxyCombine{ scene.getEngine()->getSubmeshComponentsRegister().getDefaultComponentCombine() }
	{
	}

	bool BillboardBase::initialise( RenderDevice const & device
		, uint32_t count )
	{
		if ( !m_initialised )
		{
			m_count = count;
			Quad vertices
			{
				BillboardVertex{ castor::Point3f{ -0.5f, -0.5f, 1.0f }, castor::Point2f{ 0.0f, 0.0f } },
				BillboardVertex{ castor::Point3f{ -0.5f, +0.5f, 1.0f }, castor::Point2f{ 0.0f, 1.0f } },
				BillboardVertex{ castor::Point3f{ +0.5f, -0.5f, 1.0f }, castor::Point2f{ 1.0f, 0.0f } },
				BillboardVertex{ castor::Point3f{ +0.5f, +0.5f, 1.0f }, castor::Point2f{ 1.0f, 1.0f } },
			};
			m_bufferOffsets = device.vertexPools->getBuffer< Quad >( 1u );
			auto const & vb = m_bufferOffsets.getBufferChunk( SubmeshData::ePositions );
			{
				auto queueData = device.graphicsData();
				InstantDirectUploadData uploader{ *queueData->queue
					, device
					, "BillboardBase"
					, *queueData->commandPool };
				uploader->pushUpload( &vertices
					, sizeof( Quad )
					, vb.getBuffer()
					, vb.getOffset()
					, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
					, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
			}

			m_quadLayout = std::make_unique< ashes::PipelineVertexInputStateCreateInfo >( 0u
				, ashes::VkVertexInputBindingDescriptionArray{ { 0u, sizeof( BillboardVertex ), VK_VERTEX_INPUT_RATE_VERTEX } }
				, ashes::VkVertexInputAttributeDescriptionArray{ { 0u, 0u, VK_FORMAT_R32G32B32_SFLOAT, offsetof( BillboardVertex, position ) }
					, { 1u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( BillboardVertex, uv ) } } );

			ashes::BufferCRefArray buffers;
			std::vector< uint64_t > offsets;
			ashes::PipelineVertexInputStateCreateInfoCRefArray layouts;
			doGatherBuffers( buffers, offsets, layouts );

			m_geometryBuffers.buffers = buffers;
			m_geometryBuffers.offsets = offsets;
			m_geometryBuffers.layouts = layouts;
			m_initialised = true;
		}

		return m_initialised;
	}

	void BillboardBase::cleanup( RenderDevice const & device )
	{
		if ( m_initialised )
		{
			m_initialised = false;
			device.vertexPools->putBuffer( m_bufferOffsets );
			m_bufferOffsets.reset();
			m_geometryBuffers.buffers.clear();
			m_geometryBuffers.offsets.clear();
			m_geometryBuffers.layouts.clear();
			m_quadLayout.reset();
			m_vertexLayout.reset();
			device.bufferPool->putBuffer( m_vertexBuffer );
		}
	}

	void BillboardBase::update( GpuUpdater & updater )
	{
		if ( m_count && !m_gpuFilled )
		{
			auto gpuBuffer = m_vertexBuffer.getData().data();
			castor::ByteArray copy{ gpuBuffer
				, gpuBuffer + ( size_t( m_vertexStride ) * m_count ) };
			std::vector< billboard::Element > elements;
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
					, [this]( billboard::Element const & a
					, billboard::Element const & b )
					{
						return castor::point::lengthSquared( a.m_position - m_cameraPosition )
							> castor::point::lengthSquared( b.m_position - m_cameraPosition );
					} );

				for ( auto const & element : elements )
				{
					std::memcpy( gpuBuffer, element.m_buffer, m_vertexStride );
					gpuBuffer += m_vertexStride;
				}

				m_vertexBuffer.markDirty( VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
					, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
			}
			catch ( castor::Exception const & exc )
			{
				log::error << "Submesh::SortFaces - Error: " << exc.what() << std::endl;
			}
		}
	}

	ProgramFlags BillboardBase::getProgramFlags()const
	{
		return ProgramFlag::eBillboards;
	}

	uint32_t BillboardBase::getId( Pass const & pass )const
	{
		auto it = m_ids.find( &pass );
		return it == m_ids.end() ? 0u : it->second.first;
	}

	BillboardRenderNode const * BillboardBase::getRenderNode( Pass const & pass )const
	{
		auto it = m_ids.find( &pass );
		return it == m_ids.end() ? nullptr : it->second.second;
	}

	void BillboardBase::fillData( BillboardUboConfiguration & data )const
	{
		data.dimensions = getDimensions();
		data.isSpherical = getBillboardType() == BillboardType::eSpherical ? 1u : 0u;
		data.isFixedSize = getBillboardSize() == BillboardSize::eFixed ? 1u : 0u;
	}

	void BillboardBase::setId( Pass const & pass
		, BillboardRenderNode const * renderNode
		, uint32_t id )
	{
		m_ids[&pass] = { id, renderNode };
	}

	void BillboardBase::setMaterial( MaterialObs value )
	{
		auto oldMaterial = getMaterial();

		if ( value )
		{
			value->initialise();
		}

		if ( oldMaterial != value )
		{
			if ( oldMaterial )
			{
				if ( value )
				{
					getParentScene().getRenderNodes().reportPassChange( *this
						, *oldMaterial
						, *value );
				}

				m_material = value;

				for ( auto const & pass : *oldMaterial )
				{
					if ( auto itPass = m_ids.find( pass.get() );
						itPass != m_ids.end() )
					{
						m_ids.erase( itPass );
					}
				}
			}
			else
			{
				m_material = value;
			}

			getParentScene().markDirty( *this );
		}
	}

	void BillboardBase::setCount( uint32_t value )
	{
		if ( m_count != value )
		{
			m_count = value;
			getParentScene().markDirty( *this );
		}
	}

	void BillboardBase::doGatherBuffers( ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts )
	{
		layouts.emplace_back( *m_quadLayout );
		buffers.emplace_back( m_bufferOffsets.getBuffer( SubmeshData::ePositions ) );
		offsets.emplace_back( 0u );

		layouts.emplace_back( *m_vertexLayout );
		buffers.emplace_back( m_vertexBuffer.getBuffer().getBuffer() );
		offsets.emplace_back( m_vertexBuffer.getOffset() );
	}

	//*************************************************************************************************

	BillboardList::BillboardList( castor::String const & name
		, Scene & scene
		, SceneNode & node )
		: MovableObject( name
			, scene
			, MovableType::eBillboard
			, node )
		, BillboardBase{ scene
			, &node
			, std::make_unique< ashes::PipelineVertexInputStateCreateInfo >( 0u
				, ashes::VkVertexInputBindingDescriptionArray{ { 1u, sizeof( castor::Point4f ), VK_VERTEX_INPUT_RATE_INSTANCE } }
				, ashes::VkVertexInputAttributeDescriptionArray{ { 2u, 1u, VK_FORMAT_R32G32B32_SFLOAT, 0u } } )
			, sizeof( castor::Point4f ) }
	{
	}
	
	BillboardList::BillboardList( castor::String const & name
		, Scene & scene )
		: MovableObject( name
			, scene
			, MovableType::eBillboard )
		, BillboardBase{ scene
			, nullptr
			, std::make_unique< ashes::PipelineVertexInputStateCreateInfo >( 0u
				, ashes::VkVertexInputBindingDescriptionArray{ { 1u, sizeof( castor::Point4f ), VK_VERTEX_INPUT_RATE_INSTANCE } }
				, ashes::VkVertexInputAttributeDescriptionArray{ { 2u, 1u, VK_FORMAT_R32G32B32_SFLOAT, 0u } } )
			, sizeof( castor::Point4f ) }
	{
	}

	bool BillboardList::initialise( RenderDevice const & device )
	{
		if ( !m_vertexLayout
			|| !m_vertexBuffer
			|| m_arrayPositions.size() > m_vertexBuffer.getSize() )
		{
			if ( m_vertexBuffer )
			{
				device.bufferPool->putBuffer( m_vertexBuffer );
			}

			m_vertexBuffer = device.bufferPool->getBuffer< uint8_t >( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
				, uint32_t( m_arrayPositions.size() ) * m_vertexStride
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

			auto * buffer = reinterpret_cast< castor::Point4f * >( m_vertexBuffer.getData().data() );
			auto up = billboard::convert( m_arrayPositions );
			std::copy( up.begin()
				, up.end()
				, buffer );

			m_vertexBuffer.markDirty( VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
		}

		return BillboardBase::initialise( device, uint32_t( m_arrayPositions.size() ) );
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
