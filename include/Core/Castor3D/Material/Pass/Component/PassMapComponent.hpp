/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassMapComponent_H___
#define ___C3D_PassMapComponent_H___

#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Material/Pass/Component/PassComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureSourceInfo.hpp"

namespace castor3d
{
	using TextureSourceMap = std::unordered_map< TextureSourceInfo
		, PassTextureConfig
		, TextureSourceInfoHasher >;
	using TextureSourceMapVT = TextureSourceMap::value_type;
	using TextureSourceSet = std::unordered_set< TextureSourceInfo, TextureSourceInfoHasher >;

	struct PassMapComponent
		: public PassComponent
	{
		/**
		*\name
		*	Construction / Destruction.
		*/
		/**@{*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	pass	The parent pass.
		 *\param[in]	type	The component type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass	Le sous-maillage pass.
		 *\param[in]	type	Le type de composant.
		 */
		C3D_API PassMapComponent( Pass & pass
			, castor::String const & type );
		/**@}*/
		/**
		*\~english
		*\name
		*	Getters.
		*\~french
		*\name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API virtual TextureFlags getTextureFlags()const
		{
			return TextureFlag::eNone;
		}
		/**@}*/
	};
}

#endif
