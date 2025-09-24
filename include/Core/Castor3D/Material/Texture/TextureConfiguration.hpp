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

namespace c3d
{
	struct TextureTransform
	{
		Point3f translate{ 0, 0, 0 };
		Angle rotate;
		Point3f scale{ 1, 1, 1 };

	private:
		friend bool operator==( TextureTransform const & lhs
			, TextureTransform const & rhs )noexcept
		{
			return lhs.translate == rhs.translate
				&& lhs.rotate == rhs.rotate
				&& lhs.scale == rhs.scale;
		}
	};

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

	private:
		friend bool operator==( TextureFlagConfiguration const & lhs
			, TextureFlagConfiguration const & rhs )noexcept
		{
			return lhs.flag == rhs.flag
				&& lhs.componentsMask == rhs.componentsMask
				&& lhs.startIndex == rhs.startIndex;
		}
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
	C3D_API size_t getHash( TextureFlagConfiguration const & config );
	/**@}*/
	struct TextureConfiguration
	{
		TextureConfiguration() = default;
		/**
		*\~english
		*\brief
		*	The configuration per RGBA component.
		*\~french
		*\brief
		*	Les configurations par composante RGBA.
		*/
		TextureFlagConfigurations components{};
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
		bool normalDirectX{};
		bool normal2Channels{};
		bool needsYInversion{};
		bool needsXInversion{};
		bool needsZInversion{};
		TextureTransform transform{};
		Point4ui tileSet{};
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

	private:
		friend bool operator==( TextureConfiguration const & lhs
			, TextureConfiguration const & rhs )noexcept
		{
			return lhs.components[0] == rhs.components[0]
				&& lhs.components[1] == rhs.components[1]
				&& lhs.components[2] == rhs.components[2]
				&& lhs.components[3] == rhs.components[3]
				&& lhs.normalFactor == rhs.normalFactor
				&& lhs.heightFactor == rhs.heightFactor
				&& lhs.normalDirectX == rhs.normalDirectX
				&& lhs.needsYInversion == rhs.needsYInversion
				&& lhs.needsXInversion == rhs.needsXInversion
				&& lhs.needsZInversion == rhs.needsZInversion
				&& lhs.normal2Channels == rhs.normal2Channels
				&& lhs.transform.translate == rhs.transform.translate
				&& lhs.transform.rotate == rhs.transform.rotate
				&& lhs.transform.scale == rhs.transform.scale;
		}
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
	C3D_API size_t getHash( TextureConfiguration const & config );
	C3D_API TextureFlagsSet getFlags( TextureConfiguration const & config );
	C3D_API PixelComponents getPixelComponents( uint32_t mask );
	C3D_API PixelComponents getPixelComponents( TextureConfiguration const & config );
	C3D_API void updateIndices( PixelFormat format
		, TextureConfiguration & config );
	C3D_API TextureFlagConfigurations::const_iterator checkFlag( TextureFlagConfigurations const & lhs
		, PassComponentTextureFlag rhs );
	C3D_API TextureFlagConfigurations::iterator checkFlag( TextureFlagConfigurations & lhs
		, PassComponentTextureFlag rhs );
	C3D_API bool hasAny( TextureFlagConfigurations const & lhs
		, PassComponentTextureFlag rhs );
	C3D_API bool removeFlag( TextureConfiguration & config
		, PassComponentTextureFlag rhs );
	C3D_API void removeFlagConfiguration( TextureConfiguration & config
		, TextureFlagConfiguration const & flagConfiguration );
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
		SamplerObs sampler{};
		uint32_t texcoordSet{};

	private:
		friend bool operator==( PassTextureConfig const & lhs
			, PassTextureConfig const & rhs )noexcept
		{
			return lhs.sampler == rhs.sampler
				&& lhs.texcoordSet == rhs.texcoordSet;
		}
	};
}

#endif
