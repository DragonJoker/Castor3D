#include "BloomPostEffect/BloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Cache/SamplerCache.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

using namespace castor;

namespace Bloom
{
	namespace
	{
		static uint32_t constexpr BaseFilterCount = 4u;
		static uint32_t constexpr BaseKernelSize = 5u;
	}

	//*********************************************************************************************

	String const PostEffect::Type = cuT( "bloom" );
	String const PostEffect::Name = cuT( "Bloom PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
		: castor3d::PostEffect{ PostEffect::Type
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, params
			, BaseFilterCount + BaseFilterCount + 1u + 1u }
		, m_blurKernelSize{ BaseKernelSize }
		, m_blurPassesCount{ BaseFilterCount }
	{
		String count;

		if ( params.get( cuT( "Size" ), count ) )
		{
			m_blurKernelSize = uint32_t( string::toLong( count ) );
		}

		if ( params.get( cuT( "Passes" ), count ) )
		{
			m_blurPassesCount = uint32_t( string::toLong( count ) );
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
		m_hiPass->accept( visitor );
#if !Bloom_DebugHiPass
		m_blurXPass->accept( visitor );
		m_blurYPass->accept( visitor );
		m_combinePass->accept( visitor );

		visitor.visit( cuT( "Kernel Size" )
			, m_blurKernelSize );
#endif
	}

	bool PostEffect::doInitialise( castor3d::RenderDevice const & device
		, castor3d::RenderPassTimer const & timer )
	{
		VkExtent2D size{ m_target->getWidth(), m_target->getHeight() };

#if !Bloom_DebugHiPass
		// Create vertex buffer
		m_vertexBuffer = castor3d::makeVertexBuffer< castor3d::NonTexturedQuad >( device
			, 1u
			, 0u
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "PostEffect" );

		if ( auto data = m_vertexBuffer->lock( 0u, 1u, 0u ) )
		{
			castor3d::NonTexturedQuad buffer
			{
				{ 
					Point2f{ -1.0, -1.0 },
					Point2f{ -1.0, +1.0 },
					Point2f{ +1.0, -1.0 },
					Point2f{ +1.0, -1.0 },
					Point2f{ -1.0, +1.0 },
					Point2f{ +1.0, +1.0 }
				},
			};
			*data = buffer;
			m_vertexBuffer->flush( 0u, 1u );
			m_vertexBuffer->unlock();
		}

		auto format = m_target->getPixelFormat();
		ashes::ImageCreateInfo image
		{
			0u,
			VK_IMAGE_TYPE_2D,
			format,
			{ size.width >> 1, size.height >> 1, 1u },
			m_blurPassesCount,
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT ),
		};
		m_blurTexture = std::make_shared< castor3d::TextureLayout >( *getRenderSystem()
			, image
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "BloomBlur" ) );
		m_blurTexture->initialise( device );
#endif

		m_hiPass = std::make_unique< HiPass >( device
			, m_target->getPixelFormat()
			, m_target->getDefaultView().getSampledView()
			, size
			, m_blurPassesCount );
#if !Bloom_DebugHiPass
		m_blurXPass = std::make_unique< BlurPass >( device
			, m_target->getPixelFormat()
			, m_hiPass->getResult()
			, *m_blurTexture
			, size
			, m_blurKernelSize
			, m_blurPassesCount
			, false );
		m_blurYPass = std::make_unique< BlurPass >( device
			, m_target->getPixelFormat()
			, *m_blurTexture
			, m_hiPass->getResult()
			, size
			, m_blurKernelSize
			, m_blurPassesCount
			, true );
		m_combinePass = std::make_unique< CombinePass >( device
			, m_target->getPixelFormat()
			, m_target->getDefaultView().getSampledView()
			, m_hiPass->getResult().getDefaultView().getSampledView()
			, size
			, m_blurPassesCount );
#endif
		uint32_t index = 0u;
		m_commands.emplace_back( std::move( m_hiPass->getCommands( timer, index ) ) );

#if !Bloom_DebugHiPass
		for ( auto & command : m_blurXPass->getCommands( timer, index, *m_vertexBuffer ) )
		{
			m_commands.emplace_back( std::move( command ) );
		}

		for ( auto & command : m_blurYPass->getCommands( timer, index, *m_vertexBuffer ) )
		{
			m_commands.emplace_back( std::move( command ) );
		}

		m_commands.emplace_back( std::move( m_combinePass->getCommands( timer, index, *m_vertexBuffer ) ) );
#endif

#if Bloom_DebugHiPass
		m_result = &m_hiPass->getResult();
#else
		m_result = &m_combinePass->getResult();
#endif
		return true;
	}

	void PostEffect::doCleanup( castor3d::RenderDevice const & device )
	{
		m_combinePass.reset();
		m_blurXPass.reset();
		m_blurYPass.reset();
		m_hiPass.reset();
		m_blurTexture.reset();
		m_vertexBuffer.reset();
	}

	bool PostEffect::doWriteInto( StringStream & file, String const & tabs )
	{
		file << ( tabs + cuT( "postfx \"" ) + Type + cuT( "\" -Size=" ) + string::toString( m_blurKernelSize )
			+ cuT( " -Passes=" ) + string::toString( m_blurPassesCount ) );
		return true;
	}
}
