/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PrepassModule_H___
#define ___C3D_PrepassModule_H___

#include "Castor3D/Render/RenderModule.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Prepass */
	//@{

	/**
	*\~english
	*\brief
	*	Enumerator of textures used by the visibility passes.
	*\~french
	*\brief
	*	Enumération des textures utilisées par les passes de visibilité.
	*/
	enum class PpTexture
		: uint8_t
	{
		eDepthObj = 0, // R => Normalised depth, G => Linear depth, B => Node ID, A => Lighting Model ID
		eVisibility = 1, // R => node ID + pipeline ID, G => primitive ID
		CU_ScopedEnumBounds( eDepthObj, eVisibility ),
	};
	C3D_API castor::String getTextureName( PpTexture texture );
	C3D_API castor::String getTexName( PpTexture texture );
	C3D_API VkFormat getFormat( RenderDevice const & device, PpTexture texture );
	C3D_API VkClearValue getClearValue( PpTexture texture );
	C3D_API VkImageUsageFlags getUsageFlags( PpTexture texture );
	C3D_API VkBorderColor getBorderColor( PpTexture texture );
	inline uint32_t getMipLevels( RenderDevice const & /*device*/
		, PpTexture /*texture*/
		, castor::Size const & /*size*/ )
	{
		return 1u;
	}
	inline VkCompareOp getCompareOp( PpTexture /*texture*/ )
	{
		return VK_COMPARE_OP_NEVER;
	}
	/**
	*\~english
	*\brief
	*	Depth prepass.
	*\~french
	*\brief
	*	Classe de prépasse de profondeur.
	*/
	class DepthPass;
	/**
	*\~english
	*\brief
	*	The render technique part dedicated to prepass.
	*\~french
	*\brief
	*	La partie de render technique dédiée à la prépasse.
	*/
	class PrepassRendering;
	/**
	*\~english
	*\brief
	*	The result of the prepass.
	*\~french
	*\brief
	*	Résultat de la pré-passe.
	*/
	class PrepassResult;
	/**
	*\~english
	*\brief
	*	The render nodes pass writing visibility buffer.
	*\~french
	*\brief
	*	La passe de noeuds de rendu générant le buffer de visibilité.
	*/
	class VisibilityPass;

	CU_DeclareSmartPtr( castor3d, DepthPass, C3D_API );
	CU_DeclareSmartPtr( castor3d, PrepassRendering, C3D_API );
	CU_DeclareSmartPtr( castor3d, PrepassResult, C3D_API );
	CU_DeclareSmartPtr( castor3d, VisibilityPass, C3D_API );

	//@}
	//@}
}

#endif
