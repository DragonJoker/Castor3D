#include "ShadowMapPoint.hpp"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>

#include <FrameBuffer/DepthStencilRenderBuffer.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/BillboardList.hpp>
#include <Scene/Light/Light.hpp>
#include <Scene/Light/PointLight.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/ShaderProgram.hpp>
#include <ShadowMap/ShadowMapPassPoint.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>
#include <Miscellaneous/BlockTracker.hpp>

using namespace Castor;
using namespace Castor3D;

namespace deferred
{
	namespace
	{
		static String const ShadowMapUbo = cuT( "ShadowMap" );
		static String const WorldLightPosition = cuT( "c3d_v3WorldLightPosition" );
		static String const FarPlane = cuT( "c3d_fFarPlane" );
		static String const ShadowMatrices = cuT( "c3d_mtxShadowMatrices" );

		TextureUnit DoInitialisePoint( Engine & p_engine, Size const & p_size )
		{
			auto l_sampler = p_engine.GetSamplerCache().Add( cuT( "ShadowMap_Point" ) );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
			l_sampler->SetComparisonMode( ComparisonMode::eRefToTexture );
			l_sampler->SetComparisonFunc( ComparisonFunc::eLEqual );
			TextureUnit l_unit{ p_engine };
			auto l_texture = p_engine.GetRenderSystem()->CreateTexture(
				TextureType::eCube,
				AccessType::eNone,
				AccessType::eRead | AccessType::eWrite,
				PixelFormat::eD32F,
				p_size );
			l_unit.SetTexture( l_texture );
			l_unit.SetSampler( l_sampler );

			for ( auto & l_image : *l_texture )
			{
				l_image->InitialiseSource();
			}

			return l_unit;
		}
	}

	ShadowMapPoint::ShadowMapPoint( Engine & p_engine )
		: Castor3D::ShadowMap{ p_engine }
		, m_shadowMap{ DoInitialisePoint( p_engine, Size{ 1024, 1024 } ) }
	{
	}

	ShadowMapPoint::~ShadowMapPoint()
	{
	}
	
	void ShadowMapPoint::Update( Camera const & p_camera
		, RenderQueueArray & p_queues )
	{
		for ( auto & l_pass : m_passes )
		{
			l_pass.second->Update( p_queues, 0 );
		}
	}

	void ShadowMapPoint::Render( PointLight const & p_light )
	{
		CASTOR_TRACK( l_tracker );
		auto l_it = m_passes.find( &p_light.GetLight() );
		REQUIRE( l_it != m_passes.end() && "Light not found, call AddLight..." );
		m_frameBuffer->Bind( FrameBufferMode::eManual, FrameBufferTarget::eDraw );
		m_depthAttach->Attach( AttachmentPoint::eDepth );
		m_depthAttach->Clear( BufferComponent::eDepth );
		l_it->second->Render();
		m_depthAttach->Detach();
		m_frameBuffer->Unbind();
	}

	int32_t ShadowMapPoint::DoGetMaxPasses()const
	{
		return 1;
	}

	Size ShadowMapPoint::DoGetSize()const
	{
		return m_shadowMap.GetTexture()->GetDimensions();
	}

	void ShadowMapPoint::DoInitialise()
	{
		constexpr float l_component = std::numeric_limits< float >::max();
		m_frameBuffer->SetClearColour( l_component, l_component, l_component, l_component );
		auto l_texture = m_shadowMap.GetTexture();
		l_texture->Initialise();
		m_depthAttach = m_frameBuffer->CreateAttachment( l_texture );
		m_depthAttach->SetTarget( l_texture->GetType() );
	}

	void ShadowMapPoint::DoCleanup()
	{
		m_depthAttach.reset();
		m_shadowMap.Cleanup();
	}

	ShadowMapPassSPtr ShadowMapPoint::DoCreatePass( Light & p_light )const
	{
		return std::make_shared< ShadowMapPassPoint >( *GetEngine(), p_light, *this );
	}

