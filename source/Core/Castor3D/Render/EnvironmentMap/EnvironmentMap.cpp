#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMapPass.hpp"
#include "Castor3D/Render/PBR/IblTextures.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Background/Background.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Miscellaneous/BitSize.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnableGraph.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static Size const MapSize{ EnvironmentMap::Size, EnvironmentMap::Size };

		Texture createTexture( RenderDevice const & device
			, crg::ResourceHandler & handler
			, std::string const & name
			, Size const & size )
		{
			return Texture{ device
				, handler
				, name
				, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
				, makeExtent3D( size )
				, 6u * EnvironmentMap::Count
				, uint32_t( castor::getBitSize( MapSize[0] ) )
				, VK_FORMAT_R16G16B16A16_SFLOAT
				, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT
					| VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
				, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK };
		}

		Texture createDepthBuffer( RenderDevice const & device
			, crg::ResourceHandler & handler
			, std::string const & name
			, Size const & size )
		{
			return Texture{ device
				, handler
				, name + "Dpt"
				, 0u
				, makeExtent3D( size )
				, 6u * EnvironmentMap::Count
				, 1u
				, device.selectSuitableDepthStencilFormat( VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
				, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
				, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK };
		}

		EnvironmentMap::EnvironmentMapPasses createPass( crg::FrameGraph & graph
			, RenderDevice const & device
			, EnvironmentMap & map
			, uint32_t index
			, SceneBackground & background )
		{
			static castor::Point3f const position;
			static std::array< Quaternion, size_t( CubeMapFace::eCount ) > orients
			{
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ -1.0f, +0.0f, +0.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ) ),// Positive X
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ +1.0f, +0.0f, +0.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ) ),// Negative X
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ +0.0f, -1.0f, +0.0f }, castor::Point3f{ +0.0f, +0.0f, +1.0f } ) ),// Positive Y
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ +0.0f, +1.0f, +0.0f }, castor::Point3f{ +0.0f, +0.0f, -1.0f } ) ),// Negative Y
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ +0.0f, +0.0f, -1.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ) ),// Positive Z
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ +0.0f, +0.0f, +1.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ) ),// Negative Z
			};

			auto & scene = background.getScene();
			std::array< SceneNodeSPtr, size_t( CubeMapFace::eCount ) > nodes
			{
				std::make_shared< SceneNode >( "C3D_Env" + scene.getName() + cuT( "PosX" ), scene ),
				std::make_shared< SceneNode >( "C3D_Env" + scene.getName() + cuT( "NegX" ), scene ),
				std::make_shared< SceneNode >( "C3D_Env" + scene.getName() + cuT( "PosY" ), scene ),
				std::make_shared< SceneNode >( "C3D_Env" + scene.getName() + cuT( "NegY" ), scene ),
				std::make_shared< SceneNode >( "C3D_Env" + scene.getName() + cuT( "PosZ" ), scene ),
				std::make_shared< SceneNode >( "C3D_Env" + scene.getName() + cuT( "NegZ" ), scene ),
			};

			auto i = 0u;

			for ( auto & node : nodes )
			{
				node->setOrientation( orients[i] );
				++i;
			}

			return { std::make_unique< EnvironmentMapPass >( graph, device, map, nodes[0], index, CubeMapFace::ePositiveX, background )
				, std::make_unique< EnvironmentMapPass >( graph, device, map, nodes[1], index, CubeMapFace::eNegativeX, background )
				, std::make_unique< EnvironmentMapPass >( graph, device, map, nodes[2], index, CubeMapFace::ePositiveY, background )
				, std::make_unique< EnvironmentMapPass >( graph, device, map, nodes[3], index, CubeMapFace::eNegativeY, background )
				, std::make_unique< EnvironmentMapPass >( graph, device, map, nodes[4], index, CubeMapFace::ePositiveZ, background )
				, std::make_unique< EnvironmentMapPass >( graph, device, map, nodes[5], index, CubeMapFace::eNegativeZ, background ) };
		}

		std::vector< ashes::ImageView > createViews( Texture const & envMap
			, ashes::ImagePtr & image )
		{
			std::vector< ashes::ImageView > result;
			VkImageViewCreateInfo createInfo{ envMap.wholeViewId.data->info };
			createInfo.image = envMap.image;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
			createInfo.subresourceRange.layerCount = 6u;
			image = std::make_unique< ashes::Image >( **envMap.device
				, envMap.image
				, envMap.imageId.data->info );

			for ( uint32_t i = 0u; i < EnvironmentMap::Count; ++i )
			{
				createInfo.subresourceRange.baseArrayLayer = i * 6u;
				result.emplace_back( image->createView( envMap.imageId.data->name + std::to_string( i )
					, createInfo ) );
			}

			return result;
		}

		std::multimap< double, SceneNode * > sortNodes( std::set< SceneNode * > nodes
			, Camera const & camera )
		{
			std::multimap< double, SceneNode * > result;

			for ( auto & node : nodes )
			{
				result.emplace( point::distanceSquared( camera.getParent()->getDerivedPosition()
					, node->getDerivedPosition() )
					, node );
			}

			return result;
		}
	}

	uint32_t const EnvironmentMap::Count = 10u;
	uint32_t const EnvironmentMap::Size = 128u;

	EnvironmentMap::EnvironmentMap( crg::ResourceHandler & handler
		, RenderDevice const & device
		, QueueData const & queueData
		, Scene const & scene )
		: OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, m_device{ device }
		, m_scene{ scene }
		, m_environmentMap{ createTexture( device, handler, "Env" + scene.getName(), MapSize ) }
		, m_depthBuffer{ createDepthBuffer( device, handler, "Env" + scene.getName(), MapSize ) }
		, m_extent{ getExtent( m_environmentMap.imageId ) }
		, m_render{ 0u }
		, m_onSetBackground{ scene.onSetBackground.connect( [this]( SceneBackground const & background )
			{
				for ( uint32_t index = 0; index < m_passes.size(); ++index )
				{
					m_graphs[index] = std::make_unique< crg::FrameGraph >( getEngine()->getGraphResourceHandler()
						, "Env" + m_scene.getName() + std::to_string( index ) );
					m_runnables[index].reset();
					auto & graph = *m_graphs[index];
					m_passes[index] = {};
					m_passes[index] = createPass( graph
						, m_device
						, *this
						, index
						, *m_scene.getBackground() );
					m_runnables[index] = graph.compile( m_device.makeContext() );
					auto runnable = m_runnables[index].get();
					m_device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
						, [runnable]( RenderDevice const &
							, QueueData const & )
						{
							runnable->record();
						} ) );
				}
			} ) }
	{
		m_environmentMap.create();
		m_environmentMapViews = createViews( m_environmentMap, m_image );
		auto commandBuffer = queueData.commandPool->createCommandBuffer( "Env" + scene.getName() + "InitialiseViews" );
		commandBuffer->begin();

		for ( auto & view : m_environmentMapViews )
		{
			commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, view.makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );
		}

		commandBuffer->end();
		auto fence = device->createFence();
		queueData.queue->submit( *commandBuffer, fence.get() );
		fence->wait( ashes::MaxTimeout );
		log::trace << "Created EnvironmentMap " << scene.getName() << std::endl;
	}

	void EnvironmentMap::cleanup()
	{
		m_runnables.clear();
		m_passes.clear();
		m_environmentMapViews.clear();
		m_image.reset();
		m_environmentMap.destroy();
		m_graphs.clear();
		m_sortedNodes.clear();
		m_reflectionNodes.clear();
		m_savedReflectionNodes.clear();
		m_count = 0u;
		m_render = 0u;
	}

	void EnvironmentMap::update( CpuUpdater & updater )
	{
		if ( m_savedReflectionNodes != m_reflectionNodes )
		{
			auto sortedNodes = sortNodes( m_reflectionNodes, *updater.camera );
			m_count = std::min( Count, uint32_t( sortedNodes.size() ) );
			m_savedReflectionNodes = m_reflectionNodes;
			m_sortedNodes.clear();
			auto it = sortedNodes.begin();

			for ( uint32_t i = 0u; i < m_count; ++i )
			{
				m_sortedNodes.emplace( it->second, i );
				auto & passes = m_passes[i];

				for ( auto & pass : passes )
				{
					pass->attachTo( *it->second );
				}

				++it;
			}

			m_first = true;
		}

		if ( m_first )
		{
			uint32_t index = 0u;

			for ( auto & passes : m_passes )
			{
				if ( index < m_count )
				{
					for ( auto & pass : passes )
					{
						pass->update( updater );
					}
				}

				++index;
			}
		}
		else
		{
			auto & passes = m_passes[m_render];

			for ( auto & pass : passes )
			{
				pass->update( updater );
			}
		}
	}

	void EnvironmentMap::update( GpuUpdater & updater )
	{
		if ( !m_count )
		{
			return;
		}

		if ( m_first )
		{
			uint32_t index = 0u;

			for ( auto & passes : m_passes )
			{
				if ( index < m_count )
				{
					for ( auto & pass : passes )
					{
						pass->update( updater );
					}
				}

				++index;
			}
		}
		else
		{
			auto & passes = m_passes[m_render];

			for ( auto & pass : passes )
			{
				pass->update( updater );
			}
		}
	}

	crg::SemaphoreWaitArray EnvironmentMap::render( crg::SemaphoreWaitArray const & toWait
		, ashes::Queue const & queue )
	{
		if ( !m_count )
		{
			return toWait;
		}

		crg::SemaphoreWaitArray result = toWait;

		if ( m_first )
		{
			uint32_t index = 0u;
			// On first run, render all env maps.
			for ( auto & runnable : m_runnables )
			{
				if ( index < m_count )
				{
					result = runnable->run( result, queue );
				}

				++index;
			}
		}
		else
		{
			// Else render only one (rolling)
			result = m_runnables[m_render]->run( result, queue );
			++m_render;

			if ( m_render >= m_count )
			{
				m_render = 0u;
			}
		}

		m_first = false;
		return result;
	}

	void EnvironmentMap::addNode( SceneNode & node )
	{
		auto ires = m_reflectionNodes.insert( &node );

		if ( ires.second
			&& m_reflectionNodes.size() < Count )
		{
			doAddPass();
		}
	}

	void EnvironmentMap::removeNode( SceneNode & node )
	{
		auto it = m_reflectionNodes.find( &node );

		if ( it != m_reflectionNodes.end() )
		{
			m_reflectionNodes.insert( &node );
		}
	}

	bool EnvironmentMap::hasNode( SceneNode & node )const
	{
		return m_reflectionNodes.end() != m_reflectionNodes.find( &node );
	}

	uint32_t EnvironmentMap::getIndex( SceneNode const & node )const
	{
		auto it = m_sortedNodes.find( &node );

		if ( it != m_sortedNodes.end() )
		{
			return it->second;
		}

		return ~( 0u );
	}

	void EnvironmentMap::doAddPass()
	{
		auto index = uint32_t( m_graphs.size() );
		m_graphs.emplace_back( std::make_unique< crg::FrameGraph >( getEngine()->getGraphResourceHandler(), "Env" + m_scene.getName() + std::to_string( index ) ) );
		auto & graph = *m_graphs.back();
		m_passes.emplace_back( createPass( graph
			, m_device
			, *this
			, index
			, *m_scene.getBackground() ) );
		m_runnables.emplace_back( graph.compile( m_device.makeContext() ) );
		m_runnables.back()->record();
	}
}
