/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TransparentTechniqueModule_H___
#define ___C3D_TransparentTechniqueModule_H___

#include "Castor3D/Technique/TechniqueModule.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Technique */
	//@{
	/**@name Transparent */
	//@{

	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Enumerator of textures used in weighted blend OIT.
	\~french
	\brief		Enumération des textures utilisées lors du weighted blend OIT.
	*/
	enum class WbTexture
		: uint8_t
	{
		eDepth,
		eAccumulation,
		eRevealage,
		eVelocity,
		CU_ScopedEnumBounds( eDepth ),
	};
	//!\~english	The weighted blend pass result.
	//!\~french		Le résultat de la passe de weighted blend.
	using WeightedBlendTextures = std::array< ashes::ImageView, size_t( WbTexture::eCount ) >;
	/**
	*\~english
	*\brief
	*	Program used to combine the transparent and opaque passes.
	*\~french
	*\brief
	*	Programme utilisé pour combiner les passes opaque et transparente.
	*/
	struct FinalCombineProgram;
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
	*	Handles the weighted blend rendering.
	*\~french
	*\brief
	*	Gère le rendu du weighted blend.
	*/
	class WeightedBlendRendering;
	/**
	*\~english
	*\brief
	*	Retrieve the name for given texture enum value.
	*\param[in] texture
	*	The value.
	*\return
	*	The name.
	*\~french
	*\brief
	*	Récupère le nom pour la valeur d'énumeration de texture.
	*\param[in] texture
	*	La valeur.
	*\return
	*	Le nom.
	*/
	castor::String getTextureName( WbTexture texture );
	/**
	*\~english
	*\brief
	*	Retrieve the pixel format for given texture enum value.
	*\param[in] texture
	*	The value.
	*\return
	*	The pixel format.
	*\~french
	*\brief
	*	Récupère le format de pixels pour la valeur d'énumeration de texture.
	*\param[in] texture
	*	La valeur.
	*\return
	*	Le format de pixels.
	*/
	VkFormat getTextureFormat( WbTexture texture );
	/**
	*\~english
	*\brief
	*	Retrieve the attachment point for given texture enum value.
	*\param[in] texture
	*	The value.
	*\return
	*	The attachment point.
	*\~french
	*\brief
	*	Récupère le point d'attache pour la valeur d'énumeration de texture.
	*\param[in] texture
	*	La valeur.
	*\return
	*	Le point d'attache.
	*/
	VkImageAspectFlags getTextureAttachmentPoint( WbTexture texture );
	/**
	*\~english
	*\brief
	*	Retrieve the attachment index for given texture enum value.
	*\param[in] texture
	*	The value.
	*\return
	*	The attachment index.
	*\~french
	*\brief
	*	Récupère l'indice d'attache pour la valeur d'énumeration de texture.
	*\param[in] texture
	*	La valeur.
	*\return
	*	L'indice d'attache.
	*/
	uint32_t getTextureAttachmentIndex( WbTexture texture );

	//@}
	//@}
}

#endif
