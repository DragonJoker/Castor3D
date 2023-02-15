/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderShadowMapModule_H___
#define ___C3D_RenderShadowMapModule_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include <CastorUtils/Graphics/Size.hpp>

#include <RenderGraph/FrameGraphPrerequisites.hpp>

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name ShadowMap */
	//@{
	/**
	*\~english
	*\brief
	*	Enumerator of textures used in deferred rendering.
	*\~french
	*\brief
	*	Enumération des textures utilisées lors du rendu différé.
	*/
	enum class SmTexture
		: uint8_t
	{
		eDepth,			// D24S8 Depth stencil buffer
		eLinearDepth,	// R32F Shadow map used for raw and PCF shadow maps
		eVariance,		// R32G32F Shadow map used by VSM.
		eNormal,		// R16G16B16A16F, RGB => World space normal used by RSM.
		ePosition,		// R16G16B16A16F, RGB => World space position used by RSM.
		eFlux,			// R16G16B16A16F, RGB => Light flux, used by RSM.
		CU_ScopedEnumBounds( eDepth ),
	};
	C3D_API castor::String getTextureName( LightType light
		, SmTexture texture );
	C3D_API castor::String getTexName( SmTexture texture );
	C3D_API VkFormat getFormat( RenderDevice const & device, SmTexture texture );
	C3D_API VkClearValue getClearValue( SmTexture texture );
	C3D_API VkImageUsageFlags getUsageFlags( SmTexture texture );
	C3D_API VkBorderColor getBorderColor( SmTexture texture );
	C3D_API VkCompareOp getCompareOp( SmTexture texture );
	C3D_API uint32_t getMipLevels( RenderDevice const & device
		, SmTexture texture
		, castor::Size const & size );
	inline uint32_t getMipLevels( RenderDevice const & device
		, SmTexture texture
		, VkExtent3D const & size )
	{
		return getMipLevels( device
			, texture
			, castor::Size{ size.width, size.height } );
	}
	/**
	*\~english
	*\brief
	*	Shadow mapping implementation for directional lights.
	*\~french
	*\brief
	*	Implémentation du mappage d'ombres pour les lumières directionnelles.
	*/
	class ShadowMapDirectional;
	/**
	*\~english
	*\brief
	*	Shadow mapping implementation.
	*\~french
	*\brief
	*	Implémentation du mappage d'ombres.
	*/
	class ShadowMapPoint;
	/**
	*\~english
	*\brief
	*	Shadow mapping implementation for spot lights.
	*\~french
	*\brief
	*	Implémentation du mappage d'ombres pour les lumières spot.
	*/
	class ShadowMapSpot;
	/**
	*\~english
	*\brief
	*	Shadow mapping implementation.
	*\~french
	*\brief
	*	Implémentation du mappage d'ombres.
	*/
	class ShadowMapPass;
	/**
	*\~english
	*\brief
	*	The textures resulting from a shadow map.
	*\~french
	*\brief
	*	Les textures résultant d'une shadow map.
	*/
	class ShadowMapResult;
	/**
	*\~english
	*\brief
	*	Shadow mapping implementation for spot lights.
	*\~french
	*\brief
	*	Implémentation du mappage d'ombres pour les lumières spot.
	*/
	class ShadowMapPassDirectional;
	/**
	*\~english
	*\brief
	*	Shadow mapping implementation.
	*\~french
	*\brief
	*	Implémentation du mappage d'ombres.
	*/
	class ShadowMapPassPoint;
	/**
	*\~english
	*\brief
	*	Shadow mapping implementation for spot lights.
	*\~french
	*\brief
	*	Implémentation du mappage d'ombres pour les lumières spot.
	*/
	class ShadowMapPassSpot;

	CU_DeclareCUSmartPtr( castor3d, ShadowMap, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, ShadowMapPass, C3D_API );

	//@}
	//@}
}

#endif
