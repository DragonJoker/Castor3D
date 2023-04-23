/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneRenderNodes_H___
#define ___C3D_SceneRenderNodes_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Transform/TransformModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Design/OwnedBy.hpp>

#include <ashespp/Buffer/Buffer.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <mutex>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace castor3d
{
	struct SceneRenderNodes
		: castor::OwnedBy< Scene >
	{
	public:
		template< typename NodeT >
		using NodesPtrMapT = std::unordered_map< size_t, castor::UniquePtr< NodeT > >;
		struct NodeData
		{
			Pass const * pass;
			SceneNode const * node;
			RenderedObject const * object;
		};
		using NodeDataArray = std::vector< NodeData >;

	public:
		C3D_API explicit SceneRenderNodes( Scene & scene );
		C3D_API ~SceneRenderNodes();

		C3D_API void registerCuller( SceneCuller & culler );
		C3D_API void unregisterCuller( SceneCuller & culler );
		C3D_API void clear();
		C3D_API SubmeshRenderNode & createNode( Pass & pass
			, Submesh & data
			, Geometry & instance
			, AnimatedMesh * mesh
			, AnimatedSkeleton * skeleton );
		C3D_API BillboardRenderNode & createNode( Pass & pass
			, BillboardBase & instance );
		C3D_API SubmeshRenderNode const * getSubmeshNode( uint32_t nodeId );
		C3D_API BillboardRenderNode const * getBillboardNode( uint32_t nodeId );
		C3D_API void reportPassChange( Pass const & pass
			, PassComponentCombineID oldComponents
			, PassComponentCombineID newComponents );
		C3D_API void reportPassChange( Submesh & data
			, Geometry & instance
			, Material const & oldMaterial
			, Material const & newMaterial );
		C3D_API void reportPassChange( BillboardBase & billboard
			, Material const & oldMaterial
			, Material const & newMaterial );
		C3D_API void update( CpuUpdater & updater );
		C3D_API void update( GpuUpdater & updater );
		C3D_API bool hasNodes( LightingModelID lightingModelId )const;

		C3D_API crg::FramePass const & createVertexTransformPass( crg::FramePassGroup & graph );

		bool hasNodes()const
		{
			return !m_submeshNodes.empty()
				|| !m_billboardNodes.empty();
		}

		ashes::Buffer< ModelBufferConfiguration > const & getModelBuffer()const
		{
			return *m_modelsData;
		}

		ashes::Buffer< BillboardUboConfiguration > const & getBillboardsBuffer()const
		{
			return *m_billboardsData;
		}

		NodesPtrMapT< SubmeshRenderNode > const & getSubmeshNodes()const
		{
			return m_submeshNodes;
		}

		NodesPtrMapT< BillboardRenderNode > const & getBillboardNodes()const
		{
			return m_billboardNodes;
		}

	private:
		RenderDevice const & m_device;
		std::mutex m_nodesMutex;
		NodesPtrMapT< SubmeshRenderNode > m_submeshNodes;
		NodesPtrMapT< BillboardRenderNode > m_billboardNodes;
		ashes::BufferPtr< ModelBufferConfiguration > m_modelsData;
		ashes::BufferPtr< BillboardUboConfiguration > m_billboardsData;
		castor::ArrayView< ModelBufferConfiguration > m_modelsBuffer;
		castor::ArrayView< BillboardUboConfiguration > m_billboardsBuffer;
		FramePassTimerUPtr m_timerRenderNodes;
		NodeDataArray m_nodesData;
		uint32_t m_nodeId{};
		std::vector< SceneCuller * > m_cullers;
		bool m_dirty{ true };
		VertexTransformingUPtr m_vertexTransform;
		std::map< LightingModelID, size_t > m_lightingModels;
		std::map< Pass const *, OnPassChangedConnection > m_onPassChanged;
	};
}

#endif
