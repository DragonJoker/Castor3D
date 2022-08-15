/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VisibilityModule_H___
#define ___C3D_VisibilityModule_H___

#include "Castor3D/Render/Technique/TechniqueModule.hpp"

#include <CastorUtils/Graphics/Size.hpp>

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Technique */
	//@{
	/**@name Visibility */
	//@{

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
	*	The post lighting resolve pass.
	*\~french
	*\brief
	*	La passe de résolution post éclairage.
	*/
	class VisibilityResolvePass;

	CU_DeclareCUSmartPtr( castor3d, VisibilityReorderPass, C3D_API );

	//@}
	//@}
	//@}
}

#endif
