#include "Castor3D/Render/ShadowMap/ShadowMapPoint.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/Passes/GaussianBlur.hpp"
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

namespace c3d
{
	namespace shdmappoint
	{
		static String getPassName( uint32_t index
			, bool needsVsm
			, bool needsRsm
			, bool isStatic )
		{
			auto result = cuT( "PointSML" ) + string::toString( index / 6u ) + cuT( "F" ) + string::toString( index % 6u );

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

	ShadowMapPoint::ShadowMapPoint( crg::ResourcesCache & resources
		, RenderDevice const & device
		, Scene & scene
		, ProgressBar * progress )
		: ShadowMap{ resources
			, device
			, scene
			, LightType::ePoint
			, ImageCreateFlags::eCubeCompatible
			, { ShadowMapPointTextureSize, ShadowMapPointTextureSize }
			, 6u * shader::getPointShadowMapCount()
			, shader::getPointShadowMapCount() }
		, m_blurIntermediate{ resources.getHandler().createImageId( crg::ImageData{ "PointGB"
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
		m_resources.createImage( device.makeContext(), m_blurIntermediate );
		stepProgressBarLocal( progress, cuT( "Creating ShadowMapPoint" ) );
	}

	void ShadowMapPoint::doCreatePass( crg::FramePassGroup & graph
		, uint32_t index, bool vsm, bool rsm, bool isStatic
		, Passes & passes )
	{
		Engine const & engine = *m_scene.getEngine();
		ShadowMapResult & smResult = getShadowPassResult( isStatic );

		doRegisterGraphIO( graph, vsm, rsm, isStatic );

		for ( uint32_t face = 0u; face < 6u; ++face )
		{
			auto faceIndex = index * 6u + face;
			auto debugName = toUtf8( shdmappoint::getPassName( faceIndex, vsm, rsm, isStatic ) );
			auto & group = graph.createPassGroup( debugName );

			if ( m_passes[m_passesIndex].cameraUbos.size() <= faceIndex )
			{
				m_passes[m_passesIndex].cameraUbos.push_back( makeRawUnique< CameraUbo >( m_device ) );
				CU_Require( m_passes[m_passesIndex].cameraUbos.size() > faceIndex );
			}

			auto & cameraUbo = *m_passes[m_passesIndex].cameraUbos[faceIndex];
			passes.passes.emplace_back( makeRawUnique< ShadowMap::PassData >( makeUnique< Viewport >( engine )
				, nullptr ) );
			auto & passData = *passes.passes.back();
			passData.frustum = makeUnique< Frustum >( *passData.viewport );
			passData.ownCuller = makeUniqueDerived< SceneCuller, FrustumCuller >( m_scene, *passData.frustum, isStatic );
			passData.culler = passData.ownCuller.get();
			auto & pass = group.createPass( "Nodes"
				, [&passData, this, vsm, rsm, isStatic, &cameraUbo]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnableGraph )
				{
					auto res = makeRawUnique< ShadowMapPassPoint >( framePass
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
					m_device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
						, res->getTimer() );
					return res;
				} );

			if ( isStatic )
			{
				smResult.setLastAttach( SmTexture::eDepth, faceIndex
					, pass.addOutputDepthTarget( smResult.getTargetViewId( SmTexture::eDepth, faceIndex ), getClearValue( SmTexture::eDepth ).depthStencil() ) );
				smResult.setLastAttach( SmTexture::eLinearDepth, faceIndex
					, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::eLinearDepth, faceIndex ), getClearValue( SmTexture::eLinearDepth ).color() ) );

				if ( vsm )
				{
					smResult.setLastAttach( SmTexture::eVariance, faceIndex
						, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::eVariance, faceIndex ), getClearValue( SmTexture::eVariance ).color() ) );
				}

				if ( rsm )
				{
					smResult.setLastAttach( SmTexture::eNormal, faceIndex
						, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::eNormal, faceIndex ), getClearValue( SmTexture::eNormal ).color() ) );
					smResult.setLastAttach( SmTexture::ePosition, faceIndex
						, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::ePosition, faceIndex ), getClearValue( SmTexture::ePosition ).color() ) );
					smResult.setLastAttach( SmTexture::eFlux, faceIndex
						, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::eFlux, faceIndex ), getClearValue( SmTexture::eFlux ).color() ) );
				}
			}
			else
			{
				smResult.setLastAttach( SmTexture::eDepth, faceIndex
					, pass.addInOutDepthTarget( *smResult.getLastAttach( SmTexture::eDepth, faceIndex ) ) );
				smResult.setLastAttach( SmTexture::eLinearDepth, faceIndex
					, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::eLinearDepth, faceIndex ) ) );

				if ( vsm )
				{
					smResult.setLastAttach( SmTexture::eVariance, faceIndex
						, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::eVariance, faceIndex ) ) );
				}

				if ( rsm )
				{
					smResult.setLastAttach( SmTexture::eNormal, faceIndex
						, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::eNormal, faceIndex ) ) );
					smResult.setLastAttach( SmTexture::ePosition, faceIndex
						, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::ePosition, faceIndex ) ) );
					smResult.setLastAttach( SmTexture::eFlux, faceIndex
						, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::eFlux, faceIndex ) ) );
				}
			}

			if ( isStatic )
			{
				ShadowMapResult & nstSmResult = getShadowPassResult( false );
				auto & copyPass = group.createPass( "CopyToNonStatic"
					, [this, isStatic, faceIndex]( crg::FramePass const & framePass
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
							, crg::ImageCopy::IsEnabledCallback( [this, faceIndex](){ return doEnableCopyStatic( faceIndex ); } ) );
						getOwner()->registerTimer( makeString( framePass.getFullName() )
							, result->getTimer() );
						return result;
					} );
				copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eDepth, faceIndex ) );
				nstSmResult.setLastAttach( SmTexture::eDepth, faceIndex
						, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eDepth, faceIndex ) ) );
				copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eLinearDepth, faceIndex ) );
				nstSmResult.setLastAttach( SmTexture::eLinearDepth, faceIndex
						, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eLinearDepth, faceIndex ) ) );

				if ( vsm )
				{
					copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eVariance, faceIndex ) );
					nstSmResult.setLastAttach( SmTexture::eVariance, faceIndex
						, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eVariance, faceIndex ) ) );
				}

				if ( rsm )
				{
					copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eNormal, faceIndex ) );
					nstSmResult.setLastAttach( SmTexture::eNormal, faceIndex
						, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eNormal, faceIndex ) ) );
					copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::ePosition, faceIndex ) );
					nstSmResult.setLastAttach( SmTexture::ePosition, faceIndex
						, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::ePosition, faceIndex ) ) );
					copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eFlux, faceIndex ) );
					nstSmResult.setLastAttach( SmTexture::eFlux, faceIndex
						, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eFlux, faceIndex ) ) );
				}
			}
			else if ( vsm )
			{
				passes.blurs.push_back( makeUnique< GaussianBlur >( group
					, m_device
					, cuT( "ShadowMapPoint" )
					, *smResult.getLastAttach( SmTexture::eVariance, faceIndex )
					, m_blurIntermediateView
					, 5u
					, crg::ImageCopy::IsEnabledCallback( [this, faceIndex]() { return doEnableBlur( faceIndex ); } ) ) );
				smResult.setLastAttach( SmTexture::eVariance, faceIndex, &passes.blurs.back()->getResultAttach() );
			}
		}
	}

	bool ShadowMapPoint::doIsUpToDate( uint32_t index
		, ShadowMap::Passes const & passes )const
	{
		if ( uint32_t offset = index * 6u;
			passes.passes.size() >= offset + 6u )
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
		if ( uint32_t offset = index * 6u;
			passes.passes.size() >= offset + 6u )
		{
			for ( auto const & data : makeArrayView( passes.passes.begin() + offset, passes.passes.begin() + offset + 6u ) )
			{
				data->pass->setUpToDate();
			}
		}
	}

	void ShadowMapPoint::doUpdate( CpuUpdater & updater
		, ShadowMap::Passes & passes )
	{
		auto oldIndex = updater.index;
		auto oldRenderSize = updater.renderSize;
		updater.renderSize = { ShadowMapPointTextureSize, ShadowMapPointTextureSize };
		uint32_t offset = updater.index * 6u;

		for ( uint32_t face = offset; face < offset + 6u; ++face )
		{
			updater.index = face - offset;
			auto & pass = *passes.passes[face];
			pass.pass->update( updater );

			PointLightInstance const & pointLight = static_cast< PointLightInstance & >( *updater.light );
			m_passes[m_passesIndex].cameraUbos[face]->cpuUpdate( *updater.camera
				, pointLight.getViewMatrix( CubeMapFace( updater.index ) )
				, static_cast< ShadowMapPassPoint const & >( *pass.pass ).getProjection() );
		}

		updater.renderSize = oldRenderSize;
		updater.index = oldIndex;
	}

	void ShadowMapPoint::doUpdate( GpuUpdater & updater
		, ShadowMapPoint::Passes & passes )
	{
		auto save = updater.index;
		PointLightInstance const & pointLight = static_cast< PointLightInstance & >( *updater.light );
		updater.light->updateShadow( *updater.camera, nullptr, int32_t( updater.index ) );
		uint32_t offset = updater.index * 6u;

		for ( uint32_t face = offset; face < offset + 6u; ++face )
		{
			ShadowMapPassPoint const & pass = static_cast< ShadowMapPassPoint & >( *passes.passes[face]->pass );
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
