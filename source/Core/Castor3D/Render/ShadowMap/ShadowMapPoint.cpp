#include "Castor3D/Render/ShadowMap/ShadowMapPoint.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/DummyCuller.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassPoint.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
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
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/BlockTracker.hpp>

#include <RenderGraph/FrameGraph.hpp>

#include <algorithm>

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
			auto & smResult = shadowMap.getShadowPassResult();
			auto & depth = smResult[SmTexture::eDepth];
			auto & linear = smResult[SmTexture::eNormalLinear];
			auto & variance = smResult[SmTexture::eVariance];
			auto & position = smResult[SmTexture::ePosition];
			auto & flux = smResult[SmTexture::eFlux];

			for ( uint32_t layer = 0u; layer < shader::getPointShadowMapCount(); ++layer )
			{
				std::string debugName = "PointSML" + std::to_string( layer );

				for ( uint32_t face = 0u; face < 6u; ++face )
				{
					ShadowMap::PassData passData
					{
						std::make_unique< MatrixUbo >( device ),
						nullptr,
						std::make_unique< DummyCuller >( scene ),
						nullptr,
					};
					auto index = uint32_t( result.size() );
					auto & matrixUbo = *passData.matrixUbo;
					auto & culler = *passData.culler;
					auto & pass = graph.createPass( debugName + "F" + std::to_string( face )
						, [index, &matrixUbo, &culler, &previousPass, &device, &shadowMap, &scene]( crg::FramePass const & pass
							, crg::GraphContext const & context
							, crg::RunnableGraph & graph )
						{
							auto result = std::make_unique< ShadowMapPassPoint >( pass
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
					pass.addOutputDepthView( depth.subViews[layer * 6u + face], getClearValue( SmTexture::eDepth ) );
					pass.addOutputColourView( linear.subViews[layer * 6u + face], getClearValue( SmTexture::eNormalLinear ) );
					pass.addOutputColourView( variance.subViews[layer * 6u + face], getClearValue( SmTexture::eVariance ) );
					pass.addOutputColourView( position.subViews[layer * 6u + face], getClearValue( SmTexture::ePosition ) );
					pass.addOutputColourView( flux.subViews[layer * 6u + face], getClearValue( SmTexture::eFlux ) );

					result.emplace_back( std::move( passData ) );
				}
			}

			return result;
		}
	}

	ShadowMapPoint::ShadowMapPoint( crg::FrameGraph & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, Scene & scene )
		: ShadowMap{ device
			, scene
			, LightType::ePoint
			, ShadowMapResult{ graph
				, device
				, cuT( "Point" )
				, VkImageCreateFlags( VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT )
				, { ShadowMapPassPoint::TextureSize, ShadowMapPassPoint::TextureSize }
				, 6u * shader::getPointShadowMapCount() }
			, shader::getPointShadowMapCount() }
	{
		auto previous = &previousPass;
		m_passes = createPasses( graph
			, previous
			, device
			, scene
			, *this );
		//m_blur = std::make_unique< GaussianBlur >( graph
		//	, previousPass
		//	, device
		//	, "PointSM"
		//	, m_result[SmTexture::eVariance].wholeView
		//	, 5u );
		log::trace << "Created ShadowMapPoint" << std::endl;
	}

	ShadowMapPoint::~ShadowMapPoint()
	{
		m_passesData.clear();
		m_blur.reset();
	}

	void ShadowMapPoint::update( CpuUpdater & updater )
	{
		m_passesData[updater.index].shadowType = updater.light->getShadowType();
		uint32_t offset = updater.index * 6u;
		updater.light->getPointLight()->updateShadow( updater.index );

		for ( uint32_t face = offset; face < offset + 6u; ++face )
		{
			updater.index = face - offset;
			m_passes[face].pass->update( updater );
		}
	}

	void ShadowMapPoint::update( GpuUpdater & updater )
	{
		uint32_t offset = updater.index * 6u;

		for ( uint32_t face = offset; face < offset + 6u; ++face )
		{
			updater.index = face - offset;
			m_passes[face].pass->update( updater );
		}
	}

	bool ShadowMapPoint::isUpToDate( uint32_t index )const
	{
		uint32_t offset = index * 6u;
		return std::all_of( m_passes.begin() + offset
			, m_passes.begin() + offset + 6u
			, []( ShadowMap::PassData const & data )
			{
				return data.pass->isUpToDate();
			} );
	}
}
