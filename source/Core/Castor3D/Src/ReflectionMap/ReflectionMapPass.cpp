#include "ReflectionMapPass.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "ReflectionMap/ReflectionMap.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		void DoUpdateMatrices( Point3r const & p_position
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

	ReflectionMapPass::ReflectionMapPass( Engine & p_engine
		, SceneNode & p_node
		, ReflectionMap const & p_reflectionMap )
		: RenderPass{ cuT( "ShadowMap" ), p_engine, true }
		, m_reflectionMap{ p_reflectionMap }
		, m_node{ p_node }
		, m_viewport{ p_engine }
	{
		m_renderQueue.Initialise( *p_node.GetScene() );
	}

	ReflectionMapPass::~ReflectionMapPass()
	{
	}

	bool ReflectionMapPass::Initialise( Size const & p_size )
	{
		real const l_aspect = real( p_size.width() ) / p_size.height();
		real const l_near = 1.0_r;
		real const l_far = 2000.0_r;
		matrix::perspective( m_projection, Angle::from_degrees( 90.0_r ), l_aspect, l_near, l_far );

		m_viewport.Resize( p_size );
		m_viewport.Initialise();
		m_projectionUniform->SetValue( m_projection );
		return true;
	}

	void ReflectionMapPass::Cleanup()
	{
		m_viewport.Cleanup();
		m_matrixUbo.Cleanup();
		m_onNodeChanged.disconnect();
	}

	void ReflectionMapPass::DoUpdate( RenderQueueArray & p_queues )
	{
		DoUpdateMatrices( m_node.GetDerivedPosition(), m_matrices );
	}

	void ReflectionMapPass::DoRender( uint32_t p_face )
	{
		if ( m_initialised )
		{
			m_viewport.Apply();
			m_viewUniform->SetValue( m_matrices[p_face] );
			m_matrixUbo.Update();
			auto & l_nodes = m_renderQueue.GetRenderNodes();
			DoRenderInstancedSubmeshes( l_nodes.m_instancedNodes.m_backCulled );
			DoRenderStaticSubmeshes( l_nodes.m_staticNodes.m_backCulled );
			DoRenderSkinningSubmeshes( l_nodes.m_skinningNodes.m_backCulled );
			DoRenderMorphingSubmeshes( l_nodes.m_morphingNodes.m_backCulled );
			DoRenderBillboards( l_nodes.m_billboardNodes.m_backCulled );
		}
	}
}
