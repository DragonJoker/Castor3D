/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderNodeModule_H___
#define ___C3D_RenderNodeModule_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Node */
	//@{

	/**
	*\~english
	*\brief
	*	Node used to render billboards.
	*\~french
	*\brief
	*	Noeud utilisé pour le dessin de billboards.
	*/
	struct BillboardRenderNode;
	/**
	*\~english
	*\brief
	*	Node used to render a submesh.
	*\~french
	*\brief
	*	Noeud utilisé pour le dessin un submesh.
	*/
	struct SubmeshRenderNode;
	/**
	*\~english
	*\brief
	*	The nodes used to render a scene (unculled).
	*\~french
	*\brief
	*	Les noeuds utilisés pour dessiner une scène (non culled).
	*/
	struct QueueRenderNodes;
	/**
	*\~english
	*\brief
	*	The nodes used to render a scene (culled).
	*\~french
	*\brief
	*	Les noeuds utilisés pour dessiner une scène (culled).
	*/
	struct QueueCulledRenderNodes;
	/**
	*\~english
	*\brief
	*	The base holder of all the render nodes of a scene.
	*\~french
	*\brief
	*	Le conteneur de base de tous les noeuds de rendu d'une scène.
	*/
	struct SceneRenderNodes;

	CU_DeclareCUSmartPtr( castor3d, BillboardRenderNode, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, SubmeshRenderNode, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, QueueRenderNodes, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, QueueCulledRenderNodes, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, SceneRenderNodes, C3D_API );

	/**@name Traits */
	//@{

	template< typename NodeT >
	struct RenderNodeTraitsT
	{
		using Culled = CulledSubmesh;
		using Object = Submesh;
	};

	template<>
	struct RenderNodeTraitsT< BillboardRenderNode >
	{
		using Culled = CulledBillboard;
		using Object = BillboardList;
	};

	template< typename NodeT >
	using NodeCulledT = typename RenderNodeTraitsT< NodeT >::Culled;
	template< typename NodeT >
	using NodeObjectT = typename RenderNodeTraitsT< NodeT >::Object;

	//@}
	/**@name All nodes */
	//@{
	/**@name Non instanced */
	//@{

	template< typename NodeT >
	using NodeMapT = std::map< NodeCulledT< NodeT > const *, NodeT * >;

	using SubmeshRenderNodeMap = NodeMapT< SubmeshRenderNode >;
	using BillboardRenderNodeMap = NodeMapT< BillboardRenderNode >;

	template< typename NodeT >
	using NodeByPipelineMapT = std::map< RenderPipelineRPtr, NodeMapT< NodeT > >;

	using SubmeshRenderNodeByPipelineMap = NodeByPipelineMapT< SubmeshRenderNode >;
	using BillboardRenderNodeByPipelineMap = NodeByPipelineMapT< BillboardRenderNode >;

	//@}
	/**@name Instanced */
	//@{

	template< typename NodeT >
	using ObjectNodesMapT = std::map< NodeObjectT< NodeT > *, NodeMapT< NodeT > >;

	using SubmeshRenderNodesMap = ObjectNodesMapT< SubmeshRenderNode >;

	template< typename NodeT >
	using ObjectNodesByPassMapT = std::map< PassRPtr, ObjectNodesMapT< NodeT > >;

	using SubmeshRenderNodesByPassMap = ObjectNodesByPassMapT< SubmeshRenderNode >;

	template< typename NodeT >
	using ObjectNodesByPipelineMapT = std::map< RenderPipelineRPtr, ObjectNodesByPassMapT< NodeT > >;

	using SubmeshRenderNodesByPipelineMap = ObjectNodesByPipelineMapT< SubmeshRenderNode >;

	//@}
	//@}
	/**@name Culled nodes */
	//@{
	/**@name Non instanced */
	//@{

	template< typename NodeT >
	using NodePtrArrayT = std::vector< NodeT * >;

	using SubmeshRenderNodePtrArray = NodePtrArrayT< SubmeshRenderNode >;
	using BillboardRenderNodePtrArray = NodePtrArrayT< BillboardRenderNode >;

	template< typename NodeT >
	using NodePtrByPipelineMapT = std::map< RenderPipelineRPtr, NodePtrArrayT< NodeT > >;

	using SubmeshRenderNodePtrByPipelineMap = NodePtrByPipelineMapT< SubmeshRenderNode >;
	using BillboardRenderNodePtrByPipelineMap = NodePtrByPipelineMapT< BillboardRenderNode >;

	//@}
	/**@name Instanced */
	//@{

	template< typename NodeT >
	using ObjectNodesPtrMapT = std::map< NodeObjectT< NodeT > *, NodePtrArrayT< NodeT > >;

	using SubmeshRenderNodesPtrMap = ObjectNodesPtrMapT< SubmeshRenderNode >;

	template< typename NodeT >
	using ObjectNodesPtrByPassT = std::map< PassRPtr, ObjectNodesPtrMapT< NodeT > >;

	using SubmeshRenderNodesPtrByPassMap = ObjectNodesPtrByPassT< SubmeshRenderNode >;

	template< typename NodeT >
	using ObjectNodesPtrByPipelineMapT = std::map< RenderPipelineRPtr, ObjectNodesPtrByPassT< NodeT > >;

	using SubmeshRenderNodesPtrByPipelineMap = ObjectNodesPtrByPipelineMapT< SubmeshRenderNode >;

	//@}
	//@}
	//@}
	//@}
}

#endif
