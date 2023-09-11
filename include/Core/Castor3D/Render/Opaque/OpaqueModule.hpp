/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OpaqueTechniqueModule_H___
#define ___C3D_OpaqueTechniqueModule_H___

#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Graphics/Size.hpp>

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Technique */
	//@{
	/**@name Opaque */
	//@{

	/**
	*\~english
	*\brief
	*	The render technique part dedicated to opaque nodes.
	*\~french
	*\brief
	*	La partie de render technique dédiée aux noeuds opaques.
	*/
	class OpaqueRendering;
	/**
	*\~english
	*\brief
	*	Pass used to count materials use in a visibility buffer.
	*\~french
	*\brief
	*	Passe comptant les matériaux utilisés dans le visibility buffer.
	*/
	class VisibilityReorderPass;
	/**
	*\~english
	*\brief
	*	The post visibility resolve pass.
	*\~french
	*\brief
	*	La passe de résolution de la visibilité.
	*/
	class VisibilityResolvePass;

	CU_DeclareSmartPtr( castor3d, OpaqueRendering, C3D_API );
	CU_DeclareSmartPtr( castor3d, VisibilityReorderPass, C3D_API );

	//@}
	//@}
	//@}
}

#endif
