#include "LightStreaksPostEffect/LightStreaksPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
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
	castor::String const PostEffect::Type = cuT( "light_streaks" );
	castor::String const PostEffect::Name = cuT( "LightStreaks PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
		: castor3d::PostEffect{ PostEffect::Type
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, params
			, Count + 2u }
		, m_kawaseUbo{ renderSystem.getRenderDevice() }
	{
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
		m_hiPass->accept( visitor );
		m_kawasePass->accept( visitor );
		m_combinePass->accept( visitor );

		for ( auto & view : m_hiImage.subViewsId )
		{
			visitor.visit( "PostFX: LS - Hi " + std::to_string( view.data->info.subresourceRange.baseArrayLayer )
				, view
				, m_renderTarget.getGraph().getFinalLayout( view ).layout
				, castor3d::TextureFactors{}.invert( true ) );
		}

		for ( auto & view : m_kawaseImage.subViewsId )
		{
			visitor.visit( "PostFX: LS - Kawase " + std::to_string( view.data->info.subresourceRange.baseArrayLayer )
				, view
				, m_renderTarget.getGraph().getFinalLayout( view ).layout
				, castor3d::TextureFactors{}.invert( true ) );
		}

		visitor.visit( cuT( "Attenuation" )
			, m_kawaseCfg.attenuation );
		visitor.visit( cuT( "Samples" )
			, m_kawaseCfg.samples );
	}

	crg::ImageViewId const * PostEffect::doInitialise( castor3d::RenderDevice const & device
		, crg::FramePass const & previousPass )
	{
		auto extent = castor3d::getSafeBandedExtent3D( m_renderTarget.getSize() );
		auto & graph = m_renderTarget.getGraph();

		auto size = castor3d::makeExtent2D( extent );
		size.width >>= 2;
		size.height >>= 2;
		uint32_t index = 0u;
		static float constexpr factor = 0.2f;
		static std::array< castor::Point2f, Count > directions
		{
			{
				castor::Point2f{ factor, factor },
				castor::Point2f{ -factor, -factor },
				castor::Point2f{ -factor, factor },
				castor::Point2f{ factor, -factor }
			}
		};

		m_hiImage = { device
			, graph.getHandler()
			, "LSHi"
			, 0u
			, VkExtent3D{ size.width, size.height, 1u }
			, Count + 1u
			, 1u
			, m_target->data->info.format
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) };
		m_kawaseImage = { device
			, graph.getHandler()
			, "LSKaw"
			, 0u
			, VkExtent3D{ size.width, size.height, 1u }
			, Count
			, 1u
			, m_target->data->info.format
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) };

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

		m_hiPass = std::make_unique< HiPass >( graph
			, previousPass
			, device
			, *m_target
			, m_hiImage.subViewsId
			, size
			, &isEnabled() );
		m_kawasePass = std::make_unique< KawasePass >( graph
			, m_hiPass->getLastPass()
			, device
			, m_hiImage.subViewsId
			, m_kawaseImage.subViewsId
			, m_kawaseUbo
			, size
			, &isEnabled() );
		m_combinePass = std::make_unique< CombinePass >( graph
			, m_kawasePass->getLastPass()
			, device
			, *m_target
			, m_kawaseImage.subViewsId
			, castor3d::makeExtent2D( extent )
			, &isEnabled() );
		m_pass = &m_combinePass->getPass();
		m_hiImage.create();
		m_kawaseImage.create();
		return &m_combinePass->getResult();
	}

	void PostEffect::doCleanup( castor3d::RenderDevice const & device )
	{
		m_combinePass.reset();
		m_kawasePass.reset();
		m_hiPass.reset();
		m_hiImage.destroy();
		m_kawaseImage.destroy();
	}

	void PostEffect::doCpuUpdate( castor3d::CpuUpdater & updater )
	{
		m_kawaseUbo.update( m_kawaseCfg );
	}

	bool PostEffect::doWriteInto( castor::StringStream & file, castor::String const & tabs )
	{
		file << ( tabs + cuT( "postfx \"" ) + Type + cuT( "\"" ) );
		return true;
	}
}
