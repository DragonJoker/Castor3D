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

	static constexpr uint32_t ShadowMapDirectionalTileCountX = 2u;
	static constexpr uint32_t ShadowMapDirectionalTileCountY = 2u;
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
		eDepth, // D24S8 Depth stencil buffer
		eNormalLinear, // R32G32B32A32F, RGB => World space normal used by RSM, A => Shadow map used for raw and PCF shadow maps
		eVariance, // R32G32F Shadow map used by VSM.
		ePosition, // R16G16B16A16F, RGB => World space position used by RSM.
		eFlux, // R16G16B16A16F, RGB => Light flux, used by RSM.
		CU_ScopedEnumBounds( eDepth ),
	};
	castor::String getTextureName( LightType light
		, SmTexture texture );
	castor::String getName( SmTexture texture );
	VkFormat getFormat( SmTexture texture );
	VkClearValue getClearValue( SmTexture texture );
	VkImageUsageFlags getUsageFlags( SmTexture texture );
	VkBorderColor getBorderColor( SmTexture texture );
	uint32_t getMipLevels( SmTexture texture, castor::Size const & size );
	inline uint32_t getMipLevels( SmTexture texture
		, VkExtent3D const & size )
	{
		return getMipLevels( texture
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
	/**
	*\~english
	*\brief
	*	Writes the shadow maps descriptors to the given writes.
	*\~french
	*\brief
	*	Ecrit les descripteurs des shadow maps dans les writes donnés.
	*/
	void bindShadowMaps( crg::RunnableGraph const & graph
		, PipelineFlags const & pipelineFlags
		, ShadowMapLightTypeArray const & shadowMaps
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index );

	//@}
	//@}
}

#endif
