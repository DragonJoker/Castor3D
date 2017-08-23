#include "DepthPass.hpp"

#include <Engine.hpp>
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
	DepthPass::DepthPass( String const & p_name
		, Scene & p_scene
		, Camera * p_camera )
		: castor3d::RenderPass{ p_name
			, *p_scene.getEngine()
			, nullptr }
		, m_scene{ p_scene }
		, m_camera{ p_camera }
	{
	}

	DepthPass::DepthPass( String const & p_name
		, Scene & p_scene
		, Camera * p_camera
		, bool p_oit )
		: castor3d::RenderPass{ p_name
			, *p_scene.getEngine()
			, p_oit
			, nullptr }
		, m_scene{ p_scene }
		, m_camera{ p_camera }
	{
	}

	DepthPass::~DepthPass()
	{
	}

	void DepthPass::render()
	{
		doRenderNodes( m_renderQueue.getRenderNodes()
			, *m_camera );
	}

	bool DepthPass::doInitialise( Size const & CU_PARAM_UNUSED( p_size ) )
	{
		if ( m_camera )
		{
			m_renderQueue.initialise( m_scene, *m_camera );
		}
		else
		{
			m_renderQueue.initialise( m_scene );
		}

		return true;
	}

	void DepthPass::doCleanup()
	{
	}

	void DepthPass::doUpdate( RenderQueueArray & p_queues )
	{
		p_queues.push_back( m_renderQueue );
	}

	void DepthPass::doUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		remFlag( programFlags, ProgramFlag::eLighting );
		remFlag( programFlags, ProgramFlag::eEnvironmentMapping );
	}

	void DepthPass::doRenderNodes( SceneRenderNodes & p_nodes
		, Camera const & p_camera )const
	{
		if ( !p_nodes.m_staticNodes.m_backCulled.empty()
			|| !p_nodes.m_instantiatedStaticNodes.m_backCulled.empty()
			|| !p_nodes.m_skinnedNodes.m_backCulled.empty()
			|| !p_nodes.m_instantiatedSkinnedNodes.m_backCulled.empty()
			|| !p_nodes.m_morphingNodes.m_backCulled.empty()
			|| !p_nodes.m_billboardNodes.m_backCulled.empty() )
		{
			m_matrixUbo.update( p_camera.getView()
				, p_camera.getViewport().getProjection() );
			doRender( p_nodes.m_instantiatedStaticNodes.m_frontCulled, p_camera );
			doRender( p_nodes.m_staticNodes.m_frontCulled, p_camera );
			doRender( p_nodes.m_skinnedNodes.m_frontCulled, p_camera );
			doRender( p_nodes.m_instantiatedSkinnedNodes.m_frontCulled, p_camera );
			doRender( p_nodes.m_morphingNodes.m_frontCulled, p_camera );
			doRender( p_nodes.m_billboardNodes.m_frontCulled, p_camera );

			doRender( p_nodes.m_instantiatedStaticNodes.m_backCulled, p_camera );
			doRender( p_nodes.m_staticNodes.m_backCulled, p_camera );
			doRender( p_nodes.m_skinnedNodes.m_backCulled, p_camera );
			doRender( p_nodes.m_instantiatedSkinnedNodes.m_backCulled, p_camera );
			doRender( p_nodes.m_morphingNodes.m_backCulled, p_camera );
			doRender( p_nodes.m_billboardNodes.m_backCulled, p_camera );
		}
	}

	void DepthPass::doPrepareFrontPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
	}

	void DepthPass::doUpdatePipeline( RenderPipeline & p_pipeline )const
	{
	}

	void DepthPass::doPrepareBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto it = m_backPipelines.find( p_flags );

		if ( it == m_backPipelines.end() )
		{
			DepthStencilState dsState;
			dsState.setDepthTest( true );
			dsState.setDepthMask( WritingMask::eAll );
			RasteriserState rsState;
			rsState.setCulledFaces( Culling::eNone );
			auto & pipeline = *m_backPipelines.emplace( p_flags
				, getEngine()->getRenderSystem()->createRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, BlendState{}
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;

			getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, p_flags]()
			{
				pipeline.addUniformBuffer( m_matrixUbo.getUbo() );
				pipeline.addUniformBuffer( m_modelMatrixUbo.getUbo() );
				pipeline.addUniformBuffer( m_modelUbo.getUbo() );

				if ( checkFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
				{
					pipeline.addUniformBuffer( m_billboardUbo.getUbo() );
				}

				if ( checkFlag( p_flags.m_programFlags, ProgramFlag::eSkinning )
					&& !checkFlag( p_flags.m_programFlags, ProgramFlag::eInstantiation ) )
				{
					pipeline.addUniformBuffer( m_skinningUbo.getUbo() );
				}

				if ( checkFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
				{
					pipeline.addUniformBuffer( m_morphingUbo.getUbo() );
				}
			} ) );
		}
	}

	glsl::Shader DepthPass::doGetVertexShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		using namespace glsl;
		auto writer = getEngine()->getRenderSystem()->createGlslWriter();
		// Vertex inputs
		auto position = writer.declAttribute< Vec4 >( ShaderProgram::Position );
		auto bone_ids0 = writer.declAttribute< IVec4 >( ShaderProgram::BoneIds0, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declAttribute< IVec4 >( ShaderProgram::BoneIds1, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declAttribute< Vec4 >( ShaderProgram::Weights0, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declAttribute< Vec4 >( ShaderProgram::Weights1, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declAttribute< Mat4 >( ShaderProgram::Transform, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declAttribute< Vec4 >( ShaderProgram::Position2, checkFlag( programFlags, ProgramFlag::eMorphing ) );

		UBO_MATRIX( writer );
		UBO_MODEL_MATRIX( writer );
		SkinningUbo::declare( writer, programFlags );
		UBO_MORPHING( writer, programFlags );
		UBO_MODEL( writer );

		// Outputs
		auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > main = [&]()
		{
			auto v4Vertex = writer.declLocale( cuT( "v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto mtxModel = writer.declLocale< Mat4 >( cuT( "mtxModel" ) );

			if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				mtxModel = SkinningUbo::computeTransform( writer, programFlags );
			}
			else if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				mtxModel = transform;
			}
			else
			{
				mtxModel = c3d_mtxModel;
			}

			if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
			{
				auto time = writer.declLocale( cuT( "time" ), 1.0_f - c3d_time );
				v4Vertex = vec4( v4Vertex.xyz() * time + position2.xyz() * c3d_time, 1.0 );
			}

			v4Vertex = mtxModel * v4Vertex;
			v4Vertex = c3d_mtxView * v4Vertex;
			gl_Position = c3d_mtxProjection * v4Vertex;
		};

		writer.implementFunction< void >( cuT( "main" ), main );
		return writer.finalise();
	}

	glsl::Shader DepthPass::doGetGeometryShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return glsl::Shader{};
	}

	glsl::Shader DepthPass::doGetLegacyPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return doGetPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	glsl::Shader DepthPass::doGetPbrMRPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return doGetPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	glsl::Shader DepthPass::doGetPbrSGPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return doGetPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	glsl::Shader DepthPass::doGetPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
		} );

		return writer.finalise();
	}
}
