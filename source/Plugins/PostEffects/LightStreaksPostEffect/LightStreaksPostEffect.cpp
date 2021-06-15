#include "LightStreaksPostEffect/LightStreaksPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Cache/SamplerCache.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

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
	namespace
	{
		std::unique_ptr< ast::Shader > getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			castor3d::shader::Utils utils{ writer };
			utils.declareNegateVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = ( position + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( position.xy(), 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

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
		, m_kawaseUbo{ *renderSystem.getMainRenderDevice() }
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
	}

	crg::ImageViewId const * PostEffect::doInitialise( castor3d::RenderDevice const & device
		, castor3d::RenderPassTimer const & timer
		, crg::FramePass const & previousPass )
	{
		VkExtent2D dimensions{ m_target->data->image.data->info.extent.width
			, m_target->data->image.data->info.extent.height };
		auto & graph = m_renderTarget.getGraph();

		auto size = dimensions;
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
			, m_hiPass->getPasses()
			, device
			, m_hiImage.subViewsId
			, m_kawaseImage.subViewsId
			, m_kawaseUbo
			, size
			, &isEnabled() );
		m_combinePass = std::make_unique< CombinePass >( graph
			, m_kawasePass->getPasses()
			, device
			, *m_target
			, m_kawaseImage.subViewsId
			, dimensions
			, &isEnabled() );
		m_pass = &m_combinePass->getPass();
		return &m_combinePass->getResult();
	}

	void PostEffect::doCleanup( castor3d::RenderDevice const & device )
	{
		m_combinePass.reset();
		m_kawasePass.reset();
		m_hiPass.reset();
	}

	bool PostEffect::doWriteInto( castor::StringStream & file, castor::String const & tabs )
	{
		file << ( tabs + cuT( "postfx \"" ) + Type + cuT( "\"" ) );
		return true;
	}
}
