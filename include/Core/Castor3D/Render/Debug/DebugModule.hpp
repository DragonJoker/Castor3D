/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderDebugModule_H___
#define ___C3D_RenderDebugModule_H___

#include "Castor3D/Castor3DModule.hpp"

#include <CastorUtils/Config/SmartPtr.hpp>

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Debug */
	//@{
	/**
	*\~english
	*\brief
	*	Draws debug geometries.
	*\~french
	*\brief
	*	Dessine les géométries de debug.
	*/
	class DebugDrawer;

	CU_DeclareSmartPtr( castor3d, DebugDrawer, C3D_API );

	//@}
	//@}
}

#endif
