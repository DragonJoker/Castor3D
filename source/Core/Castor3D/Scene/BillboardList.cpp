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
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( c3d, BillboardBase )
CU_ImplementSmartPtr( c3d, BillboardList )

namespace c3d
{
	//*************************************************************************************************

	namespace billboard
	{
		struct Element
		{
			ArrayView< uint8_t > m_buffer;
			PointView3f m_position;
			uint32_t m_stride;

			Element( Element const & rhs ) = delete;
			Element & operator=( Element const & rhs ) = delete;
			Element( Element && rhs )noexcept = default;
			Element & operator=( Element && rhs )noexcept = default;
			~Element()noexcept = default;

			Element( uint8_t * buffer
				, uint32_t offset
				, uint32_t stride )noexcept
				: m_buffer{ buffer, buffer + stride }
				, m_position{ reinterpret_cast< float * >( buffer + offset ) }
				, m_stride{ stride }
			{
			}
		};

		static Point4fArray convert( Point3fArray const & src )
		{
			Point4fArray result;
			result.reserve( src.size() );

			for ( auto & value : src )
			{
				result.push_back( Point4f{ value->x, value->y, value->z, 1.0f } );
			}

			return result;
		}

		static CU_ImplementAttributeParserBlock( parserParent, BillboardsContext )
		{
			if ( blockContext->billboards )
			{
				auto name = getPrefixedName( params[0]->get< String >(), *blockContext );

				if ( auto parent = blockContext->scene->scene->findSceneNode( name ) )
				{
					parent->attachObject( *blockContext->billboards );
				}
				else
				{
					CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "Geometry not initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserType, BillboardsContext )
		{
			if ( !blockContext->billboards )
			{
				CU_ParsingError( cuT( "Billboard not initialised" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->billboards->setBillboardType( BillboardType( params[0]->get< uint32_t >() ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSize, BillboardsContext )
		{
			if ( !blockContext->billboards )
			{
				CU_ParsingError( cuT( "Billboard not initialised" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->billboards->setBillboardSize( BillboardSize( params[0]->get< uint32_t >() ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPositions, BillboardsContext )
		{
			// Only push the block
		}
		CU_EndAttributePushBlock( CSCNSection::eBillboardList, blockContext )

		static CU_ImplementAttributeParserBlock( parserMaterial, BillboardsContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( !blockContext->billboards )
			{
				CU_ParsingError( cuT( "Billboard not initialised" ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< String >(), *blockContext );

				if ( auto material = getEngine( *blockContext )->tryFindMaterial( name ) )
				{
					blockContext->billboards->setMaterial( material );
				}
				else
				{
					CU_ParsingError( cuT( "Material [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDimensions, BillboardsContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->billboards->setDimensions( params[0]->get< Point2f >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnd, BillboardsContext )
		{
			log::info << "Loaded billboards [" << blockContext->billboards->getName() << "]" << std::endl;

			if ( blockContext->ownBillboards )
			{
				blockContext->scene->scene->addBillboardList( blockContext->billboards->getName()
					, blockContext->ownBillboards
					, true );
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserPoint, BillboardsContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->billboards->addPoint( params[0]->get< Point3f >() );
			}
		}
		CU_EndAttribute()
	}

	//*************************************************************************************************

	BillboardBase::BillboardBase( Scene & scene
		, SceneNode * node
		, ashes::PipelineVertexInputStateCreateInfoPtr vertexLayout
		, uint32_t vertexStride
		, GpuBufferOffsetT< uint8_t > vertexBuffer )
		: m_scene{ scene }
		, m_node{ node }
		, m_vertexBuffer{ c3d::move( vertexBuffer ) }
		, m_vertexLayout{ c3d::move( vertexLayout ) }
		, m_vertexStride{ vertexStride }
		, m_proxyCombine{ scene.getEngine()->getSubmeshComponentsRegister().getDefaultComponentCombine() }
	{
	}

	bool BillboardBase::initialiseBase( RenderDevice const & device
		, uint32_t count )
	{
		if ( !m_initialised )
		{
			m_count = count;
			Quad vertices
			{
				BillboardVertex{ Point3f{ -0.5f, -0.5f, 1.0f }, Point2f{ 0.0f, 0.0f } },
				BillboardVertex{ Point3f{ -0.5f, +0.5f, 1.0f }, Point2f{ 0.0f, 1.0f } },
				BillboardVertex{ Point3f{ +0.5f, -0.5f, 1.0f }, Point2f{ 1.0f, 0.0f } },
				BillboardVertex{ Point3f{ +0.5f, +0.5f, 1.0f }, Point2f{ 1.0f, 1.0f } },
			};
			m_bufferOffsets = device.vertexPools->getBuffer< Quad >( 1u );
			auto const & vb = m_bufferOffsets.getBufferChunk( SubmeshData::ePositions );
			{
				auto queueData = device.graphicsData();
				InstantDirectUploadData uploader{ *queueData->queue
					, device, cuT( "BillboardBaseVBUpload" ), *queueData->commandPool };
				uploader->pushUpload( &vertices
					, sizeof( Quad )
					, vb.getBuffer(), vb.getOffset()
					, VertexAttributeInputState );
			}

			m_quadLayout = makeRawUnique< ashes::PipelineVertexInputStateCreateInfo >( 0u
				, ashes::VkVertexInputBindingDescriptionArray{ { 0u, sizeof( BillboardVertex ), VK_VERTEX_INPUT_RATE_VERTEX } }
				, ashes::VkVertexInputAttributeDescriptionArray{ { 0u, 0u, VK_FORMAT_R32G32B32_SFLOAT, offsetof( BillboardVertex, position ) }
					, { 1u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( BillboardVertex, uv ) } } );

			ashes::BufferCRefArray buffers;
			Vector< uint64_t > offsets;
			ashes::PipelineVertexInputStateCreateInfoCRefArray layouts;
			doGatherBuffers( buffers, offsets, layouts );

			m_geometryBuffers.buffers = buffers;
			m_geometryBuffers.offsets = offsets;
			m_geometryBuffers.layouts = layouts;

			auto stages = VkShaderStageFlags( VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT );
			ashes::VkDescriptorSetLayoutBindingArray bindings;
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::ePosition )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stages ) );
			m_descriptorLayout = device->createDescriptorSetLayout( "BillboardBaseVtx"
				, c3d::move( bindings ) );
			m_descriptorPool = m_descriptorLayout->createPool( "BillboardBaseVtx"
				, 1u );
			m_descriptorSet = m_descriptorPool->createDescriptorSet( "BillboardBaseVtx"
				, RenderPipeline::eMeshBuffers );
			ashes::WriteDescriptorSetArray writes;
			m_vertexBuffer.addDescriptorWriteT( writes, MeshBuffersIdx::ePosition );
			m_descriptorSet->setBindings( c3d::move( writes ) );
			m_descriptorSet->update();

			m_initialised = true;
		}

		return m_initialised;
	}

	void BillboardBase::cleanup( RenderDevice const & device )
	{
		if ( m_initialised )
		{
			m_initialised = false;
			m_descriptorSet.reset();
			m_descriptorPool.reset();
			m_descriptorLayout.reset();
			device.vertexPools->putBuffer< Quad >( m_bufferOffsets );
			m_bufferOffsets.reset();
			m_geometryBuffers.buffers.clear();
			m_geometryBuffers.offsets.clear();
			m_geometryBuffers.layouts.clear();
			m_quadLayout.reset();
			m_vertexLayout.reset();
			device.bufferPool->putBuffer( m_vertexBuffer );
		}
	}

	void BillboardBase::update( [[maybe_unused]] GpuUpdater const & updater )
	{
		if ( m_count && !m_gpuFilled )
		{
			auto gpuBuffer = m_vertexBuffer.getData().data();
			ByteArray copy{ gpuBuffer
				, gpuBuffer + ( size_t( m_vertexStride ) * m_count ) };
			Vector< billboard::Element > elements;
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
						return point::lengthSquared( a.m_position - m_cameraPosition )
							> point::lengthSquared( b.m_position - m_cameraPosition );
					} );

				for ( auto const & element : elements )
				{
					std::memcpy( gpuBuffer, element.m_buffer.data(), m_vertexStride );
					gpuBuffer += m_vertexStride;
				}

				m_vertexBuffer.markDirty( VertexAttributeInputState );
			}
			catch ( Exception const & exc )
			{
				log::error << cuT( "Submesh::SortFaces - Error: " ) << makeString( exc.what() ) << std::endl;
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
		, Vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts )
	{
		layouts.emplace_back( *m_quadLayout );
		buffers.emplace_back( m_bufferOffsets.getBuffer( SubmeshData::ePositions ).getBuffer() );
		offsets.emplace_back( 0u );

		layouts.emplace_back( *m_vertexLayout );
		buffers.emplace_back( m_vertexBuffer.getBuffer().getBuffer() );
		offsets.emplace_back( m_vertexBuffer.getOffset() );
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
			, makeRawUnique< ashes::PipelineVertexInputStateCreateInfo >( 0u
				, ashes::VkVertexInputBindingDescriptionArray{ { 1u, sizeof( Point4f ), VK_VERTEX_INPUT_RATE_INSTANCE } }
				, ashes::VkVertexInputAttributeDescriptionArray{ { 2u, 1u, VK_FORMAT_R32G32B32_SFLOAT, 0u } } )
			, sizeof( Point4f ) }
	{
	}
	
	BillboardList::BillboardList( String const & name
		, Scene & scene )
		: MovableObject( name
			, scene
			, MovableType::eBillboard )
		, BillboardBase{ scene
			, nullptr
			, makeRawUnique< ashes::PipelineVertexInputStateCreateInfo >( 0u
				, ashes::VkVertexInputBindingDescriptionArray{ { 1u, sizeof( Point4f ), VK_VERTEX_INPUT_RATE_INSTANCE } }
				, ashes::VkVertexInputAttributeDescriptionArray{ { 2u, 1u, VK_FORMAT_R32G32B32_SFLOAT, 0u } } )
			, sizeof( Point4f ) }
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

			m_vertexBuffer = device.bufferPool->getBuffer< uint8_t >( BufferUsageFlags::eVertexBuffer | BufferUsageFlags::eStorageBuffer
				, uint32_t( m_arrayPositions.size() ) * m_vertexStride
				, MemoryPropertyFlags::eDeviceLocal );

			auto * buffer = reinterpret_cast< Point4f * >( m_vertexBuffer.getData().data() );
			auto up = billboard::convert( m_arrayPositions );
			std::copy( up.begin(), up.end(), buffer );

			m_vertexBuffer.markDirty( VertexAttributeInputState );
		}

		return initialiseBase( device, uint32_t( m_arrayPositions.size() ) );
	}

	void BillboardList::removePoint( uint32_t index )
	{
		if ( index < m_arrayPositions.size() )
		{
			m_arrayPositions.erase( m_arrayPositions.begin() + index );
			m_needUpdate = true;
		}
	}

	void BillboardList::addPoint( Point3f const & position )
	{
		m_arrayPositions.push_back( position );
		m_needUpdate = true;
	}

	void BillboardList::addPoints( Point3fArray const & positions )
	{
		m_arrayPositions.insert( m_arrayPositions.end(), positions.begin(), positions.end() );
		m_needUpdate = true;
	}

	void BillboardList::attachTo( SceneNode & node )
	{
		MovableObject::attachTo( node );
		setNode( node );
	}

	void BillboardList::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< BillboardsContext > listCtx{ result, CSCNSection::eBillboard, CSCNSection::eScene };
		BlockParserContextT< BillboardsContext > billboardCtx{ result, CSCNSection::eBillboardList, CSCNSection::eBillboard };

		listCtx.addParser( cuT( "parent" ), billboard::parserParent, { makeParameter< ParameterType::eName >() } );
		listCtx.addParser( cuT( "type" ), billboard::parserType, { makeParameter < ParameterType::eCheckedText, BillboardType >() } );
		listCtx.addParser( cuT( "size" ), billboard::parserSize, { makeParameter < ParameterType::eCheckedText, BillboardSize >() } );
		listCtx.addParser( cuT( "material" ), billboard::parserMaterial, { makeParameter< ParameterType::eName >() } );
		listCtx.addParser( cuT( "dimensions" ), billboard::parserDimensions, { makeParameter< ParameterType::ePoint2F >() } );
		listCtx.addPushParser( cuT( "positions" ), CSCNSection::eBillboardList, billboard::parserPositions );
		listCtx.addPopParser( cuT( "}" ), billboard::parserEnd );

		billboardCtx.addParser( cuT( "pos" ), billboard::parserPoint, { makeParameter< ParameterType::ePoint3F >() } );
		billboardCtx.addDefaultPopParser();
	}

	//*************************************************************************************************
}
