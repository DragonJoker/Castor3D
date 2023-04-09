/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TransparentTechniqueModule_H___
#define ___C3D_TransparentTechniqueModule_H___

#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Graphics/Size.hpp>

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Technique */
	//@{
	/**@name Transparent */
	//@{

	/**
	\~english
	\brief		Enumerator of textures used in weighted blend OIT.
	\~french
	\brief		Enumération des textures utilisées lors du weighted blend OIT.
	*/
	enum class WbTexture
		: uint8_t
	{
		eAccumulation,
		eRevealage,
		CU_ScopedEnumBounds( eAccumulation ),
	};
	C3D_API castor::String getTextureName( WbTexture texture );
	C3D_API castor::String getTexName( WbTexture texture );
	C3D_API VkFormat getFormat( RenderDevice const & device
		, WbTexture texture );
	C3D_API VkClearValue getClearValue( WbTexture texture );
	C3D_API VkImageUsageFlags getUsageFlags( WbTexture texture );
	C3D_API VkBorderColor getBorderColor( WbTexture texture );
	inline uint32_t getMipLevels( RenderDevice const & device
		, WbTexture texture
		, castor::Size const & size )
	{
		return 1u;
	}
	inline uint32_t getMipLevels( RenderDevice const & device
		, WbTexture texture
		, VkExtent3D const & size )
	{
		return getMipLevels( device
			, texture
			, castor::Size{ size.width, size.height } );
	}
	inline VkCompareOp getCompareOp( WbTexture texture )
	{
		return VK_COMPARE_OP_NEVER;
	}
	/**
	*\~english
	*\brief
	*	Transparent nodes pass using Weighted Blend OIT.
	*\~french
	*\brief
	*	Passe pour les noeuds transparents, utilisant le Weighted Blend OIT.
	*/
	class TransparentPass;
	/**
	*\~english
	*\brief
	*	Result of the transparent accumulation pass.
	*\~french
	*\brief
	*	Résultat de la passe d'accumulation de transparence.
	*/
	class TransparentPassResult;
	/**
	*\~english
	*\brief
	*	The render technique part dedicated to transparent nodes.
	*\~french
	*\brief
	*	La partie de render technique dédiée aux noeuds transparents.
	*/
	class TransparentRendering;
	/**
	*\~english
	*\brief
	*	Handles the weighted blend rendering.
	*\~french
	*\brief
	*	Gère le rendu du weighted blend.
	*/
	class WeightedBlendRendering;

	CU_DeclareSmartPtr( castor3d, WeightedBlendRendering, C3D_API );
	CU_DeclareSmartPtr( castor3d, TransparentPassResult, C3D_API );
	CU_DeclareSmartPtr( castor3d, TransparentRendering, C3D_API );

	//@}
	//@}
}

#endif
