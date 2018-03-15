#include "ShadowMapPassPoint.hpp"

#include "Mesh/Submesh.hpp"
#include "Buffer/VertexBuffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Light/PointLight.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureView.hpp"

#include <Graphics/Image.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		void doUpdateShadowMatrices( Point3r const & position
			, std::array< Matrix4x4r, size_t( CubeMapFace::eCount ) > & matrices )
		{
			matrices =
			{
				{
					matrix::lookAt( position, position + Point3r{ +1, +0, +0 }, Point3r{ +0, -1, +0 } ),// Positive X
					matrix::lookAt( position, position + Point3r{ -1, +0, +0 }, Point3r{ +0, -1, +0 } ),// Negative X
					matrix::lookAt( position, position + Point3r{ +0, +1, +0 }, Point3r{ +0, +0, +1 } ),// Positive Y
					matrix::lookAt( position, position + Point3r{ +0, -1, +0 }, Point3r{ +0, +0, -1 } ),// Negative Y
					matrix::lookAt( position, position + Point3r{ +0, +0, +1 }, Point3r{ +0, -1, +0 } ),// Positive Z
					matrix::lookAt( position, position + Point3r{ +0, +0, -1 }, Point3r{ +0, -1, +0 } ),// Negative Z
				}
			};
		}
	}

	String const ShadowMapPassPoint::ShadowMapUbo = cuT( "ShadowMap" );
	String const ShadowMapPassPoint::WorldLightPosition = cuT( "c3d_worldLightPosition" );
	String const ShadowMapPassPoint::FarPlane = cuT( "c3d_farPlane" );

	ShadowMapPassPoint::ShadowMapPassPoint( Engine & engine
		, Scene & scene
		, ShadowMap const & shadowMap )
		: ShadowMapPass{ engine, scene, shadowMap }
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
		light.updateShadow( position
			, m_viewport
			, index );
		doUpdateShadowMatrices( position, m_matrices );
		auto & config = m_shadowConfig->getData();
		config.worldLightPosition = position;
		config.farPlane = m_viewport.getFar();
		doUpdate( queues );
	}

	void ShadowMapPassPoint::render( uint32_t index )
	{
		if ( m_initialised )
		{
			m_shadowConfig->upload();
			m_matrixUbo.update( m_matrices[index], m_projection );
			doUpdateNodes( m_renderQueue.getRenderNodes() );
		}
	}

	void ShadowMapPassPoint::doUpdateNodes( SceneRenderNodes & nodes )
	{
		RenderPass::doUpdate( nodes.instancedStaticNodes.backCulled );
		RenderPass::doUpdate( nodes.staticNodes.backCulled );
		RenderPass::doUpdate( nodes.skinnedNodes.backCulled );
		RenderPass::doUpdate( nodes.instancedSkinnedNodes.backCulled );
		RenderPass::doUpdate( nodes.morphingNodes.backCulled );
		RenderPass::doUpdate( nodes.billboardNodes.backCulled );
	}

	bool ShadowMapPassPoint::doInitialise( Size const & size )
	{
		real const aspect = real( size.getWidth() ) / size.getHeight();
		real const near = 1.0_r;
		real const far = 2000.0_r;
		matrix::perspective( m_projection, 90.0_degrees, aspect, near, far );

		m_shadowConfig = renderer::makeUniformBuffer< Configuration >( *getEngine()->getRenderSystem()->getCurrentDevice()
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible | renderer::MemoryPropertyFlag::eHostCoherent );

		m_viewport.resize( size );
		return true;
	}

	void ShadowMapPassPoint::doCleanup()
	{
		m_matrixUbo.cleanup();
		m_shadowConfig.reset();
		m_onNodeChanged.disconnect();
	}

	void ShadowMapPassPoint::doFillDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node )
	{
		node.descriptorSet->createBinding( layout.getBinding( ShadowMapPassPoint::UboBindingPoint )
			, *m_shadowConfig );
	}

	void ShadowMapPassPoint::doFillDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node )
	{
		node.descriptorSet->createBinding( layout.getBinding( ShadowMapPassPoint::UboBindingPoint )
			, *m_shadowConfig );
	}

	void ShadowMapPassPoint::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( m_renderQueue );
	}

	void ShadowMapPassPoint::doPreparePipeline( renderer::ShaderStageStateArray & program
		, PipelineFlags const & flags )
	{
		if ( m_backPipelines.find( flags ) == m_backPipelines.end() )
		{
			renderer::RasterisationState rsState;
			rsState.cullMode = renderer::CullModeFlag::eNone;
			renderer::DepthStencilState dsState;
			auto & pipeline = *m_backPipelines.emplace( flags
				, std::make_unique< RenderPipeline >( *getEngine()->getRenderSystem()
					, std::move( dsState )
					, std::move( rsState )
					, renderer::ColourBlendState::createDefault()
					, renderer::MultisampleState{}
					, program
					, flags ) ).first->second;

			auto initialise = [this, &pipeline, flags]()
			{
				auto uboBindings = doCreateUboBindings( flags );
				uboBindings.emplace_back( ShadowMapPassPoint::UboBindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment );
				auto layout = getEngine()->getRenderSystem()->getCurrentDevice()->createDescriptorSetLayout( std::move( uboBindings ) );
				std::vector< renderer::DescriptorSetLayoutPtr > layouts;
				layouts.emplace_back( std::move( layout ) );
				pipeline.setDescriptorSetLayouts( std::move( layouts ) );
				m_initialised = true;
			};

			if ( getEngine()->getRenderSystem()->hasCurrentDevice() )
			{
				initialise();
			}
			else
			{
				getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender, initialise ) );
			}
		}
	}
}
