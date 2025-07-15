/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderEnvironmentMapModule_H___
#define ___C3D_RenderEnvironmentMapModule_H___

#include "Castor3D/Render/RenderModule.hpp"

namespace c3d
{
	/**@name Render */
	//@{
	/**@name EnvironmentMap */
	//@{

	/**
	*\~english
	*\brief
	*	Reflection mapping implementation.
	*\~french
	*\brief
	*	Implémentation du reflection mapping.
	*/
	class EnvironmentMap;
	/**
	*\~english
	*\brief
	*	Reflection mapping pass implementation.
	*\~french
	*\brief
	*	Implémentation d'une passe de reflection mapping.
	*/
	class EnvironmentMapPass;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, EnvironmentMap, C3D_API );
	CU_DeclareSmartPtr( c3d, EnvironmentMapPass, C3D_API );
	/** @endcond */

	//@}
	//@}
}

#endif
