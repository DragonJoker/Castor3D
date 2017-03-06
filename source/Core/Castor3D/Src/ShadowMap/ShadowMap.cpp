#include "ShadowMap.hpp"

#include "Engine.hpp"
#include "Cache/ShaderCache.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "FrameBuffer/ColourRenderBuffer.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/DirectionalLight.hpp"
#include "Scene/Light/SpotLight.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBuffer.hpp"
#include "ShadowMap/ShadowMapPass.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	ShadowMap::ShadowMap( Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
		, m_shadowMap{ p_engine }
	{
	}

	ShadowMap::~ShadowMap()
	{
	}

	bool ShadowMap::Initialise( Size const & p_size )
	{
		bool l_return = true;

		if ( !m_frameBuffer )
		{
			m_frameBuffer = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			l_return = m_frameBuffer->Create();

			if ( l_return )
			{
				l_return = m_frameBuffer->Initialise( p_size );
			}

			if ( l_return )
			{
				l_return = DoInitialise( p_size );
			}

			auto l_texture = m_shadowMap.GetTexture();
			m_depthAttach.resize( DoGetMaxPasses() );
			int i = 0;

			for ( auto & l_attach : m_depthAttach )
			{
				l_attach = m_frameBuffer->CreateAttachment( l_texture );
			}
		
			m_frameBuffer->Bind( FrameBufferMode::eConfig );
			m_frameBuffer->Attach( AttachmentPoint::eDepth, 0, m_depthAttach[0], l_texture->GetType(), 0u );
			bool l_return = m_frameBuffer->IsComplete();
			m_frameBuffer->Unbind();

			for ( auto & l_it : m_shadowMaps )
			{
				l_it.second->Initialise( p_size );
			}
		}

		return l_return;
	}

	void ShadowMap::Cleanup()
	{
		for ( auto & l_it : m_shadowMaps )
		{
			l_it.second->Cleanup();
		}

		if ( m_frameBuffer )
		{
			m_frameBuffer->Bind( FrameBufferMode::eConfig );
			m_frameBuffer->DetachAll();
			m_frameBuffer->Unbind();

			m_shadowMap.Cleanup();
			
			for ( auto & l_attach : m_depthAttach )
			{
				l_attach.reset();
			}
		
			DoCleanup();

			m_frameBuffer->Cleanup();
			m_frameBuffer->Destroy();
			m_frameBuffer.reset();
		}

		for ( auto l_buffer : m_geometryBuffers )
		{
			l_buffer->Cleanup();
		}

		m_geometryBuffers.clear();
	}

	void ShadowMap::Update( Camera const & p_camera
		, RenderQueueArray & p_queues )
	{
		std::map< double, ShadowMapPassSPtr > l_sorted;

		for ( auto & l_it : m_shadowMaps )
		{
			l_sorted.emplace( point::distance_squared( p_camera.GetParent()->GetDerivedPosition()
					, l_it.first->GetParent()->GetDerivedPosition() )
				, l_it.second );
		}

		auto l_it = l_sorted.begin();

		for ( int32_t i = 0; i < DoGetMaxPasses() && l_it != l_sorted.end(); ++i, ++l_it )
		{
			l_it->second->Update( p_queues, i );
		}
	}

	void ShadowMap::Render()
	{
		m_frameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );

		for ( auto & l_it : m_shadowMaps )
		{
			m_frameBuffer->Clear();
			l_it.second->Render();
		}

		m_frameBuffer->Unbind();
	}

	void ShadowMap::AddLight( Light & p_light )
	{
		m_shadowMaps.emplace( &p_light, DoCreatePass( p_light ) );
	}

	void ShadowMap::UpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags )const
	{
		RemFlag( p_programFlags, ProgramFlag::eLighting );
		RemFlag( p_programFlags, ProgramFlag::eAlphaBlending );
		RemFlag( p_textureFlags, TextureChannel( uint16_t( TextureChannel::eAll ) & ~uint16_t( TextureChannel::eOpacity ) ) );
		DoUpdateFlags( p_textureFlags, p_programFlags );
	}

	String ShadowMap::GetVertexShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags
		, bool p_invertNormals )const
	{
		return DoGetVertexShaderSource( p_textureFlags, p_programFlags, p_sceneFlags, p_invertNormals );
	}

	String ShadowMap::GetGeometryShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags )const
	{
		return DoGetGeometryShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );
	}

	String ShadowMap::GetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags )const
	{
		return DoGetPixelShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );
	}

	String ShadowMap::DoGetVertexShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags
		, bool p_invertNormals )const
	{
		using namespace GLSL;
		auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();
		// Vertex inputs
		auto position = l_writer.GetAttribute< Vec4 >( ShaderProgram::Position );
		auto normal = l_writer.GetAttribute< Vec3 >( ShaderProgram::Normal );
		auto tangent = l_writer.GetAttribute< Vec3 >( ShaderProgram::Tangent );
		auto bitangent = l_writer.GetAttribute< Vec3 >( ShaderProgram::Bitangent );
		auto texture = l_writer.GetAttribute< Vec3 >( ShaderProgram::Texture );
		auto bone_ids0 = l_writer.GetAttribute< IVec4 >( ShaderProgram::BoneIds0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = l_writer.GetAttribute< IVec4 >( ShaderProgram::BoneIds1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Weights0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Weights1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto transform = l_writer.GetAttribute< Mat4 >( ShaderProgram::Transform, CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Position2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = l_writer.GetAttribute< Vec3 >( ShaderProgram::Normal2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = l_writer.GetAttribute< Vec3 >( ShaderProgram::Tangent2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto bitangent2 = l_writer.GetAttribute< Vec3 >( ShaderProgram::Bitangent2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = l_writer.GetAttribute< Vec3 >( ShaderProgram::Texture2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( l_writer.GetBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( l_writer );
		UBO_MODEL_MATRIX( l_writer );
		UBO_SKINNING( l_writer, p_programFlags );
		UBO_MORPHING( l_writer, p_programFlags );

		// Outputs
		auto vtx_worldSpacePosition = l_writer.GetOutput< Vec3 >( cuT( "vtx_worldSpacePosition" ) );
		auto vtx_normal = l_writer.GetOutput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = l_writer.GetOutput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = l_writer.GetOutput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = l_writer.GetOutput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = l_writer.GetOutput< Int >( cuT( "vtx_instance" ) );
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > l_main = [&]()
		{
			auto l_v4Vertex = l_writer.GetLocale( cuT( "l_v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto l_v4Normal = l_writer.GetLocale( cuT( "l_v4Normal" ), vec4( normal, 0.0 ) );
			auto l_v4Tangent = l_writer.GetLocale( cuT( "l_v4Tangent" ), vec4( tangent, 0.0 ) );
			auto l_v4Bitangent = l_writer.GetLocale( cuT( "l_v4Bitangent" ), vec4( bitangent, 0.0 ) );
			auto l_v3Texture = l_writer.GetLocale( cuT( "l_v3Texture" ), texture );
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
				l_v4Normal = vec4( l_v4Normal.xyz() * l_time + normal2.xyz() * c3d_fTime, 1.0 );
				l_v4Tangent = vec4( l_v4Tangent.xyz() * l_time + tangent2.xyz() * c3d_fTime, 1.0 );
				l_v4Bitangent = vec4( l_v4Bitangent.xyz() * l_time + bitangent2.xyz() * c3d_fTime, 1.0 );
				l_v3Texture = l_v3Texture * l_writer.Paren( 1.0_f - c3d_fTime ) + texture2 * c3d_fTime;
			}

			vtx_texture = l_v3Texture;
			l_v4Vertex = l_mtxModel * l_v4Vertex;
			vtx_worldSpacePosition = l_v4Vertex.xyz();
			l_v4Vertex = c3d_mtxView * l_v4Vertex;

			if ( p_invertNormals )
			{
				vtx_normal = normalize( l_writer.Paren( l_mtxModel * -l_v4Normal ).xyz() );
			}
			else
			{
				vtx_normal = normalize( l_writer.Paren( l_mtxModel * l_v4Normal ).xyz() );
			}

			vtx_tangent = normalize( l_writer.Paren( l_mtxModel * l_v4Tangent ).xyz() );
			vtx_bitangent = normalize( l_writer.Paren( l_mtxModel * l_v4Bitangent ).xyz() );
			vtx_instance = gl_InstanceID;
			gl_Position = c3d_mtxProjection * l_v4Vertex;
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

	String ShadowMap::DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags )const
	{
		return String{};
	}
}
