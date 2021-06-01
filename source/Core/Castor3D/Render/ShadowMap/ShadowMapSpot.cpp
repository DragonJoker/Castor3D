#include "Castor3D/Render/ShadowMap/ShadowMapSpot.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassSpot.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/BlockTracker.hpp>

#include <RenderGraph/FrameGraph.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		std::vector< ShadowMap::PassData > createPasses( crg::FrameGraph & graph
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, Scene & scene
			, ShadowMap & shadowMap )
		{
			auto & engine = *scene.getEngine();
			std::vector< ShadowMap::PassData > result;
			Viewport viewport{ engine };
			viewport.resize( Size{ ShadowMapPassSpot::TextureSize, ShadowMapPassSpot::TextureSize } );
			auto & smResult = shadowMap.getShadowPassResult();
			auto & depth = smResult[SmTexture::eDepth];
			auto & linear = smResult[SmTexture::eNormalLinear];
			auto & variance = smResult[SmTexture::eVariance];
			auto & position = smResult[SmTexture::ePosition];
			auto & flux = smResult[SmTexture::eFlux];

			for ( auto i = 0u; i < shader::getSpotShadowMapCount(); ++i )
			{
				std::string debugName = "SpotSM" + std::to_string( i );
				ShadowMap::PassData passData
				{
					std::make_unique< MatrixUbo >( device ),
					std::make_shared< Camera >( cuT( "ShadowMapSpot" )
						, scene
						, *scene.getCameraRootNode()
						, std::move( viewport ) ),
					nullptr,
					nullptr,
				};
				passData.culler = std::make_unique< FrustumCuller >( scene, *passData.camera );
				auto index = uint32_t( result.size() );
				auto & matrixUbo = *passData.matrixUbo;
				auto & culler = *passData.culler;
				auto & pass = graph.createPass( debugName
					, [index, &matrixUbo, &culler, &previousPass, &device, &shadowMap, &scene]( crg::FramePass const & pass
						, crg::GraphContext const & context
						, crg::RunnableGraph & graph )
					{
						auto result = std::make_unique< ShadowMapPassSpot >( pass
							, context
							, graph
							, device
							, index
							, matrixUbo
							, culler
							, shadowMap );
						shadowMap.setPass( index, result.get() );
						return result;
					} );
				pass.addDependency( *previousPass );
				previousPass = &pass;
				pass.addOutputDepthView( depth.subViews[i], getClearValue( SmTexture::eDepth ) );
				pass.addOutputColourView( linear.subViews[i], getClearValue( SmTexture::eNormalLinear ) );
				pass.addOutputColourView( variance.subViews[i], getClearValue( SmTexture::eVariance ) );
				pass.addOutputColourView( position.subViews[i], getClearValue( SmTexture::ePosition ) );
				pass.addOutputColourView( flux.subViews[i], getClearValue( SmTexture::eFlux ) );
				result.emplace_back( std::move( passData ) );
			}

			return result;
		}
	}

	ShadowMapSpot::ShadowMapSpot( crg::FrameGraph & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, Scene & scene )
		: ShadowMap{ device
			, scene
			, LightType::eSpot
			, ShadowMapResult{ graph
				, device
				, cuT( "Spot" )
				, 0u
				, Size{ ShadowMapPassSpot::TextureSize, ShadowMapPassSpot::TextureSize }
				, shader::getSpotShadowMapCount() }
			, shader::getSpotShadowMapCount() }
	{
		auto previous = &previousPass;
		m_passes = createPasses( graph
			, previous
			, device
			, scene
			, *this );
		m_blur = std::make_unique< GaussianBlur >( graph
			, *previous
			, device
			, "SpotSM"
			, m_result[SmTexture::eVariance].wholeView
			, 5u );
		log::trace << "Created ShadowMapSpot" << std::endl;
	}

	ShadowMapSpot::~ShadowMapSpot()
	{
		m_blur.reset();
	}
	
	void ShadowMapSpot::update( CpuUpdater & updater )
	{
		m_passes[updater.index].pass->update( updater );
	}

	void ShadowMapSpot::update( GpuUpdater & updater )
	{
		m_passes[updater.index].pass->update( updater );
	}

	bool ShadowMapSpot::isUpToDate( uint32_t index )const
	{
		return m_passes[index].pass->isUpToDate();
	}
}
