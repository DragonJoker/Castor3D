#include "BloomPostEffect/BloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

namespace Bloom
{
	namespace postfx
	{
		static uint32_t constexpr BaseFilterCount = 4u;
		static uint32_t constexpr BaseKernelSize = 5u;
	}

	//*********************************************************************************************

	c3d::String const PostEffect::Type = cuT( "bloom" );
	c3d::MbString const PostEffect::Name = "HDR Bloom PostEffect";

	PostEffect::PostEffect( c3d::RenderTarget & renderTarget
		, c3d::RenderSystem & renderSystem
		, c3d::Parameters const & params )
		: c3d::PostEffect{ PostEffect::Type
			, cuT( "Bloom" )
			, c3d::makeString( PostEffect::Name )
			, renderTarget
			, renderSystem
			, params }
		, m_blurKernelSize{ postfx::BaseKernelSize }
		, m_blurPassesCount{ postfx::BaseFilterCount }
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
		if ( m_hiPass )
		{
			m_hiPass->accept( visitor );
		}

#if !Bloom_DebugHiPass
		if ( m_blurXPass )
		{
			m_blurXPass->accept( visitor );
		}

		if ( m_blurYPass )
		{
			m_blurYPass->accept( visitor );
		}

		if ( m_combinePass )
		{
			m_combinePass->accept( visitor );
		}

		for ( auto & view : m_blurViews )
		{
			visitor.visit( cuT( "PostFX: HDRB - Blur " ) + c3d::string::toString( view.data->info.subresourceRange.baseMipLevel )
				, view
				, m_renderTarget.getGraph().getFinalLayoutState( view ).layout
				, c3d::TextureFactors{}.invert( true ) );
		}

		visitor.visit( cuT( "Kernel Size" )
			, m_blurKernelSize );
#endif
	}

	void PostEffect::setParameters( c3d::Parameters parameters )
	{
		c3d::String count;

		if ( parameters.get( cuT( "Size" ), count ) )
		{
			m_blurKernelSize = uint32_t( c3d::string::toLong( count ) );
		}

		if ( parameters.get( cuT( "Passes" ), count ) )
		{
			m_blurPassesCount = uint32_t( c3d::string::toLong( count ) );
		}

		m_passesCount = m_blurPassesCount * 2u + 2u;
	}

	bool PostEffect::doInitialise( c3d::RenderDevice const & device
		, c3d::Texture const & source
		, c3d::Texture const & target
		, crg::FramePass const & previousPass )
	{
		c3d::Extent2D size{ c3d::makeExtent2D( target.getExtent() ) };

#if !Bloom_DebugHiPass
		m_blurImg = m_graph.createImage( crg::ImageData{ "Blur"
			, c3d::ImageCreateFlags::eNone
			, c3d::ImageType::e2D
			, target.getFormat()
			, c3d::Extent3D{ size.width >> 1, size.height >> 1, 1u }
			, ( c3d::ImageUsageFlags::eColorAttachment
				| c3d::ImageUsageFlags::eSampled
				| c3d::ImageUsageFlags::eTransferSrc )
			, m_blurPassesCount } );

		for ( uint32_t i = 0u; i < m_blurPassesCount; ++i )
		{
			m_blurViews.push_back( m_graph.createView( crg::ImageViewData{ m_blurImg.data->name + c3d::string::toMbString( i )
				, m_blurImg
				, c3d::ImageViewCreateFlags::eNone
				, c3d::ImageViewType::e2D
				, getFormat( m_blurImg )
				, { c3d::ImageAspectFlags::eColor, i, 1u, 0u, 1u } } ) );
		}
#endif

		m_hiPass = c3d::makeRawUnique< HiPass >( m_graph
			, previousPass
			, device
			, crg::ImageViewIdArray{ source.sampledViewId, target.sampledViewId }
			, size
			, m_blurPassesCount
			, &isEnabled()
			, &m_passIndex );
#if !Bloom_DebugHiPass
		m_blurXPass = c3d::makeRawUnique< BlurPass >( m_graph
			, m_hiPass->getPass()
			, device
			, m_hiPass->getResult()
			, m_blurViews
			, size
			, m_blurKernelSize
			, m_blurPassesCount
			, false
			, &isEnabled() );
		m_blurYPass = c3d::makeRawUnique< BlurPass >( m_graph
			, m_blurXPass->getPasses()
			, device
			, m_blurViews
			, m_hiPass->getResult()
			, size
			, m_blurKernelSize
			, m_blurPassesCount
			, true
			, &isEnabled() );
		m_combinePass = c3d::makeRawUnique< CombinePass >( m_graph
			, m_blurYPass->getPasses()
			, device
			, crg::ImageViewIdArray{ source.sampledViewId, target.sampledViewId }
			, m_hiPass->getResult()
			, crg::ImageViewIdArray{ target.targetViewId, source.targetViewId }
			, size
			, m_blurPassesCount
			, &isEnabled()
			, & m_passIndex );
#endif

#if Bloom_DebugHiPass
		m_pass = &m_hiPass->getPass();
		return &m_hiPass->getResult();
#else
		m_pass = &m_combinePass->getPass();
		return true;
#endif
	}

	void PostEffect::doCleanup( c3d::RenderDevice const & device )
	{
		m_combinePass.reset();
		m_blurXPass.reset();
		m_blurYPass.reset();
		m_hiPass.reset();
	}

	void PostEffect::doCpuUpdate( c3d::CpuUpdater & updater )
	{
		if ( m_blurXPass )
		{
			m_blurXPass->update( m_blurKernelSize );
		}

		if ( m_blurYPass )
		{
			m_blurYPass->update( m_blurKernelSize );
		}
	}

	bool PostEffect::doWriteInto( c3d::StringStream & file, c3d::String const & tabs )
	{
		file << ( tabs + cuT( "postfx \"" ) + Type + cuT( "\" -Size=" ) + c3d::string::toString( m_blurKernelSize )
			+ cuT( " -Passes=" ) + c3d::string::toString( m_blurPassesCount )
			+ cuT( "\n" ) );
		return true;
	}
}
