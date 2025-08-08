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

#include "Castor3D/Buffer/GpuBuffer.hpp"

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Design/OwnedBy.hpp>

#include <ashespp/Buffer/Buffer.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <mutex>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace c3d
{
	struct SceneRenderNodes
		: OwnedBy< Scene >
	{
	public:
		template< typename NodeT >
		using NodesPtrMapT = HashMap< size_t, UniquePtr< NodeT > >;
		struct NodeData
		{
			NodeData( Pass const * pass
				, SceneNode const * node
				, RenderedObject const * object )
				: pass{ pass }
				, node{ node }
				, object{ object }
			{
			}

			Pass const * pass;
			SceneNode const * node;
			RenderedObject const * object;
		};
		using NodeDataArray = Vector< NodeData >;

	public:
		C3D_API explicit SceneRenderNodes( Scene & scene );
		C3D_API ~SceneRenderNodes()noexcept;

		C3D_API void registerCuller( SceneCuller & culler );
		C3D_API void unregisterCuller( SceneCuller & culler )noexcept;
		C3D_API void clear()noexcept;
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

		void createVertexTransformPass( crg::FramePassGroup & graph );
		crg::Attachment const & getVertexTransform()const;

		bool hasNodes()const noexcept
		{
			return !m_submeshNodes.empty()
				|| !m_billboardNodes.empty();
		}

		BufferBase const & getModelBuffer()const noexcept
		{
			return *m_modelsData;
		}

		BufferBase const & getBillboardsBuffer()const noexcept
		{
			return *m_billboardsData;
		}

		NodesPtrMapT< SubmeshRenderNode > const & getSubmeshNodes()const noexcept
		{
			return m_submeshNodes;
		}

		NodesPtrMapT< BillboardRenderNode > const & getBillboardNodes()const noexcept
		{
			return m_billboardNodes;
		}

	private:
		RenderDevice const & m_device;
		Mutex m_nodesMutex;
		NodesPtrMapT< SubmeshRenderNode > m_submeshNodes;
		NodesPtrMapT< BillboardRenderNode > m_billboardNodes;
		BufferUPtrT< ModelBufferConfiguration > m_modelsData;
		BufferUPtrT< BillboardUboConfiguration > m_billboardsData;
		ArrayView< ModelBufferConfiguration > m_modelsBuffer;
		ArrayView< BillboardUboConfiguration > m_billboardsBuffer;
		FramePassTimerUPtr m_timerRenderNodes;
		NodeDataArray m_nodesData;
		uint32_t m_nodeId{};
		Vector< SceneCuller * > m_cullers;
		bool m_dirty{ true };
		VertexTransformingUPtr m_vertexTransform;
		Map< LightingModelID, size_t > m_lightingModels;
		Map< Pass const *, OnPassChangedConnection > m_onPassChanged;
	};
}

#endif
