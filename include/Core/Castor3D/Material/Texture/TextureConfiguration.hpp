/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureConfiguration_H___
#define ___C3D_TextureConfiguration_H___
#pragma once

#include "TextureModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Graphics/GraphicsModule.hpp>
#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ashespp/Image/ImageCreateInfo.hpp>

namespace castor3d
{
	struct TextureTransform
	{
		castor::Point4f translate{ 0, 0, 0, 0 };
		castor::Angle rotate;
		castor::Point4f scale{ 1, 1, 1, 0 };
	};
	/**
	*\~english
	*name
	*	Comparison operators.
	*\~french
	*name
	*	Opérateurs de comparaison.
	*/
	/**@{*/
	C3D_API bool operator==( TextureTransform const & lhs
		, TextureTransform const & rhs )noexcept;
	inline bool operator!=( TextureTransform const & lhs
		, TextureTransform const & rhs )noexcept
	{
		return !( lhs == rhs );
	}
	/**@}*/
	struct TextureFlagConfiguration
	{
		/**
		*\~english
		*\brief
		*	To know what this configuration relates to.
		*\~french
		*\brief
		*	Pour savoir à quoi cette configuration est liée.
		*/
		PassComponentTextureFlag flag{};
		/**
		*\~english
		*\brief
		*	Mask, used with start index to filter image components (AARRGGBB).
		*\~french
		*\brief
		*	Masque, utilisés avec l'indice de début pour filtrer les composantes d'une image (AARRGGBB).
		*/
		uint32_t componentsMask{};
		/**
		*\~english
		*\brief
		*	Start index, used with the mask to filter image components (AARRGGBB).
		*\~french
		*\brief
		*	Indice de début, utilisé avec le masque pour filtrer les composantes d'une image (AARRGGBB).
		*/
		uint32_t startIndex{};
	};
	/**
	*\~english
	*name
	*	Comparison operators.
	*\~french
	*name
	*	Opérateurs de comparaison.
	*/
	/**@{*/
	C3D_API bool shallowEqual( TextureFlagConfiguration const & lhs
		, TextureFlagConfiguration const & rhs );
	C3D_API bool operator==( TextureFlagConfiguration const & lhs
		, TextureFlagConfiguration const & rhs )noexcept;
	inline bool operator!=( TextureFlagConfiguration const & lhs
		, TextureFlagConfiguration const & rhs )noexcept
	{
		return !( lhs == rhs );
	}
	/**@}*/
	struct TextureConfiguration
	{
		/**
		*\~english
		*\brief
		*	The configuration per RGBA component.
		*\~french
		*\brief
		*	Les configurations par composante RGBA.
		*/
		TextureFlagConfigurations components;
		/**
		*\~english
		*name
		*	Factors.
		*\~french
		*name
		*	Facteurs.
		*/
		/**@{*/
		float normalFactor{ 1.0f };
		float heightFactor{ 0.1f };
		/**@}*/
		/**
		*\~english
		*name
		*	Miscellaneous.
		*\~french
		*name
		*	Divers.
		*/
		/**@{*/
		float normalGMultiplier{ 1.0f };
		uint32_t needsYInversion{ 0u };
		TextureTransform transform;
		castor::Point4ui tileSet;
		uint32_t tiles{ 1u };
		TextureSpaces textureSpace{};
		/**@}*/
		/**
		*\~english
		*name
		*	Component masks.
		*\~french
		*name
		*	Masques de composante.
		*/
		/**@{*/
		static uint32_t constexpr AlphaMask = 0xFF000000;
		static uint32_t constexpr BlueMask = 0x00FF0000;
		static uint32_t constexpr GreenMask = 0x0000FF00;
		static uint32_t constexpr RedMask = 0x000000FF;
		static uint32_t constexpr RgMask = RedMask | GreenMask;
		static uint32_t constexpr RgbMask = RgMask | BlueMask;
		static uint32_t constexpr RgbaMask = RgbMask | AlphaMask;
		/**@}*/
	};
	/**
	*\~english
	*name
	*	Comparison operators.
	*\~french
	*name
	*	Opérateurs de comparaison.
	*/
	/**@{*/
	C3D_API bool shallowEqual( TextureConfiguration const & lhs
		, TextureConfiguration const & rhs );
	C3D_API bool operator==( TextureConfiguration const & lhs
		, TextureConfiguration const & rhs );
	inline bool operator!=( TextureConfiguration const & lhs
		, TextureConfiguration const & rhs )noexcept
	{
		return !( lhs == rhs );
	}
	/**@}*/
	/**
	*\~english
	*name
	*	Other functions.
	*\~french
	*name
	*	Autres fonctions.
	*/
	/**@{*/
	C3D_API TextureFlagsArray getFlags( TextureConfiguration const & config );
	C3D_API castor::PixelComponents getPixelComponents( uint32_t mask );
	C3D_API castor::PixelComponents getPixelComponents( TextureConfiguration const & config );
	C3D_API void updateIndices( castor::PixelFormat format
		, TextureConfiguration & config );
	C3D_API TextureFlagConfigurations::const_iterator checkFlags( TextureFlagConfigurations const & lhs
		, PassComponentTextureFlag rhs );
	C3D_API TextureFlagConfigurations::iterator checkFlags( TextureFlagConfigurations & lhs
		, PassComponentTextureFlag rhs );
	C3D_API void addFlagConfiguration( TextureConfiguration & config
		, TextureFlagConfiguration flagConfiguration );
	C3D_API TextureFlagConfiguration & getFlagConfiguration( TextureConfiguration & config
		, PassComponentTextureFlag textureFlag );
	C3D_API uint32_t getComponentsMask( TextureConfiguration const & config
		, PassComponentTextureFlag textureFlag );
	C3D_API PassComponentTextureFlag getEnabledFlag( TextureConfiguration const & config );
	C3D_API void mergeConfigs( TextureConfiguration const & lhs
		, TextureConfiguration & rhs );
	C3D_API void mergeConfigsBase( TextureConfiguration const & lhs
		, TextureConfiguration & rhs );
	C3D_API TextureFlagConfigurations::const_iterator findFirstEmpty( TextureConfiguration const & config );
	C3D_API TextureFlagConfigurations::iterator findFirstEmpty( TextureConfiguration & config );
	C3D_API TextureFlagConfigurations::const_iterator findFirstNonEmpty( TextureConfiguration const & config );
	C3D_API TextureFlagConfigurations::iterator findFirstNonEmpty( TextureConfiguration & config );
	/**@}*/

	struct PassTextureConfig
	{
		ashes::ImageCreateInfo imageInfo{ {} };
		TextureConfiguration config{};
		uint32_t texcoordSet{};
	};
}

#endif
