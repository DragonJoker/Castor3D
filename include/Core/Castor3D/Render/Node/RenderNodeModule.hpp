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
		eFrontSubmesh = 0,
		eFrontSubmeshMeshlet = 1,
		eFrontSubmeshInstanced = 2,
		eFrontBillboard = 3,
		eBackSubmesh = 4,
		eBackSubmeshMeshlet = 5,
		eBackSubmeshInstanced = 6,
		eBackBillboard = 7,
		CU_ScopedEnumBounds( eFrontSubmesh, eBackBillboard )
	};
	RenderNodeType getRenderNodeType( ProgramFlags const & flags );
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

	using PipelineBuffer = castor::Pair< PipelineBaseHash, ashes::BufferBase const * >;

	/**@name Traits */
	//@{

	template< typename NodeT >
	struct RenderNodeTraitsT
	{
		using Object = Submesh;
		using Command = VkDrawIndexedIndirectCommand;
	};

	template<>
	struct RenderNodeTraitsT< BillboardRenderNode >
	{
		using Object = BillboardBase;
		using Command = VkDrawIndirectCommand;
	};

	template< typename NodeT >
	using NodeObjectT = typename RenderNodeTraitsT< NodeT >::Object;
	template< typename NodeT >
	using NodeCommandT = typename RenderNodeTraitsT< NodeT >::Command;

	using PipelineBufferArray = castor::Vector< PipelineBuffer >;

	//@}
	/**@name All nodes */
	//@{
	/**@name Non instanced */
	//@{

	template< typename NodeT >
	class PipelinesNodesT;

	//@}
	/**@name Instanced */
	//@{

	template< typename NodeT >
	class InstantiatedPipelinesNodesT;

	//@}
	//@}
	//@}
	//@}
}

#endif
