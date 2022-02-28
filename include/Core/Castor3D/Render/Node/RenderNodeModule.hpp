/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderNodeModule_H___
#define ___C3D_RenderNodeModule_H___

#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Node */
	//@{

	enum class RenderNodeType
	{
		eFrontStatic,
		eFrontSkinned,
		eFrontInstancedStatic,
		eFrontInstancedSkinned,
		eFrontMorphing,
		eFrontBillboard,
		eBackStatic,
		eBackSkinned,
		eBackInstancedStatic,
		eBackInstancedSkinned,
		eBackMorphing,
		eBackBillboard,
		CU_ScopedEnumBounds( eFrontStatic )
	};
	RenderNodeType getRenderNodeType( ProgramFlags const & flags );
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
	*	The base holder of all the render nodes of a scene.
	*\~french
	*\brief
	*	Le conteneur de base de tous les noeuds de rendu d'une scène.
	*/
	struct SceneRenderNodes;

	CU_DeclareCUSmartPtr( castor3d, BillboardRenderNode, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, SubmeshRenderNode, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, QueueRenderNodes, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, SceneRenderNodes, C3D_API );

	/**@name Traits */
	//@{

	struct PipelineComp
	{
		C3D_API bool operator()( RenderPipeline const * lhs
			, RenderPipeline const * rhs )const;
	};

	template< typename NodeT >
	struct RenderNodeTraitsT
	{
		using Object = Submesh;
	};

	template<>
	struct RenderNodeTraitsT< BillboardRenderNode >
	{
		using Object = BillboardBase;
	};

	template< typename NodeT >
	using NodeObjectT = typename RenderNodeTraitsT< NodeT >::Object;

	//@}
	/**@name All nodes */
	//@{
	/**@name Non instanced */
	//@{

	template< typename NodeT >
	using NodePtrArrayT = std::vector< NodeT const * >;

	using SubmeshRenderNodePtrArray = NodePtrArrayT< SubmeshRenderNode >;
	using BillboardRenderNodePtrArray = NodePtrArrayT< BillboardRenderNode >;

	template< typename NodeT >
	using NodePtrByBufferMapT = std::map< ashes::BufferBase const *, NodePtrArrayT< NodeT > >;

	template< typename NodeT >
	using NodePtrByPipelineMapT = std::map< uint32_t, std::pair< RenderPipeline *, NodePtrByBufferMapT< NodeT > > >;

	using SubmeshRenderNodePtrByPipelineMap = NodePtrByPipelineMapT< SubmeshRenderNode >;
	using BillboardRenderNodePtrByPipelineMap = NodePtrByPipelineMapT< BillboardRenderNode >;

	//@}
	/**@name Instanced */
	//@{

	template< typename NodeT >
	using ObjectNodesPtrMapT = std::map< NodeObjectT< NodeT > *, NodePtrArrayT< NodeT > >;

	template< typename NodeT >
	using ObjectNodesPtrByPassT = std::map< PassRPtr, ObjectNodesPtrMapT< NodeT > >;

	template< typename NodeT >
	using ObjectNodesPtrByBufferMapT = std::map< ashes::BufferBase const *, ObjectNodesPtrByPassT< NodeT > >;

	template< typename NodeT >
	using ObjectNodesPtrByPipelineMapT = std::map< uint32_t, std::pair< RenderPipeline *, ObjectNodesPtrByBufferMapT< NodeT > > >;

	using SubmeshRenderNodesPtrByPipelineMap = ObjectNodesPtrByPipelineMapT< SubmeshRenderNode >;

	//@}
	//@}
	//@}
	//@}
}

#endif
