#include "Castor3D/Render/ShadowMap/ShadowMapSpot.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/Passes/GaussianBlur.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassSpot.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/ImageCopy.hpp>

namespace c3d
{
	namespace shdmapspot
	{
		static String getPassName( uint32_t index
			, bool needsVsm
			, bool needsRsm
			, bool isStatic )
		{
			auto result = cuT( "SpotSM" ) + string::toString( index );

			if ( needsVsm )
			{
				result += cuT( "_VSM" );
			}

			if ( needsRsm )
			{
				result += cuT( "_RSM" );
			}

			if ( isStatic )
			{
				result += cuT( "_Statics" );
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
			, ImageCreateFlags::eNone
			, Size{ ShadowMapSpotTextureSize, ShadowMapSpotTextureSize }
			, shader::getSpotShadowMapCount()
			, shader::getSpotShadowMapCount() }
		, m_blurIntermediate{ resources.getHandler().createImageId( crg::ImageData{ "SpotGB"
			, ImageCreateFlags::eNone
			, ImageType::e2D
			, getFormat( device, SmTexture::eVariance )
			, ( *m_result.begin() )->getExtent()
			, ( ImageUsageFlags::eColorAttachment
				| ImageUsageFlags::eSampled
				| ImageUsageFlags::eTransferSrc ) } ) }
		, m_blurIntermediateView{ resources.getHandler().createViewId( crg::ImageViewData{ m_blurIntermediate.data->name
			, m_blurIntermediate
			, ImageViewCreateFlags::eNone
			, ImageViewType::e2D
			, getFormat( m_blurIntermediate )
			, { ImageAspectFlags::eColor, 0u, 1u, 0u, 1u } } ) }
	{
		stepProgressBarLocal( progress, cuT( "Creating ShadowMapSpot" ) );
		m_resources.createImage( device.makeContext(), m_blurIntermediate );
	}

	void ShadowMapSpot::doCreatePass( crg::FramePassGroup & graph
		, uint32_t index, bool vsm, bool rsm, bool isStatic
		, Passes & passes )
	{
		Engine const & engine = *m_scene.getEngine();
		Viewport viewport{ engine };
		ShadowMapResult & smResult = getShadowPassResult( isStatic );

		auto debugName = toUtf8( shdmapspot::getPassName( index, vsm, rsm, isStatic ) );
		doRegisterGraphIO( graph, vsm, rsm, isStatic );

		if ( m_passes[m_passesIndex].cameras.size() <= index )
		{
			m_passes[m_passesIndex].cameraUbos.push_back( makeRawUnique< CameraUbo >( m_device ) );
			m_passes[m_passesIndex].cameras.push_back( m_scene.createCamera( shdmapspot::getPassName( index, false, false, false )
				, m_scene
				, *m_scene.getCameraRootNode()
				, c3d::move( viewport ) ) );
			CU_Require( m_passes[m_passesIndex].cameras.size() > index );
		}

		auto & group = graph.createPassGroup( debugName );
		auto & camera = *m_passes[m_passesIndex].cameras[index];
		auto & cameraUbo = *m_passes[m_passesIndex].cameraUbos[index];
		passes.passes.emplace_back( makeRawUnique< ShadowMap::PassData >( nullptr ) );
		auto & passData = *passes.passes.back();
		passData.ownCuller = makeUniqueDerived< SceneCuller, FrustumCuller >( m_scene, camera, isStatic );
		passData.culler = passData.ownCuller.get();
		auto & pass = group.createPass( "Nodes"
			, [&passData, this, vsm, rsm, isStatic, &cameraUbo]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = makeRawUnique< ShadowMapPassSpot >( framePass
					, context
					, runnableGraph
					, m_device
					, cameraUbo
					, *m_renderUbo
					, *passData.culler
					, *this
					, vsm
					, rsm
					, isStatic );
				passData.pass = res.get();
				c3d::getEngine( m_device ).registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );

		if ( isStatic )
		{
			smResult.setLastAttach( SmTexture::eDepth, index
				, pass.addOutputDepthTarget( smResult.getTargetViewId( SmTexture::eDepth, index ), getClearValue( SmTexture::eDepth ).depthStencil() ) );
			smResult.setLastAttach( SmTexture::eLinearDepth, index
				, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::eLinearDepth, index ), getClearValue( SmTexture::eLinearDepth ).color() ) );

			if ( vsm )
			{
				smResult.setLastAttach( SmTexture::eVariance, index
					, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::eVariance, index ), getClearValue( SmTexture::eVariance ).color() ) );
			}

