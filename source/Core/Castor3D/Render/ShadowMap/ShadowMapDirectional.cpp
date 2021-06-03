#include "Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/Culling/InstantiatedFrustumCuller.hpp"
#include "Castor3D/Render/Passes/GaussianBlur.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassDirectional.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/BlockTracker.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>

namespace castor3d
{
	namespace
	{
		std::vector< ShadowMap::PassData > createPasses( crg::FrameGraph & graph
			, crg::FramePass const *& previousPass
			, std::vector< std::unique_ptr< GaussianBlur > > & blurs
			, crg::ImageViewId intermediate
			, RenderDevice const & device
			, Scene & scene
			, ShadowMap & shadowMap
			, uint32_t cascadeCount )
		{
			auto & engine = *scene.getEngine();
			std::vector< ShadowMap::PassData > result;
			auto const width = ShadowMapPassDirectional::TileSize;
			auto const height = ShadowMapPassDirectional::TileSize;
			auto const w = float( width );
			auto const h = float( height );
			Viewport viewport{ engine };
			viewport.setOrtho( -w / 2, w / 2, -h / 2, h / 2, -5120.0, 5120.0 );
			viewport.resize( { width, height } );
			viewport.update();
			auto & smResult = shadowMap.getShadowPassResult();
			auto & depth = smResult[SmTexture::eDepth];
			auto & linear = smResult[SmTexture::eNormalLinear];
			auto & variance = smResult[SmTexture::eVariance];
			auto & position = smResult[SmTexture::ePosition];
			auto & flux = smResult[SmTexture::eFlux];

#if C3D_UseTiledDirectionalShadowMap

			ShadowMap::PassData passData
			{
				std::make_unique< MatrixUbo >( device ),
				std::make_shared< Camera >( cuT( "ShadowMapDirectional" )
					, scene
					, *scene.getCameraRootNode()
					, viewport
					, true ),
				nullptr,
				nullptr,
			};
			passData.culler = std::make_unique< InstantiatedFrustumCuller >( scene
				, *passData.camera
				, scene.getDirectionalShadowCascades() );
			auto index = result.size();
			auto & matrixUbo = *passData.matrixUbo;
			auto & culler = *passData.culler;
			auto & pass = graph.createPass( passData.camera->getName()
				, [index, &matrixUbo, &culler, &previousPass, &device, &shadowMap, &scene]( crg::FramePass const & pass
					, crg::GraphContext const & context
					, crg::RunnableGraph & graph )
				{
					auto result = std::make_unique< ShadowMapPassDirectional >( pass
						, context
						, graph
						, device
						, matrixUbo
						, culler
						, shadowMap
						, scene.getDirectionalShadowCascades() );
					shadowMap.setPass( index, result.get() );
					return result;
				} );
			pass.addDependency( *previousPass );
			previousPass = &pass;
			pass.addOutputDepthView( depth.wholeView, getClearValue( SmTexture::eDepth ) );
			pass.addOutputColourView( linear.wholeView, getClearValue( SmTexture::eNormalLinear ) );
			pass.addOutputColourView( variance.wholeView, getClearValue( SmTexture::eVariance ) );
			pass.addOutputColourView( position.wholeView, getClearValue( SmTexture::ePosition ) );
			pass.addOutputColourView( flux.wholeView, getClearValue( SmTexture::eFlux ) );
			result.emplace_back( std::move( passData ) );

#else

			for ( uint32_t cascade = 0u; cascade < cascadeCount; ++cascade )
			{
				std::string debugName = "DirectionalSMC" + std::to_string( cascade + 1u );
				ShadowMap::PassData passData
				{
					std::make_unique< MatrixUbo >( device ),
					std::make_shared< Camera >( debugName
						, scene
						, *scene.getCameraRootNode()
						, viewport
						, true ),
					nullptr,
					nullptr,
				};
				passData.culler = std::make_unique< FrustumCuller >( scene, *passData.camera );
				auto index = uint32_t( result.size() );
				auto & matrixUbo = *passData.matrixUbo;
				auto & culler = *passData.culler;
				auto & pass = graph.createPass( debugName
					, [index, &matrixUbo, &culler, &device, &shadowMap, &cascade]( crg::FramePass const & pass
						, crg::GraphContext const & context
						, crg::RunnableGraph & graph )
					{
						auto result = std::make_unique< ShadowMapPassDirectional >( pass
							, context
							, graph
							, device
							, matrixUbo
							, culler
							, shadowMap
							, cascade );
						shadowMap.setPass( index, result.get() );
						return result;
					} );
				pass.addDependency( *previousPass );
				previousPass = &pass;

				if ( cascadeCount == 1u )
				{
					pass.addOutputDepthView( depth.wholeView, getClearValue( SmTexture::eDepth ) );
					pass.addOutputColourView( linear.wholeView, getClearValue( SmTexture::eNormalLinear ) );
					pass.addOutputColourView( variance.wholeView, getClearValue( SmTexture::eVariance ) );
					pass.addOutputColourView( position.wholeView, getClearValue( SmTexture::ePosition ) );
					pass.addOutputColourView( flux.wholeView, getClearValue( SmTexture::eFlux ) );

					blurs.push_back( std::make_unique< GaussianBlur >( graph
						, *previousPass
						, device
						, debugName
						, variance.wholeView
						, 5u ) );
				}
				else
				{
					pass.addOutputDepthView( depth.subViews[cascade], getClearValue( SmTexture::eDepth ) );
					pass.addOutputColourView( linear.subViews[cascade], getClearValue( SmTexture::eNormalLinear ) );
					pass.addOutputColourView( variance.subViews[cascade], getClearValue( SmTexture::eVariance ) );
					pass.addOutputColourView( position.subViews[cascade], getClearValue( SmTexture::ePosition ) );
					pass.addOutputColourView( flux.subViews[cascade], getClearValue( SmTexture::eFlux ) );

					blurs.push_back( std::make_unique< GaussianBlur >( graph
						, *previousPass
						, device
						, debugName
						, variance.subViews[cascade]
						, intermediate
						, 5u ) );
				}

				previousPass = &blurs.back()->getLastPass();
				result.emplace_back( std::move( passData ) );
			}

#endif

			return result;
		}
	}