	void ShadowMapPoint::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, SceneFlags & p_sceneFlags )const
	{
		AddFlag( p_programFlags, ProgramFlag::eShadowMapPoint );
	}

	String ShadowMapPoint::DoGetVertexShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, bool p_invertNormals )const
	{
		using namespace GLSL;
		auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Vertex inputs
		auto position = l_writer.GetAttribute< Vec4 >( ShaderProgram::Position );
		auto bone_ids0 = l_writer.GetAttribute< IVec4 >( ShaderProgram::BoneIds0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = l_writer.GetAttribute< IVec4 >( ShaderProgram::BoneIds1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Weights0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Weights1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto transform = l_writer.GetAttribute< Mat4 >( ShaderProgram::Transform, CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Position2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );

		UBO_MATRIX( l_writer );
		UBO_MODEL_MATRIX( l_writer );
		UBO_SKINNING( l_writer, p_programFlags );
		UBO_MORPHING( l_writer, p_programFlags );

		// Outputs
		auto vtx_worldSpacePosition = l_writer.GetOutput< Vec3 >( cuT( "vtx_worldSpacePosition" ) );
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > l_main = [&]()
		{
			auto l_v4Vertex = l_writer.GetLocale( cuT( "l_v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto l_mtxModel = l_writer.GetLocale< Mat4 >( cuT( "l_mtxModel" ) );

			if ( CheckFlag( p_programFlags, ProgramFlag::eSkinning ) )
			{
				auto l_mtxBoneTransform = l_writer.GetLocale< Mat4 >( cuT( "l_mtxBoneTransform" ) );
				l_mtxBoneTransform = c3d_mtxBones[bone_ids0[0_i]] * weights0[0_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[1_i]] * weights0[1_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[2_i]] * weights0[2_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[3_i]] * weights0[3_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[0_i]] * weights1[0_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[1_i]] * weights1[1_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[2_i]] * weights1[2_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[3_i]] * weights1[3_i];
				l_mtxModel = c3d_mtxModel * l_mtxBoneTransform;
			}
			else if ( CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) )
			{
				l_mtxModel = transform;
			}
			else
			{
				l_mtxModel = c3d_mtxModel;
			}

			if ( CheckFlag( p_programFlags, ProgramFlag::eMorphing ) )
			{
				auto l_time = l_writer.GetLocale( cuT( "l_time" ), 1.0_f - c3d_fTime );
				l_v4Vertex = vec4( l_v4Vertex.xyz() * l_time + position2.xyz() * c3d_fTime, 1.0 );
			}

			l_v4Vertex = l_mtxModel * l_v4Vertex;
			vtx_worldSpacePosition = l_v4Vertex.xyz();
			gl_Position = l_v4Vertex;
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

	String ShadowMapPoint::DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Geometry layout
		l_writer << InputLayout{ InputLayout::eTriangles };
		l_writer << OutputLayout{ OutputLayout::eTriangleStrip, 18 };

		// Geometry inputs
		Ubo l_shadowMap{ l_writer, ShadowMapUbo };
		auto c3d_v3WordLightPosition( l_shadowMap.GetUniform< Vec3 >( WorldLightPosition ) );
		auto c3d_fFarPlane( l_shadowMap.GetUniform< Float >( FarPlane ) );
		auto c3d_mtxShadowMatrices = l_shadowMap.GetUniform< Mat4 >( ShadowMatrices, 6u );
		l_shadowMap.End();

		auto gl_in = l_writer.GetBuiltin< gl_PerVertex >( cuT( "gl_in" ), 8u );
		auto gl_Layer = l_writer.GetBuiltin< Int >( cuT( "gl_Layer" ) );

		//Geometry outputs
		auto geo_position = l_writer.GetOutput< Vec4 >( cuT( "geo_position" ) );
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > l_main = [&]()
		{
			for( int face = 0; face < 6; ++face )
			{
				gl_Layer = face; // built-in variable that specifies to which face we render.

				for ( int i = 0; i < 3; ++i )
				{
					geo_position = gl_in[i].gl_Position();
					gl_Position = c3d_mtxShadowMatrices[face] * geo_position;
					l_writer.EmitVertex();
					l_writer << Endl();
				}

				l_writer.EndPrimitive();
			}
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

	String ShadowMapPoint::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Fragment Intputs
		Ubo l_shadowMap{ l_writer, ShadowMapUbo };
		auto c3d_v3WordLightPosition( l_shadowMap.GetUniform< Vec3 >( WorldLightPosition ) );
		auto c3d_fFarPlane( l_shadowMap.GetUniform< Float >( FarPlane ) );
		auto c3d_mtxShadowMatrices = l_shadowMap.GetUniform< Mat4 >( ShadowMatrices, 6u );
		l_shadowMap.End();

		auto geo_position( l_writer.GetInput< Vec4 >( cuT( "geo_position" ) ) );

		// Fragment Outputs
		auto gl_FragDepth( l_writer.GetBuiltin< Float >( cuT( "gl_FragDepth" ) ) );

		auto l_main = [&]()
		{
			auto l_distance = l_writer.GetLocale( cuT( "l_distance" ), length( geo_position.xyz() - c3d_v3WordLightPosition ) );
			gl_FragDepth = l_distance / c3d_fFarPlane;
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}
}
