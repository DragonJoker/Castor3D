#include "ShadowMapPassPoint.hpp"

#include "Engine.hpp"
#include "SamplerCache.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Light/Light.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/MatrixFrameVariable.hpp"
#include "Shader/PointFrameVariable.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureImage.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		static String const ShadowMapUbo = cuT( "ShadowMap" );
		static String const WorldLightPosition = cuT( "c3d_v3WorldLightPosition" );
		static String const FarPlane = cuT( "c3d_fFarPlane" );
		static String const ShadowMatricesUbo = cuT( "ShadowMatrices" );
		static String const ShadowMatrices = cuT( "c3d_mtxShadowMatrices" );

		void DoUpdateShadowMatrices( Matrix4x4r const & p_projection, Point3r const & p_position, ShaderProgram & p_program )
		{
			std::array< Matrix4x4r, size_t( CubeMapFace::eCount ) > const l_views
			{
				{
					p_projection * matrix::look_at( p_position, p_position + Point3r{ +1, 0, 0 }, Point3r{ 0, -1, 0 } ),
					p_projection * matrix::look_at( p_position, p_position + Point3r{ -1, 0, 0 }, Point3r{ 0, -1, 0 } ),
					p_projection * matrix::look_at( p_position, p_position + Point3r{ 0, +1, 0 }, Point3r{ 0, 0, +1 } ),
					p_projection * matrix::look_at( p_position, p_position + Point3r{ 0, -1, 0 }, Point3r{ 0, 0, -1 } ),
					p_projection * matrix::look_at( p_position, p_position + Point3r{ 0, 0, +1 }, Point3r{ 0, -1, 0 } ),
					p_projection * matrix::look_at( p_position, p_position + Point3r{ 0, 0, -1 }, Point3r{ 0, -1, 0 } ),
				}
			};

			auto l_shadowMapUbo = p_program.FindFrameVariableBuffer( ShadowMapUbo );
			Point3fFrameVariableSPtr l_worldLightPosition;
			OneFloatFrameVariableSPtr l_farPlane;
			l_shadowMapUbo->GetVariable( WorldLightPosition, l_worldLightPosition )->SetValue( p_position );
			l_shadowMapUbo->GetVariable( FarPlane, l_farPlane )->SetValue( 2000.0_r );

			auto l_shadowMatricesUbo = p_program.FindFrameVariableBuffer( ShadowMatricesUbo );
			Matrix4x4fFrameVariableSPtr l_shadowMatrices;
			l_shadowMatricesUbo->GetVariable( ShadowMatrices, l_shadowMatrices );
			uint32_t l_index{ 0 };

			for ( auto & l_view : l_views )
			{
				l_shadowMatrices->SetValue( l_view, l_index++ );
			}
		}

		template< typename MapType, typename FuncType >
		void DoTraverseNodes( MapType & p_nodes
							  , FuncType p_function )
		{
			for ( auto l_itPipelines : p_nodes )
			{
				l_itPipelines.first->Apply();

				for ( auto l_itPass : l_itPipelines.second )
				{
					for ( auto l_itSubmeshes : l_itPass.second )
					{
						p_function( *l_itPipelines.first, *l_itPass.first, *l_itSubmeshes.first, l_itSubmeshes.second );
					}
				}
			}
		}

		template< typename MapType >
		void DoRenderNonInstanced( Scene & p_scene
								   , MapType & p_nodes )
		{
			auto l_depthMaps = DepthMapArray{};

			for ( auto l_itPipelines : p_nodes )
			{
				l_itPipelines.first->Apply();

				for ( auto & l_renderNode : l_itPipelines.second )
				{
					if ( l_renderNode.m_data.IsInitialised() )
					{
						l_renderNode.Render( l_depthMaps );
					}
				}
			}
		}
	}

	ShadowMapPassPoint::ShadowMapPassPoint( Engine & p_engine, Scene & p_scene, Light & p_light, TextureUnit & p_shadowMap, uint32_t p_index )
		: ShadowMapPass{ p_engine, p_scene, p_light, p_shadowMap, p_index }
	{
		m_renderQueue.Initialise( m_scene );
	}

	ShadowMapPassPoint::~ShadowMapPassPoint()
	{
	}

	ShadowMapPassSPtr ShadowMapPassPoint::Create( Engine & p_engine, Scene & p_scene, Light & p_light, TextureUnit & p_shadowMap, uint32_t p_index )
	{
		return std::make_shared< ShadowMapPassPoint >( p_engine, p_scene, p_light, p_shadowMap, p_index );
	}

	void ShadowMapPassPoint::DoRenderOpaqueNodes( SceneRenderNodes & p_nodes )
	{
		DoRenderInstancedSubmeshesInstanced( p_nodes.m_scene, p_nodes.m_instancedGeometries.m_opaqueRenderNodesBack );
		DoRenderStaticSubmeshesNonInstanced( p_nodes.m_scene, p_nodes.m_staticGeometries.m_opaqueRenderNodesBack );
		DoRenderAnimatedSubmeshesNonInstanced( p_nodes.m_scene, p_nodes.m_animatedGeometries.m_opaqueRenderNodesBack );
		DoRenderBillboards( p_nodes.m_scene, p_nodes.m_billboards.m_opaqueRenderNodesBack );
	}

	void ShadowMapPassPoint::DoRenderTransparentNodes( SceneRenderNodes & p_nodes )
	{
		DoRenderInstancedSubmeshesInstanced( p_nodes.m_scene, p_nodes.m_instancedGeometries.m_transparentRenderNodesBack );
		DoRenderStaticSubmeshesNonInstanced( p_nodes.m_scene, p_nodes.m_staticGeometries.m_transparentRenderNodesBack );
		DoRenderAnimatedSubmeshesNonInstanced( p_nodes.m_scene, p_nodes.m_animatedGeometries.m_transparentRenderNodesBack );
		DoRenderBillboards( p_nodes.m_scene, p_nodes.m_billboards.m_transparentRenderNodesBack );
	}

	void ShadowMapPassPoint::DoRenderInstancedSubmeshesInstanced( Scene & p_scene, SubmeshStaticRenderNodesByPipelineMap & p_nodes )
	{
		DoTraverseNodes( p_nodes, [this]( Pipeline & p_pipeline, Pass & p_pass, Submesh & p_submesh, StaticGeometryRenderNodeArray & p_renderNodes )
		{
			if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
			{
				uint32_t l_count = uint32_t( p_renderNodes.size() );
				uint8_t * l_buffer = p_submesh.GetMatrixBuffer().data();
				const uint32_t l_stride = 16 * sizeof( real );

				for ( auto const & l_renderNode : p_renderNodes )
				{
					std::memcpy( l_buffer, l_renderNode.m_sceneNode.GetDerivedTransformationMatrix().const_ptr(), l_stride );
					l_buffer += l_stride;
				}

				auto l_depthMaps = DepthMapArray{};
				p_renderNodes[0].BindPass( l_depthMaps, MASK_MTXMODE_MODEL );
				p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
				p_renderNodes[0].UnbindPass( l_depthMaps );
			}
		} );
	}

	void ShadowMapPassPoint::DoRenderStaticSubmeshesNonInstanced( Scene & p_scene, StaticGeometryRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced( p_scene, p_nodes );
	}

	void ShadowMapPassPoint::DoRenderAnimatedSubmeshesNonInstanced( Scene & p_scene, AnimatedGeometryRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced( p_scene, p_nodes );
	}

	void ShadowMapPassPoint::DoRenderBillboards( Scene & p_scene, BillboardRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced( p_scene, p_nodes );
	}

	bool ShadowMapPassPoint::DoInitialise( Size const & p_size )
	{
		auto l_texture = m_shadowMap.GetTexture();
		m_depthAttach = m_frameBuffer->CreateAttachment( l_texture );
		bool l_return{ false };

		if ( m_frameBuffer->Bind( FrameBufferMode::eConfig ) )
		{
			m_frameBuffer->Attach( AttachmentPoint::eDepth, 0, m_depthAttach, l_texture->GetType(), m_index );
			l_return = m_frameBuffer->IsComplete();
			m_frameBuffer->Unbind();
		}

		real const l_aspect = real( p_size.width() ) / p_size.height();
		real const l_near = 1.0_r;
		real const l_far = 2000.0_r;
		matrix::perspective( m_projection, Angle::from_degrees( 90.0_r ), l_aspect, l_near, l_far );

		constexpr float l_component = std::numeric_limits< float >::max();
		m_frameBuffer->SetClearColour( l_component, l_component, l_component, l_component );
		return true;
	}

	void ShadowMapPassPoint::DoCleanup()
	{
		auto l_node = m_light.GetParent();

		if ( l_node )
		{
			l_node->UnregisterObject( m_onNodeChanged );
		}

		m_onNodeChanged = 0;
		m_depthAttach.reset();
		m_shadowMap.Cleanup();
	}

	void ShadowMapPassPoint::DoUpdate()
	{
		auto l_position = m_light.GetParent()->GetDerivedPosition();
		m_light.Update( l_position );
		m_renderQueue.Update();
		//l_position[2] = -l_position[2];

		for ( auto & l_it : m_frontOpaquePipelines )
		{
			DoUpdateShadowMatrices( m_projection, l_position, l_it.second->GetProgram() );
		}

		for ( auto & l_it : m_backOpaquePipelines )
		{
			DoUpdateShadowMatrices( m_projection, l_position, l_it.second->GetProgram() );
		}

		for ( auto & l_it : m_frontTransparentPipelines )
		{
			DoUpdateShadowMatrices( m_projection, l_position, l_it.second->GetProgram() );
		}

		for ( auto & l_it : m_backTransparentPipelines )
		{
			DoUpdateShadowMatrices( m_projection, l_position, l_it.second->GetProgram() );
		}
	}

	void ShadowMapPassPoint::DoRender()
	{
		if ( m_frameBuffer->Bind( FrameBufferMode::eManual, FrameBufferTarget::eDraw ) )
		{
			m_frameBuffer->Clear();
			auto & l_nodes = m_renderQueue.GetRenderNodes();
			DoRenderOpaqueNodes( l_nodes );
			DoRenderTransparentNodes( l_nodes );
			m_frameBuffer->Unbind();
		}
	}

	void ShadowMapPassPoint::DoUpdateProgram( ShaderProgram & p_program )
	{
		auto l_ubo = p_program.FindFrameVariableBuffer( ShadowMapUbo );

		if ( !l_ubo )
		{
			auto & l_shadowMapUbo = p_program.CreateFrameVariableBuffer( ShadowMapUbo, MASK_SHADER_TYPE_PIXEL );
			l_shadowMapUbo.CreateVariable< Point3fFrameVariable >( WorldLightPosition );
			l_shadowMapUbo.CreateVariable< OneFloatFrameVariable >( FarPlane );

			auto & l_shadowMatricesUbo = p_program.CreateFrameVariableBuffer( ShadowMatricesUbo, MASK_SHADER_TYPE_GEOMETRY );
			l_shadowMatricesUbo.CreateVariable< Matrix4x4fFrameVariable >( ShadowMatrices, 6 );
		}
	}

	String ShadowMapPassPoint::DoGetVertexShaderSource( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags, bool p_invertNormals )const
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
		UBO_ANIMATION( l_writer, p_programFlags );

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
				l_mtxBoneTransform = c3d_mtxBones[bone_ids0[Int( 0 )]] * weights0[Int( 0 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[Int( 1 )]] * weights0[Int( 1 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[Int( 2 )]] * weights0[Int( 2 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[Int( 3 )]] * weights0[Int( 3 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[Int( 0 )]] * weights1[Int( 0 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[Int( 1 )]] * weights1[Int( 1 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[Int( 2 )]] * weights1[Int( 2 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[Int( 3 )]] * weights1[Int( 3 )];
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
				auto l_time = l_writer.GetLocale( cuT( "l_time" ), Float( 1.0 ) - c3d_fTime );
				l_v4Vertex = vec4( l_v4Vertex.xyz() * l_time + position2.xyz() * c3d_fTime, 1.0 );
			}

			l_v4Vertex = l_mtxModel * l_v4Vertex;
			vtx_worldSpacePosition = l_v4Vertex.xyz();
			gl_Position = l_v4Vertex;
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

	String ShadowMapPassPoint::DoGetGeometryShaderSource( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )const
	{
		using namespace GLSL;
		auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Geometry layout
		l_writer.InputGeometryLayout( cuT( "triangles" ) );
		l_writer.OutputGeometryLayout( cuT( "triangle_strip" ), 18 );

		// Geometry inputs
		Ubo l_shadowMatricesUbo{ l_writer, ShadowMatricesUbo };
		auto c3d_mtxShadowMatrices = l_shadowMatricesUbo.GetUniform< Mat4 >( ShadowMatrices, 6u );
		l_shadowMatricesUbo.End();

		auto gl_in = l_writer.GetBuiltin< gl_PerVertex >( cuT( "gl_in" ), 8u );
		auto gl_Layer = l_writer.GetBuiltin< Int >( cuT( "gl_Layer" ) );

		//Geometry outputs
		auto geo_position = l_writer.GetOutput< Vec4 >( cuT( "geo_position" ) );
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > l_main = [&]()
		{
			for ( int face = 0; face < 6; ++face )
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

	String ShadowMapPassPoint::DoGetOpaquePixelShaderSource( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_renderSystem.CreateGlslWriter();

		// Fragment Intputs
		Ubo l_shadowMap{ l_writer, ShadowMapUbo };
		auto c3d_v3WordLightPosition( l_shadowMap.GetUniform< Vec3 >( WorldLightPosition ) );
		auto c3d_fFarPlane( l_shadowMap.GetUniform< Float >( FarPlane ) );
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
