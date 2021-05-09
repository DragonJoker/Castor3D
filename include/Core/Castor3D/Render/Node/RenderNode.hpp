/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderNode_H___
#define ___C3D_RenderNode_H___

#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/DistanceRenderNode.hpp"
#include "Castor3D/Render/Node/MorphingRenderNode.hpp"
#include "Castor3D/Render/Node/ObjectRenderNode.hpp"
#include "Castor3D/Render/Node/PassRenderNode.hpp"
#include "Castor3D/Render/Node/SceneRenderNode.hpp"
#include "Castor3D/Render/Node/SkinningRenderNode.hpp"
#include "Castor3D/Render/Node/StaticRenderNode.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Node */
	//@{
	/**@name Traits */
	//@{

	template< typename NodeT >
	struct RenderNodeTraitsT
	{
		using Culled = CulledSubmesh;
		using Object = Submesh;
		using Base = SubmeshRenderNode;
	};

	template<>
	struct RenderNodeTraitsT< BillboardRenderNode >
	{
		using Culled = CulledBillboard;
		using Object = BillboardList;
		using Base = BillboardListRenderNode;
	};

	template<>
	struct RenderNodeTraitsT< BillboardListRenderNode >
	{
		using Culled = CulledBillboard;
		using Object = BillboardList;
		using Base = BillboardListRenderNode;
	};

	template< typename NodeT >
	using NodeCulledT = typename RenderNodeTraitsT< NodeT >::Culled;
	template< typename NodeT >
	using NodeObjectT = typename RenderNodeTraitsT< NodeT >::Object;
	template< typename NodeT >
	using NodeBaseT = typename RenderNodeTraitsT< NodeT >::Base;

	//@}
	/**@name All nodes */
	//@{
	/**@name Distance sorted */
	//@{

	template< typename NodeT >
	using NodeByDistanceMapT = std::multimap< double, NodeT * >;

	using StaticRenderNodePtrByDistanceMap = NodeByDistanceMapT< StaticRenderNode >;
	using SkinningRenderNodePtrByDistanceMap = NodeByDistanceMapT< SkinningRenderNode >;
	using MorphingRenderNodePtrByDistanceMap = NodeByDistanceMapT< MorphingRenderNode >;
	using BillboardRenderNodeByDistanceMap = NodeByDistanceMapT< BillboardRenderNode >;

	//@}
	/**@name Non instanced */
	//@{

	template< typename NodeT >
	using NodeMapT = std::map< NodeCulledT< NodeT > const *, NodeT >;

	using StaticRenderNodeMap = NodeMapT< StaticRenderNode >;
	using SkinningRenderNodeMap = NodeMapT< SkinningRenderNode >;
	using MorphingRenderNodeMap = NodeMapT< MorphingRenderNode >;
	using BillboardRenderNodeMap = NodeMapT< BillboardRenderNode >;

	template< typename NodeT >
	using NodeByPipelineMapT = std::map< RenderPipelineRPtr, NodeMapT< NodeT > >;

	using StaticRenderNodeByPipelineMap = NodeByPipelineMapT< StaticRenderNode >;
	using SkinningRenderNodeByPipelineMap = NodeByPipelineMapT< SkinningRenderNode >;
	using MorphingRenderNodeByPipelineMap = NodeByPipelineMapT< MorphingRenderNode >;
	using BillboardRenderNodeByPipelineMap = NodeByPipelineMapT< BillboardRenderNode >;

	//@}
	/**@name Instanced */
	//@{

	template< typename NodeT >
	using ObjectNodesMapT = std::map< NodeObjectT< NodeT > *, NodeMapT< NodeT > >;

	using SubmeshStaticRenderNodesMap = ObjectNodesMapT< StaticRenderNode >;
	using SubmeshSkinningRenderNodesMap = ObjectNodesMapT< SkinningRenderNode >;

	template< typename NodeT >
	using ObjectNodesByPassMapT = std::map< PassRPtr, ObjectNodesMapT< NodeT > >;

	using SubmeshStaticRenderNodesByPassMap = ObjectNodesByPassMapT< StaticRenderNode >;
	using SubmeshSkinninRenderNodesByPassMap = ObjectNodesByPassMapT< SkinningRenderNode >;

	template< typename NodeT >
	using ObjectNodesByPipelineMapT = std::map< RenderPipelineRPtr, ObjectNodesByPassMapT< NodeT > >;

	using SubmeshStaticRenderNodesByPipelineMap = ObjectNodesByPipelineMapT< StaticRenderNode >;
	using SubmeshSkinningRenderNodesByPipelineMap = ObjectNodesByPipelineMapT< SkinningRenderNode >;

	//@}
	//@}
	/**@name Culled nodes */
	//@{
	/**@name Non instanced */
	//@{

	template< typename NodeT >
	using NodePtrArrayT = std::vector< NodeT * >;

	using StaticRenderNodePtrArray = NodePtrArrayT< StaticRenderNode >;
	using SkinningRenderNodePtrArray = NodePtrArrayT< SkinningRenderNode >;
	using MorphingRenderNodePtrArray = NodePtrArrayT< MorphingRenderNode >;
	using BillboardRenderNodePtrArray = NodePtrArrayT< BillboardRenderNode >;

	template< typename NodeT >
	using NodePtrByPipelineMapT = std::map< RenderPipelineRPtr, NodePtrArrayT< NodeT > >;

	using StaticRenderNodePtrByPipelineMap = NodePtrByPipelineMapT< StaticRenderNode >;
	using SkinningRenderNodePtrByPipelineMap = NodePtrByPipelineMapT< SkinningRenderNode >;
	using MorphingRenderNodePtrByPipelineMap = NodePtrByPipelineMapT< MorphingRenderNode >;
	using BillboardRenderNodePtrByPipelineMap = NodePtrByPipelineMapT< BillboardRenderNode >;

	//@}
	/**@name Instanced */
	//@{

	template< typename NodeT >
	using ObjectNodesPtrMapT = std::map< NodeObjectT< NodeT > *, NodePtrArrayT< NodeT > >;

	using SubmeshStaticRenderNodesPtrMap = ObjectNodesPtrMapT< StaticRenderNode >;
	using SubmeshSkinningRenderNodesPtrMap = ObjectNodesPtrMapT< SkinningRenderNode >;

	template< typename NodeT >
	using ObjectNodesPtrByPassT = std::map< PassRPtr, ObjectNodesPtrMapT< NodeT > >;

	using SubmeshStaticRenderNodesPtrByPassMap = ObjectNodesPtrByPassT< StaticRenderNode >;
	using SubmeshSkinninRenderNodesPtrByPassMap = ObjectNodesPtrByPassT< SkinningRenderNode >;

	template< typename NodeT >
	using ObjectNodesPtrByPipelineMapT = std::map< RenderPipelineRPtr, ObjectNodesPtrByPassT< NodeT > >;

	using SubmeshStaticRenderNodesPtrByPipelineMap = ObjectNodesPtrByPipelineMapT< StaticRenderNode >;
	using SubmeshSkinningRenderNodesPtrByPipelineMap = ObjectNodesPtrByPipelineMapT< SkinningRenderNode >;

	//@}
	//@}
	//@}
	//@}
}

#endif
