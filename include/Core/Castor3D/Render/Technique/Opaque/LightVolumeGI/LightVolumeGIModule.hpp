/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightVolumeGIModule_H___
#define ___C3D_LightVolumeGIModule_H___

#include "Castor3D/Render/Technique/Opaque/OpaqueModule.hpp"

#include "Castor3D/Scene/Light/LightModule.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Technique */
	//@{
	/**@name Opaque */
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
	*	Lighting propagation volumes resolution pass.
	*\~french
	*\brief
	*	Passe de résolution du light propagation volumes.
	*/
	class LightVolumeGIPass;
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
	*	Base class for all light passes with light propagation volumes.
	*\~french
	*\brief
	*	Classe de base pour toutes les passes d'éclairage avec du light propagation volumes.
	*/
	template< LightType LtType >
	class LightPassVolumePropagationShadow;

	//@}
	//@}
	//@}
	//@}
}

#endif
