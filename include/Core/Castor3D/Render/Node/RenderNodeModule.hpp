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
	*	Helper structure used to render billboards.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour le dessin des billboards.
	*/
	struct BillboardRenderNode;
	/**
	*\~english
	*\brief
	*	Helper structure used to render distance sorted nodes.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour le dessin de noeuds triés par distance.
	*/
	struct DistanceRenderNodeBase;
	/**
	*\~english
	*\brief
	*	Helper structure used to render distance sorted typed nodes.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour le dessin de noeuds typés, triés par distance.
	*/
	template< typename NodeType >
	struct DistanceRenderNode;
	/**
	*\~english
	*\brief
	*	Helper structure used to render animated submeshes.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour le dessin des sous-maillages animés.
	*/
	struct MorphingRenderNode;
	/**
	*\~english
	*\brief
	*	Helper structure used to render objects.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour le dessin d'objets.
	*/
	template< typename DataTypeT, typename InstanceTypeT >
	struct ObjectRenderNode;
	/**
	*\~english
	*\brief
	*	Helper structure used link a pass and a shader program.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour lier une passe et un programme shader.
	*/
	struct PassRenderNode;
	/**
	*\~english
	*\brief
	*	Helper structure used to render submeshes.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour le dessin des sous-maillages.
	*/
	struct SceneRenderNode;
	/**
	*\~english
	*\brief
	*	Helper structure used to render skeleton animated submeshes.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour le dessin des sous-maillages animés par squelette.
	*/
	struct SkinningRenderNode;
	/**
	*\~english
	*\brief
	*	Helper structure used to render static submeshes.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour le dessin des sous-maillages non animés.
	*/
	struct StaticRenderNode;

	using SubmeshRenderNode = ObjectRenderNode< Submesh, Geometry >;
	using BillboardListRenderNode = ObjectRenderNode< BillboardBase, BillboardBase >;

	//@}
	//@}
}

#endif