	ShadowMapDirectional::ShadowMapDirectional( crg::FrameGraph & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, Scene & scene )
		: ShadowMap{ device
			, scene
			, LightType::eDirectional
			, ShadowMapResult{ graph
				, device
				, cuT( "Directional" )
				, 0u
#if C3D_UseTiledDirectionalShadowMap
				, { ShadowMapPassDirectional::TileSize * ShadowMapPassDirectional::TileCountX
					, ShadowMapPassDirectional::TileSize * ShadowMapPassDirectional::TileCountY }
				, 1u }
#else
				, { ShadowMapPassDirectional::TileSize, ShadowMapPassDirectional::TileSize }
				, scene.getDirectionalShadowCascades() }
#endif
			, 1u }
		, m_blurIntermediate{ graph.createImage( crg::ImageData{ "DirectionalGB"
			, 0u
			, VK_IMAGE_TYPE_2D
			, getFormat( SmTexture::eVariance )
			, m_result.begin()->getExtent()
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT ) } ) }
		, m_blurIntermediateView{ graph.createView( crg::ImageViewData{ m_blurIntermediate.data->name
			, m_blurIntermediate
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, getFormat( m_blurIntermediate )
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } ) }
		, m_cascades{ scene.getDirectionalShadowCascades() }
	{
		m_lastPass = &previousPass;
		m_passes = createPasses( graph
			, m_lastPass
			, m_blurs
			, m_blurIntermediateView
			, device
			, scene
			, *this
			, scene.getDirectionalShadowCascades() );
		log::trace << "Created ShadowMapDirectional" << std::endl;
	}

	void ShadowMapDirectional::update( CpuUpdater & updater )
	{
		auto & light = *updater.light;
		auto & camera = *updater.camera;
		m_shadowType = light.getShadowType();
		auto node = light.getParent();
		node->update();

		auto & directional = *light.getDirectionalLight();

#if C3D_UseTiledDirectionalShadowMap
		if ( directional.updateShadow( camera )
			|| m_passes[0u].pass->isDirty() )
		{
			m_passes[0u].pass->update( updater );
		}
#else
		if ( directional.updateShadow( camera ) )
		{
			for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
			{
				auto & culler = m_passes[cascade].pass->getCuller();
				auto & lightCamera = culler.getCamera();
				lightCamera.attachTo( *node );
				lightCamera.setProjection( directional.getProjMatrix( m_cascades - 1u ) );
				lightCamera.setView( directional.getViewMatrix( m_cascades - 1u ) );
				lightCamera.updateFrustum();

				updater.index = cascade;
				m_passes[cascade].pass->update( updater );
			}
		}
#endif
	}

	void ShadowMapDirectional::update( GpuUpdater & updater )
	{
		for ( uint32_t cascade = 0u; cascade < std::min( m_cascades, uint32_t( m_passes.size() ) ); ++cascade )
		{
			updater.index = cascade;
			m_passes[cascade].pass->update( updater );
		}
	}

	bool ShadowMapDirectional::isUpToDate( uint32_t index )const
	{
		return std::all_of( m_passes.begin()
			, m_passes.begin() + std::min( m_cascades, uint32_t( m_passes.size() ) )
			, []( ShadowMap::PassData const & data )
			{
				return data.pass->isUpToDate();
			} );
	}
}
