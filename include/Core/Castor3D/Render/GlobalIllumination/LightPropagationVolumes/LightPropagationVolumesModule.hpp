/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPropagationVolumesModule_H___
#define ___C3D_LightPropagationVolumesModule_H___

#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Limits.hpp"

#include <CastorUtils/Graphics/Size.hpp>

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Global Illumination */
	//@{
	/**@name Light Propagation Volumes */
	//@{

	/**
	*\~english
	*\brief
	*	Enumerator of textures used by the light volume GI passes.
	*\~french
	*\brief
	*	Enumération des textures utilisées par les passes de light volume GI.
	*/
	enum class LpvTexture
		: uint8_t
	{
		eR = 0,
		eG = 1,
		eB = 2,
		CU_ScopedEnumBounds( eR, eB ),
	};
	C3D_API castor::String getTextureName( LpvTexture texture
		, std::string const & infix );
	C3D_API castor::String getTexName( LpvTexture texture );
	C3D_API VkFormat getFormat( RenderDevice const & device, LpvTexture texture );
	C3D_API VkClearValue getClearValue( LpvTexture texture );
	C3D_API VkImageUsageFlags getUsageFlags( LpvTexture texture );
	C3D_API VkBorderColor getBorderColor( LpvTexture texture );
	inline uint32_t getMipLevels( RenderDevice const & /*device*/
		, LpvTexture /*texture*/
		, castor::Size const & /*size*/ )
	{
		return 1u;
	}
	inline uint32_t getMipLevels( RenderDevice const & device
		, LpvTexture texture
		, VkExtent3D const & size )
	{
		return getMipLevels( device
			, texture
			, castor::Size{ size.width, size.height } );
	}
	inline VkCompareOp getCompareOp( LpvTexture /*texture*/ )
	{
		return VK_COMPARE_OP_NEVER;
	}
	/**
	*\~english
	*\brief
	*	Geometry injection pass.
	*\~french
	*\brief
	*	Passe d'injection des géométries.
	*/
	class GeometryInjectionPass;
	/**
	*\~english
	*\brief
	*	Lighting propagation volumes generation pass.
	*\~french
	*\brief
	*	Passe de génération des light propagation volumes.
	*/
	template< bool InjectGeometryT >
	class LayeredLightPropagationVolumesT;
	/**
	*\~english
	*\brief
	*	Lighting injection pass.
	*\~french
	*\brief
	*	Passe d'injection de l'éclairage.
	*/
	class LightInjectionPass;
	/**
	*\~english
	*\brief
	*	Lighting propagation pass.
	*\~french
	*\brief
	*	Passe de propagation de l'éclairage.
	*/
	class LightPropagationPass;
	/**
	*\~english
	*\brief
	*	Lighting propagation volumes generation pass.
	*\~french
	*\brief
	*	Passe de génération des light propagation volumes.
	*/
	template< bool InjectGeometryT >
	class LightPropagationVolumesT;
	/**
	*\~english
	*\brief
	*	Light propagation data.
	*\~french
	*\brief
	*	Données de light propagation.
	*/
	struct LpvConfig;

	using LayeredLightPropagationVolumes = LayeredLightPropagationVolumesT< false >;
	using LayeredLightPropagationVolumesG = LayeredLightPropagationVolumesT< true >;
	using LightPropagationVolumes = LightPropagationVolumesT< false >;
	using LightPropagationVolumesG = LightPropagationVolumesT< true >;

	CU_DeclareSmartPtr( castor3d, GeometryInjectionPass, C3D_API );
	CU_DeclareSmartPtr( castor3d, LayeredLightPropagationVolumes, C3D_API );
	CU_DeclareSmartPtr( castor3d, LayeredLightPropagationVolumesG, C3D_API );
	CU_DeclareSmartPtr( castor3d, LightInjectionPass, C3D_API );
	CU_DeclareSmartPtr( castor3d, LightPropagationPass, C3D_API );
	CU_DeclareSmartPtr( castor3d, LightPropagationVolumes, C3D_API );
	CU_DeclareSmartPtr( castor3d, LightPropagationVolumesG, C3D_API );

	using LightPropagationVolumesLightType = std::array< LightPropagationVolumesUPtr, size_t( LightType::eCount ) >;
	using LayeredLightPropagationVolumesLightType = std::array< LayeredLightPropagationVolumesUPtr, size_t( LightType::eCount ) >;
	using LightPropagationVolumesGLightType = std::array< LightPropagationVolumesGUPtr, size_t( LightType::eCount ) >;
	using LayeredLightPropagationVolumesGLightType = std::array< LayeredLightPropagationVolumesGUPtr, size_t( LightType::eCount ) >;

	CU_DeclareVector( GeometryInjectionPass, GeometryInjectionPass );
	CU_DeclareVector( LightInjectionPass, LightInjectionPass );

	//@}
	//@}
	//@}
}

#endif
