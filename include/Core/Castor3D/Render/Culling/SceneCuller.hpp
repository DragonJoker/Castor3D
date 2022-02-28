/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneCuller_H___
#define ___C3D_SceneCuller_H___

#include "CullingModule.hpp"

#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"

namespace castor3d
{
	using PipelineNodes = std::array< castor::Point4ui, 10'000u >;
	using PipelineBuffer = std::pair< size_t, ashes::BufferBase const * >;

	AnimatedObjectSPtr findAnimatedObject( Scene const & scene
		, castor::String const & name );
	uint32_t getPipelineBaseHash( ProgramFlags programFlags
		, PassFlags passFlags
		, uint32_t texturesCount
		, TextureFlags texturesFlags );
	uint32_t getPipelineBaseHash( RenderNodesPass const & renderPass
		, Submesh const & data
		, Pass const & pass
		, bool isFrontCulled );
	uint32_t getPipelineBaseHash( RenderNodesPass const & renderPass
		, BillboardBase const & data
		, Pass const & pass );

	size_t hash( SubmeshRenderNode const & culled );
	bool isCulled( SubmeshRenderNode const & node );
	bool isVisible( Camera const & camera
		, SubmeshRenderNode const & node );
	bool isVisible( Frustum const & frustum
		, SubmeshRenderNode const & node );

	size_t hash( BillboardRenderNode const & culled );
	bool isCulled( BillboardRenderNode const & node );
	bool isVisible( Camera const & camera
		, BillboardRenderNode const & node );
	bool isVisible( Frustum const & frustum
		, BillboardRenderNode const & node );

	class SceneCuller
	{
	public:
		template< typename NodeT >
		using NodeArrayT = std::vector< NodeT const * >;
		template< typename NodeT >
		using SidedNodeArrayT = std::vector< std::pair< NodeT const *, bool > >;

		template< typename NodeT >
		using SidedNodeBufferMapT = std::map< ashes::BufferBase const *, SidedNodeArrayT< NodeT > >;
		template< typename NodeT >
		using SidedNodePipelineMapT = std::map< uint32_t, SidedNodeBufferMapT< NodeT > >;

		template< typename NodeT >
		using SidedObjectNodeMapT = std::map< NodeObjectT< NodeT > const *, SidedNodeArrayT< NodeT > >;
		template< typename NodeT >
		using SidedObjectNodePassMapT = std::map< Pass const *, SidedObjectNodeMapT< NodeT > >;
		template< typename NodeT >
		using SidedObjectNodeBufferMapT = std::map< ashes::BufferBase const *, SidedObjectNodePassMapT< NodeT > >;
		template< typename NodeT >
		using SidedObjectNodePipelineMapT = std::map< uint32_t, SidedObjectNodeBufferMapT< NodeT > >;

		using IndexedDrawCommandsBuffer = ashes::BufferPtr< VkDrawIndexedIndirectCommand >;
		using DrawCommandsBuffer = ashes::BufferPtr< VkDrawIndirectCommand >;
		using PipelineNodesBuffer = ashes::BufferPtr< PipelineNodes >;

		using PipelineBufferArray = std::vector< PipelineBuffer >;

		struct RenderPassBuffers
		{
			IndexedDrawCommandsBuffer submeshIdxIndirectCommands;
			DrawCommandsBuffer submeshNIdxIndirectCommands;
			DrawCommandsBuffer billboardIndirectCommands;
			PipelineNodesBuffer pipelinesNodes;
			PipelineBufferArray nodesIds;
			SidedNodePipelineMapT< SubmeshRenderNode > sortedSubmeshes;
			SidedObjectNodePipelineMapT< SubmeshRenderNode > sortedInstancedSubmeshes;
			SidedNodePipelineMapT< BillboardRenderNode > sortedBillboards;
		};

	public:
		C3D_API SceneCuller( Scene & scene
			, Camera * camera );
		C3D_API virtual ~SceneCuller() = default;
		C3D_API void registerRenderPass( RenderNodesPass const & renderPass );
		C3D_API void unregisterRenderPass( RenderNodesPass const & renderPass );
		C3D_API void compute();
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

		float getMinCastersZ()
		{
			return m_minCullersZ;
		}

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

		bool areAllChanged()const
		{
			return m_allChanged;
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

	public:
		mutable SceneCullerSignal onCompute;

	private:
		void onSceneChanged( Scene const & scene );
		void onCameraChanged( Camera const & camera );
		void doClearAll();
		void doClearCulled();
		void doUpdateMinCuller();
		void doSortNodes();
		void doFillIndirect();
		virtual void doCullGeometries() = 0;
		virtual void doCullBillboards() = 0;

	private:
		Scene & m_scene;
		Camera * m_camera;

	protected:
		uint32_t m_index;
		bool m_allChanged{ true };
		bool m_culledChanged{ true };
		bool m_sceneDirty{ true };
		bool m_cameraDirty{ true };
		float m_minCullersZ{ 0.0f };

		NodeArrayT< SubmeshRenderNode > m_culledSubmeshes;
		NodeArrayT< BillboardRenderNode > m_culledBillboards;

		std::map< RenderNodesPass const *, RenderPassBuffers > m_renderPasses;

		OnSceneChangedConnection m_sceneChanged;
		OnCameraChangedConnection m_cameraChanged;
	};
}

#endif
