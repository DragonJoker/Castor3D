#include "DepthOfFieldPostEffect/DepthOfFieldPostEffect.hpp"

#include "DepthOfFieldPostEffect/Combine.hpp"
#include "DepthOfFieldPostEffect/ComputeCircleOfConfusion.hpp"
#include "DepthOfFieldPostEffect/FirstBlur.hpp"
#include "DepthOfFieldPostEffect/SecondBlur.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderTechnique.hpp>
#include <Castor3D/Render/Texture.hpp>
#include <Castor3D/Scene/Camera.hpp>

#include <numeric>

namespace dof
{
	//*********************************************************************************************

	c3d::String const PostEffect::Type = cuT( "depth_of_field" );
	c3d::MbString const PostEffect::Name = "Depth Of Field PostEffect";
	c3d::String const PostEffect::FocalDistance = cuT( "focalDistance" );
	c3d::String const PostEffect::FocalLength = cuT( "focalLength" );
	c3d::String const PostEffect::BokehScale = cuT( "bokehScale" );
	c3d::String const PostEffect::EnableFarBlur = cuT( "enableFarBlur" );

	PostEffect::PostEffect( c3d::RenderTarget & renderTarget
		, c3d::RenderSystem & renderSystem
		, c3d::Parameters const & params )
		: c3d::PostEffect{ PostEffect::Type
			, cuT( "DepthOfField" )
			, c3d::makeString( PostEffect::Name )
			, renderTarget
			, renderSystem
			, params }
		, m_ubo{ renderSystem.getRenderDevice() }
	{
		setParameters( params );
	}

	c3d::PostEffectUPtr PostEffect::create( c3d::RenderTarget & renderTarget
		, c3d::RenderSystem & renderSystem
		, c3d::Parameters const & params )
	{
		return c3d::makeUniqueDerived< c3d::PostEffect, PostEffect >( renderTarget
			, renderSystem
			, params );
	}

	void PostEffect::accept( c3d::ConfigurationVisitorBase & visitor )
	{
		m_data.accept( visitor );
	}

	void PostEffect::setParameters( c3d::Parameters parameters )
	{
		m_data.setParameters( std::move( parameters ) );
	}

