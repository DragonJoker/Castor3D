#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
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
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Background/Background.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/GenerateMipmaps.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static Size const MapSize{ 128, 128 };

		Texture doCreateTexture( RenderDevice const & device
			, crg::ResourceHandler & handler
			, std::string const & name
			, Size const & size )
		{
			return Texture{ device
				, handler
				, name
				, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
				, makeExtent3D( size )
				, 6u
				, uint32_t( castor::getBitSize( MapSize[0] ) )
				, VK_FORMAT_R16G16B16A16_SFLOAT
				, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT
					| VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
				, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK };
		}

		Texture doCreateDepthBuffer( RenderDevice const & device
			, crg::ResourceHandler & handler
			, std::string const & name
			, Size const & size )
		{
			return Texture{ device
				, handler
				, name + "Dpt"
				, 0u
				, makeExtent3D( size )
				, 6u
				, uint32_t( castor::getBitSize( MapSize[0] ) )
				, device.selectSuitableDepthStencilFormat( VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
				, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
				, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK };
		}

		EnvironmentMap::EnvironmentMapPasses doCreatePasses( crg::FrameGraph & graph
			, crg::FramePass const * previousPass
			, RenderDevice const & device
			, EnvironmentMap & map
			, SceneNode & node
			, RenderPassTimer & timer
			, SceneBackground & background )
		{
			static castor::Point3f const position;
			std::array< SceneNodeSPtr, size_t( CubeMapFace::eCount ) > nodes
			{
				std::make_shared< SceneNode >( node.getName() + cuT( " PosX" ), *node.getScene() ),
				std::make_shared< SceneNode >( node.getName() + cuT( " NegX" ), *node.getScene() ),
				std::make_shared< SceneNode >( node.getName() + cuT( " PosY" ), *node.getScene() ),
				std::make_shared< SceneNode >( node.getName() + cuT( " NegY" ), *node.getScene() ),
				std::make_shared< SceneNode >( node.getName() + cuT( " PosZ" ), *node.getScene() ),
				std::make_shared< SceneNode >( node.getName() + cuT( " NegZ" ), *node.getScene() ),
			};
			std::array< Quaternion, size_t( CubeMapFace::eCount ) > orients
			{
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ -1.0f, +0.0f, +0.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ) ),// Positive X
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ +1.0f, +0.0f, +0.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ) ),// Negative X
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ +0.0f, -1.0f, +0.0f }, castor::Point3f{ +0.0f, +0.0f, +1.0f } ) ),// Positive Y
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ +0.0f, +1.0f, +0.0f }, castor::Point3f{ +0.0f, +0.0f, -1.0f } ) ),// Negative Y
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ +0.0f, +0.0f, -1.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ) ),// Positive Z
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ +0.0f, +0.0f, +1.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ) ),// Negative Z
			};

			auto i = 0u;

			for ( auto & node : nodes )
			{
				node->setOrientation( orients[i] );
				++i;
			}

			return EnvironmentMap::EnvironmentMapPasses
			{
				{
					std::make_unique< EnvironmentMapPass >( graph, previousPass, device, map, nodes[0], node, CubeMapFace::ePositiveX, timer, background ),
					std::make_unique< EnvironmentMapPass >( graph, previousPass, device, map, nodes[1], node, CubeMapFace::eNegativeX, timer, background ),
					std::make_unique< EnvironmentMapPass >( graph, previousPass, device, map, nodes[2], node, CubeMapFace::ePositiveY, timer, background ),
					std::make_unique< EnvironmentMapPass >( graph, previousPass, device, map, nodes[3], node, CubeMapFace::eNegativeY, timer, background ),
					std::make_unique< EnvironmentMapPass >( graph, previousPass, device, map, nodes[4], node, CubeMapFace::ePositiveZ, timer, background ),
					std::make_unique< EnvironmentMapPass >( graph, previousPass, device, map, nodes[5], node, CubeMapFace::eNegativeZ, timer, background ),
				}
			};
		}
	}

	uint32_t EnvironmentMap::m_count = 0u;

	EnvironmentMap::EnvironmentMap( crg::ResourceHandler & handler
		, RenderDevice const & device
		, SceneNode & node )
		: OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, m_device{ device }
		, m_graph{ handler, "Environment" + node.getName() }
		, m_environmentMap{ doCreateTexture( device, handler, "Env" + node.getName(), MapSize ) }
		, m_depthBuffer{ doCreateDepthBuffer( device, handler, "Env" + node.getName(), MapSize ) }
		, m_node{ node }
		, m_extent{ getExtent( m_environmentMap.imageId ) }
		, m_index{ ++m_count }
		, m_timer{ std::make_shared< RenderPassTimer >( m_device
			, cuT( "EnvironmentMap" )
			, m_node.getName()
			, uint32_t( 6u * 3u + 1u ) ) } // passes + mipmap generation
		, m_passes{ doCreatePasses( m_graph, nullptr, m_device, *this, node, *m_timer, *node.getScene()->getBackground() ) }
		, m_render{ ( m_count % 5u ) }
	{
		auto & mipsGen = m_graph.createPass( "Env" + node.getName() + "Mips"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				return std::make_unique< crg::GenerateMipmaps >( pass
					, context
					, graph
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			} );
		for ( auto & pass : m_passes )
		{
			mipsGen.addDependency( pass->getLastPass() );
		}

		mipsGen.addTransferInOutView( mipsGen.mergeViews( m_environmentMap.subViewsId ) );

		m_runnable = m_graph.compile( crg::GraphContext{ *device
			, VK_NULL_HANDLE
			, device->getAllocationCallbacks()
			, device->getMemoryProperties()
			, device->getProperties()
			, false
			, device->vkGetDeviceProcAddr } );
		m_environmentMapView = m_runnable->createImageView( m_environmentMap.wholeViewId );
		m_runnable->record();
		log::trace << "Created EnvironmentMap " << node.getName() << std::endl;
	}
	
	void EnvironmentMap::update( CpuUpdater & updater )
	{
		// Compute for next frame (or first)
		updater.node = &m_node;

		for ( auto & pass : m_passes )
		{
			pass->update( updater );
		}
	}

	void EnvironmentMap::update( GpuUpdater & updater )
	{
		// Compute for next frame (or first)
		for ( auto & pass : m_passes )
		{
			pass->update( updater );
		}
	}

	crg::SemaphoreWait EnvironmentMap::render( crg::SemaphoreWait const & toWait )
	{
		crg::SemaphoreWait result = toWait;

		// Render current frame (or first)
		if ( m_first || ( m_render % 5u ) == 0u )
		{
			result = m_runnable->run( result
				, *m_device.graphicsQueue );
			m_render = 0u;
		}

		m_render = m_first
			? m_render
			: m_render + 1u;
		m_first = false;
		return result;
	}
}
