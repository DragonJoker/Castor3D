/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderDebugModule_H___
#define ___C3D_RenderDebugModule_H___

#include "Castor3D/Castor3DModule.hpp"

#include <CastorUtils/Config/SmartPtr.hpp>

#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>

namespace c3d
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

	struct DebugIndexBuffer
	{
		VkBuffer buffer;
		VkDeviceSize offset;
		VkDeviceSize count;
	};

	struct DebugVertexBuffers
	{
		std::vector< VkBuffer > buffers;
		std::vector< VkDeviceSize > offsets;
		VkDeviceSize count;
	};

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, DebugDrawer, C3D_API );
	/** @endcond */

	/**
	 *\~english
	 *\brief		Adds a buffer containing AABBs to draw in debug drawer.
	 *\param[in]	drawer				The debug drawer.
	 *\param[in]	descriptorBindings	The shader data bindings.
	 *\param[in]	descriptorWrites	The shader data.
	 *\param[in]	instanceCount		The number of instances to draw.
	 *\param[in]	shader				The shader used to draw the AABB.
	 *\param[in]	enableDepthTest		\p true to enable depth test.
	 *\~french
	 *\brief		Ajoute un buffer d'AABB à dessiner dans le debug drawer.
	 *\param[in]	drawer				Le debug drawer.
	 *\param[in]	descriptorBindings	Les bindings des données à passer au shader.
	 *\param[in]	descriptorWrites	Les données à passer au shader.
	 *\param[in]	instanceCount		Le nombre d'instances à dessiner.
	 *\param[in]	shader				Le shader utilisé pour dessiner les AABB.
	 *\param[in]	enableDepthTest		\p true pour activer le depth test.
	 */
	C3D_API void addDebugAabbs( DebugDrawer & drawer
		, ashes::VkDescriptorSetLayoutBindingArray const & descriptorBindings
		, ashes::WriteDescriptorSetArray const & descriptorWrites
		, VkDeviceSize instanceCount
		, ashes::PipelineShaderStageCreateInfoArray const & shader
		, bool enableDepthTest );
	/**
	 *\~english
	 *\brief		Adds a drawable geometry description to draw in debug drawer.
	 *\param[in]	drawer				The debug drawer.
	 *\param[in]	vertexBuffers		The geometry vertex buffers.
	 *\param[in]	indexBuffer			The geometry index buffers.
	 *\param[in]	vertexAttributes	The geometry vertex attributes.
	 *\param[in]	vertexBindings		The geometry vertex bindings.
	 *\param[in]	descriptorBindings	The shader data bindings.
	 *\param[in]	descriptorWrites	The shader data.
	 *\param[in]	instanceCount		The number of instances to draw.
	 *\param[in]	shader				The shader used to draw the geometries.
	 *\param[in]	enableDepthTest		\p true to enable depth test.
	 *\~french
	 *\brief		Ajoute la description de géométries à dessiner dans le debug drawer.
	 *\param[in]	drawer				Le debug drawer.
	 *\param[in]	vertexBuffers		Les vertex buffers de la géométrie.
	 *\param[in]	indexBuffer			L'index buffer de la géométrie.
	 *\param[in]	vertexAttributes	Les attributs de sommet de la géométrie.
	 *\param[in]	vertexBindings		Les attaches de sommet de la géométrie.
	 *\param[in]	descriptorBindings	Les bindings des données à passer au shader.
	 *\param[in]	descriptorWrites	Les données à passer au shader.
	 *\param[in]	instanceCount		Le nombre d'instances à dessiner.
	 *\param[in]	shader				Le shader utilisé pour dessiner les géometries.
	 *\param[in]	enableDepthTest		\p true pour activer le depth test.
	 */
	C3D_API void addDebugDrawable( DebugDrawer & drawer
		, DebugVertexBuffers const & vertexBuffers
		, DebugIndexBuffer const & indexBuffer
		, ashes::VkVertexInputAttributeDescriptionArray const & vertexAttributes
		, ashes::VkVertexInputBindingDescriptionArray const & vertexBindings
		, ashes::VkDescriptorSetLayoutBindingArray const & descriptorBindings
		, ashes::WriteDescriptorSetArray const & descriptorWrites
		, VkDeviceSize instanceCount
		, ashes::PipelineShaderStageCreateInfoArray const & shader
		, bool enableDepthTest );

	//@}
	//@}
}

#endif
