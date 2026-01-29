#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMapPass.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Background/Background.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <RenderGraph/RunnableGraph.hpp>

CU_ImplementSmartPtr( c3d, EnvironmentMap )

namespace c3d
{
	namespace envmap
	{
		static Size const MapSize{ EnvironmentMapSize, EnvironmentMapSize };

		static Texture createTexture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, Size const & size )
		{
			return Texture{ device
				, resources
				, name
				, { ImageCreateFlags::eCubeCompatible
					, makeExtent3D( size ), 6u * MaxEnvironmentMapCount
					, uint32_t( getBitSize( MapSize[0] ) )
					, device.selectSmallestFormatRGBUFloatFormat( VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT
						| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT
						| VK_FORMAT_FEATURE_TRANSFER_DST_BIT
						| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT )
					, ( ImageUsageFlags::eColorAttachment
						| ImageUsageFlags::eSampled
						| ImageUsageFlags::eTransferDst
						| ImageUsageFlags::eTransferSrc ) }
				, { BorderColour::eFloatOpaqueBlack } };
		}

		static Texture createDepthBuffer( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, Size const & size )
		{
			return Texture{ device
				, resources
				, name + cuT( "/Depth" )
				, { ImageCreateFlags::eNone
					, makeExtent3D( size ), 6u * MaxEnvironmentMapCount, 1u
					, device.selectSuitableDepthStencilFormat( VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
						| VK_FORMAT_FEATURE_TRANSFER_DST_BIT )
					, ( ImageUsageFlags::eDepthStencilAttachment
						| ImageUsageFlags::eTransferDst ) }
				, { BorderColour::eFloatOpaqueBlack } };
		}

