/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OpaqueTechniqueModule_H___
#define ___C3D_OpaqueTechniqueModule_H___

#include "Castor3D/Render/Technique/TechniqueModule.hpp"

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
	*	Enumerator of textures used by the deferred renderer's GBUffer.
	*\~french
	*\brief
	*	Enumération des textures utilisées par le GBuffer du rendu différé.
	*/
	enum class DsTexture
		: uint8_t
	{
		eData0, // R => Depth, G => Linearised Depth, G => Object ID, A => Material ID
		eData1, // RGB => Normal, A => Object Material flags
		eData2, // RGB => Diffuse/Albedo - SSR: A => Shininess - PBR: A => Roughness
		eData3, // RGB => Specular - PBR: A => Metalness - SSR: A => Unused
		eData4, // RGB => Emissive, A => Transmittance
		eData5, // RG => Velocity, B => , A => AO
		CU_ScopedEnumBounds( eData0 ),
	};
	castor::String getTextureName( DsTexture texture );
	castor::String getName( DsTexture texture );
	VkFormat getFormat( DsTexture texture );
	VkClearValue getClearValue( DsTexture texture );
	VkImageUsageFlags getUsageFlags( DsTexture texture );
	VkBorderColor getBorderColor( DsTexture texture );
	inline uint32_t getMipLevels( DsTexture texture, castor::Size const & size )
	{
		return 1u;
	}
	/*
	*\~english
	*\brief
	*	The deferred rendering class.
	*\~french
	*\brief
	*	Classe de rendu différé.
	*/
	class DeferredRendering;
	/**
	*\~english
	*\brief
	*	Handles the indirect lighting passes.
	*\~french
	*\brief
	*	Gère les passes d'éclairage indirect.
	*/
	class IndirectLightingPass;
	/**
	*\~english
	*\brief
	*	Handles the direct lighting passes.
	*\~french
	*\brief
	*	Gère les passes d'éclairage direct.
	*/
	class LightingPass;
	/**
	*\~english
	*\brief
	*	The result of the geometry pass in deferred rendering.
	*\~french
	*\brief
	*	Résultat de la passe de géométries dans le rendu différé.
	*/
	class OpaquePassResult;
	/**
	*\~english
	*\brief
	*	Deferred lighting Render technique pass.
	*\~french
	*\brief
	*	Classe de passe de technique de rendu implémentant le Deferred lighting.
	*/
	class OpaquePass;
	/**
	*\~english
	*\brief
	*	The post lighting resolve pass.
	*\~french
	*\brief
	*	La passe de résolution post éclairage.
	*/
	class OpaqueResolvePass;

	CU_DeclareCUSmartPtr( castor3d, DeferredRendering, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, IndirectLightingPass, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, LightingPass, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, OpaqueResolvePass, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, OpaquePassResult, C3D_API );

	//@}
	//@}
	//@}
}

#endif
