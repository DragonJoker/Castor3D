#include "ForwardRenderTechniquePass.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderTarget.hpp"
#include "Render/RenderNode/RenderNode_Render.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Animation/AnimatedObjectGroup.hpp"
#include "Shader/Uniform.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>
#include <GlslShadow.hpp>

using namespace Castor;

namespace Castor3D
{
	ForwardRenderTechniquePass::ForwardRenderTechniquePass( String const & p_name
		, RenderTarget & p_renderTarget
		, RenderTechnique & p_technique
		, bool p_opaque
		, bool p_multisampling )
		: RenderTechniquePass{ p_name
			, p_renderTarget
			, p_technique
			, p_opaque
			, p_multisampling }
		, m_directionalShadowMap{ *p_renderTarget.GetEngine() }
		, m_spotShadowMap{ *p_renderTarget.GetEngine() }
		, m_pointShadowMap{ *p_renderTarget.GetEngine() }
	{
	}

	ForwardRenderTechniquePass::~ForwardRenderTechniquePass()
	{
	}

	bool ForwardRenderTechniquePass::InitialiseShadowMaps()
	{
		auto & l_scene = *m_target.GetScene();
		l_scene.GetLightCache().ForEach( [&l_scene, this]( Light & p_light )
		{
			if ( p_light.IsShadowProducer() )
			{
				switch ( p_light.GetLightType() )
				{
				case LightType::eDirectional:
					m_directionalShadowMap.AddLight( p_light );
					break;

				case LightType::ePoint:
					m_pointShadowMap.AddLight( p_light );
					break;

				case LightType::eSpot:
					m_spotShadowMap.AddLight( p_light );
					break;
				}
			}
		} );

		bool l_result = m_directionalShadowMap.Initialise();

		if ( l_result )
		{
			l_result = m_spotShadowMap.Initialise();
		}

		if ( l_result )
		{
			l_result = m_pointShadowMap.Initialise();
		}

		ENSURE( l_result );
		return l_result;
	}

	void ForwardRenderTechniquePass::CleanupShadowMaps()
	{
		m_pointShadowMap.Cleanup();
		m_spotShadowMap.Cleanup();
		m_directionalShadowMap.Cleanup();
	}

	void ForwardRenderTechniquePass::UpdateShadowMaps( RenderQueueArray & p_queues )
	{
		m_pointShadowMap.Update( *m_target.GetCamera(), p_queues );
		m_spotShadowMap.Update( *m_target.GetCamera(), p_queues );
		m_directionalShadowMap.Update( *m_target.GetCamera(), p_queues );
	}

	void ForwardRenderTechniquePass::RenderShadowMaps()
	{
		m_directionalShadowMap.Render();
		m_pointShadowMap.Render();
		m_spotShadowMap.Render();
	}

	void ForwardRenderTechniquePass::DoGetDepthMaps( DepthMapArray & p_depthMaps )
	{
		p_depthMaps.push_back( std::ref( m_directionalShadowMap.GetTexture() ) );
		p_depthMaps.push_back( std::ref( m_spotShadowMap.GetTexture() ) );

		for ( auto & l_map : m_pointShadowMap.GetTextures() )
		{
			p_depthMaps.push_back( std::ref( l_map ) );
		}
	}
}
