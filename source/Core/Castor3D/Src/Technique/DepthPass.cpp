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
		, Scene const & scene
		, Camera & camera
		, SsaoConfig const & ssaoConfig
		, TextureLayoutSPtr depthBuffer )
		: RenderTechniquePass{ name
			, scene
			, &camera
			, false
			, nullptr
			, ssaoConfig }
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

	void DepthPass::render( RenderInfo & info
		, ShadowMapLightTypeArray & shadowMaps
		, Point2r const & jitter )
	{
		m_camera->apply();
		m_frameBuffer->bind();
		m_frameBuffer->clear( BufferComponent::eDepth | BufferComponent::eStencil );
		doRender( info
			, shadowMaps
			, jitter );
		m_frameBuffer->unbind();
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
		addFlag( programFlags, ProgramFlag::eDepthPass );
		remFlag( programFlags, ProgramFlag::eLighting );
		remFlag( programFlags, ProgramFlag::eEnvironmentMapping );
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
		auto gl_InstanceID( writer.declBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( writer );
		UBO_MODEL_MATRIX( writer );
		UBO_MODEL( writer );
		SkinningUbo::declare( writer, programFlags );
		UBO_MORPHING( writer, programFlags );

		// Outputs
		auto vtx_curPosition = writer.declOutput< Vec3 >( cuT( "vtx_curPosition" ) );
		auto vtx_prvPosition = writer.declOutput< Vec3 >( cuT( "vtx_prvPosition" ) );
		auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_material = writer.declOutput< Int >( cuT( "vtx_material" ) );
		auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

		writer.implementFunction< void >( cuT( "main" )
			, [&]()
		{
				auto curPosition = writer.declLocale( cuT( "curPosition" )
					, vec4( position.xyz(), 1.0 ) );
				auto v3Texture = writer.declLocale( cuT( "v3Texture" )
					, texture );

				if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
				{
					auto mtxModel = writer.declLocale( cuT( "mtxModel" )
						, SkinningUbo::computeTransform( writer, programFlags ) );
				}
				else if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
				{
					auto mtxModel = writer.declLocale( cuT( "mtxModel" )
						, transform );
				}
				else
				{
					auto mtxModel = writer.declLocale( cuT( "mtxModel" )
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

				auto mtxModel = writer.declBuiltin< Mat4 >( cuT( "mtxModel" ) );

				if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
				{
					curPosition = vec4( glsl::mix( curPosition.xyz(), position2.xyz(), c3d_time ), 1.0 );
					v3Texture = glsl::mix( v3Texture, texture2, c3d_time );
				}

				vtx_texture = v3Texture;
				curPosition = mtxModel * curPosition;
				auto prvPosition = writer.declLocale( cuT( "prvPosition" )
					, c3d_prvViewProj * curPosition );
				gl_Position = c3d_curViewProj * curPosition;
				// Convert the jitter from non-homogeneous coordiantes to homogeneous
				// coordinates and add it:
				// (note that for providing the jitter in non-homogeneous projection space,
				//  pixel coordinates (screen space) need to multiplied by two in the C++
				//  code)
				gl_Position.xy() -= c3d_curJitter * gl_Position.w();
				prvPosition.xy() -= c3d_prvJitter * gl_Position.w();
				vtx_curPosition = gl_Position.xyw();
				vtx_prvPosition = prvPosition.xyw();
				// Positions in projection space are in [-1, 1] range, while texture
				// coordinates are in [0, 1] range. So, we divide by 2 to get velocities in
				// the scale (and flip the y axis):
				vtx_curPosition.xy() *= vec2( 0.5_f, -0.5 );
				vtx_prvPosition.xy() *= vec2( 0.5_f, -0.5 );
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
		auto vtx_curPosition = writer.declInput< Vec3 >( cuT( "vtx_curPosition" ) );
		auto vtx_prvPosition = writer.declInput< Vec3 >( cuT( "vtx_prvPosition" ) );
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
