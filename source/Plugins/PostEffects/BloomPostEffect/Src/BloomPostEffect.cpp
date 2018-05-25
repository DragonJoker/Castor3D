#include "BloomPostEffect.hpp"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Mesh/Vertex.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderPassTimer.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>

#include <Buffer/VertexBuffer.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/SubpassDependency.hpp>
#include <RenderPass/SubpassDescription.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <Graphics/Image.hpp>

#include <numeric>

#include <GlslSource.hpp>

using namespace castor;

namespace Bloom
{
	namespace
	{
		static uint32_t constexpr BaseFilterCount = 4u;
		static uint32_t constexpr BaseKernelSize = 5u;

		glsl::Shader getVertexProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			Vec2 position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = writer.paren( position + 1.0 ) / 2.0;
				out.gl_Position() = writer.rendererScalePosition( vec4( position, 0.0, 1.0 ) );
			} );
			return writer.finalise();
		}

		glsl::Shader getHiPassProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 0u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_fragColor = vec4( texture( c3d_mapDiffuse, writer.adjustTexCoords( vtx_texture ), 0.0f ).xyz(), 1.0 );
				auto maxComponent = writer.declLocale( cuT( "maxComponent" ), glsl::max( pxl_fragColor.r(), pxl_fragColor.g() ) );
				maxComponent = glsl::max( maxComponent, pxl_fragColor.b() );

				IF( writer, maxComponent > 1.0_f )
				{
					pxl_fragColor.xyz() /= maxComponent;
				}
				ELSE
				{
					pxl_fragColor.xyz() = vec3( 0.0_f, 0.0_f, 0.0_f );
				}
				FI;
			} );
			return writer.finalise();
		}
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
			, renderer::ShaderStageFlag::eVertex
			, m_hiPass->getVertexShader() );
		visitor.visit( cuT( "HiPass" )
			, renderer::ShaderStageFlag::eFragment
			, m_hiPass->getPixelShader() );

		visitor.visit( cuT( "BlurX" )
			, renderer::ShaderStageFlag::eVertex
			, m_blurXPass->getVertexShader() );
		visitor.visit( cuT( "BlurX" )
			, renderer::ShaderStageFlag::eFragment
			, m_blurXPass->getPixelShader() );

		visitor.visit( cuT( "BlurY" )
			, renderer::ShaderStageFlag::eVertex
			, m_blurYPass->getVertexShader() );
		visitor.visit( cuT( "BlurY" )
			, renderer::ShaderStageFlag::eFragment
			, m_blurYPass->getPixelShader() );

		visitor.visit( cuT( "Combine" )
			, renderer::ShaderStageFlag::eVertex
			, m_combinePass->getVertexShader() );
		visitor.visit( cuT( "Combine" )
			, renderer::ShaderStageFlag::eFragment
			, m_combinePass->getPixelShader() );

		visitor.visit( cuT( "Kernel Size" )
			, m_blurKernelSize );
	}

	bool PostEffect::doInitialise( castor3d::RenderPassTimer const & timer )
	{
		auto & device = *getRenderSystem()->getCurrentDevice();
		renderer::Extent2D size{ m_target->getWidth(), m_target->getHeight() };

		// Create vertex buffer
		m_vertexBuffer = renderer::makeVertexBuffer< castor3d::NonTexturedQuad >( device
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );

		if ( auto data = m_vertexBuffer->lock( 0u, 1u, renderer::MemoryMapFlag::eWrite ) )
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
		renderer::ImageCreateInfo image{};
		image.flags = 0u;
		image.arrayLayers = 1u;
		image.extent.width = size.width >> 1;
		image.extent.height = size.height >> 1;
		image.extent.depth = 1u;
		image.format = format;
		image.imageType = renderer::TextureType::e2D;
		image.initialLayout = renderer::ImageLayout::eUndefined;
		image.mipLevels = m_blurPassesCount;
		image.samples = renderer::SampleCountFlag::e1;
		image.sharingMode = renderer::SharingMode::eExclusive;
		image.tiling = renderer::ImageTiling::eOptimal;
		image.usage = renderer::ImageUsageFlag::eColourAttachment
			| renderer::ImageUsageFlag::eSampled;
		m_blurTexture = std::make_shared< castor3d::TextureLayout >( *getRenderSystem()
			, image
			, renderer::MemoryPropertyFlag::eDeviceLocal );
		m_blurTexture->initialise();

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
		m_commands.emplace_back( std::move( m_hiPass->getCommands( timer, *m_vertexBuffer ) ) );

#if !Bloom_DebugHiPass
		for ( auto & command : m_blurXPass->getCommands( *m_vertexBuffer ) )
		{
			m_commands.emplace_back( std::move( command ) );
		}

		for ( auto & command : m_blurYPass->getCommands( *m_vertexBuffer ) )
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

	bool PostEffect::doWriteInto( TextFile & file )
	{
		return file.writeText( cuT( " -Size=" ) + string::toString( m_blurKernelSize ) ) > 0;
	}
}
