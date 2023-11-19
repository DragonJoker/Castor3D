/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderNodeModule_H___
#define ___C3D_RenderNodeModule_H___

#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

#include <unordered_map>

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Node */
	//@{

	enum class RenderNodeType
		: uint8_t
	{
		eFrontSubmesh,
		eFrontSubmeshMeshlet,
		eFrontSubmeshInstanced,
		eFrontBillboard,
		eBackSubmesh,
		eBackSubmeshMeshlet,
		eBackSubmeshInstanced,
		eBackBillboard,
		CU_ScopedEnumBounds( eFrontSubmesh )
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

	CU_DeclareSmartPtr( castor3d, BillboardRenderNode, C3D_API );
	CU_DeclareSmartPtr( castor3d, SubmeshRenderNode, C3D_API );
	CU_DeclareSmartPtr( castor3d, QueueRenderNodes, C3D_API );
	CU_DeclareSmartPtr( castor3d, SceneRenderNodes, C3D_API );

	using PipelineBuffer = std::pair< PipelineBaseHash, ashes::BufferBase const * >;

	/**@name Traits */
	//@{

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

	using PipelineBufferArray = std::vector< PipelineBuffer >;

	//@}
	/**@name All nodes */
	//@{
	/**@name Non instanced */
	//@{

	template< typename NodeT >
	struct CountedNodeT
	{
		NodeT const * node{};
		uint32_t instanceCount{};
		bool visibleOrFrontCulled{};
	};

	template< typename NodeT >
	using NodeArrayT = std::vector< CountedNodeT< NodeT > >;

	template< typename NodeT >
	using NodePtrByBufferMapT = std::unordered_map< ashes::BufferBase const *, NodeArrayT< NodeT > >;

	template< typename NodeT >
	using NodePtrByRenderDataMapT = std::unordered_map< SubmeshRenderData *, NodePtrByBufferMapT< NodeT > >;

	template< typename NodeT >
	using NodePtrByPipelineMapT = std::unordered_map< RenderPipeline *, NodePtrByRenderDataMapT< NodeT > >;

	//@}
	/**@name Instanced */
	//@{

	template< typename NodeT >
	using ObjectNodesPtrMapT = std::unordered_map< NodeObjectT< NodeT > *, NodeArrayT< NodeT > >;

	template< typename NodeT >
	using ObjectNodesPtrByPassT = std::unordered_map< PassRPtr, ObjectNodesPtrMapT< NodeT > >;

	template< typename NodeT >
	using ObjectNodesPtrByBufferMapT = std::unordered_map< ashes::BufferBase const *, ObjectNodesPtrByPassT< NodeT > >;

	template< typename NodeT >
	using ObjectNodesPtrByRenderDataMapT = std::unordered_map< SubmeshRenderData *, ObjectNodesPtrByBufferMapT< NodeT > >;

	template< typename NodeT >
	using ObjectNodesPtrByPipelineMapT = std::unordered_map< RenderPipeline *, ObjectNodesPtrByRenderDataMapT< NodeT > >;

	//@}
	//@}
	//@}
	//@}
}

#endif
