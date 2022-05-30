/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TransformModule_H___
#define ___C3D_TransformModule_H___

#include "Castor3D/Render/RenderModule.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Transform */
	//@{

	/**
	*\~english
	*\brief
	*	Holds the base pipeline data for the transformations of a submesh type.
	*\~french
	*\brief
	*	Détient les données de pipeline pour la transformation des sommets d'un type de submesh.
	*/
	struct TransformPipeline;
	/**
	*\~english
	*\brief
	*	Holds the vertex transforming compute pass.
	*\~french
	*\brief
	*	Détient la compute pass de transformation de sommets.
	*/
	class VertexTransforming;
	/**
	*\~english
	*\brief
	*	The vertex transforming compute pass.
	*\~french
	*\brief
	*	La compute pass de transformation de sommets.
	*/
	class VertexTransformingPass;
	/**
	*\~english
	*\brief
	*	Pass used to transform the vertics of one submesh.
	*\~french
	*\brief
	*	Passe de transformation des sommets d'un submesh.
	*/
	class VertexTransformPass;

	CU_DeclareCUSmartPtr( castor3d, VertexTransforming, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, VertexTransformPass, C3D_API );

	//@}
	//@}
}

#endif
