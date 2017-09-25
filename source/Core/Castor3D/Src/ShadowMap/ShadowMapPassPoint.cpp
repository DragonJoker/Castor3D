#include "ShadowMapPassPoint.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureImage.hpp"

#include <Graphics/Image.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static String const ShadowMapUbo = cuT( "ShadowMap" );
		static String const WorldLightPosition = cuT( "c3d_worldLightPosition" );
		static String const FarPlane = cuT( "c3d_farPlane" );

		void doUpdateShadowMatrices( Point3r const & p_position
			, std::array< Matrix4x4r, size_t( CubeMapFace::eCount ) > & p_matrices )
		{
			p_matrices =
			{
				{
					matrix::lookAt( p_position, p_position + Point3r{ +1, +0, +0 }, Point3r{ +0, -1, +0 } ),// Positive X
					matrix::lookAt( p_position, p_position + Point3r{ -1, +0, +0 }, Point3r{ +0, -1, +0 } ),// Negative X
					matrix::lookAt( p_position, p_position + Point3r{ +0, +1, +0 }, Point3r{ +0, +0, +1 } ),// Positive Y
					matrix::lookAt( p_position, p_position + Point3r{ +0, -1, +0 }, Point3r{ +0, +0, -1 } ),// Negative Y
					matrix::lookAt( p_position, p_position + Point3r{ +0, +0, +1 }, Point3r{ +0, -1, +0 } ),// Positive Z
					matrix::lookAt( p_position, p_position + Point3r{ +0, +0, -1 }, Point3r{ +0, -1, +0 } ),// Negative Z
				}
			};
		}
	}

	ShadowMapPassPoint::ShadowMapPassPoint( Engine & engine
		, Scene & scene
		, ShadowMap const & shadowMap )
		: ShadowMapPass{ engine, scene, shadowMap }
		, m_shadowConfig{ ShadowMapUbo
			, *engine.getRenderSystem()
			, 8u }
		, m_viewport{ engine }
		, m_worldLightPosition{ *m_shadowConfig.createUniform< UniformType::eVec3f >( WorldLightPosition ) }
		, m_farPlane{ *m_shadowConfig.createUniform< UniformType::eFloat >( FarPlane ) }
	{
		m_renderQueue.initialise( scene );
	}

	ShadowMapPassPoint::~ShadowMapPassPoint()
	{
	}

	void ShadowMapPassPoint::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		auto position = light.getParent()->getDerivedPosition();
		light.update( position
			, m_viewport
			, index );
		doUpdateShadowMatrices( position, m_matrices );
		m_worldLightPosition.setValue( position );
		m_farPlane.setValue( m_viewport.getFar() );
		doUpdate( queues );
	}

	void ShadowMapPassPoint::render( uint32_t index )
	{
		if ( m_initialised )
		{
			m_shadowConfig.update();
			m_shadowConfig.bindTo( 8u );
			m_viewport.apply();
			m_matrixUbo.update( m_matrices[index], m_projection );
			doRenderNodes( m_renderQueue.getRenderNodes() );
		}
	}

	void ShadowMapPassPoint::doRenderNodes( SceneRenderNodes & p_nodes )
	{
		RenderPass::doRender( p_nodes.m_instantiatedStaticNodes.m_backCulled );
		RenderPass::doRender( p_nodes.m_staticNodes.m_backCulled );
		RenderPass::doRender( p_nodes.m_skinnedNodes.m_backCulled );
		RenderPass::doRender( p_nodes.m_instantiatedSkinnedNodes.m_backCulled );
		RenderPass::doRender( p_nodes.m_morphingNodes.m_backCulled );
		RenderPass::doRender( p_nodes.m_billboardNodes.m_backCulled );
	}

	bool ShadowMapPassPoint::doInitialise( Size const & p_size )
	{
		real const aspect = real( p_size.getWidth() ) / p_size.getHeight();
		real const near = 1.0_r;
		real const far = 2000.0_r;
		matrix::perspective( m_projection, Angle::fromDegrees( 90.0_r ), aspect, near, far );

		m_viewport.resize( p_size );
		m_viewport.initialise();
		return true;
	}

	void ShadowMapPassPoint::doCleanup()
	{
		m_viewport.cleanup();
		m_matrixUbo.getUbo().cleanup();
		m_shadowConfig.cleanup();
		m_onNodeChanged.disconnect();
	}
	void ShadowMapPassPoint::doUpdate( RenderQueueArray & p_queues )
	{
		p_queues.push_back( m_renderQueue );
	}

	void ShadowMapPassPoint::doPreparePipeline( ShaderProgram & program
		, PipelineFlags const & flags )
	{
		if ( m_backPipelines.find( flags ) == m_backPipelines.end() )
		{
			RasteriserState rsState;
			rsState.setCulledFaces( Culling::eNone );
			DepthStencilState dsState;
			dsState.setDepthTest( true );
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
					pipeline.addUniformBuffer( m_modelUbo.getUbo() );
					pipeline.addUniformBuffer( m_modelMatrixUbo.getUbo() );
					pipeline.addUniformBuffer( m_shadowConfig );

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

					m_initialised = true;
				} ) );
		}
	}
}