	bool PostEffect::doInitialise( c3d::RenderDevice const & device
		, c3d::Texture const & source
		, c3d::Texture const & target
		, crg::FramePass const & previousPass )
	{
		auto extent = c3d::convert( ashes::getSubresourceDimensions( convert( target.getExtent() ), 1u ) );
		m_nearCoC = c3d::Texture{ device
			, m_renderTarget.getResources()
			, "DoFNearCoC"
			, { c3d::ImageCreateFlags::eNone
				, extent, 1u, 1u
				, c3d::PixelFormat::eR16_SFLOAT
				, ( c3d::ImageUsageFlags::eColorAttachment
					| c3d::ImageUsageFlags::eSampled ) }
			, {} };
		m_nearBlur = c3d::Texture{ device
			, m_renderTarget.getResources()
			, "DoFNearBlur"
			, { c3d::ImageCreateFlags::eNone
				, extent, 1u, 1u
				, target.getFormat()
				, ( c3d::ImageUsageFlags::eColorAttachment
					| c3d::ImageUsageFlags::eSampled ) }
			, {} };
		m_farCoC = c3d::Texture{ device
			, m_renderTarget.getResources()
			, "DoFFarCoC"
			, { c3d::ImageCreateFlags::eNone
				, extent, 1u, 1u
				, c3d::PixelFormat::eR16_SFLOAT
				, ( c3d::ImageUsageFlags::eColorAttachment
					| c3d::ImageUsageFlags::eSampled ) }
			, {} };
		m_farBlur = c3d::Texture{ device
			, m_renderTarget.getResources()
			, "DoFFarBlur"
			, { c3d::ImageCreateFlags::eNone
				, extent, 1u, 1u
				, target.getFormat()
				, ( c3d::ImageUsageFlags::eColorAttachment
					| c3d::ImageUsageFlags::eSampled ) }
			, {} };
		m_intermediate = c3d::Texture{ device
			, m_renderTarget.getResources()
			, "DoFFarBlur"
			, { c3d::ImageCreateFlags::eNone
				, extent, 1u, 1u
				, target.getFormat()
				, ( c3d::ImageUsageFlags::eColorAttachment
					| c3d::ImageUsageFlags::eSampled ) }
			, {} };
		m_nearCoC.create();
		m_farCoC.create();
		m_nearBlur.create();
		m_farBlur.create();
		m_intermediate.create();
		auto passes = createComputeCircleOfConfusionPass( device
			, m_graph
			, crg::FramePassArray{ &previousPass }
			, m_ubo
			, m_renderTarget.getTechnique().getDepthObj()
			, crg::ImageViewIdArray{ source.sampledViewId, target.sampledViewId }
			, m_nearCoC
			, m_farCoC
			, &isEnabled()
			, &m_passIndex );
		auto & nearGroup = m_graph.createPassGroup( "Near" );
		m_blurNearCoC = std::make_unique< c3d::GaussianBlur >( nearGroup
			, *passes.front()
			, device
			, "Near"
			, m_nearCoC.sampledViewId
			, 5u
			, crg::RunnablePass::IsEnabledCallback( [this]() { return isEnabled(); } ) );
		passes = createFirstBlurPass( device
			, nearGroup
			, { &m_blurNearCoC->getLastPass() }
			, m_ubo
			, crg::ImageViewIdArray{ source.sampledViewId, target.sampledViewId }
			, m_nearCoC
			, m_intermediate
			, crg::RunnablePass::IsEnabledCallback( [this]() { return isEnabled(); } )
			, &m_passIndex );
		passes = createSecondBlurPass( device
			, nearGroup
			, std::move( passes )
			, m_ubo
			, m_intermediate
			, m_nearBlur
			, crg::RunnablePass::IsEnabledCallback( [this]() { return isEnabled(); } )
			, &m_passIndex );

		auto & farGroup = m_graph.createPassGroup( "Far" );
		m_blurFarCoC = std::make_unique< c3d::GaussianBlur >( farGroup
			, *passes.front()
			, device
			, "Far"
			, m_farCoC.sampledViewId
			, 5u
			, crg::RunnablePass::IsEnabledCallback( [this]() { return isEnabled() && m_data.enableFarBlur; } ) );
		passes = createFirstBlurPass( device
			, farGroup
			, { &m_blurFarCoC->getLastPass() }
			, m_ubo
			, crg::ImageViewIdArray{ source.sampledViewId, target.sampledViewId }
			, m_farCoC
			, m_intermediate
			, crg::RunnablePass::IsEnabledCallback( [this]() { return isEnabled() && m_data.enableFarBlur; } )
			, &m_passIndex );
		passes = createSecondBlurPass( device
			, farGroup
			, std::move( passes )
			, m_ubo
			, m_intermediate
			, m_farBlur
			, crg::RunnablePass::IsEnabledCallback( [this]() { return isEnabled() && m_data.enableFarBlur; } )
			, &m_passIndex );
		m_lastPass = &createCombinePass( device
			, m_graph
			, std::move( passes )
			, m_ubo
			, crg::ImageViewIdArray{ source.sampledViewId, target.sampledViewId }
			, m_nearBlur
			, m_farBlur
			, crg::ImageViewIdArray{ target.targetViewId, source.targetViewId }
			, &isEnabled()
			, &m_passIndex );
		return true;
	}

	void PostEffect::doCleanup( c3d::RenderDevice const & device )
	{
		m_nearCoC.destroy();
		m_farCoC.destroy();
		m_nearBlur.destroy();
		m_farBlur.destroy();
		m_intermediate.destroy();
	}

	void PostEffect::doCpuUpdate( c3d::CpuUpdater & updater )
	{
		if ( m_renderTarget.getCamera() )
		{
			m_data.pixelStepHalf->x = 1.0f / float( m_nearCoC.getExtent().width );
			m_data.pixelStepHalf->y = 1.0f / float( m_nearCoC.getExtent().height );
			m_data.pixelStepFull->x = m_data.pixelStepHalf->x / 2.0f;
			m_data.pixelStepFull->y = m_data.pixelStepHalf->y / 2.0f;
			m_ubo.cpuUpdate( m_data );
		}
	}

	bool PostEffect::doWriteInto( c3d::StringStream & file, c3d::String const & tabs )
	{
		return m_data.write( file, tabs );
	}
}
