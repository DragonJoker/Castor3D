#include "ShadowMap.hpp"

#include "Engine.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Light/Light.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"
#include "Shader/Ubos/ModelUbo.hpp"
#include "Shader/Ubos/MorphingUbo.hpp"
#include "Shader/Ubos/SceneUbo.hpp"
#include "Shader/Ubos/SkinningUbo.hpp"
#include "ShadowMap/ShadowMapPass.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>

#include <GlslSource.hpp>
#include "Shader/Shaders/GlslMaterial.hpp"

using namespace castor;

namespace castor3d
{
	ShadowMap::ShadowMap( Engine & engine
		, TextureUnit && shadowMap
		, TextureUnit && linearMap
		, ShadowMapPassSPtr pass )
		: OwnedBy< Engine >{ engine }
		, m_shadowMap{ std::move( shadowMap ) }
		, m_linearMap{ std::move( linearMap ) }
		, m_pass{ pass }
	{
	}

	ShadowMap::~ShadowMap()
	{
	}

	bool ShadowMap::initialise()
	{
		bool result = true;

		if ( !m_initialised )
		{
			m_shadowMap.initialise();
			m_linearMap.initialise();
			auto size = m_shadowMap.getTexture()->getDimensions();
			result = m_pass->initialise( { size.width, size.height } );

			if ( result )
			{
				doInitialise();
				m_initialised = true;
			}
		}

		return result;
	}

	void ShadowMap::cleanup()
	{
		m_pass->cleanup();

		if ( m_initialised )
		{
			m_initialised = false;
			doCleanup();
			m_shadowMap.cleanup();
			m_linearMap.cleanup();
		}
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
		, renderer::CompareOp alphaFunc )const
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
		auto position = writer.declAttribute< Vec4 >( cuT( "position" ), Submesh::Position );
		auto texture = writer.declAttribute< Vec3 >( cuT( "texcoord" ), Submesh::Texture );
		auto bone_ids0 = writer.declAttribute< IVec4 >( cuT( "bone_ids0" )
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declAttribute< IVec4 >( cuT( "bone_ids1" )
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declAttribute< Vec4 >( cuT( "weights0" )
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declAttribute< Vec4 >( cuT( "weights1" )
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declAttribute< Mat4 >( cuT( "transform" )
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declAttribute< Int >( cuT( "material" )
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declAttribute< Vec4 >( cuT( "position2" )
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declAttribute< Vec3 >( cuT( "texture2" )
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( writer.declBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, programFlags );

		// Outputs
		auto vtx_worldPosition = writer.declOutput< Vec3 >( cuT( "vtx_worldPosition" ), 0u );
		auto vtx_viewPosition = writer.declOutput< Vec3 >( cuT( "vtx_viewPosition" ), 1u );
		auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" ), 2u );
		auto vtx_instance = writer.declOutput< Int >( cuT( "vtx_instance" ), 3u );
		auto vtx_material = writer.declOutput< Int >( cuT( "vtx_material" ), 4u );
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
			vtx_worldPosition = v4Vertex.xyz();
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
		, renderer::CompareOp alphaFunc
		, glsl::Int const & materialIndex
		, shader::Materials const & materials )const
	{
		using namespace glsl;

		if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
		{
			auto material = materials.getBaseMaterial( materialIndex );
			auto alpha = writer.declLocale( cuT( "alpha" )
				, material->m_opacity() );
			auto alphaRef = writer.declLocale( cuT( "alphaRef" )
				, material->m_alphaRef() );

			auto c3d_mapOpacity = writer.getBuiltin< glsl::Sampler2D >( cuT( "c3d_mapOpacity" ) );
			auto vtx_texture = writer.getBuiltin< glsl::Vec3 >( cuT( "vtx_texture" ) );
			alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
			shader::applyAlphaFunc( writer
				, alphaFunc
				, alpha
				, alphaRef );
		}
		else if ( alphaFunc != renderer::CompareOp::eAlways )
		{
			auto material = materials.getBaseMaterial( materialIndex );
			auto alpha = writer.declLocale( cuT( "alpha" )
				, material->m_opacity() );
			auto alphaRef = writer.declLocale( cuT( "alphaRef" )
				, material->m_alphaRef() );

			shader::applyAlphaFunc( writer
				, alphaFunc
				, alpha
				, alphaRef );
		}
	}
}
