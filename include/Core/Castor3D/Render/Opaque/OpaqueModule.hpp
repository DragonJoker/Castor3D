/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OpaqueTechniqueModule_H___
#define ___C3D_OpaqueTechniqueModule_H___

#include "Castor3D/Render/RenderModule.hpp"

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
		eNmlOcc, // RGB => Normal, A => Occlusion
		eColMtl, // RGB => Diffuse/Albedo - PBR: A => Metalness - Phong: A => Unused
		eSpcRgh, // RGB => Specular - Phong: A => Shininess - PBR: A => Roughness
		eEmsTrn, // RGB => Emissive, A => Transmittance
		CU_ScopedEnumBounds( eNmlOcc ),
	};
	C3D_API castor::String getTextureName( DsTexture texture );
	C3D_API castor::String getImageName( DsTexture texture );
	C3D_API castor::String getTexName( DsTexture texture );
	C3D_API VkFormat getFormat( RenderDevice const & device, DsTexture texture );
	C3D_API VkClearValue getClearValue( DsTexture texture );
	C3D_API VkImageUsageFlags getUsageFlags( DsTexture texture );
	C3D_API VkBorderColor getBorderColor( DsTexture texture );
	inline uint32_t getMipLevels( RenderDevice const & device
		, DsTexture texture
		, castor::Size const & size )
	{
		return 1u;
	}
	inline VkCompareOp getCompareOp( DsTexture texture )
	{
		return VK_COMPARE_OP_NEVER;
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
	*	The render technique part dedicated to opaque nodes.
	*\~french
	*\brief
	*	La partie de render technique dédiée aux noeuds opaques.
	*/
	class OpaqueRendering;
	/**
	*\~english
	*\brief
	*	The post lighting resolve pass.
	*\~french
	*\brief
	*	La passe de résolution post éclairage.
	*/
	class OpaqueResolvePass;
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
	*	The post visibility resolve pass.
	*\~french
	*\brief
	*	La passe de résolution de la visibilité.
	*/
	class VisibilityResolvePass;

	CU_DeclareSmartPtr( castor3d, DeferredRendering, C3D_API );
	CU_DeclareSmartPtr( castor3d, IndirectLightingPass, C3D_API );
	CU_DeclareSmartPtr( castor3d, LightingPass, C3D_API );
	CU_DeclareSmartPtr( castor3d, OpaquePassResult, C3D_API );
	CU_DeclareSmartPtr( castor3d, OpaqueRendering, C3D_API );
	CU_DeclareSmartPtr( castor3d, OpaqueResolvePass, C3D_API );
	CU_DeclareSmartPtr( castor3d, VisibilityReorderPass, C3D_API );

	//@}
	//@}
	//@}
}

#endif