			if ( rsm )
			{
				smResult.setLastAttach( SmTexture::eNormal, index
					, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::eNormal, index ), getClearValue( SmTexture::eNormal ).color() ) );
				smResult.setLastAttach( SmTexture::ePosition, index
					, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::ePosition, index ), getClearValue( SmTexture::ePosition ).color() ) );
				smResult.setLastAttach( SmTexture::eFlux, index
					, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::eFlux, index ), getClearValue( SmTexture::eFlux ).color() ) );
			}
		}
		else
		{
			smResult.setLastAttach( SmTexture::eDepth, index
				, pass.addInOutDepthTarget( *smResult.getLastAttach( SmTexture::eDepth, index ) ) );
			smResult.setLastAttach( SmTexture::eLinearDepth, index
				, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::eLinearDepth, index ) ) );

			if ( vsm )
			{
				smResult.setLastAttach( SmTexture::eVariance, index
					, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::eVariance, index ) ) );
			}

			if ( rsm )
			{
				smResult.setLastAttach( SmTexture::eNormal, index
					, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::eNormal, index ) ) );
				smResult.setLastAttach( SmTexture::ePosition, index
					, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::ePosition, index ) ) );
				smResult.setLastAttach( SmTexture::eFlux, index
					, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::eFlux, index ) ) );
			}
		}

		 if ( isStatic )
		{
			ShadowMapResult & nstSmResult = getShadowPassResult( false );
			auto & copyPass = group.createPass( debugName + "/CopyToNonStatic"
				, [this, isStatic, index]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnableGraph )
				{
					auto result = makeRawUnique< crg::ImageCopy >( framePass
						, context
						, runnableGraph
						, getShadowPassResult( isStatic ).getExtent()
						, ImageLayout::eShaderReadOnly
						, crg::ru::Config{}
						, crg::ImageCopy::GetPassIndexCallback( [](){ return 0u; } )
						, crg::ImageCopy::IsEnabledCallback( [this, index](){ return doEnableCopyStatic( index ); } ) );
					getOwner()->registerTimer( makeString( framePass.getFullName() )
						, result->getTimer() );
					return result;
				} );
			copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eDepth, index ) );
			nstSmResult.setLastAttach( SmTexture::eDepth, index
					, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eDepth, index ) ) );
			copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eLinearDepth, index ) );
			nstSmResult.setLastAttach( SmTexture::eLinearDepth, index
					, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eLinearDepth, index ) ) );

			if ( vsm )
			{
				copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eVariance, index ) );
				nstSmResult.setLastAttach( SmTexture::eVariance, index
					, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eVariance, index ) ) );
			}

			if ( rsm )
			{
				copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eNormal, index ) );
				nstSmResult.setLastAttach( SmTexture::eNormal, index
					, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eNormal, index ) ) );
				copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::ePosition, index ) );
				nstSmResult.setLastAttach( SmTexture::ePosition, index
					, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::ePosition, index ) ) );
				copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eFlux, index ) );
				nstSmResult.setLastAttach( SmTexture::eFlux, index
					, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eFlux, index ) ) );
			}
		}
		else if ( vsm )
		{
			passes.blurs.push_back( makeUnique< GaussianBlur >( group
				, m_device
				, cuT( "ShadowMapSpot" )
				, *smResult.getLastAttach( SmTexture::eVariance, index )
				, m_blurIntermediateView
				, 5u
				, crg::ImageCopy::IsEnabledCallback( [this, index]() { return doEnableBlur( index ); } ) ) );
			smResult.setLastAttach( SmTexture::eVariance, index, &passes.blurs.back()->getResultAttach() );
		}
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
		auto oldRenderSize = updater.renderSize;
		updater.renderSize = { ShadowMapSpotTextureSize, ShadowMapSpotTextureSize };

		auto & pass = *passes.passes[updater.index];
		pass.pass->update( updater );

		auto const & myCamera = pass.pass->getCuller().getCamera();
		m_passes[m_passesIndex].cameraUbos[updater.index]->cpuUpdate( myCamera );

		updater.renderSize = oldRenderSize;
	}

	void ShadowMapSpot::doUpdate( GpuUpdater & updater
		, ShadowMap::Passes & passes )
	{
		auto const & pass = *passes.passes[updater.index]->pass;
		auto & myCamera = pass.getCuller().getCamera();
		updater.light->updateShadow( *updater.camera
			, &myCamera
			, int32_t( updater.index ) );
	}

	uint32_t ShadowMapSpot::doGetMaxCount()const
	{
		return shader::getSpotShadowMapCount();
	}
}
