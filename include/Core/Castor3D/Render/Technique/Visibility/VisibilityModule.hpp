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
	*	Enumerator of textures used by the visibility buffer.
	*\~french
	*\brief
	*	Enumération des textures utilisées par le buffer de visibilité.
	*/
	enum class VbTexture
		: uint8_t
	{
		eDepth, // D => Depth
		eData, // R => NodeID, G => DrawID, B => PrimitiveID, A => Unused
		CU_ScopedEnumBounds( eData ),
	};
	C3D_API castor::String getTextureName( VbTexture texture );
	C3D_API castor::String getTexName( VbTexture texture );
	C3D_API VkFormat getFormat( RenderDevice const & device, VbTexture texture );
	C3D_API VkClearValue getClearValue( VbTexture texture );
	C3D_API VkImageUsageFlags getUsageFlags( VbTexture texture );
	C3D_API VkBorderColor getBorderColor( VbTexture texture );
	inline uint32_t getMipLevels( RenderDevice const & device
		, VbTexture texture
		, castor::Size const & size )
	{
		return 1u;
	}
	/**
	*\~english
	*\brief
	*	The render nodes pass writing visibility buffer.
	*\~french
	*\brief
	*	La passe de noeuds de rendu générant le buffer de visibilité.
	*/
	class VisibilityPass;
	/**
	*\~english
	*\brief
	*	The result of the visibility pass.
	*\~french
	*\brief
	*	Résultat de la passe de visibilité.
	*/
	class VisibilityPassResult;
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

	CU_DeclareCUSmartPtr( castor3d, VisibilityPass, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, VisibilityPassResult, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, VisibilityReorderPass, C3D_API );

	//@}
	//@}
	//@}
}

#endif
