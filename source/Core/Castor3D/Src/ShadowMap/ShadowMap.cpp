#include "ShadowMap.hpp"

#include "Engine.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Light/Light.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "ShadowMap/ShadowMapPass.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <GlslSource.hpp>
#include "Shader/Shaders/GlslMaterial.hpp"

using namespace castor;

namespace castor3d
{
	ShadowMap::ShadowMap( Engine & engine
		, TextureUnit && shadowMap
		, ShadowMapPassSPtr pass )
		: OwnedBy< Engine >{ engine }
		, m_shadowMap{ std::move( shadowMap ) }
		, m_pass{ pass }
	{
	}

	ShadowMap::~ShadowMap()
	{
	}

	bool ShadowMap::initialise()
	{
		bool result = true;

		if ( !m_frameBuffer )
		{
			m_shadowMap.initialise();
			m_frameBuffer = getEngine()->getRenderSystem()->createFrameBuffer();
			result = m_frameBuffer->initialise();
			auto size = m_shadowMap.getTexture()->getDimensions();

			if ( result )
			{
				result = m_pass->initialise( size );
			}

			if ( result )
			{
				doInitialise();
			}

			m_blur = std::make_unique< GaussianBlur >( *getEngine()
				, m_shadowMap.getTexture()->getDimensions()
				, m_shadowMap.getTexture()->getPixelFormat()
				, 3u );
		}

		return result;
	}

	void ShadowMap::cleanup()
	{
		m_pass->cleanup();

		if ( m_frameBuffer )
		{
			m_frameBuffer->bind();
			m_frameBuffer->detachAll();
			m_frameBuffer->unbind();

			doCleanup();

			m_frameBuffer->cleanup();
			m_frameBuffer.reset();

			m_shadowMap.cleanup();
		}

		for ( auto buffer : m_geometryBuffers )
		{
			buffer->cleanup();
		}

		m_geometryBuffers.clear();
	}

	void ShadowMap::updateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		remFlag( programFlags, ProgramFlag::eLighting );
		remFlag( passFlags, PassFlag::eAlphaBlending );
		remFlag( textureFlags, TextureChannel( uint16_t( TextureChannel::eAll )
			& ~uint16_t( TextureChannel::eOpacity ) ) );
		doUpdateFlags( passFlags
			, textureFlags
			, programFlags
			, sceneFlags );
	}

	glsl::Shader ShadowMap::getVertexShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		return doGetVertexShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, invertNormals );
	}

	glsl::Shader ShadowMap::getGeometryShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return doGetGeometryShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags );
	}

	glsl::Shader ShadowMap::getPixelShaderSource( PassFlags const & passFlags
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

	glsl::Shader ShadowMap::doGetVertexShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		using namespace glsl;
		auto writer = getEngine()->getRenderSystem()->createGlslWriter();

		// Vertex inputs
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
		auto vtx_position = writer.declOutput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_viewPosition = writer.declOutput< Vec3 >( cuT( "vtx_viewPosition" ) );
		auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = writer.declOutput< Int >( cuT( "vtx_instance" ) );
		auto vtx_material = writer.declOutput< Int >( cuT( "vtx_material" ) );
		auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > main = [&]()
		{
			auto v4Vertex = writer.declLocale( cuT( "v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto v3Texture = writer.declLocale( cuT( "v3Texture" ), texture );
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

			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				vtx_material = material;
			}
			else
			{
				vtx_material = c3d_materialIndex;
			}

			if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
			{
				auto time = writer.declLocale( cuT( "time" ), 1.0_f - c3d_time );
				v4Vertex = vec4( v4Vertex.xyz() * time + position2.xyz() * c3d_time, 1.0 );
				v3Texture = v3Texture * writer.paren( 1.0_f - c3d_time ) + texture2 * c3d_time;
			}

			vtx_texture = v3Texture;
			v4Vertex = mtxModel * v4Vertex;
			vtx_position = v4Vertex.xyz();
			vtx_instance = gl_InstanceID;
			v4Vertex = c3d_curView * v4Vertex;
			vtx_viewPosition = v4Vertex.xyz();
			gl_Position = c3d_projection * v4Vertex;
		};

		writer.implementFunction< void >( cuT( "main" ), main );
		return writer.finalise();
	}

	glsl::Shader ShadowMap::doGetGeometryShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return glsl::Shader{};
	}

	void ShadowMap::doDiscardAlpha( glsl::GlslWriter & writer
		, TextureChannels const & textureFlags
		, ComparisonFunc alphaFunc
		, glsl::Int const & materialIndex
		, shader::Materials const & materials )const
	{
		using namespace glsl;
		auto material = materials.getBaseMaterial( materialIndex );
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
	}
}
