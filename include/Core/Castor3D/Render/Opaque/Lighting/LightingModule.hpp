/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightingModule_H___
#define ___C3D_LightingModule_H___

#include "Castor3D/Render/Opaque/OpaqueModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Scene/Light/LightModule.hpp"

#include <CastorUtils/Graphics/Size.hpp>

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Technique */
	//@{
	/**@name Opaque */
	//@{
	/**@name Lighting */
	//@{

	/**
	*\~english
	*\brief
	*	The lighting passes common bindings index.
	*\~french
	*\brief
	*	L'index des binding communs des passes d'éclairage.
	*/
	enum class LightPassIdx
		: uint32_t
	{
		eMaterials,
		eSssProfiles,
		eModels,
		eCamera,
		eDepthObj,
		eNmlOcc,
		eColMtl,
		eSpcRgh,
		eEmsTrn,
		eCount,
	};
	/**
	*\~english
	*\brief
	*	The lighting passes light specific bindings index.
	*\~french
	*\brief
	*	L'index des bindings spécifiques à la source lumineuse, pour les passes d'éclairage.
	*/
	enum class LightPassLgtIdx
		: uint32_t
	{
		eLights,
		eMatrix,
		eModelMatrix,
		eSmLinear,
		eSmLinearCmp,
		eSmVariance,
		eShadowBuffer,
		eRandomStorage,
		eCount,
	};
	/**
	*\~english
	*\brief
	*	The lighting passes light specific bindings index.
	*\~french
	*\brief
	*	L'index des bindings spécifiques à la source lumineuse, pour les passes d'éclairage.
	*/
	enum class ClusteredLightPassLgtIdx
		: uint32_t
	{
		eLights,
		eMatrix,
		eClusters,
		ePointLightGridIndex,
		ePointLightGridCluster,
		eSpotLightGridIndex,
		eSpotLightGridCluster,
		eCount,
	};
	/**
	*\~english
	*\brief
	*	Enumerator of textures used by the lighting pass in deferred rendering.
	*\~french
	*\brief
	*	Enumération des textures utilisées par la passe d'éclairage lors du rendu différé.
	*/
	enum class LpTexture
		: uint8_t
	{
		eDiffuse,
		eSpecular,
		eScattering,
		eIndirectDiffuse,
		eIndirectSpecular,
		CU_ScopedEnumBounds( eDiffuse ),
	};
	castor::String getTextureName( LpTexture texture );
	castor::String getTexName( LpTexture texture );
	VkFormat getFormat( RenderDevice const & device, LpTexture texture );
	VkClearValue getClearValue( LpTexture texture );
	VkImageUsageFlags getUsageFlags( LpTexture texture );
	VkBorderColor getBorderColor( LpTexture texture );
	inline uint32_t getMipLevels( RenderDevice const & device
		, LpTexture texture
		, castor::Size const & size )
	{
		return 1u;
	}
	inline uint32_t getMipLevels( RenderDevice const & device
		, LpTexture texture
		, VkExtent3D const & size )
	{
		return getMipLevels( device
			, texture
			, castor::Size{ size.width, size.height } );
	}
	inline VkCompareOp getCompareOp( LpTexture texture )
	{
		return VK_COMPARE_OP_NEVER;
	}
	/**
	*\~english
	*\brief
	*	The result of the light pass in deferred rendering.
	*\~french
	*\brief
	*	Résultat de la passe d'éclairage dans le rendu différé.
	*/
	class LightPassResult;
	/**
	*\~english
	*\brief
	*	Base class for all light passes.
	*\remarks
	*	The result of each light pass is blended with the previous one.
	*\~french
	*\brief
	*	Classe de base pour toutes les passes d'éclairage.
	*\remarks
	*	Le résultat de chaque passe d'éclairage est mélangé avec celui de la précédente.
	*/
	class LightPass;
	/**
	*\~english
	*\brief
	*	Directional light pass.
	*\~french
	*\brief
	*	Passe de lumière directionnelle.
	*/
	class DirectionalLightPass;
	/**
	*\~english
	*\brief
	*	Base class for light passes that need a mesh instead of a quad.
	*\~french
	*\brief
	*	Classe de base pour les passes d'éclairage nécessitant un maillage plutôt qu'un quad.
	*/
	class MeshLightPass;
	/**
	*\~english
	*\brief
	*	Point light pass.
	*\~french
	*\brief
	*	Passe de lumière omnidirectionnelle.
	*/
	class PointLightPass;
	/**
	*\~english
	*\brief
	*	Spot light pass.
	*\~french
	*\brief
	*	Passe de lumière projecteur.
	*/
	class SpotLightPass;
	/**
	*\~english
	*\brief
	*	Subsurface scattering pass.
	*\~french
	*\brief
	*	Passe de Subsurface scattering.
	*/
	class SubsurfaceScatteringPass;

	CU_DeclareSmartPtr( castor3d, SubsurfaceScatteringPass, C3D_API );

	//@}
	//@}
	//@}
	//@}
}

#endif
