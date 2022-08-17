/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneCuller_H___
#define ___C3D_SceneCuller_H___

#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"

#include "Castor3D/Limits.hpp"

namespace castor3d
{
	AnimatedObjectSPtr findAnimatedObject( Scene const & scene
		, castor::String const & name );

	size_t hash( SubmeshRenderNode const & culled );
	bool isVisible( Camera const & camera
		, SubmeshRenderNode const & node );
	bool isVisible( Frustum const & frustum
		, SubmeshRenderNode const & node );

	size_t hash( BillboardRenderNode const & culled );
	bool isVisible( Camera const & camera
		, BillboardRenderNode const & node );
	bool isVisible( Frustum const & frustum
		, BillboardRenderNode const & node );

	class SceneCuller
	{
	public:
		template< typename NodeT >
		struct CulledNodeT
		{
			NodeT const * node{};
			bool visible{};
			uint32_t instanceCount{};
		};

		template< typename NodeT >
		using NodeArrayT = std::vector< CulledNodeT< NodeT > >;
		template< typename NodeT >
		using SidedNodeT = std::pair< CulledNodeT< NodeT >, bool >;
		template< typename NodeT >
		using SidedNodeArrayT = std::vector< SidedNodeT< NodeT > >;

		template< typename NodeT >
		using SidedNodeBufferMapT = std::map< ashes::BufferBase const *, SidedNodeArrayT< NodeT > >;
		template< typename NodeT >
		using SidedNodePipelineMapT = std::map< PipelineBaseHash, SidedNodeBufferMapT< NodeT > >;

		template< typename NodeT >
		using SidedObjectNodeMapT = std::map< NodeObjectT< NodeT > const *, SidedNodeArrayT< NodeT > >;
		template< typename NodeT >
		using SidedObjectNodePassMapT = std::map< Pass const *, SidedObjectNodeMapT< NodeT > >;
		template< typename NodeT >
		using SidedObjectNodeBufferMapT = std::map< ashes::BufferBase const *, SidedObjectNodePassMapT< NodeT > >;
		template< typename NodeT >
		using SidedObjectNodePipelineMapT = std::map< PipelineBaseHash, SidedObjectNodeBufferMapT< NodeT > >;

#if VK_NV_mesh_shader
		using IndexedMeshDrawCommandsBuffer = ashes::BufferPtr< VkDrawMeshTasksIndirectCommandNV >;
#endif
		using IndexedDrawCommandsBuffer = ashes::BufferPtr< VkDrawIndexedIndirectCommand >;
		using DrawCommandsBuffer = ashes::BufferPtr< VkDrawIndirectCommand >;
		using PipelineNodesBuffer = ashes::BufferPtr< PipelineNodes >;

		using PipelineBufferArray = std::vector< PipelineBuffer >;

		struct RenderPassBuffers
		{
#if VK_NV_mesh_shader
			IndexedMeshDrawCommandsBuffer submeshMeshletIndirectCommands;
#endif
			IndexedDrawCommandsBuffer submeshIdxIndirectCommands;
			DrawCommandsBuffer submeshNIdxIndirectCommands;
			DrawCommandsBuffer billboardIndirectCommands;
			PipelineNodesBuffer pipelinesNodes;
			PipelineBufferArray nodesIds;
			std::map< uint32_t, uint32_t > nodesPipelinesIds;
			SidedNodePipelineMapT< SubmeshRenderNode > sortedSubmeshes;
			SidedObjectNodePipelineMapT< SubmeshRenderNode > sortedInstancedSubmeshes;
			SidedNodePipelineMapT< BillboardRenderNode > sortedBillboards;
		};

	public:
		C3D_API SceneCuller( Scene & scene
			, Camera * camera );
		C3D_API virtual ~SceneCuller();
		C3D_API void registerRenderPass( RenderNodesPass const & renderPass );
		C3D_API void unregisterRenderPass( RenderNodesPass const & renderPass );
		C3D_API void update( CpuUpdater & updater );
		C3D_API std::pair< uint32_t, uint32_t > fillPipelinesIds( RenderNodesPass const & renderPass
			, castor::ArrayView< uint32_t > nodesPipelinesIds )const;
		C3D_API void registerNodePipeline( RenderNodesPass const & renderPass
			, uint32_t nodeId
			, uint32_t pipelineId );
		C3D_API PipelineBufferArray const & getPassPipelineNodes( RenderNodesPass const & renderPass )const;
		C3D_API uint32_t getPipelineNodesIndex( RenderNodesPass const & renderPass
			, PipelineBaseHash const & hash
			, ashes::BufferBase const & buffer )const;
		C3D_API uint32_t getPipelineNodesIndex( RenderNodesPass const & renderPass
			, Submesh const & submesh
			, Pass const & pass
			, ashes::BufferBase const & buffer
			, bool isFrontCulled )const;
		C3D_API uint32_t getPipelineNodesIndex( RenderNodesPass const & renderPass
			, BillboardBase const & billboard
			, Pass const & pass
			, ashes::BufferBase const & buffer
			, bool isFrontCulled )const;
		C3D_API void removeCulled( SubmeshRenderNode const & node );
		C3D_API void removeCulled( BillboardRenderNode const & node );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		Scene & getScene()const
		{
			return m_scene;
		}

		bool hasCamera()const
		{
			return m_camera != nullptr;
		}

		Camera const & getCamera()const
		{
			CU_Require( hasCamera() );
			return *m_camera;
		}

		Camera & getCamera()
		{
			CU_Require( hasCamera() );
			return *m_camera;
		}

