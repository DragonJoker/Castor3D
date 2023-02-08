#include "Castor3D/Render/ShadowMap/ShadowMapPoint.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
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
#include <RenderGraph/RunnablePasses/ImageCopy.hpp>

#include <algorithm>

namespace castor3d
{
	namespace shdmappoint
	{
		static castor::String getPassName( uint32_t index
			, bool needsVsm
			, bool needsRsm
			, bool isStatic )
		{
			auto result = cuT( "PointSML" ) + castor::string::toString( index / 6u ) + "F" + castor::string::toString( index % 6u );

			if ( needsVsm )
			{
				result += "_VSM";
			}

			if ( needsRsm )
			{
				result += "_RSM";
			}

			if ( isStatic )
			{
				result += "_Statics";
			}

			return result;
		}
	}

	ShadowMapPoint::ShadowMapPoint( crg::ResourcesCache & resources
		, RenderDevice const & device
		, Scene & scene
		, ProgressBar * progress )
		: ShadowMap{ resources
			, device
			, scene
			, LightType::ePoint
			, VkImageCreateFlags( VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT )
			, { ShadowMapPointTextureSize, ShadowMapPointTextureSize }
			, 6u * shader::getPointShadowMapCount()
			, shader::getPointShadowMapCount() }
		, m_blurIntermediate{ resources.getHandler().createImageId( crg::ImageData{ "PointGB"
			, 0u
			, VK_IMAGE_TYPE_2D
			, getFormat( device, SmTexture::eVariance )
			, ( *m_result.begin() )->getExtent()
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) } ) }
		, m_blurIntermediateView{ resources.getHandler().createViewId( crg::ImageViewData{ m_blurIntermediate.data->name
			, m_blurIntermediate
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, getFormat( m_blurIntermediate )
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } ) }
	{
		stepProgressBar( progress, "Creating ShadowMapPoint" );
	}

	crg::FramePassArray ShadowMapPoint::doCreatePass( crg::FrameGraph & graph
		, crg::FramePassArray const & previousPasses
		, uint32_t index
		, bool vsm
		, bool rsm
		, bool isStatic
		, Passes & passes )
	{
		auto & engine = *m_scene.getEngine();
		auto & smResult = getShadowPassResult( isStatic );
		auto & depth = smResult[SmTexture::eDepth];
		auto & linear = smResult[SmTexture::eLinearDepth];
		auto & variance = smResult[SmTexture::eVariance];
		auto & normal = smResult[SmTexture::eNormal];
		auto & position = smResult[SmTexture::ePosition];
		auto & flux = smResult[SmTexture::eFlux];

		std::string debugName = shdmappoint::getPassName( index, vsm, rsm, isStatic );
		doRegisterGraphIO( graph, vsm, rsm, isStatic );

		auto & group = graph.getDefaultGroup();
		crg::FramePass const * previousPass{};
		crg::FramePassArray result;

		for ( uint32_t face = 0u; face < 6u; ++face )
		{
			passes.passes.emplace_back( std::make_unique< ShadowMap::PassData >( std::make_unique< CameraUbo >( m_device )
				, castor::makeUnique< Viewport >( engine )
				, nullptr ) );
			auto & passData = *passes.passes.back();
			passData.viewport->resize( castor::Size{ ShadowMapPointTextureSize
				, ShadowMapPointTextureSize } );
			passData.frustum = castor::makeUnique< Frustum >( *passData.viewport );
			passData.ownCuller = castor::makeUniqueDerived< SceneCuller, FrustumCuller >( m_scene, *passData.frustum );
			passData.culler = passData.ownCuller.get();
			auto faceIndex = index * 6u + face;
			auto name = debugName + "F" + std::to_string( face );
			auto & pass = group.createPass( name
				, [faceIndex, &passData, this, vsm, rsm, isStatic]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnableGraph )
				{
					auto res = std::make_unique< ShadowMapPassPoint >( framePass
						, context
						, runnableGraph
						, m_device
						, faceIndex
						, *passData.cameraUbo
						, *passData.culler
						, *this
						, vsm
						, rsm
						, isStatic );
					passData.pass = res.get();
					m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
						, res->getTimer() );
					return res;
				} );

			if ( !isStatic )
			{
				pass.addDependency( *previousPasses[face] );
			}

			if ( previousPass )
			{
				pass.addDependency( *previousPass );
			}

			previousPass = &pass;

			if ( isStatic )
			{
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
			}
			else
			{
				pass.addInOutDepthView( depth.subViewsId[faceIndex] );
				pass.addInOutColourView( linear.subViewsId[faceIndex] );

				if ( vsm )
				{
					pass.addInOutColourView( variance.subViewsId[faceIndex] );
				}

				if ( rsm )
				{
					pass.addInOutColourView( normal.subViewsId[faceIndex] );
					pass.addInOutColourView( position.subViewsId[faceIndex] );
					pass.addInOutColourView( flux.subViewsId[faceIndex] );
				}
			}

			if ( vsm && !isStatic )
			{
				passes.blurs.push_back( castor::makeUnique< GaussianBlur >( group
					, *previousPass
					, m_device
					, cuT( "ShadowMapPoint" )
					, name
					, variance.subViewsId[faceIndex]
					, m_blurIntermediateView
					, 5u ) );
				result.push_back( &passes.blurs.back()->getLastPass() );
			}
			else if ( isStatic )
			{
				auto & nstSmResult = getShadowPassResult( false );
				auto & copyPass = graph.createPass( name + "/CopyToNonStatic"
					, [this, isStatic]( crg::FramePass const & pass
						, crg::GraphContext & context
						, crg::RunnableGraph & runnableGraph )
					{
						auto result = std::make_unique< crg::ImageCopy >( pass
							, context
							, runnableGraph
							, getShadowPassResult( isStatic )[SmTexture::eDepth].getExtent()
							, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
							, crg::ru::Config{} );
						getOwner()->registerTimer( pass.getFullName()
							, result->getTimer() );
						return result;
					} );
				copyPass.addDependency( *previousPass );
				copyPass.addTransferInputView( depth.subViewsId[faceIndex] );
				copyPass.addTransferOutputView( nstSmResult[SmTexture::eDepth].subViewsId[faceIndex] );
				copyPass.addTransferInputView( linear.subViewsId[faceIndex] );
				copyPass.addTransferOutputView( nstSmResult[SmTexture::eLinearDepth].subViewsId[faceIndex] );

				if ( vsm )
				{
					copyPass.addTransferInputView( variance.subViewsId[faceIndex] );
					copyPass.addTransferOutputView( nstSmResult[SmTexture::eVariance].subViewsId[faceIndex] );
				}

				if ( rsm )
				{
					copyPass.addTransferInputView( normal.subViewsId[faceIndex] );
					copyPass.addTransferOutputView( nstSmResult[SmTexture::eNormal].subViewsId[faceIndex] );
					copyPass.addTransferInputView( position.subViewsId[faceIndex] );
					copyPass.addTransferOutputView( nstSmResult[SmTexture::ePosition].subViewsId[faceIndex] );
					copyPass.addTransferInputView( flux.subViewsId[faceIndex] );
					copyPass.addTransferOutputView( nstSmResult[SmTexture::eFlux].subViewsId[faceIndex] );
				}

				previousPass = &copyPass;
				result.push_back( previousPass );
			}
			else
			{
				result.push_back( &pass );
			}
		}

		return result;
	}

	bool ShadowMapPoint::doIsUpToDate( uint32_t index
		, ShadowMap::Passes const & passes )const
	{
		uint32_t offset = index * 6u;

		if ( passes.passes.size() >= offset + 6u )
		{
			return std::all_of( passes.passes.begin() + offset
				, passes.passes.begin() + offset + 6u
				, []( ShadowMap::PassDataPtr const & data )
				{
					return data->pass->isUpToDate();
				} );
		}

		return true;
	}

	void ShadowMapPoint::doSetUpToDate( uint32_t index
		, ShadowMap::Passes & passes )
	{
		uint32_t offset = index * 6u;

		if ( passes.passes.size() >= offset + 6u )
		{
			for ( auto & data : castor::makeArrayView( passes.passes.begin() + offset, passes.passes.begin() + offset + 6u ) )
			{
				data->pass->setUpToDate();
			}
		}
	}

	void ShadowMapPoint::doUpdate( CpuUpdater & updater
		, ShadowMap::Passes & passes )
	{
		auto save = updater.index;
		uint32_t offset = updater.index * 6u;

		for ( uint32_t face = offset; face < offset + 6u; ++face )
		{
			updater.index = face - offset;
			auto & pass = *passes.passes[face];
			pass.pass->update( updater );

			auto & pointLight = *updater.light->getPointLight();
			pass.cameraUbo->cpuUpdate( *updater.camera
				, false
				, pointLight.getViewMatrix( CubeMapFace( updater.index ) )
				, static_cast< ShadowMapPassPoint const & >( *pass.pass ).getProjection() );
		}

		updater.index = save;
	}

	void ShadowMapPoint::doUpdate( GpuUpdater & updater
		, ShadowMapPoint::Passes & passes )
	{
		auto save = updater.index;
		auto & pointLight = *updater.light->getPointLight();
		pointLight.updateShadow( int32_t( updater.index ) );
		uint32_t offset = updater.index * 6u;

		for ( uint32_t face = offset; face < offset + 6u; ++face )
		{
			auto & pass = static_cast< ShadowMapPassPoint & >( *passes.passes[face]->pass );
			updater.index = face - offset;
			pass.updateFrustum( pointLight.getViewMatrix( CubeMapFace( updater.index ) ) );
		}

		updater.index = save;
	}

	uint32_t ShadowMapPoint::doGetMaxCount()const
	{
		return shader::getPointShadowMapCount();
	}
}
