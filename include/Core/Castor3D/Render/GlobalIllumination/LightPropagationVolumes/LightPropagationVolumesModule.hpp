/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPropagationVolumesModule_H___
#define ___C3D_LightPropagationVolumesModule_H___

#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

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
		eR,
		eG,
		eB,
		CU_ScopedEnumBounds( eR ),
	};
	castor::String getTextureName( LpvTexture texture
		, std::string const & infix );
	castor::String getName( LpvTexture texture );
	VkFormat getFormat( LpvTexture texture );
	VkClearValue getClearValue( LpvTexture texture );
	VkImageUsageFlags getUsageFlags( LpvTexture texture );
	VkBorderColor getBorderColor( LpvTexture texture );
	inline uint32_t getMipLevels( LpvTexture texture
		, castor::Size const & size )
	{
		return 1u;
	}
	inline uint32_t getMipLevels( LpvTexture texture
		, VkExtent3D const & size )
	{
		return getMipLevels( texture
			, castor::Size{ size.width, size.height } );
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
	*	Lighting propagation volumes pass resulting buffers.
	*\~french
	*\brief
	*	Buffers résultant d'une passe de light propagation volumes.
	*/
	class LightVolumePassResult;
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

	CU_DeclareCUSmartPtr( castor3d, GeometryInjectionPass, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, LayeredLightPropagationVolumes, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, LayeredLightPropagationVolumesG, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, LightInjectionPass, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, LightPropagationPass, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, LightPropagationVolumes, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, LightPropagationVolumesG, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, LightVolumePassResult, C3D_API );

	namespace lpv
	{
		// 2 passes: First and Blend.
		static uint32_t constexpr ResolvePassCount = 2u;
		// 2 passes: with or without geometry occlusion.
		static uint32_t constexpr PropagationPassCount = 2u;
	}

	using LightPropagationVolumesLightType = std::array< LightPropagationVolumesUPtr, size_t( LightType::eCount ) >;
	using LayeredLightPropagationVolumesLightType = std::array< LayeredLightPropagationVolumesUPtr, size_t( LightType::eCount ) >;
	using LightPropagationVolumesGLightType = std::array< LightPropagationVolumesGUPtr, size_t( LightType::eCount ) >;
	using LayeredLightPropagationVolumesGLightType = std::array< LayeredLightPropagationVolumesGUPtr, size_t( LightType::eCount ) >;

	CU_DeclareArray( LightPropagationPassUPtr, lpv::PropagationPassCount, LightPropagationPass );

	CU_DeclareVector( GeometryInjectionPass, GeometryInjectionPass );
	CU_DeclareVector( LightInjectionPass, LightInjectionPass );
	CU_DeclareVector( LightVolumePassResultUPtr, LightVolumePassResult );

	//@}
	//@}
	//@}
}

#endif
