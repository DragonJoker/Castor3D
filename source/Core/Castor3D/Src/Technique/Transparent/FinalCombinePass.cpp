#include "FinalCombinePass.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Fog.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Buffer/GeometryBuffers.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <Pipeline/ColourBlendState.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/MultisampleState.hpp>
#include <Pipeline/RasterisationState.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslFog.hpp"
#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		SamplerSPtr doCreateSampler( Engine & engine )
		{
			String const name = cuT( "FinalCombinePass" );
			SamplerSPtr result;

			if ( engine.getSamplerCache().has( name ) )
			{
				result = engine.getSamplerCache().find( name );
			}
			else
			{
				result = engine.getSamplerCache().add( name );
			}

			return result;
		}

		renderer::VertexLayoutPtr doCreateVertexLayout( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = *renderSystem.getCurrentDevice();
			renderer::VertexLayoutPtr result = renderer::makeLayout< TexturedQuad >( device, 0u );
			createVertexAttribute( result, TexturedQuad::Vertex, position, 0u );
			createVertexAttribute( result, TexturedQuad::Vertex, texture, 1u );
			return result;
		}

		renderer::VertexBufferPtr< TexturedQuad > doCreateVbo( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = *renderSystem.getCurrentDevice();
			auto result = renderer::makeVertexBuffer< TexturedQuad >( device
				, 1u
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );

			if ( auto buffer = result->lock( 0u
				, 1u
				, renderer::MemoryMapFlag::eWrite | renderer::MemoryMapFlag::eInvalidateRange ) )
			{
				*buffer = TexturedQuad
				{
					{
						{ Point2f{ -1.0, -1.0 }, Point2f{ 0.0, 0.0 } },
						{ Point2f{ -1.0, +1.0 }, Point2f{ 0.0, 1.0 } },
						{ Point2f{ +1.0, -1.0 }, Point2f{ 1.0, 0.0 } },
						{ Point2f{ +1.0, +1.0 }, Point2f{ 1.0, 1.0 } }
					}
				};
				result->unlock( 1u, true );
			}

			return result;
		}

		renderer::DescriptorSetLayoutPtr doCreateUboDescriptorLayout( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = *renderSystem.getCurrentDevice();
			renderer::DescriptorSetLayoutBindingArray bindings
			{
				{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
				{ 1u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
			};
			return device.createDescriptorSetLayout( std::move( bindings ) );
		}

		renderer::DescriptorSetPtr doCreateUboDescriptorSet( renderer::DescriptorSetPool & pool
			, SceneUbo const & sceneUbo
			, GpInfoUbo const & gpInfoUbo )
		{
			auto & layout = pool.getLayout();
			auto result = pool.createDescriptorSet( 0u );
			result->createBinding( layout.getBinding( 0u )
				, sceneUbo.getUbo()
				, 0u
				, 1u );
			result->createBinding( layout.getBinding( 1u )
				, gpInfoUbo.getUbo()
				, 0u
				, 1u );
			return result;
		}

		renderer::DescriptorSetLayoutPtr doCreateTexDescriptorLayout( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = *renderSystem.getCurrentDevice();
			renderer::DescriptorSetLayoutBindingArray bindings
			{
				{ 0u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
				{ 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
				{ 2u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
			};
			return device.createDescriptorSetLayout( std::move( bindings ) );
		}

		renderer::DescriptorSetPtr doCreateTexDescriptorSet( renderer::DescriptorSetPool & pool
			, renderer::TextureViewCRef const & depth
			, renderer::TextureViewCRef const & accumulation
			, renderer::TextureViewCRef const & revealage
			, Sampler const & sampler )
		{
			auto & layout = pool.getLayout();
			auto result = pool.createDescriptorSet( 1u );
			result->createBinding( layout.getBinding( 0u )
				, depth.get()
				, sampler.getSampler() );
			result->createBinding( layout.getBinding( 1u )
				, accumulation.get()
				, sampler.getSampler() );
			result->createBinding( layout.getBinding( 2u )
				, revealage.get()
				, sampler.getSampler() );
			return result;
		}

		glsl::Shader doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			auto texture = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = texture;
					gl_Position = vec4( position, 0.0, 1.0 );
				} );
			return writer.finalise();
		}
		
		glsl::Shader doGetPixelProgram( Engine & engine
			, FogType fogType )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_SCENE( writer, 0u, 0u );
			UBO_GPINFO( writer, 1u, 0u );
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( WbTexture::eDepth ), 0u, 1u );
			auto c3d_mapAccumulation = writer.declSampler< Sampler2D >( getTextureName( WbTexture::eAccumulation ), 1u, 1u );
			auto c3d_mapRevealage = writer.declSampler< Sampler2D >( getTextureName( WbTexture::eRevealage ), 2u, 1u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0u );

			glsl::Utils utils{ writer };
			utils.declareCalcVSPosition();

			shader::Fog fog{ fogType, writer };

			auto maxComponent = writer.implementFunction< Float >( cuT( "maxComponent" )
				, [&]( Vec3 const & v )
				{
					writer.returnStmt( max( max( v.x(), v.y() ), v.z() ) );
				}
				, InVec3{ &writer, cuT( "v" ) } );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto coord = writer.declLocale( cuT( "coord" )
						, ivec2( gl_FragCoord.xy() ) );
					auto revealage = writer.declLocale( cuT( "revealage" )
						, texelFetch( c3d_mapRevealage, coord, 0 ).r() );

					IF( writer, revealage == 1.0_f )
					{
						// Save the blending and color texture fetch cost
						writer.discard();
					}
					FI;

					auto accum = writer.declLocale( cuT( "accum" )
						, texelFetch( c3d_mapAccumulation, coord, 0 ) );

					// Suppress overflow
					IF( writer, glsl::isinf( maxComponent( glsl::abs( accum.rgb() ) ) ) )
					{
						accum.rgb() = vec3( accum.a() );
					}
					FI;

					auto averageColor = writer.declLocale( cuT( "averageColor" )
						, accum.rgb() / max( accum.a(), 0.00001_f ) );

					pxl_fragColor = vec4( averageColor.rgb(), 1.0_f - revealage );

					if ( fogType != FogType::eDisabled )
					{
						auto position = writer.declLocale( cuT( "position" )
							, utils.calcVSPosition( vtx_texture
								, texture( c3d_mapDepth, vtx_texture ).r()
								, c3d_mtxInvProj ) );
						fog.applyFog( pxl_fragColor, length( position ), position.z() );
					}
				} );
			return writer.finalise();
		}
		
		renderer::ShaderProgram & doCreateProgram( Engine & engine
			, FogType fogType )
		{
			auto vtx = doGetVertexProgram( engine );
			auto pxl = doGetPixelProgram( engine, fogType );
			auto & program = engine.getShaderProgramCache().getNewProgram( false );
			program.createModule( vtx.getSource(), renderer::ShaderStageFlag::eVertex );
			program.createModule( pxl.getSource(), renderer::ShaderStageFlag::eFragment );
			return program;
		}

		RenderPipelineUPtr doCreateRenderPipeline( Engine & engine
			, renderer::ShaderProgram & program
			, renderer::DescriptorSetLayout const & uboLayout
			, renderer::DescriptorSetLayout const & texLayout
			, renderer::VertexLayout const & vtxLayout )
		{
			renderer::DepthStencilState dsstate
			{
				0u,
				false,
				false
			};
			renderer::RasterisationState rsstate
			{
				0u,
				false,
				false,
				renderer::PolygonMode::eFill,
				renderer::CullModeFlag::eNone
			};
			renderer::ColourBlendState bdState;
			bdState.addAttachment( {
				true,
				renderer::BlendFactor::eSrcAlpha,
				renderer::BlendFactor::eInvSrcAlpha,
				renderer::BlendOp::eAdd,
				renderer::BlendFactor::eSrcAlpha,
				renderer::BlendFactor::eInvSrcAlpha,
				renderer::BlendOp::eAdd
			} );
			auto pipeline = std::make_unique< RenderPipeline >( *engine.getRenderSystem()
				, std::move( dsstate )
				, std::move( rsstate )
				, std::move( bdState )
				, renderer::MultisampleState{}
				, program
				, PipelineFlags{} );
			pipeline->setDescriptorSetLayouts( { uboLayout, texLayout } );
			pipeline->setVertexLayouts( { vtxLayout } );
			return pipeline;
		}
	}

	//*********************************************************************************************

	FinalCombineProgram::FinalCombineProgram( Engine & engine
		, renderer::DescriptorSetLayout const & uboLayout
		, renderer::DescriptorSetLayout const & texLayout
		, renderer::VertexLayout const & vtxLayout
		, FogType fogType )
		: m_pipeline{ doCreateRenderPipeline( engine
			, doCreateProgram( engine, fogType )
			, uboLayout
			, texLayout
			, vtxLayout ) }
		, m_commandBuffer{ engine.getRenderSystem()->getCurrentDevice()->getGraphicsCommandPool().createCommandBuffer( false ) }
	{
	}

	FinalCombineProgram::~FinalCombineProgram()
	{
		m_pipeline->cleanup();
		m_pipeline.reset();
	}

	void FinalCombineProgram::initialise( renderer::RenderPass const & renderPass
		, Size const & size
		, renderer::DescriptorSet const & uboDescriptorSet
		, renderer::DescriptorSet const & texDescriptorSet
		, GeometryBuffers const & geometryBuffers )
	{
		m_pipeline->initialise( renderPass, renderer::PrimitiveTopology::eTriangleStrip );

		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eRenderPassContinue ) )
		{
			m_commandBuffer->bindPipeline( m_pipeline->getPipeline() );
			m_commandBuffer->bindDescriptorSets( { uboDescriptorSet, texDescriptorSet }
				, m_pipeline->getPipelineLayout() );
			m_commandBuffer->bindGeometryBuffers( geometryBuffers );
			m_commandBuffer->setViewport( { size.getWidth(), size.getHeight(), 0, 0 } );
			m_commandBuffer->setScissor( { 0, 0, size.getWidth(), size.getHeight() } );
			m_commandBuffer->draw( 4u );
			m_commandBuffer->end();
		}
	}

	//*********************************************************************************************

	FinalCombinePass::FinalCombinePass( Engine & engine
		, Size const & size
		, SceneUbo & sceneUbo
		, WeightedBlendTextures const & wbResult
		, renderer::RenderPass const & renderPass )
		: m_size{ size }
		, m_sceneUbo{ sceneUbo }
		, m_gpInfo{ engine }
		, m_sampler{ doCreateSampler( engine ) }
		, m_vertexBuffer{ doCreateVbo( engine ) }
		, m_vertexLayout{ doCreateVertexLayout( engine ) }
		, m_geometryBuffers{ doCreateVao( engine, *m_vertexLayout, *m_vertexBuffer ) }
		, m_uboDescriptorLayout{ doCreateUboDescriptorLayout( engine ) }
		, m_uboDescriptorPool{ m_uboDescriptorLayout->createPool( uint32_t( FogType::eCount ) ) }
		, m_uboDescriptorSet{ doCreateUboDescriptorSet( *m_uboDescriptorPool, m_sceneUbo, m_gpInfo ) }
		, m_texDescriptorLayout{ doCreateTexDescriptorLayout( engine ) }
		, m_texDescriptorPool{ m_texDescriptorLayout->createPool( uint32_t( FogType::eCount ) ) }
		, m_texDescriptorSet{ doCreateTexDescriptorSet( *m_texDescriptorPool, wbResult[0], wbResult[1], wbResult[2], *m_sampler ) }
		, m_programs
		{
			FinalCombineProgram{ engine, *m_uboDescriptorLayout, *m_texDescriptorLayout, *m_vertexLayout, FogType::eDisabled },
			FinalCombineProgram{ engine, *m_uboDescriptorLayout, *m_texDescriptorLayout, *m_vertexLayout, FogType::eLinear },
			FinalCombineProgram{ engine, *m_uboDescriptorLayout, *m_texDescriptorLayout, *m_vertexLayout, FogType::eExponential },
			FinalCombineProgram{ engine, *m_uboDescriptorLayout, *m_texDescriptorLayout, *m_vertexLayout, FogType::eSquaredExponential }
		}
	{
		for ( auto & program : m_programs )
		{
			program.initialise( renderPass
				, m_size
				, *m_uboDescriptorSet
				, *m_texDescriptorSet
				, *m_geometryBuffers );
		}
	}

	FinalCombinePass::~FinalCombinePass()
	{
		m_gpInfo.cleanup();
	}

	void FinalCombinePass::update( Camera const & camera
		, Matrix4x4r const & invViewProj
		, Matrix4x4r const & invView
		, Matrix4x4r const & invProj )
	{
		m_gpInfo.update( m_size
			, camera
			, invViewProj
			, invView
			, invProj );
	}

	renderer::CommandBuffer const & FinalCombinePass::getCommandBuffer( FogType fogType )
	{
		return m_programs[size_t( fogType )].getCommandBuffer();
	}

	//*********************************************************************************************
}
