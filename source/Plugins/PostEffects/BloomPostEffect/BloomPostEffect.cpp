#include "BloomPostEffect/BloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/SamplerCache.hpp>
#include <Castor3D/Mesh/Vertex.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Shader/GlslToSpv.hpp>
#include <Castor3D/Texture/Sampler.hpp>
#include <Castor3D/Texture/TextureLayout.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <Ashes/Buffer/VertexBuffer.hpp>
#include <Ashes/Image/Texture.hpp>
#include <Ashes/Image/TextureView.hpp>
#include <Ashes/RenderPass/FrameBufferAttachment.hpp>
#include <Ashes/RenderPass/RenderPass.hpp>
#include <Ashes/RenderPass/RenderPassCreateInfo.hpp>
#include <Ashes/RenderPass/SubpassDependency.hpp>
#include <Ashes/RenderPass/SubpassDescription.hpp>
#include <Ashes/Sync/ImageMemoryBarrier.hpp>

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
			, params }
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
		visitor.visit( cuT( "HiPass" )
			, ashes::ShaderStageFlag::eVertex
			, *m_hiPass->getVertexShader().shader );
		visitor.visit( cuT( "HiPass" )
			, ashes::ShaderStageFlag::eFragment
			, *m_hiPass->getPixelShader().shader );

#if !Bloom_DebugHiPass
		visitor.visit( cuT( "BlurX" )
			, ashes::ShaderStageFlag::eVertex
			, *m_blurXPass->getVertexShader().shader );
		visitor.visit( cuT( "BlurX" )
			, ashes::ShaderStageFlag::eFragment
			, *m_blurXPass->getPixelShader().shader );

		visitor.visit( cuT( "BlurY" )
			, ashes::ShaderStageFlag::eVertex
			, *m_blurYPass->getVertexShader().shader );
		visitor.visit( cuT( "BlurY" )
			, ashes::ShaderStageFlag::eFragment
			, *m_blurYPass->getPixelShader().shader );

		visitor.visit( cuT( "Combine" )
			, ashes::ShaderStageFlag::eVertex
			, *m_combinePass->getVertexShader().shader );
		visitor.visit( cuT( "Combine" )
			, ashes::ShaderStageFlag::eFragment
			, *m_combinePass->getPixelShader().shader );

		visitor.visit( cuT( "Kernel Size" )
			, m_blurKernelSize );
#endif
	}

	bool PostEffect::doInitialise( castor3d::RenderPassTimer const & timer )
	{
		auto & device = getCurrentDevice( *this );
		ashes::Extent2D size{ m_target->getWidth(), m_target->getHeight() };

#if !Bloom_DebugHiPass
		// Create vertex buffer
		m_vertexBuffer = ashes::makeVertexBuffer< castor3d::NonTexturedQuad >( device
			, 1u
			, 0u
			, ashes::MemoryPropertyFlag::eHostVisible );

		if ( auto data = m_vertexBuffer->lock( 0u, 1u, ashes::MemoryMapFlag::eWrite ) )
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
		ashes::ImageCreateInfo image{};
		image.flags = 0u;
		image.arrayLayers = 1u;
		image.extent.width = size.width >> 1;
		image.extent.height = size.height >> 1;
		image.extent.depth = 1u;
		image.format = format;
		image.imageType = ashes::TextureType::e2D;
		image.initialLayout = ashes::ImageLayout::eUndefined;
		image.mipLevels = m_blurPassesCount;
		image.samples = ashes::SampleCountFlag::e1;
		image.sharingMode = ashes::SharingMode::eExclusive;
		image.tiling = ashes::ImageTiling::eOptimal;
		image.usage = ashes::ImageUsageFlag::eColourAttachment
			| ashes::ImageUsageFlag::eSampled;
		m_blurTexture = std::make_shared< castor3d::TextureLayout >( *getRenderSystem()
			, image
			, ashes::MemoryPropertyFlag::eDeviceLocal );
		m_blurTexture->initialise();
#endif

		m_hiPass = std::make_unique< HiPass >( *getRenderSystem()
			, m_target->getPixelFormat()
			, m_target->getDefaultView()
			, size
			, m_blurPassesCount );
#if !Bloom_DebugHiPass
		m_blurXPass = std::make_unique< BlurPass >( *getRenderSystem()
			, m_target->getPixelFormat()
			, m_hiPass->getResult()
			, *m_blurTexture
			, size
			, m_blurKernelSize
			, m_blurPassesCount
			, false );
		m_blurYPass = std::make_unique< BlurPass >( *getRenderSystem()
			, m_target->getPixelFormat()
			, *m_blurTexture
			, m_hiPass->getResult()
			, size
			, m_blurKernelSize
			, m_blurPassesCount
			, true );
		m_combinePass = std::make_unique< CombinePass >( *getRenderSystem()
			, m_target->getPixelFormat()
			, m_target->getDefaultView()
			, m_hiPass->getResult().getDefaultView()
			, size
			, m_blurPassesCount );
#endif
		m_commands.emplace_back( std::move( m_hiPass->getCommands( timer ) ) );

#if !Bloom_DebugHiPass
		for ( auto & command : m_blurXPass->getCommands( timer, *m_vertexBuffer ) )
		{
			m_commands.emplace_back( std::move( command ) );
		}

		for ( auto & command : m_blurYPass->getCommands( timer, *m_vertexBuffer ) )
		{
			m_commands.emplace_back( std::move( command ) );
		}

		m_commands.emplace_back( std::move( m_combinePass->getCommands( timer, *m_vertexBuffer ) ) );
#endif

#if Bloom_DebugHiPass
		m_result = &m_hiPass->getResult();
#else
		m_result = &m_combinePass->getResult();
#endif
		return true;
	}

	void PostEffect::doCleanup()
	{
		m_combinePass.reset();
		m_blurXPass.reset();
		m_blurYPass.reset();
		m_hiPass.reset();
		m_blurTexture.reset();
		m_vertexBuffer.reset();
	}

	bool PostEffect::doWriteInto( TextFile & file, String const & tabs )
	{
		return file.writeText( tabs + cuT( "postfx \"" ) + Type + cuT( "\" -Size=" ) + string::toString( m_blurKernelSize, std::locale{ "C" } )
			+ cuT( " -Passes=" ) + string::toString( m_blurPassesCount, std::locale{ "C" } ) ) > 0;
	}
}
