#include "Castor3D/Render/ShadowMap/ShadowMapPoint.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderModule.hpp"
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
#include <RenderGraph/RunnableGraph.hpp>

#include <algorithm>

namespace castor3d
{
	namespace
	{
		std::vector< ShadowMap::PassData > createPasses( crg::ResourceHandler & handler
			, std::vector< std::unique_ptr< crg::FrameGraph > > & graphs
			, std::vector< crg::RunnableGraphPtr > & runnables
			, std::vector< std::unique_ptr< GaussianBlur > > & blurs
			, crg::ImageViewId intermediate
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
				graphs.push_back( std::make_unique< crg::FrameGraph >( handler, debugName ) );
				auto & graph = *graphs.back();
				runnables.push_back( nullptr );
				crg::FramePass const * previousPass{};

				for ( uint32_t face = 0u; face < 6u; ++face )
				{
					ShadowMap::PassData passData{ std::make_unique< MatrixUbo >( device )
						, nullptr
						, std::make_unique< DummyCuller >( scene )
						, nullptr };
					auto index = uint32_t( result.size() );
					auto & matrixUbo = *passData.matrixUbo;
					auto & culler = *passData.culler;
					auto name = debugName + "F" + std::to_string( face );
					auto & pass = graph.createPass( name
						, [index, &matrixUbo, &culler, &device, &shadowMap, &scene]( crg::FramePass const & pass
							, crg::GraphContext & context
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
							device.renderSystem.getEngine()->registerTimer( cuT( "ShadowMapPoint" )
								, result->getTimer() );
							return result;
						} );

					if ( previousPass )
					{
						pass.addDependency( *previousPass );
					}

					previousPass = &pass;
					pass.addOutputDepthView( depth.subViewsId[layer * 6u + face], getClearValue( SmTexture::eDepth ) );
					pass.addOutputColourView( linear.subViewsId[layer * 6u + face], getClearValue( SmTexture::eNormalLinear ) );
					pass.addOutputColourView( variance.subViewsId[layer * 6u + face], getClearValue( SmTexture::eVariance ) );
					pass.addOutputColourView( position.subViewsId[layer * 6u + face], getClearValue( SmTexture::ePosition ) );
					pass.addOutputColourView( flux.subViewsId[layer * 6u + face], getClearValue( SmTexture::eFlux ) );

					blurs.push_back( std::make_unique< GaussianBlur >( graph
						, *previousPass
						, device
						, cuT( "ShadowMapPoint" )
						, name
						, variance.subViewsId[layer * 6u + face]
						, intermediate
						, 5u ) );

					result.emplace_back( std::move( passData ) );
				}
			}

			return result;
		}
	}

	ShadowMapPoint::ShadowMapPoint( crg::ResourceHandler & handler
		, RenderDevice const & device
		, Scene & scene )
		: ShadowMap{ handler
			, device
			, scene
			, LightType::ePoint
			, VkImageCreateFlags( VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT )
			, { ShadowMapPassPoint::TextureSize, ShadowMapPassPoint::TextureSize }
			, 6u * shader::getPointShadowMapCount()
			, shader::getPointShadowMapCount() }
		, m_blurIntermediate{ handler.createImageId( crg::ImageData{ "PointGB"
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
		m_passes = createPasses( handler
			, m_graphs
			, m_runnables
			, m_blurs
			, m_blurIntermediateView
			, device
			, scene
			, *this );
		log::trace << "Created ShadowMapPoint" << std::endl;
	}

	ShadowMapPoint::~ShadowMapPoint()
	{
	}

	void ShadowMapPoint::update( CpuUpdater & updater )
	{
		if ( m_runnables[updater.index] )
		{
			uint32_t offset = updater.index * 6u;
			updater.light->getPointLight()->updateShadow( updater.index );

			for ( uint32_t face = offset; face < offset + 6u; ++face )
			{
				if ( m_passes[face].pass )
				{
					updater.index = face - offset;
					m_passes[face].pass->update( updater );
				}
			}
		}
	}

	void ShadowMapPoint::update( GpuUpdater & updater )
	{
		if ( m_runnables[updater.index] )
		{
			uint32_t offset = updater.index * 6u;

			for ( uint32_t face = offset; face < offset + 6u; ++face )
			{
				if ( m_passes[face].pass )
				{
					updater.index = face - offset;
					m_passes[face].pass->update( updater );
				}
			}
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
