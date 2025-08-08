#include "LightStreaksPostEffect/LightStreaksPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace light_streaks
{
	c3d::String const PostEffect::Type = cuT( "light_streaks" );
	c3d::MbString const PostEffect::Name = "LightStreaks PostEffect";

	PostEffect::PostEffect( c3d::RenderTarget & renderTarget
		, c3d::RenderSystem & renderSystem
		, c3d::Parameters const & params )
		: c3d::PostEffect{ PostEffect::Type
			, cuT( "LightStreaks" )
			, c3d::makeString( PostEffect::Name )
			, renderTarget
			, renderSystem
			, params
			, Count + 2u }
		, m_kawaseUbo{ renderSystem.getRenderDevice() }
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
		m_hiPass->accept( visitor );
		m_kawasePass->accept( visitor );
		m_combinePass->accept( visitor );

		for ( auto & layerViews : m_hiImage )
		{
			visitor.visit( cuT( "PostFX: LS - Hi " ) + c3d::string::toString( getSubresourceRange( layerViews.sampledViewId ).baseArrayLayer )
				, layerViews.sampledViewId
				, m_renderTarget.getGraph().getFinalLayoutState( layerViews.sampledViewId ).layout
				, c3d::TextureFactors{}.invert( true ) );
		}

		for ( auto & layerViews : m_kawaseImage )
		{
			visitor.visit( cuT( "PostFX: LS - Kawase " ) + c3d::string::toString( getSubresourceRange( layerViews.sampledViewId ).baseArrayLayer )
				, layerViews.sampledViewId
				, m_renderTarget.getGraph().getFinalLayoutState( layerViews.sampledViewId ).layout
				, c3d::TextureFactors{}.invert( true ) );
		}

		visitor.visit( cuT( "Attenuation" )
			, m_kawaseCfg.attenuation );
		visitor.visit( cuT( "Samples" )
			, m_kawaseCfg.samples );
	}

	void PostEffect::setParameters( c3d::Parameters parameters )
	{
	}

	bool PostEffect::doInitialise( c3d::RenderDevice const & device
		, c3d::Texture const & source
		, c3d::Texture & target )
	{
		auto extent = c3d::getSafeBandedExtent3D( m_renderTarget.getDisplaySize() );

		auto size = c3d::makeExtent2D( extent );
		size.width >>= 2;
		size.height >>= 2;
		uint32_t index = 0u;
		static float constexpr factor = 0.2f;
		static c3d::Array< c3d::Point2f, Count > directions
		{
			{
				c3d::Point2f{ factor, factor },
				c3d::Point2f{ -factor, -factor },
				c3d::Point2f{ -factor, factor },
				c3d::Point2f{ factor, -factor }
			}
		};

		m_hiImage = { device
			, m_renderTarget.getResources()
			, cuT( "LSHi" )
			, { c3d::ImageCreateFlags::eNone
				, c3d::Extent3D{ size.width, size.height, 1u }, Count + 1u, 1u
				, target.getFormat()
				, ( c3d::ImageUsageFlags::eColorAttachment
					| c3d::ImageUsageFlags::eSampled
					| c3d::ImageUsageFlags::eTransferDst
					| c3d::ImageUsageFlags::eTransferSrc ) }
			, {} };
		m_kawaseImage = { device
			, m_renderTarget.getResources()
			, cuT( "LSKaw" )
			, { c3d::ImageCreateFlags::eNone
				, c3d::Extent3D{ size.width, size.height, 1u }, Count, 1u
				, target.getFormat()
				, ( c3d::ImageUsageFlags::eColorAttachment
					| c3d::ImageUsageFlags::eSampled
					| c3d::ImageUsageFlags::eTransferDst
					| c3d::ImageUsageFlags::eTransferSrc ) }
			, {} };

		for ( auto i = 0u; i < Count; ++i )
		{
			for ( uint32_t j = 0u; j < 3u; ++j )
			{
				m_kawaseUbo.update( index
					, size
					, directions[i]
					, j );
				++index;
			}
		}

		m_hiPass = c3d::makeRawUnique< HiPass >( m_graph
			, device
			, source
			, m_hiImage
			, size
			, &isEnabled()
			, &m_passIndex );
		m_kawasePass = c3d::makeRawUnique< KawasePass >( m_graph
			, device
			, m_hiImage
			, m_kawaseImage
			, m_kawaseUbo
			, size
			, &isEnabled() );
		m_combinePass = c3d::makeRawUnique< CombinePass >( m_graph
			, device
			, m_kawaseImage
			, source
			, target
			, c3d::makeExtent2D( extent )
			, &isEnabled()
			, &m_passIndex );
		m_hiImage.create();
		m_kawaseImage.create();
		return true;
	}

	void PostEffect::doCleanup( c3d::RenderDevice const & device )
	{
		m_combinePass.reset();
		m_kawasePass.reset();
		m_hiPass.reset();
		m_hiImage.destroy();
		m_kawaseImage.destroy();
	}

	void PostEffect::doCpuUpdate( c3d::CpuUpdater & updater )
	{
		m_kawaseUbo.update( m_kawaseCfg );
	}

	bool PostEffect::doWriteInto( c3d::StringStream & file, c3d::String const & tabs )
	{
		file << ( tabs + cuT( "postfx \"" ) + Type + cuT( "\"\n" ) );
		return true;
	}
}