		static Texture createTmpTexture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, Size const & size
			, PixelFormat format )
		{
			return Texture{ device
				, resources
				, name + cuT( "/Temp" )
				, { ImageCreateFlags::eNone
					, makeExtent3D( size ), 6u * MaxEnvironmentMapCount, 1u
					, format
					, ( ImageUsageFlags::eColorAttachment
						| ImageUsageFlags::eTransferSrc ) }
				, { BorderColour::eFloatOpaqueBlack } };
		}

		static EnvironmentMap::EnvironmentMapPasses createPass( RenderDevice const & device
			, EnvironmentMap & map
			, uint32_t index
			, SceneBackground & background )
		{
			static Point3f const position;
			static Array< Quaternion, size_t( CubeMapFace::eCount ) > orients
			{
				Quaternion::fromMatrix( matrix::lookAt( position, Point3f{ -1.0f, +0.0f, +0.0f }, Point3f{ +0.0f, -1.0f, +0.0f } ) ),// Positive X
				Quaternion::fromMatrix( matrix::lookAt( position, Point3f{ +1.0f, +0.0f, +0.0f }, Point3f{ +0.0f, -1.0f, +0.0f } ) ),// Negative X
				Quaternion::fromMatrix( matrix::lookAt( position, Point3f{ +0.0f, -1.0f, +0.0f }, Point3f{ +0.0f, +0.0f, +1.0f } ) ),// Positive Y
				Quaternion::fromMatrix( matrix::lookAt( position, Point3f{ +0.0f, +1.0f, +0.0f }, Point3f{ +0.0f, +0.0f, -1.0f } ) ),// Negative Y
				Quaternion::fromMatrix( matrix::lookAt( position, Point3f{ +0.0f, +0.0f, -1.0f }, Point3f{ +0.0f, -1.0f, +0.0f } ) ),// Positive Z
				Quaternion::fromMatrix( matrix::lookAt( position, Point3f{ +0.0f, +0.0f, +1.0f }, Point3f{ +0.0f, -1.0f, +0.0f } ) ),// Negative Z
			};

			auto & scene = background.getScene();
			Array< SceneNodeUPtr, size_t( CubeMapFace::eCount ) > nodes
			{
				makeUnique< SceneNode >( cuT( "C3D_Env" ) + scene.getName() + cuT( "PosX" ), scene ),
				makeUnique< SceneNode >( cuT( "C3D_Env" ) + scene.getName() + cuT( "NegX" ), scene ),
				makeUnique< SceneNode >( cuT( "C3D_Env" ) + scene.getName() + cuT( "PosY" ), scene ),
				makeUnique< SceneNode >( cuT( "C3D_Env" ) + scene.getName() + cuT( "NegY" ), scene ),
				makeUnique< SceneNode >( cuT( "C3D_Env" ) + scene.getName() + cuT( "PosZ" ), scene ),
				makeUnique< SceneNode >( cuT( "C3D_Env" ) + scene.getName() + cuT( "NegZ" ), scene ),
			};

			auto i = 0u;

			for ( auto const & node : nodes )
			{
				node->setSerialisable( false );
				node->setOrientation( orients[i] );
				++i;
			}

			return { makeUnique< EnvironmentMapPass >( device, map, c3d::move( nodes[0] ), index, CubeMapFace::ePositiveX, background )
				, makeUnique< EnvironmentMapPass >( device, map, c3d::move( nodes[1] ), index, CubeMapFace::eNegativeX, background )
				, makeUnique< EnvironmentMapPass >( device, map, c3d::move( nodes[2] ), index, CubeMapFace::ePositiveY, background )
				, makeUnique< EnvironmentMapPass >( device, map, c3d::move( nodes[3] ), index, CubeMapFace::eNegativeY, background )
				, makeUnique< EnvironmentMapPass >( device, map, c3d::move( nodes[4] ), index, CubeMapFace::ePositiveZ, background )
				, makeUnique< EnvironmentMapPass >( device, map, c3d::move( nodes[5] ), index, CubeMapFace::eNegativeZ, background ) };
		}

		static Vector< ashes::ImageView > createViews( Texture const & envMap
			, ashes::Image *& image )
		{
			Vector< ashes::ImageView > result;
			VkImageViewCreateInfo createInfo{ convert( envMap.getWholeViewId().data->info ) };
			createInfo.image = *envMap.image;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
			createInfo.subresourceRange.layerCount = 6u;
			image = envMap.image.get();

			for ( uint32_t i = 0u; i < MaxEnvironmentMapCount; ++i )
			{
				createInfo.subresourceRange.baseArrayLayer = i * 6u;
				result.emplace_back( image->createView( envMap.imageId.data->name + string::toMbString( i )
					, createInfo ) );
			}

			return result;
		}

		static MultiMap< double, SceneNode * > sortNodes( Set< SceneNode * > const & nodes
			, Camera const & camera )
		{
			MultiMap< double, SceneNode * > result;

			for ( auto const & node : nodes )
			{
				result.emplace( point::distanceSquared( camera.getParent()->getDerivedPosition()
					, node->getDerivedPosition() )
					, node );
			}

			return result;
		}
	}

	EnvironmentMap::EnvironmentMap( crg::ResourcesCache & resources
		, RenderDevice const & device
		, QueueData const & queueData
		, Scene & scene )
		: OwnedBy< Engine >{ c3d::getEngine( device ) }
		, m_device{ device }
		, m_scene{ scene }
		, m_environmentMap{ envmap::createTexture( device, resources, cuT( "Env" ) + scene.getName(), envmap::MapSize ) }
		, m_depthBuffer{ envmap::createDepthBuffer( device, resources, cuT( "Env" ) + scene.getName(), envmap::MapSize ) }
		, m_tmpImage{ envmap::createTmpTexture( device, resources, cuT( "Env" ) + scene.getName(), envmap::MapSize, m_environmentMap.getFormat() ) }
		, m_extent{ getExtent( m_environmentMap.imageId ) }
		, m_onSetBackground{ scene.onSetBackground.connect( [this]( SceneBackground const & )
			{
				for ( uint32_t index = 0; index < m_passes.size(); ++index )
				{
					m_passes[index] = {};
					m_passes[index] = envmap::createPass( m_device
						, *this
						, index
						, *m_scene.getBackground() );
					auto const & passes = m_passes[index];
					c3d::getEngine( m_device ).postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
						, [&passes]( RenderDevice const &
							, QueueData const & )
						{
							for ( auto const & pass : passes )
							{
								pass->record();
							}
						} ) );
				}
			} ) }
	{
		m_environmentMap.create();
		m_depthBuffer.create();
		m_tmpImage.create();
		m_environmentMapViews = envmap::createViews( m_environmentMap, m_image );
		auto commandBuffer = queueData.commandPool->createCommandBuffer( "Env" + toUtf8( scene.getName() ) + "InitialiseViews" );
		commandBuffer->begin();

		for ( auto const & view : m_environmentMapViews )
		{
			commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, view.makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
			commandBuffer->clear( view
				, crg::convert( opaqueBlackClearColor ) );
			commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, view.makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
		}

		commandBuffer->end();
		auto fence = device->createFence();
		queueData.queue->submit( *commandBuffer, fence.get() );
		fence->wait( ashes::MaxTimeout );
		log::trace << "Created EnvironmentMap " << scene.getName() << std::endl;
	}

	void EnvironmentMap::cleanup()noexcept
	{
		m_passes.clear();
		m_environmentMapViews.clear();
		m_image = {};
		m_tmpImage.destroy();
		m_depthBuffer.destroy();
		m_environmentMap.destroy();
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
			auto sortedNodes = envmap::sortNodes( m_reflectionNodes, *updater.camera );
			m_count = std::min( uint32_t( sortedNodes.size() )
				, std::min( MaxEnvironmentMapCount, uint32_t( m_passes.size() ) ) );
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

				m_scene.markDirty( *it->second );
				++it;
			}

			m_first = true;
		}

		if ( m_first || !getEngine()->areUpdateOptimisationsEnabled() )
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

		if ( m_first || !getEngine()->areUpdateOptimisationsEnabled() )
		{
			uint32_t index = 0u;

			for ( auto const & passes : m_passes )
			{
				if ( index < m_count )
				{
					for ( auto const & pass : passes )
					{
						pass->update( updater );
					}
				}

				++index;
			}
		}
		else
		{
			for ( auto const & pass : m_passes[m_render] )
			{
				pass->update( updater );
			}
		}
	}

	SemaphoreWaitArray EnvironmentMap::render( SemaphoreWaitArray const & toWait
		, ashes::Queue const & queue )
	{
		if ( !m_count )
		{
			return toWait;
		}

		SemaphoreWaitArray result = toWait;

		if ( m_first || !getEngine()->areUpdateOptimisationsEnabled() )
		{
			uint32_t index = 0u;
			// On first run, render all env maps.
			for ( auto const & passes : m_passes )
			{	
				if ( index < m_count )
				{
					for ( auto const & pass : passes )
					{
						result = pass->render( result, queue );
					}
				}

				++index;
			}
		}
		else
		{
			// Else render only one (rolling)
			for ( auto const & pass : m_passes[m_render] )
			{
				result = pass->render( result, queue );
			}

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
		auto res = m_reflectionNodes.emplace( &node ).second;

		if ( res && m_reflectionNodes.size() < MaxEnvironmentMapCount )
		{
			doAddPass();
		}
	}

	void EnvironmentMap::removeNode( SceneNode & node )
	{
		if ( auto it = m_reflectionNodes.find( &node );
			it != m_reflectionNodes.end() )
		{
			m_reflectionNodes.erase( it );
		}
	}

	bool EnvironmentMap::hasNode( SceneNode & node )const
	{
		return m_reflectionNodes.end() != m_reflectionNodes.find( &node );
	}

	uint32_t EnvironmentMap::getIndex( SceneNode const & node )const
	{
		if ( auto it = m_sortedNodes.find( &node );
			it != m_sortedNodes.end() )
		{
			return it->second;
		}

		return ~0u;
	}

	void EnvironmentMap::doAddPass()
	{
		auto index = uint32_t( m_passes.size() );
		m_passes.emplace_back( envmap::createPass( m_device
			, *this
			, index
			, *m_scene.getBackground() ) );

		for ( auto const & pass : m_passes.back() )
		{
			pass->record();
		}
	}
}
