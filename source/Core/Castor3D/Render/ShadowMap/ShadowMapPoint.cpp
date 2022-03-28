#include "Castor3D/Render/ShadowMap/ShadowMapPoint.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassPoint.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/BlockTracker.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>

#include <algorithm>

namespace castor3d
{
	namespace shdmappoint
	{
		static std::vector< ShadowMap::PassDataPtr > createPass( crg::ResourceHandler & handler
			, std::vector< std::unique_ptr< crg::FrameGraph > > & graphs
			, std::vector< GaussianBlurUPtr > & blurs
			, crg::ImageViewId intermediate
			, uint32_t shadowMapIndex
			, RenderDevice const & device
			, Scene & scene
			, ShadowMap & shadowMap
			, bool vsm
			, bool rsm )
		{
			auto & engine = *scene.getEngine();
			std::vector< ShadowMap::PassDataPtr > result;
			auto & smResult = shadowMap.getShadowPassResult();
			auto & depth = smResult[SmTexture::eDepth];
			auto & linear = smResult[SmTexture::eLinearDepth];
			auto & variance = smResult[SmTexture::eVariance];
			auto & normal = smResult[SmTexture::eNormal];
			auto & position = smResult[SmTexture::ePosition];
			auto & flux = smResult[SmTexture::eFlux];

			std::string debugName = "PointSM" + std::to_string( shadowMapIndex );
			graphs.push_back( std::make_unique< crg::FrameGraph >( handler, debugName ) );
			auto & graph = graphs.back()->getDefaultGroup();
			crg::FramePass const * previousPass{};

			for ( uint32_t face = 0u; face < 6u; ++face )
			{
				result.emplace_back( std::make_unique< ShadowMap::PassData >( std::make_unique< MatrixUbo >( device )
					, castor::makeUnique< Viewport >( engine )
					, nullptr ) );
				auto & passData = *result.back();
				passData.viewport->resize( castor::Size{ ShadowMapPassPoint::TextureSize
					, ShadowMapPassPoint::TextureSize } );
				passData.frustum = castor::makeUnique< Frustum >( *passData.viewport );
				passData.culler = std::make_unique< FrustumCuller >( scene, *passData.frustum );
				auto faceIndex = shadowMapIndex * 6u + face;
				auto name = debugName + "F" + std::to_string( face );
				auto & pass = graph.createPass( name
					, [faceIndex, &passData, &device, &shadowMap, vsm, rsm]( crg::FramePass const & framePass
						, crg::GraphContext & context
						, crg::RunnableGraph & runnableGraph )
					{
						auto res = std::make_unique< ShadowMapPassPoint >( framePass
							, context
							, runnableGraph
							, device
							, faceIndex
							, *passData.matrixUbo
							, *passData.culler
							, shadowMap
							, vsm
							, rsm );
						passData.pass = res.get();
						device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
							, res->getTimer() );
						return res;
					} );

				if ( previousPass )
				{
					pass.addDependency( *previousPass );
				}

				previousPass = &pass;
				pass.addOutputDepthView( depth.subViewsId[faceIndex], getClearValue( SmTexture::eDepth ) );
				pass.addOutputColourView( linear.subViewsId[faceIndex], getClearValue( SmTexture::eLinearDepth ) );

				if ( vsm )
				{
					pass.addOutputColourView( variance.subViewsId[faceIndex], getClearValue( SmTexture::eVariance ) );
				}

				if ( rsm )
				{
					pass.addOutputColourView( normal.subViewsId[faceIndex], getClearValue( SmTexture::eNormal ) );
					pass.addOutputColourView( position.subViewsId[faceIndex], getClearValue( SmTexture::ePosition ) );
					pass.addOutputColourView( flux.subViewsId[faceIndex], getClearValue( SmTexture::eFlux ) );
				}

				if ( vsm )
				{
					blurs.push_back( castor::makeUnique< GaussianBlur >( graph
						, *previousPass
						, device
						, cuT( "ShadowMapPoint" )
						, name
						, variance.subViewsId[faceIndex]
						, intermediate
						, 5u ) );
				}
			}

			return result;
		}
	}

	ShadowMapPoint::ShadowMapPoint( crg::ResourceHandler & handler
		, RenderDevice const & device
		, Scene & scene
		, ProgressBar * progress )
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
			, getFormat( device, SmTexture::eVariance )
			, ( *m_result.begin() )->getExtent()
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) } ) }
		, m_blurIntermediateView{ handler.createViewId( crg::ImageViewData{ m_blurIntermediate.data->name
			, m_blurIntermediate
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, getFormat( m_blurIntermediate )
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } ) }
	{
		stepProgressBar( progress, "Creating ShadowMapPoint" );
	}

	void ShadowMapPoint::update( GpuUpdater & updater )
	{
		auto & myPasses = m_passes[m_passesIndex];

		if ( myPasses.runnables.size() > updater.index
			&& myPasses.runnables[updater.index] )
		{
			auto & pointLight = *updater.light->getPointLight();
			pointLight.updateShadow( int32_t( updater.index ) );
			uint32_t offset = updater.index * 6u;

			for ( uint32_t face = offset; face < offset + 6u; ++face )
			{
				auto & pass = static_cast< ShadowMapPassPoint & >( *myPasses.passes[face]->pass );
				updater.index = face - offset;
				pass.update( updater );
				pass.updateFrustum( pointLight.getViewMatrix( CubeMapFace( updater.index ) ) );
			}
		}
	}

	std::vector< ShadowMap::PassDataPtr > ShadowMapPoint::doCreatePass( uint32_t index
		, bool vsm
		, bool rsm )
	{
		auto & myPasses = m_passes[m_passesIndex];
		return shdmappoint::createPass( m_handler
			, myPasses.graphs
			, myPasses.blurs
			, m_blurIntermediateView
			, index
			, m_device
			, m_scene
			, *this
			, vsm
			, rsm );
	}

	bool ShadowMapPoint::doIsUpToDate( uint32_t index )const
	{
		auto & myPasses = m_passes[m_passesIndex];
		uint32_t offset = index * 6u;

		if ( myPasses.passes.size() >= offset + 6u )
		{
			return std::all_of( myPasses.passes.begin() + offset
				, myPasses.passes.begin() + offset + 6u
				, []( ShadowMap::PassDataPtr const & data )
				{
					return data->pass->isUpToDate();
				} );
		}

		return true;
	}

	void ShadowMapPoint::doSetUpToDate( uint32_t index )
	{
		auto & myPasses = m_passes[m_passesIndex];
		uint32_t offset = index * 6u;

		if ( myPasses.passes.size() >= offset + 6u )
		{
			for ( auto & data : castor::makeArrayView( myPasses.passes.begin() + offset, myPasses.passes.begin() + offset + 6u ) )
			{
				data->pass->setUpToDate();
			}
		}
	}

	void ShadowMapPoint::doUpdate( CpuUpdater & updater )
	{
		auto & myPasses = m_passes[m_passesIndex];

		if ( myPasses.runnables.size() > updater.index
			&& myPasses.runnables[updater.index] )
		{
			uint32_t offset = updater.index * 6u;

			for ( uint32_t face = offset; face < offset + 6u; ++face )
			{
				updater.index = face - offset;
				myPasses.passes[face]->pass->update( updater );
			}
		}
	}

	uint32_t ShadowMapPoint::doGetMaxCount()const
	{
		return shader::getPointShadowMapCount();
	}
}
