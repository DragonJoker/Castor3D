#include "ShadowMapPassPoint.hpp"

#include "Engine.hpp"
#include "Cache/SamplerCache.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Light/Light.hpp"
#include "Shader/UniformBuffer.hpp"
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
		static String const ShadowMatrices = cuT( "c3d_mtxShadowMatrices" );

		void DoUpdateShadowMatrices( Matrix4x4r const & p_projection
			, Point3r const & p_position
			, UniformBuffer & p_shadowConfig )
		{
			std::array< Matrix4x4r, size_t( CubeMapFace::eCount ) > const l_views
			{
				{
					p_projection * matrix::look_at( p_position, p_position + Point3r{ +1, +0, +0 }, Point3r{ +0, -1, +0 } ),// Positive X
					p_projection * matrix::look_at( p_position, p_position + Point3r{ -1, +0, +0 }, Point3r{ +0, -1, +0 } ),// Negative X
					p_projection * matrix::look_at( p_position, p_position + Point3r{ +0, +1, +0 }, Point3r{ +0, +0, +1 } ),// Positive Y
					p_projection * matrix::look_at( p_position, p_position + Point3r{ +0, -1, +0 }, Point3r{ +0, +0, -1 } ),// Negative Y
					p_projection * matrix::look_at( p_position, p_position + Point3r{ +0, +0, +1 }, Point3r{ +0, -1, +0 } ),// Positive Z
					p_projection * matrix::look_at( p_position, p_position + Point3r{ +0, +0, -1 }, Point3r{ +0, -1, +0 } ),// Negative Z
				}
			};

			p_shadowConfig.GetUniform< UniformType::eVec3f >( WorldLightPosition )->SetValue( p_position );
			p_shadowConfig.GetUniform< UniformType::eFloat >( FarPlane )->SetValue( 4000.0f );
			auto l_shadowMatrices = p_shadowConfig.GetUniform< UniformType::eMat4x4f >( ShadowMatrices );
			uint32_t l_index{ 0 };

			for ( auto & l_view : l_views )
			{
				l_shadowMatrices->SetValue( l_view, l_index++ );
			}
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
		m_shadowConfig.CreateUniform< UniformType::eMat4x4f >( ShadowMatrices, 6u );
		m_renderQueue.Initialise( *p_light.GetScene() );
	}

	ShadowMapPassPoint::~ShadowMapPassPoint()
	{
	}

	void ShadowMapPassPoint::DoRenderNodes( SceneRenderNodes & p_nodes )
	{
		auto l_depthMaps = DepthMapArray{};
		DoRenderInstancedSubmeshes( p_nodes.m_instancedNodes.m_backCulled );
		DoRenderStaticSubmeshes( p_nodes.m_staticNodes.m_backCulled );
		DoRenderSkinningSubmeshes( p_nodes.m_skinningNodes.m_backCulled );
		DoRenderMorphingSubmeshes( p_nodes.m_morphingNodes.m_backCulled );
		DoRenderBillboards( p_nodes.m_billboardNodes.m_backCulled );
	}

	bool ShadowMapPassPoint::DoInitialise( Size const & p_size )
	{
		real const l_aspect = real( p_size.width() ) / p_size.height();
		real const l_near = 1.0_r;
		real const l_far = 2000.0_r;
		matrix::perspective( m_projection, Angle::from_degrees( 90.0_r ), l_aspect, l_near, l_far );

		constexpr float l_component = std::numeric_limits< float >::max();

		m_viewport.Resize( p_size );
		m_viewport.Initialise();
		return true;
	}

	void ShadowMapPassPoint::DoCleanup()
	{
		m_viewport.Cleanup();
		m_shadowConfig.Cleanup();

		auto l_node = m_light.GetParent();
		m_onNodeChanged.disconnect();
	}

	void ShadowMapPassPoint::DoUpdate( RenderQueueArray & p_queues )
	{
		auto l_position = m_light.GetParent()->GetDerivedPosition();
		m_light.Update( l_position
			, m_viewport
			, m_index );
		p_queues.push_back( m_renderQueue );
		DoUpdateShadowMatrices( m_projection, l_position, m_shadowConfig );
	}

	void ShadowMapPassPoint::DoRender()
	{
		if ( m_initialised )
		{
			m_viewport.Apply();
			m_shadowConfig.Update();
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
				l_pipeline.AddUniformBuffer( m_matrixUbo );
				l_pipeline.AddUniformBuffer( m_modelMatrixUbo );
				l_pipeline.AddUniformBuffer( m_sceneUbo );
				l_pipeline.AddUniformBuffer( m_shadowConfig );

				if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
				{
					l_pipeline.AddUniformBuffer( m_billboardUbo );
				}

				if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning ) )
				{
					l_pipeline.AddUniformBuffer( m_skinningUbo );
				}

				if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
				{
					l_pipeline.AddUniformBuffer( m_morphingUbo );
				}

				m_initialised = true;
			} ) );
		}
	}
}
