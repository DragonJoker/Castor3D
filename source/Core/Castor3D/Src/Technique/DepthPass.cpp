#include "DepthPass.hpp"

#include <Engine.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslMaterial.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	DepthPass::DepthPass( String const & name
		, Scene & scene
		, TextureLayoutSPtr depthBuffer )
		: RenderPass{ name
			, *scene.getEngine()
			, nullptr }
		, m_scene{ scene }
	{
		auto size = depthBuffer->getDimensions();
		m_frameBuffer = getEngine()->getRenderSystem()->createFrameBuffer();
		auto result = m_frameBuffer->initialise();
		REQUIRE( result );
		m_depthAttach = m_frameBuffer->createAttachment( depthBuffer );

		m_frameBuffer->bind();
		m_frameBuffer->attach( AttachmentPoint::eDepthStencil, m_depthAttach, depthBuffer->getType() );
		REQUIRE( m_frameBuffer->isComplete() );
		m_frameBuffer->unbind();
	}

	DepthPass::~DepthPass()
	{
		m_frameBuffer->bind();
		m_frameBuffer->detachAll();
		m_frameBuffer->unbind();
		m_frameBuffer->cleanup();
	}

	void DepthPass::render( Camera const & camera )
	{
		camera.apply();
		m_frameBuffer->bind();
		m_frameBuffer->clear( BufferComponent::eDepth | BufferComponent::eStencil );
		doRenderNodes( m_renderQueue.getRenderNodes()
			, camera );
		m_frameBuffer->unbind();
	}

	bool DepthPass::doInitialise( Size const & CU_PARAM_UNUSED( p_size ) )
	{
		m_renderQueue.initialise( m_scene );
		return true;
	}

	void DepthPass::doCleanup()
	{
	}

	void DepthPass::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( m_renderQueue );
	}

	void DepthPass::doUpdateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		remFlag( programFlags, ProgramFlag::eLighting );
		remFlag( programFlags, ProgramFlag::eEnvironmentMapping );
		addFlag( programFlags, ProgramFlag::eNoFragment );
	}

	void DepthPass::doRenderNodes( SceneRenderNodes & nodes
		, Camera const & camera )const
	{
		if ( !nodes.m_staticNodes.m_backCulled.empty()
			|| !nodes.m_instantiatedStaticNodes.m_backCulled.empty()
			|| !nodes.m_skinnedNodes.m_backCulled.empty()
			|| !nodes.m_instantiatedSkinnedNodes.m_backCulled.empty()
			|| !nodes.m_morphingNodes.m_backCulled.empty()
			|| !nodes.m_billboardNodes.m_backCulled.empty() )
		{
			m_matrixUbo.update( camera.getView()
				, camera.getViewport().getProjection() );
			doRender( nodes.m_instantiatedStaticNodes.m_frontCulled, camera );
			doRender( nodes.m_staticNodes.m_frontCulled, camera );
			doRender( nodes.m_skinnedNodes.m_frontCulled, camera );
			doRender( nodes.m_instantiatedSkinnedNodes.m_frontCulled, camera );
			doRender( nodes.m_morphingNodes.m_frontCulled, camera );
			doRender( nodes.m_billboardNodes.m_frontCulled, camera );

			doRender( nodes.m_instantiatedStaticNodes.m_backCulled, camera );
			doRender( nodes.m_staticNodes.m_backCulled, camera );
			doRender( nodes.m_skinnedNodes.m_backCulled, camera );
			doRender( nodes.m_instantiatedSkinnedNodes.m_backCulled, camera );
			doRender( nodes.m_morphingNodes.m_backCulled, camera );
			doRender( nodes.m_billboardNodes.m_backCulled, camera );
		}
	}

	void DepthPass::doUpdatePipeline( RenderPipeline & pipeline )const
	{
	}

	void DepthPass::doPrepareFrontPipeline( ShaderProgram & program
		, PipelineFlags const & flags )
	{
		auto it = m_frontPipelines.find( flags );

		if ( it == m_frontPipelines.end() )
		{
			DepthStencilState dsState;
			dsState.setDepthTest( true );
			dsState.setDepthMask( WritingMask::eAll );
			RasteriserState rsState;
			rsState.setCulledFaces( Culling::eFront );
			auto & pipeline = *m_frontPipelines.emplace( flags
				, getEngine()->getRenderSystem()->createRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, BlendState{}
					, MultisampleState{}
					, program
					, flags ) ).first->second;

			getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, flags]()
				{
					pipeline.addUniformBuffer( m_matrixUbo.getUbo() );
					pipeline.addUniformBuffer( m_modelMatrixUbo.getUbo() );
					pipeline.addUniformBuffer( m_modelUbo.getUbo() );

					if ( checkFlag( flags.m_programFlags, ProgramFlag::eBillboards ) )
					{
						pipeline.addUniformBuffer( m_billboardUbo.getUbo() );
					}

					if ( checkFlag( flags.m_programFlags, ProgramFlag::eSkinning )
						&& !checkFlag( flags.m_programFlags, ProgramFlag::eInstantiation ) )
					{
						pipeline.addUniformBuffer( m_skinningUbo.getUbo() );
					}

					if ( checkFlag( flags.m_programFlags, ProgramFlag::eMorphing ) )
					{
						pipeline.addUniformBuffer( m_morphingUbo.getUbo() );
					}
				} ) );
		}
	}

	void DepthPass::doPrepareBackPipeline( ShaderProgram & program
		, PipelineFlags const & flags )
	{
		auto it = m_backPipelines.find( flags );

		if ( it == m_backPipelines.end() )
		{
			DepthStencilState dsState;
			dsState.setDepthTest( true );
			dsState.setDepthMask( WritingMask::eAll );
			RasteriserState rsState;
			rsState.setCulledFaces( Culling::eBack );
			auto & pipeline = *m_backPipelines.emplace( flags
				, getEngine()->getRenderSystem()->createRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, BlendState{}
					, MultisampleState{}
					, program
					, flags ) ).first->second;

			getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, flags]()
				{
					pipeline.addUniformBuffer( m_matrixUbo.getUbo() );
					pipeline.addUniformBuffer( m_modelMatrixUbo.getUbo() );
					pipeline.addUniformBuffer( m_modelUbo.getUbo() );

					if ( checkFlag( flags.m_programFlags, ProgramFlag::eBillboards ) )
					{
						pipeline.addUniformBuffer( m_billboardUbo.getUbo() );
					}

					if ( checkFlag( flags.m_programFlags, ProgramFlag::eSkinning )
						&& !checkFlag( flags.m_programFlags, ProgramFlag::eInstantiation ) )
					{
						pipeline.addUniformBuffer( m_skinningUbo.getUbo() );
					}

					if ( checkFlag( flags.m_programFlags, ProgramFlag::eMorphing ) )
					{
						pipeline.addUniformBuffer( m_morphingUbo.getUbo() );
					}
				} ) );
		}
	}

	glsl::Shader DepthPass::doGetVertexShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		using namespace glsl;
		auto writer = getEngine()->getRenderSystem()->createGlslWriter();
		// Inputs
		auto position = writer.declAttribute< Vec4 >( ShaderProgram::Position );
		auto texture = writer.declAttribute< Vec3 >( ShaderProgram::Texture );
		auto bone_ids0 = writer.declAttribute< IVec4 >( ShaderProgram::BoneIds0
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declAttribute< IVec4 >( ShaderProgram::BoneIds1
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declAttribute< Vec4 >( ShaderProgram::Weights0
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declAttribute< Vec4 >( ShaderProgram::Weights1
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declAttribute< Mat4 >( ShaderProgram::Transform
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declAttribute< Int >( ShaderProgram::Material
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declAttribute< Vec4 >( ShaderProgram::Position2
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declAttribute< Vec3 >( ShaderProgram::Texture2
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );

		UBO_MATRIX( writer );
		UBO_MODEL_MATRIX( writer );
		UBO_MODEL( writer );
		SkinningUbo::declare( writer, programFlags );
		UBO_MORPHING( writer, programFlags );

		// Outputs
		auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_material = writer.declOutput< Int >( cuT( "vtx_material" ) );
		auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

		writer.implementFunction< void >( cuT( "main" )
			, [&]()
			{
				auto vertex = writer.declLocale( cuT( "vertex" )
					, vec4( position.xyz(), 1.0 ) );
				vtx_texture = texture;

				if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
				{
					auto model = writer.declLocale( cuT( "model" )
						, SkinningUbo::computeTransform( writer, programFlags ) );
				}
				else if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
				{
					auto model = writer.declLocale( cuT( "model" )
						, transform );
				}
				else
				{
					auto model = writer.declLocale( cuT( "model" )
						, c3d_mtxModel );
				}

				if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
				{
					vtx_material = material;
				}
				else
				{
					vtx_material = c3d_materialIndex;
				}

				auto model = writer.declBuiltin< Mat4 >( cuT( "model" ) );

				if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
				{
					auto time = writer.declLocale( cuT( "time" )
						, vec3( 1.0_f - c3d_time ) );
					vtx_texture = glsl::fma( vtx_texture, time, texture2 * c3d_time );
					vertex = vec4( glsl::fma( vertex.xyz(), time, position2.xyz() * c3d_time ), 1.0 );
				}

				gl_Position = c3d_projection * c3d_curView * model * vertex;
			} );

		return writer.finalise();
	}

	glsl::Shader DepthPass::doGetGeometryShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return glsl::Shader{};
	}

	glsl::Shader DepthPass::doGetLegacyPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return doGetPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	glsl::Shader DepthPass::doGetPbrMRPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return doGetPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	glsl::Shader DepthPass::doGetPbrSGPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return doGetPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	glsl::Shader DepthPass::doGetPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		// Intputs
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" ) );
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( ShaderProgram::MapOpacity
			, checkFlag( textureFlags, TextureChannel::eOpacity ) ) );
		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto materials = shader::createMaterials( writer, passFlags );
		materials->declare();

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto material = materials->getBaseMaterial( vtx_material );
			auto alpha = writer.declLocale( cuT( "alpha" )
				, material->m_opacity() );
			auto alphaRef = writer.declLocale( cuT( "alphaRef" )
				, material->m_alphaRef() );

			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				auto c3d_mapOpacity = writer.getBuiltin< glsl::Sampler2D >( ShaderProgram::MapOpacity );
				auto vtx_texture = writer.getBuiltin< glsl::Vec3 >( cuT( "vtx_texture" ) );
				alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
				shader::applyAlphaFunc( writer
					, alphaFunc
					, alpha
					, alphaRef );
			}
			else if ( alphaFunc != ComparisonFunc::eAlways )
			{
				shader::applyAlphaFunc( writer
					, alphaFunc
					, alpha
					, alphaRef );
			}
		} );

		return writer.finalise();
	}
}