		bool areAnyChanged()const
		{
			return m_anyChanged;
		}

		bool areCulledChanged()const
		{
			return m_culledChanged;
		}

		bool hasCulledNodes( RenderNodesPass const & renderPass )const
		{
			auto it = m_renderPasses.find( &renderPass );
			CU_Require( it != m_renderPasses.end() );
			return !it->second.sortedSubmeshes.empty()
				|| !it->second.sortedInstancedSubmeshes.empty()
				|| !it->second.sortedBillboards.empty();
		}

#if VK_NV_mesh_shader

		ashes::Buffer< VkDrawMeshTasksIndirectCommandNV > const & getSubmeshMeshletCommands( RenderNodesPass const & renderPass )const
		{
			auto it = m_renderPasses.find( &renderPass );
			CU_Require( it != m_renderPasses.end() );
			return *it->second.submeshMeshletIndirectCommands;
		}

#endif

		ashes::Buffer< VkDrawIndexedIndirectCommand > const & getSubmeshIdxCommands( RenderNodesPass const & renderPass )const
		{
			auto it = m_renderPasses.find( &renderPass );
			CU_Require( it != m_renderPasses.end() );
			return *it->second.submeshIdxIndirectCommands;
		}

		ashes::Buffer< VkDrawIndirectCommand > const & getSubmeshNIdxCommands( RenderNodesPass const & renderPass )const
		{
			auto it = m_renderPasses.find( &renderPass );
			CU_Require( it != m_renderPasses.end() );
			return *it->second.submeshNIdxIndirectCommands;
		}

		ashes::Buffer< VkDrawIndirectCommand > const & getBillboardCommands( RenderNodesPass const & renderPass )const
		{
			auto it = m_renderPasses.find( &renderPass );
			CU_Require( it != m_renderPasses.end() );
			return *it->second.billboardIndirectCommands;
		}

		ashes::Buffer< PipelineNodes > const & getNodesIds( RenderNodesPass const & renderPass )const
		{
			auto it = m_renderPasses.find( &renderPass );
			CU_Require( it != m_renderPasses.end() );
			return *it->second.pipelinesNodes;
		}

		SidedNodePipelineMapT< SubmeshRenderNode > const & getSubmeshNodes( RenderNodesPass const & renderPass )const
		{
			auto it = m_renderPasses.find( &renderPass );
			CU_Require( it != m_renderPasses.end() );
			return it->second.sortedSubmeshes;
		}

		SidedObjectNodePipelineMapT< SubmeshRenderNode > const & getInstancedSubmeshNodes( RenderNodesPass const & renderPass )const
		{
			auto it = m_renderPasses.find( &renderPass );
			CU_Require( it != m_renderPasses.end() );
			return it->second.sortedInstancedSubmeshes;
		}

		SidedNodePipelineMapT< BillboardRenderNode > const & getBillboardNodes( RenderNodesPass const & renderPass )const
		{
			auto it = m_renderPasses.find( &renderPass );
			CU_Require( it != m_renderPasses.end() );
			return it->second.sortedBillboards;
		}
		/**@}*/

	public:
		mutable SceneCullerSignal onCompute;

	private:
		void doInitialiseCulled();
		void doUpdateChanged( CpuUpdater::DirtyObjects & sceneObjs );
		void doUpdateCulled( CpuUpdater::DirtyObjects & sceneObjs );
		void doSortNodes();
		void doInitGpuBuffers();
		void doFillIndirect();
		void doMarkDirty( CpuUpdater::DirtyObjects & sceneObjs
			, std::vector< SubmeshRenderNode const * > & dirtySubmeshes
			, std::vector< BillboardRenderNode const * > & dirtyBillboards );
		void duUpdateCulledSubmeshes( std::vector< SubmeshRenderNode const * > const & dirtySubmeshes );
		void duUpdateCulledBillboards( std::vector< BillboardRenderNode const * > const & dirtyBillboards );
		void doMakeDirty( Geometry const & object
			, std::vector< SubmeshRenderNode const * > & dirtySubmeshes )const;
		void doMakeDirty( BillboardBase const & object
			, std::vector< BillboardRenderNode const * > & dirtyBillboards )const;
		void doAddSubmesh( CulledNodeT< SubmeshRenderNode > const & culled
			, RenderNodesPass const & renderPass
			, SidedNodePipelineMapT< SubmeshRenderNode > & sorted
			, SidedObjectNodePipelineMapT< SubmeshRenderNode > & sortedInstanced
			, PipelineBufferArray & nodesIds );
		void doAddBillboard( CulledNodeT< BillboardRenderNode > const & culled
			, RenderNodesPass const & renderPass
			, SidedNodePipelineMapT< BillboardRenderNode > & sorted
			, PipelineBufferArray & nodesIds );
		virtual bool isSubmeshVisible( SubmeshRenderNode const & node )const = 0;
		virtual bool isBillboardVisible( BillboardRenderNode const & node )const = 0;

	private:
		Scene & m_scene;

	protected:
		Camera * m_camera;
		uint32_t m_index;
		bool m_first{ true };
		bool m_anyChanged{ true };
		bool m_culledChanged{ true };
		FramePassTimerUPtr m_timer;
		FramePassTimerUPtr m_timerDirty;
		FramePassTimerUPtr m_timerCompute;
		FramePassTimerUPtr m_timerIndirect;

		NodeArrayT< SubmeshRenderNode > m_culledSubmeshes;
		NodeArrayT< BillboardRenderNode > m_culledBillboards;

		std::map< RenderNodesPass const *, RenderPassBuffers > m_renderPasses;
	};
}

#endif
