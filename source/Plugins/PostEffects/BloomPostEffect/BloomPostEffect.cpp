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
	namespace
	{
		static uint32_t constexpr BaseFilterCount = 4u;
		static uint32_t constexpr BaseKernelSize = 5u;
	}

	//*********************************************************************************************

	castor::String const PostEffect::Type = cuT( "bloom" );
	castor::String const PostEffect::Name = cuT( "Bloom PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
		: castor3d::PostEffect{ PostEffect::Type
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, params }
		, m_blurKernelSize{ BaseKernelSize }
		, m_blurPassesCount{ BaseFilterCount }
	{
		castor::String count;

		if ( params.get( cuT( "Size" ), count ) )
		{
			m_blurKernelSize = uint32_t( castor::string::toLong( count ) );
		}

		if ( params.get( cuT( "Passes" ), count ) )
		{
			m_blurPassesCount = uint32_t( castor::string::toLong( count ) );
		}

		m_passesCount = m_blurPassesCount * 2u + 2u;
	}

	castor3d::PostEffectSPtr PostEffect::create( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
	{
		return std::make_shared< PostEffect >( renderTarget
			, renderSystem
			, params );
	}

	void PostEffect::accept( castor3d::PipelineVisitorBase & visitor )
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
			visitor.visit( "PostFX: HDRB - Blur " + std::to_string( view.data->info.subresourceRange.baseMipLevel )
				, view
				, m_renderTarget.getGraph().getFinalLayoutState( view ).layout
				, castor3d::TextureFactors{}.invert( true ) );
		}

		visitor.visit( cuT( "Kernel Size" )
			, m_blurKernelSize );
#endif
	}

	crg::ImageViewId const * PostEffect::doInitialise( castor3d::RenderDevice const & device
		, crg::FramePass const & previousPass )
	{
		VkExtent2D size{ m_target->data->image.data->info.extent.width
			, m_target->data->image.data->info.extent.height };
		auto & graph = m_renderTarget.getGraph().createPassGroup( "Bloom" );

#if !Bloom_DebugHiPass
		m_blurImg = graph.createImage( crg::ImageData{ "Blur"
			, 0u
			, VK_IMAGE_TYPE_2D
			, m_target->data->info.format
			, VkExtent3D{ size.width >> 1, size.height >> 1, 1u }
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
			, m_blurPassesCount } );

		for ( uint32_t i = 0u; i < m_blurPassesCount; ++i )
		{
			m_blurViews.push_back( graph.createView( crg::ImageViewData{ m_blurImg.data->name + castor::string::toString( i )
				, m_blurImg
				, 0u
				, VK_IMAGE_VIEW_TYPE_2D
				, m_blurImg.data->info.format
				, { VK_IMAGE_ASPECT_COLOR_BIT, i, 1u, 0u, 1u } } ) );
		}
#endif

		m_hiPass = std::make_unique< HiPass >( graph
			, previousPass
			, device
			, *m_target
			, size
			, m_blurPassesCount
			, &isEnabled() );
#if !Bloom_DebugHiPass
		m_blurXPass = std::make_unique< BlurPass >( graph
			, m_hiPass->getPass()
			, device
			, m_hiPass->getResult()
			, m_blurViews
			, size
			, m_blurKernelSize
			, m_blurPassesCount
			, false
			, &isEnabled() );
		m_blurYPass = std::make_unique< BlurPass >( graph
			, m_blurXPass->getPasses()
			, device
			, m_blurViews
			, m_hiPass->getResult()
			, size
			, m_blurKernelSize
			, m_blurPassesCount
			, true
			, &isEnabled() );
		m_combinePass = std::make_unique< CombinePass >( graph
			, m_blurYPass->getPasses()
			, device
			, *m_target
			, m_hiPass->getResult()
			, size
			, m_blurPassesCount
			, &isEnabled() );
#endif

#if Bloom_DebugHiPass
		m_pass = &m_hiPass->getPass();
		return &m_hiPass->getResult();
#else
		m_pass = &m_combinePass->getPass();
		return &m_combinePass->getResult();
#endif
	}

	void PostEffect::doCleanup( castor3d::RenderDevice const & device )
	{
		m_combinePass.reset();
		m_blurXPass.reset();
		m_blurYPass.reset();
		m_hiPass.reset();
	}

	void PostEffect::doCpuUpdate( castor3d::CpuUpdater & updater )
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

	bool PostEffect::doWriteInto( castor::StringStream & file, castor::String const & tabs )
	{
		file << ( tabs + cuT( "postfx \"" ) + Type + cuT( "\" -Size=" ) + castor::string::toString( m_blurKernelSize )
			+ cuT( " -Passes=" ) + castor::string::toString( m_blurPassesCount ) );
		return true;
	}
}
