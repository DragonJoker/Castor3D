#include "ShadowMapPassPoint.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureImage.hpp"

#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		static String const ShadowMapUbo = cuT( "ShadowMap" );
		static String const WorldLightPosition = cuT( "c3d_v3WorldLightPosition" );
		static String const FarPlane = cuT( "c3d_fFarPlane" );
		static String const ShadowMatrix = cuT( "c3d_mtxShadowMatrix" );

		void DoUpdateShadowMatrices( Point3r const & p_position
			, std::array< Matrix4x4r, size_t( CubeMapFace::eCount ) > & p_matrices )
		{
			p_matrices =
			{
				{
					matrix::look_at( p_position, p_position + Point3r{ +1, +0, +0 }, Point3r{ +0, -1, +0 } ),// Positive X
					matrix::look_at( p_position, p_position + Point3r{ -1, +0, +0 }, Point3r{ +0, -1, +0 } ),// Negative X
					matrix::look_at( p_position, p_position + Point3r{ +0, +1, +0 }, Point3r{ +0, +0, +1 } ),// Positive Y
					matrix::look_at( p_position, p_position + Point3r{ +0, -1, +0 }, Point3r{ +0, +0, -1 } ),// Negative Y
					matrix::look_at( p_position, p_position + Point3r{ +0, +0, +1 }, Point3r{ +0, -1, +0 } ),// Positive Z
					matrix::look_at( p_position, p_position + Point3r{ +0, +0, -1 }, Point3r{ +0, -1, +0 } ),// Negative Z
				}
			};
		}
	}

	ShadowMapPassPoint::ShadowMapPassPoint( Engine & p_engine
		, Light & p_light
		, ShadowMap const & p_shadowMap )
		: ShadowMapPass{ p_engine, p_light, p_shadowMap }
		, m_shadowConfig{ ShadowMapUbo, *p_engine.GetRenderSystem() }
		, m_viewport{ p_engine }
	{
		m_shadowConfig.CreateUniform< UniformType::eVec3f >( WorldLightPosition );
		m_shadowConfig.CreateUniform< UniformType::eFloat >( FarPlane );
		m_renderQueue.Initialise( *p_light.GetScene() );
	}

	ShadowMapPassPoint::~ShadowMapPassPoint()
	{
	}

	void ShadowMapPassPoint::DoRenderNodes( SceneRenderNodes & p_nodes )
	{
		RenderPass::DoRender( p_nodes.m_instantiatedStaticNodes.m_backCulled );
		RenderPass::DoRender( p_nodes.m_staticNodes.m_backCulled );
		RenderPass::DoRender( p_nodes.m_skinnedNodes.m_backCulled );
		RenderPass::DoRender( p_nodes.m_instantiatedSkinnedNodes.m_backCulled );
		RenderPass::DoRender( p_nodes.m_morphingNodes.m_backCulled );
		RenderPass::DoRender( p_nodes.m_billboardNodes.m_backCulled );
	}

	bool ShadowMapPassPoint::DoInitialise( Size const & p_size )
	{
		real const l_aspect = real( p_size.width() ) / p_size.height();
		real const l_near = 1.0_r;
		real const l_far = 2000.0_r;
		matrix::perspective( m_projection, Angle::from_degrees( 90.0_r ), l_aspect, l_near, l_far );

		m_viewport.Resize( p_size );
		m_viewport.Initialise();
		return true;
	}

	void ShadowMapPassPoint::DoCleanup()
	{
		m_viewport.Cleanup();
		m_matrixUbo.GetUbo().Cleanup();
		m_shadowConfig.Cleanup();
		m_onNodeChanged.disconnect();
	}

	void ShadowMapPassPoint::DoUpdate( RenderQueueArray & p_queues )
	{
		auto l_position = m_light.GetParent()->GetDerivedPosition();
		m_light.Update( l_position
			, m_viewport
			, m_index );
		p_queues.push_back( m_renderQueue );
		DoUpdateShadowMatrices( l_position, m_matrices );
		m_shadowConfig.GetUniform< UniformType::eVec3f >( WorldLightPosition )->SetValue( l_position );
		m_shadowConfig.GetUniform< UniformType::eFloat >( FarPlane )->SetValue( 4000.0f );
	}

	void ShadowMapPassPoint::DoRender( uint32_t p_face )
	{
		if ( m_initialised )
		{
			m_shadowConfig.Update();
			m_shadowConfig.BindTo( 8u );
			m_viewport.Apply();
			m_matrixUbo.Update( m_matrices[p_face], m_projection );
			DoRenderNodes( m_renderQueue.GetRenderNodes() );
		}
	}

	void ShadowMapPassPoint::DoPrepareBackPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
		if ( m_backPipelines.find( p_flags ) == m_backPipelines.end() )
		{
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eNone );
			DepthStencilState l_dsState;
			l_dsState.SetDepthTest( true );
			auto & l_pipeline = *m_backPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState )
					, std::move( l_rsState )
					, BlendState{}
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;

			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &l_pipeline, p_flags]()
			{
				l_pipeline.AddUniformBuffer( m_matrixUbo.GetUbo() );
				l_pipeline.AddUniformBuffer( m_modelMatrixUbo.GetUbo() );
				l_pipeline.AddUniformBuffer( m_shadowConfig );

				if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
				{
					l_pipeline.AddUniformBuffer( m_billboardUbo.GetUbo() );
				}

				if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning )
					&& !CheckFlag( p_flags.m_programFlags, ProgramFlag::eInstantiation ) )
				{
					l_pipeline.AddUniformBuffer( m_skinningUbo.GetUbo() );
				}

				if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
				{
					l_pipeline.AddUniformBuffer( m_morphingUbo.GetUbo() );
				}

				m_initialised = true;
			} ) );
		}
	}
}
