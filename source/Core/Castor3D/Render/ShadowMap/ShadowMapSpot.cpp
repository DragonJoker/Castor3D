#include "Castor3D/Render/ShadowMap/ShadowMapSpot.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassSpot.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/BlockTracker.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		std::vector< ShadowMap::PassDataPtr > createPass( crg::ResourceHandler & handler
			, std::vector< std::unique_ptr< crg::FrameGraph > > & graphs
			, std::vector< std::unique_ptr< GaussianBlur > > & blurs
			, crg::ImageViewId intermediate
			, uint32_t shadowMapIndex
			, RenderDevice const & device
			, Scene & scene
			, ShadowMap & shadowMap )
		{
			auto & engine = *scene.getEngine();
			std::vector< ShadowMap::PassDataPtr > result;
			Viewport viewport{ engine };
			viewport.resize( Size{ ShadowMapPassSpot::TextureSize, ShadowMapPassSpot::TextureSize } );
			auto & smResult = shadowMap.getShadowPassResult();
			auto & depth = smResult[SmTexture::eDepth];
			auto & linear = smResult[SmTexture::eNormalLinear];
			auto & variance = smResult[SmTexture::eVariance];
			auto & position = smResult[SmTexture::ePosition];
			auto & flux = smResult[SmTexture::eFlux];

			std::string debugName = "SpotSM" + std::to_string( shadowMapIndex );
			graphs.push_back( std::make_unique< crg::FrameGraph >( handler, debugName ) );
			auto & graph = *graphs.back();
			result.emplace_back( std::make_unique< ShadowMap::PassData >( std::make_unique< MatrixUbo >( device )
				, std::make_shared< Camera >( cuT( "ShadowMapSpot" )
					, scene
					, *scene.getCameraRootNode()
					, std::move( viewport ) )
				, nullptr ) );
			auto & passData = *result.back();
			passData.culler = std::make_unique< FrustumCuller >( scene, *passData.camera );
			auto & pass = graph.createPass( debugName
				, [shadowMapIndex, &passData, &device, &shadowMap]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnableGraph )
				{
					auto res = std::make_unique< ShadowMapPassSpot >( framePass
						, context
						, runnableGraph
						, device
						, shadowMapIndex
						, *passData.matrixUbo
						, *passData.culler
						, shadowMap );
					passData.pass = res.get();
					device.renderSystem.getEngine()->registerTimer( runnableGraph.getName() + cuT( "/ShadowMapSpot" )
						, res->getTimer() );
					return res;
				} );
			auto previousPass = &pass;
			pass.addOutputDepthView( depth.subViewsId[shadowMapIndex], getClearValue( SmTexture::eDepth ) );
			pass.addOutputColourView( linear.subViewsId[shadowMapIndex], getClearValue( SmTexture::eNormalLinear ) );
			pass.addOutputColourView( variance.subViewsId[shadowMapIndex], getClearValue( SmTexture::eVariance ) );
			pass.addOutputColourView( position.subViewsId[shadowMapIndex], getClearValue( SmTexture::ePosition ) );
			pass.addOutputColourView( flux.subViewsId[shadowMapIndex], getClearValue( SmTexture::eFlux ) );

			blurs.push_back( std::make_unique< GaussianBlur >( graph
				, *previousPass
				, device
				, cuT( "ShadowMapSpot" )
				, debugName
				, variance.subViewsId[shadowMapIndex]
				, intermediate
				, 5u ) );

			return result;
		}
	}

	ShadowMapSpot::ShadowMapSpot( crg::ResourceHandler & handler
		, RenderDevice const & device
		, Scene & scene
		, ProgressBar * progress )
		: ShadowMap{ handler
			, device
			, scene
			, LightType::eSpot
			, 0u
			, Size{ ShadowMapPassSpot::TextureSize, ShadowMapPassSpot::TextureSize }
			, shader::getSpotShadowMapCount()
			, shader::getSpotShadowMapCount() }
		, m_blurIntermediate{ handler.createImageId( crg::ImageData{ "SpotGB"
			, 0u
			, VK_IMAGE_TYPE_2D
			, getFormat( SmTexture::eVariance )
			, m_result.begin()->getExtent()
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT ) } ) }
		, m_blurIntermediateView{ handler.createViewId( crg::ImageViewData{ m_blurIntermediate.data->name
			, m_blurIntermediate
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, getFormat( m_blurIntermediate )
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } ) }
	{
		stepProgressBar( progress, "Creating ShadowMapSpot" );
	}

	void ShadowMapSpot::update( GpuUpdater & updater )
	{
		if ( m_runnables.size() > updater.index
			&& m_runnables[updater.index] )
		{
			m_passes[updater.index]->pass->update( updater );
		}
	}

	std::vector< ShadowMap::PassDataPtr > ShadowMapSpot::doCreatePass( uint32_t index )
	{
		return createPass( m_handler
			, m_graphs
			, m_blurs
			, m_blurIntermediateView
			, index
			, m_device
			, m_scene
			, *this );
	}

	bool ShadowMapSpot::doIsUpToDate( uint32_t index )const
	{
		if ( m_passes.size() > index )
		{
			return m_passes[index]->pass->isUpToDate();
		}

		return true;
	}

	void ShadowMapSpot::doUpdate( CpuUpdater & updater )
	{
		if ( m_runnables.size() > updater.index
			&& m_runnables[updater.index] )
		{
			m_passes[updater.index]->pass->update( updater );
		}
	}

	uint32_t ShadowMapSpot::doGetMaxCount()const
	{
		return shader::getSpotShadowMapCount();
	}
}
