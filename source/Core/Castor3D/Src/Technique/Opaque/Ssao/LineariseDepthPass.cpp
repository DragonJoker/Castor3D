#include "LineariseDepthPass.hpp"

#include "Engine.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/Context.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "State/BlendState.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Technique/Opaque/Ssao/SsaoConfigUbo.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

#include <random>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		glsl::Shader doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			auto position = writer.declAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = writer.declAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					gl_Position = c3d_projection * vec4( position, 0.0, 1.0 );
				} );
			return writer.finalise();
		}
		
		glsl::Shader doGetLinearisePixelProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( cuT( "c3d_mapDepth" ), MinTextureIndex );

			auto c3d_clipInfo = writer.declUniform< Vec3 >( cuT( "c3d_clipInfo" ) );
			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Float >( cuT( "pxl_fragColor" ) );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto depth = writer.declLocale( cuT( "depth" )
						, texelFetch( c3d_mapDepth, ivec2( gl_FragCoord.xy() ), 0 ).r() );
					pxl_fragColor = c3d_clipInfo[0] / writer.paren( c3d_clipInfo[1] * depth + c3d_clipInfo[2] );
				} );
			return writer.finalise();
		}
		
		glsl::Shader doGetMinifyPixelProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			auto index = MinTextureIndex;
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( cuT( "c3d_mapDepth" ), MinTextureIndex );

			auto c3d_previousLevel = writer.declUniform< Int >( cuT( "c3d_previousLevel" ) );
			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Float >( cuT( "pxl_fragColor" ) );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto ssPosition = writer.declLocale( cuT( "ssP" )
						, ivec2( gl_FragCoord.xy() ) );

					// Rotated grid subsampling to avoid XY directional bias or Z precision bias while downsampling.
					// On DX9, the bit-and can be implemented with floating-point modulo
					pxl_fragColor = texelFetch( c3d_mapDepth
						, clamp( ssPosition * 2 + ivec2( ssPosition.y() & 1, ssPosition.x() & 1 )
							, ivec2( 0_i )
							, textureSize( c3d_mapDepth, c3d_previousLevel ) - ivec2( 1_i ) )
						, c3d_previousLevel ).r();
				} );
			return writer.finalise();
		}

		ShaderProgramSPtr doGetLineariseProgram( Engine & engine )
		{
			auto vtx = doGetVertexProgram( engine );
			auto pxl = doGetLinearisePixelProgram( engine );
			ShaderProgramSPtr program = engine.getShaderProgramCache().getNewProgram( false );
			program->createObject( ShaderType::eVertex );
			program->createObject( ShaderType::ePixel );
			program->setSource( ShaderType::eVertex, vtx );
			program->setSource( ShaderType::ePixel, pxl );
			program->initialise();
			return program;
		}

		ShaderProgramSPtr doGetMinifyProgram( Engine & engine )
		{
			auto vtx = doGetVertexProgram( engine );
			auto pxl = doGetMinifyPixelProgram( engine );
			ShaderProgramSPtr program = engine.getShaderProgramCache().getNewProgram( false );
			program->createObject( ShaderType::eVertex );
			program->createObject( ShaderType::ePixel );
			program->setSource( ShaderType::eVertex, vtx );
			program->setSource( ShaderType::ePixel, pxl );
			program->initialise();
			return program;
		}

		RenderPipelineUPtr doCreatePipeline( Engine & engine
			, ShaderProgram & program )
		{
			DepthStencilState dsstate;
			dsstate.setDepthTest( false );
			dsstate.setDepthMask( WritingMask::eZero );
			RasteriserState rsstate;
			rsstate.setCulledFaces( Culling::eNone );
			return engine.getRenderSystem()->createRenderPipeline( std::move( dsstate )
				, std::move( rsstate )
				, BlendState{}
				, MultisampleState{}
				, program
				, PipelineFlags{} );
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name
			, WrapMode mode )
		{
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
				sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
				sampler->setInterpolationMode( InterpolationFilter::eMip, InterpolationMode::eNearest );
				sampler->setWrappingMode( TextureUVW::eU, mode );
				sampler->setWrappingMode( TextureUVW::eV, mode );
			}

			return sampler;
		}
	}

	//*********************************************************************************************

	LineariseDepthPass::LineariseDepthPass( Engine & engine
		, Size const & size
		, MatrixUbo & matrixUbo
		, SsaoConfigUbo & ssaoConfigUbo )
		: m_engine{ engine }
		, m_size{ size }
		, m_matrixUbo{ matrixUbo }
		, m_ssaoConfigUbo{ ssaoConfigUbo }
		, m_result{ engine }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "SSAO" ), cuT( "Linearise depth" ) ) }
	{
		auto & renderSystem = *m_engine.getRenderSystem();
		auto sampler = doCreateSampler( m_engine, cuT( "LinearisePass_Result" ), WrapMode::eClampToEdge );
		auto ssaoResult = renderSystem.createTexture( TextureType::eTwoDimensions
			, AccessType::eNone
			, AccessType::eRead | AccessType::eWrite
			, MaxMipLevel + 1u );
		ssaoResult->setSource( PxBufferBase::create( m_size
			, PixelFormat::eL32F ) );
		m_result.setTexture( ssaoResult );
		m_result.setSampler( sampler );
		m_result.setIndex( MinTextureIndex );
		m_result.initialise();

		for ( uint32_t index = 0u; index <= MaxMipLevel; ++index )
		{
			auto & fbo = m_fbos[index];
			fbo = renderSystem.createFrameBuffer();
			fbo->initialise();

			auto & resultAttach = m_resultAttaches[index];
			resultAttach = fbo->createAttachment( ssaoResult, index );
			fbo->bind();
			fbo->attach( AttachmentPoint::eColour, 0u, resultAttach, ssaoResult->getType() );
			fbo->setDrawBuffer( resultAttach );
			ENSURE( fbo->isComplete() );
			fbo->unbind();
		}

		doInitialiseLinearisePass();
		doInitialiseMinifyPass();
	}

	LineariseDepthPass::~LineariseDepthPass()
	{
		doCleanupMinifyPass();
		doCleanupLinearisePass();

		for ( uint32_t index = 0u; index <= MaxMipLevel; ++index )
		{
			m_fbos[index]->cleanup();
			m_fbos[index].reset();
			m_resultAttaches[index].reset();
		}

		m_result.cleanup();
	}

	void LineariseDepthPass::linearise( TextureUnit const & depthBuffer
		, Viewport const & viewport )
	{
		m_timer->start();
		auto z_f = viewport.getFar();
		auto z_n = viewport.getNear();
		auto clipInfo = std::isinf( z_f )
			? Point3f{ z_n, -1.0f, 1.0f }
			: Point3f{ z_n * z_f, z_n - z_f, z_f };
		m_clipInfo->setValue( clipInfo );

		m_fbos[0]->bind( FrameBufferTarget::eDraw );
		m_fbos[0]->clear( BufferComponent::eColour );
		depthBuffer.getTexture()->bind( MinTextureIndex );
		depthBuffer.getSampler()->bind( MinTextureIndex );
		m_linearisePipeline->apply();
		m_engine.getRenderSystem()->getCurrentContext()->renderTexture( m_size
			, *depthBuffer.getTexture()
			, *m_linearisePipeline
			, m_matrixUbo );
		m_fbos[0]->unbind();

		for ( auto index = 1u; index <= MaxMipLevel; ++index )
		{
			m_previousLevel->setValue( index - 1 );
			m_fbos[index]->bind( FrameBufferTarget::eDraw );
			m_fbos[index]->clear( BufferComponent::eColour );
			m_minifyPipeline->apply();
			m_engine.getRenderSystem()->getCurrentContext()->renderTexture( m_size
				, *m_result.getTexture()
				, *m_minifyPipeline
				, m_matrixUbo );
			m_fbos[index]->unbind();
		}

		m_timer->stop();
	}

	void LineariseDepthPass::doInitialiseLinearisePass()
	{
		m_lineariseProgram = doGetLineariseProgram( m_engine );
		m_clipInfo = m_lineariseProgram->findUniform< UniformType::eVec3f >( cuT( "c3d_clipInfo" ), ShaderType::ePixel );
		m_linearisePipeline = doCreatePipeline( m_engine, *m_lineariseProgram );
		m_linearisePipeline->addUniformBuffer( m_matrixUbo.getUbo() );
	}

	void LineariseDepthPass::doInitialiseMinifyPass()
	{
		m_minifyProgram = doGetMinifyProgram( m_engine );
		m_previousLevel = m_minifyProgram->findUniform< UniformType::eInt >( cuT( "c3d_previousLevel" ), ShaderType::ePixel );
		m_minifyPipeline = doCreatePipeline( m_engine, *m_minifyProgram );
		m_minifyPipeline->addUniformBuffer( m_matrixUbo.getUbo() );
	}

	void LineariseDepthPass::doCleanupLinearisePass()
	{
		m_linearisePipeline->cleanup();
		m_linearisePipeline.reset();
		m_clipInfo.reset();
		m_lineariseProgram.reset();
	}

	void LineariseDepthPass::doCleanupMinifyPass()
	{
		m_minifyPipeline->cleanup();
		m_minifyPipeline.reset();
		m_previousLevel.reset();
		m_minifyProgram.reset();
	}
}
