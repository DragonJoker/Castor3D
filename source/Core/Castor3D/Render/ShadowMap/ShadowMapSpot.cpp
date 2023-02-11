#include "Castor3D/Render/ShadowMap/ShadowMapSpot.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
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
#include <RenderGraph/RunnablePasses/ImageCopy.hpp>

namespace castor3d
{
	namespace shdmapspot
	{
		static castor::String getPassName( uint32_t index
			, bool needsVsm
			, bool needsRsm
			, bool isStatic )
		{
			auto result = cuT( "SpotSM" ) + castor::string::toString( index );

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

	ShadowMapSpot::ShadowMapSpot( crg::ResourcesCache & resources
		, RenderDevice const & device
		, Scene & scene
		, ProgressBar * progress )
		: ShadowMap{ resources
			, device
			, scene
			, LightType::eSpot
			, 0u
			, castor::Size{ ShadowMapSpotTextureSize, ShadowMapSpotTextureSize }
			, shader::getSpotShadowMapCount()
			, shader::getSpotShadowMapCount() }
		, m_blurIntermediate{ resources.getHandler().createImageId( crg::ImageData{ "SpotGB"
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
		stepProgressBar( progress, "Creating ShadowMapSpot" );
	}

	crg::FramePassArray ShadowMapSpot::doCreatePass( crg::FrameGraph & graph
		, crg::FramePassArray const & previousPasses
		, uint32_t index
		, bool vsm
		, bool rsm
		, bool isStatic
		, Passes & passes )
	{
		auto & engine = *m_scene.getEngine();
		Viewport viewport{ engine };
		viewport.resize( castor::Size{ ShadowMapSpotTextureSize, ShadowMapSpotTextureSize } );
		auto & smResult = getShadowPassResult( isStatic );
		auto & depth = smResult[SmTexture::eDepth];
		auto & linear = smResult[SmTexture::eLinearDepth];
		auto & variance = smResult[SmTexture::eVariance];
		auto & normal = smResult[SmTexture::eNormal];
		auto & position = smResult[SmTexture::ePosition];
		auto & flux = smResult[SmTexture::eFlux];

		std::string debugName = shdmapspot::getPassName( index, vsm, rsm, isStatic );
		doRegisterGraphIO( graph, vsm, rsm, isStatic );

		auto & group = graph.getDefaultGroup();

		if ( m_passes[m_passesIndex].cameras.size() <= index )
		{
			m_passes[m_passesIndex].cameraUbos.push_back( std::make_unique< CameraUbo >( m_device ) );
			m_passes[m_passesIndex].cameras.push_back( std::make_shared< Camera >( cuT( "ShadowMapSpot" )
				, m_scene
				, *m_scene.getCameraRootNode()
				, std::move( viewport ) ) );
			CU_Require( m_passes[m_passesIndex].cameras.size() > index );
		}

		auto & camera = *m_passes[m_passesIndex].cameras[index];
		auto & cameraUbo = *m_passes[m_passesIndex].cameraUbos[index];
		passes.passes.emplace_back( std::make_unique< ShadowMap::PassData >( nullptr ) );
		auto & passData = *passes.passes.back();
		passData.ownCuller = castor::makeUniqueDerived< SceneCuller, FrustumCuller >( m_scene, camera, isStatic );
		passData.culler = passData.ownCuller.get();
		auto & pass = group.createPass( debugName
			, [index, &passData, this, vsm, rsm, isStatic, &cameraUbo]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = std::make_unique< ShadowMapPassSpot >( framePass
					, context
					, runnableGraph
					, m_device
					, index
					, cameraUbo
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
			pass.addDependency( *previousPasses.front() );
		}

		auto previousPass = &pass;

		if ( isStatic )
		{
			pass.addOutputDepthView( depth.subViewsId[index], getClearValue( SmTexture::eDepth ) );
			pass.addOutputColourView( linear.subViewsId[index], getClearValue( SmTexture::eLinearDepth ) );

			if ( vsm )
			{
				pass.addOutputColourView( variance.subViewsId[index], getClearValue( SmTexture::eVariance ) );
			}

			if ( rsm )
			{
				pass.addOutputColourView( normal.subViewsId[index], getClearValue( SmTexture::eNormal ) );
				pass.addOutputColourView( position.subViewsId[index], getClearValue( SmTexture::ePosition ) );
				pass.addOutputColourView( flux.subViewsId[index], getClearValue( SmTexture::eFlux ) );
			}
		}
		else
		{
			pass.addInOutDepthView( depth.subViewsId[index] );
			pass.addInOutColourView( linear.subViewsId[index] );

			if ( vsm )
			{
				pass.addInOutColourView( variance.subViewsId[index] );
			}

			if ( rsm )
			{
				pass.addInOutColourView( normal.subViewsId[index] );
				pass.addInOutColourView( position.subViewsId[index] );
				pass.addInOutColourView( flux.subViewsId[index] );
			}
		}

		crg::FramePassArray result;

		 if ( isStatic )
		{
			auto & nstSmResult = getShadowPassResult( false );
			auto & copyPass = graph.createPass( debugName + "/CopyToNonStatic"
				, [this, isStatic, index]( crg::FramePass const & pass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnableGraph )
				{
					auto result = std::make_unique< crg::ImageCopy >( pass
						, context
						, runnableGraph
						, getShadowPassResult( isStatic )[SmTexture::eDepth].getExtent()
						, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
						, crg::ru::Config{}
						, crg::ImageCopy::GetPassIndexCallback( [](){ return 0u; } )
						, crg::ImageCopy::IsEnabledCallback( [this, index](){ return doEnableCopyStatic( index ); } ) );
					getOwner()->registerTimer( pass.getFullName()
						, result->getTimer() );
					return result;
				} );
			copyPass.addDependency( *previousPass );
			copyPass.addTransferInputView( depth.subViewsId[index] );
			copyPass.addTransferOutputView( nstSmResult[SmTexture::eDepth].subViewsId[index] );
			copyPass.addTransferInputView( linear.subViewsId[index] );
			copyPass.addTransferOutputView( nstSmResult[SmTexture::eLinearDepth].subViewsId[index] );

			if ( vsm )
			{
				copyPass.addTransferInputView( variance.subViewsId[index] );
				copyPass.addTransferOutputView( nstSmResult[SmTexture::eVariance].subViewsId[index] );
			}

			if ( rsm )
			{
				copyPass.addTransferInputView( normal.subViewsId[index] );
				copyPass.addTransferOutputView( nstSmResult[SmTexture::eNormal].subViewsId[index] );
				copyPass.addTransferInputView( position.subViewsId[index] );
				copyPass.addTransferOutputView( nstSmResult[SmTexture::ePosition].subViewsId[index] );
				copyPass.addTransferInputView( flux.subViewsId[index] );
				copyPass.addTransferOutputView( nstSmResult[SmTexture::eFlux].subViewsId[index] );
			}

			previousPass = &copyPass;
			result.push_back( previousPass );
		}
		else if ( vsm )
		{
			passes.blurs.push_back( castor::makeUnique< GaussianBlur >( group
				, *previousPass
				, m_device
				, cuT( "ShadowMapSpot" )
				, debugName
				, variance.subViewsId[index]
				, m_blurIntermediateView
				, 5u
				, crg::ImageCopy::IsEnabledCallback( [this, index]() { return doEnableBlur( index ); } ) ) );
			result.push_back( &passes.blurs.back()->getLastPass() );
		}
		else
		{
			result.push_back( &pass );
		}

		return result;
	}

	bool ShadowMapSpot::doIsUpToDate( uint32_t index
		, ShadowMap::Passes const & passes )const
	{
		if ( passes.passes.size() > index )
		{
			return passes.passes[index]->pass->isUpToDate();
		}

		return true;
	}

	void ShadowMapSpot::doSetUpToDate( uint32_t index
		, ShadowMap::Passes & passes )
	{
		if ( passes.passes.size() > index )
		{
			passes.passes[index]->pass->setUpToDate();
		}
	}

	void ShadowMapSpot::doUpdate( CpuUpdater & updater
		, ShadowMap::Passes & passes )
	{
		auto & pass = *passes.passes[updater.index];
		pass.pass->update( updater );

		auto & myCamera = pass.pass->getCuller().getCamera();
		m_passes[m_passesIndex].cameraUbos[updater.index]->cpuUpdate( myCamera, false );
	}

	void ShadowMapSpot::doUpdate( GpuUpdater & updater
		, ShadowMap::Passes & passes )
	{
		auto & light = *updater.light;
		auto & pass = *passes.passes[updater.index]->pass;
		auto & myCamera = pass.getCuller().getCamera();
		light.getSpotLight()->updateShadow( myCamera
			, int32_t( updater.index ) );
	}

	uint32_t ShadowMapSpot::doGetMaxCount()const
	{
		return shader::getSpotShadowMapCount();
	}
}
