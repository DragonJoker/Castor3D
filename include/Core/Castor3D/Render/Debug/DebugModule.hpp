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

	/**
	 *\~english
	 *\brief		Adds a buffer containing AABBs to draw.
	 *\param[in]	drawer		The debug drawer.
	 *\param[in]	bindings	The shader data bindings.
	 *\param[in]	writes		The shader data.
	 *\param[in]	count		The number of AABB to draw.
	 *\param[in]	shader		The shader used to draw the AABB.
	 *\~french
	 *\brief		Ajoute un buffer d'AABB à dessiner.
	 *\param[in]	drawer		Le debug drawer.
	 *\param[in]	bindings	Les bindings des données à passer au shader.
	 *\param[in]	writes		Les données à passer au shader.
	 *\param[in]	count		Le nombre d'AABB à dessiner.
	 *\param[in]	shader		Le shader utilisé pour dessiner les AABB.
	 */
	C3D_API void addDebugAabbs( DebugDrawer & drawer
		, ashes::VkDescriptorSetLayoutBindingArray const & bindings
		, ashes::WriteDescriptorSetArray const & writes
		, VkDeviceSize count
		, ashes::PipelineShaderStageCreateInfoArray const & shader
		, bool enableDepthTest );

	//@}
	//@}
}

#endif
