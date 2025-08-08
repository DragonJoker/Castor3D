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
		PostEffect::setParameters( params );
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

		for ( auto & layerViews : m_blurXImg )
		{
			for ( auto & mipViews : layerViews.mipViews )
				visitor.visit( cuT( "PostFX: HDRB - BlurX " ) + c3d::string::toString( getSubresourceRange( mipViews.sampledViewId ).baseMipLevel )
					, mipViews.sampledViewId
					, m_renderTarget.getGraph().getFinalLayoutState( mipViews.sampledViewId ).layout
					, c3d::TextureFactors{}.invert( true ) );
		}

		for ( auto & layerViews : m_blurYImg )
		{
			for ( auto & mipViews : layerViews.mipViews )
				visitor.visit( cuT( "PostFX: HDRB - BlurY " ) + c3d::string::toString( getSubresourceRange( mipViews.sampledViewId ).baseMipLevel )
					, mipViews.sampledViewId
					, m_renderTarget.getGraph().getFinalLayoutState( mipViews.sampledViewId ).layout
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
		, c3d::Texture & target )
	{
		c3d::Extent2D size{ c3d::makeExtent2D( target.getExtent() ) };

#if !Bloom_DebugHiPass
		m_blurXImg = { device
			, m_renderTarget.getResources()
			, cuT( "BloomXBlurred" )
			, { c3d::ImageCreateFlags::eNone
				, c3d::Extent3D{ size.width >> 1, size.height >> 1, 1u }, 1u, m_blurPassesCount
				, target.getFormat()
				, ( c3d::ImageUsageFlags::eColorAttachment
					| c3d::ImageUsageFlags::eSampled
					| c3d::ImageUsageFlags::eTransferSrc ) }
			, {} };
		m_blurYImg = { device
			, m_renderTarget.getResources()
			, cuT( "BloomYBlurred" )
			, { c3d::ImageCreateFlags::eNone
				, c3d::Extent3D{ size.width >> 1, size.height >> 1, 1u }, 1u, m_blurPassesCount
				, target.getFormat()
				, ( c3d::ImageUsageFlags::eColorAttachment
					| c3d::ImageUsageFlags::eSampled
					| c3d::ImageUsageFlags::eTransferSrc ) }
			, {} };
#endif

		m_hiPass = c3d::makeRawUnique< HiPass >( m_graph
			, device
			, source
			, size
			, m_blurPassesCount
			, &isEnabled()
			, &m_passIndex );
#if !Bloom_DebugHiPass
		m_blurXPass = c3d::makeRawUnique< BlurPass >( m_graph
			, device
			, m_hiPass->getResult()
			, m_blurXImg
			, size
			, m_blurKernelSize
			, m_blurPassesCount
			, false
			, &isEnabled() );
		m_blurYPass = c3d::makeRawUnique< BlurPass >( m_graph
			, device
			, m_blurXPass->getResult()
			, m_blurYImg
			, size
			, m_blurKernelSize
			, m_blurPassesCount
			, true
			, &isEnabled() );
		m_combinePass = c3d::makeRawUnique< CombinePass >( m_graph
			, device
			, source
			, m_blurYPass->getResult()
			, target
			, size
			, m_blurPassesCount
			, &isEnabled()
			, & m_passIndex );
#endif

		return true;
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
