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
	C3D_API bool operator==( TextureTransform const & lhs
		, TextureTransform const & rhs )noexcept;
	inline bool operator!=( TextureTransform const & lhs
		, TextureTransform const & rhs )noexcept
	{
		return !( lhs == rhs );
	}
	/**
	*\~english
	*\brief
	*	Specifies the usages of a texture, per image component.
	*\~french
	*\brief
	*	Définit les utilisations d'une texture, par composante d'image.
	*/
	struct TextureConfiguration
	{
		/**
		*\~english
		*\brief
		*	Masks and start indices, used to filter image components (AARRGGBB).
		*\remarks
		*\~french
		*\brief
		*	Masques et indices de début, utilisés pour filtrer les composantes d'une image (AARRGGBB).
		*/
		/**@{*/
		castor::Point2ui colourMask{ 0u, 0u };
		castor::Point2ui specularMask{ 0u, 0u };
		castor::Point2ui metalnessMask{ 0u, 0u };
		castor::Point2ui glossinessMask{ 0u, 0u };
		castor::Point2ui roughnessMask{ 0u, 0u };
		castor::Point2ui opacityMask{ 0u, 0u };
		castor::Point2ui emissiveMask{ 0u, 0u };
		castor::Point2ui normalMask{ 0u, 0u };
		castor::Point2ui heightMask{ 0u, 0u };
		castor::Point2ui occlusionMask{ 0u, 0u };
		castor::Point2ui transmittanceMask{ 0u, 0u };
		/**@}*/
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
		TextureSpace textureSpace{ TextureSpace::eColour };
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
		/**@}*/
		/**
		*\~english
		*name
		*	Predefined texture configurations.
		*\~french
		*name
		*	Configurations de texture prédéfinies.
		*/
		/**@{*/
		C3D_API static TextureConfiguration const DiffuseTexture;
		C3D_API static TextureConfiguration const AlbedoTexture;
		C3D_API static TextureConfiguration const SpecularTexture;
		C3D_API static TextureConfiguration const MetalnessTexture;
		C3D_API static TextureConfiguration const GlossinessTexture;
		C3D_API static TextureConfiguration const ShininessTexture;
		C3D_API static TextureConfiguration const RoughnessTexture;
		C3D_API static TextureConfiguration const OpacityTexture;
		C3D_API static TextureConfiguration const EmissiveTexture;
		C3D_API static TextureConfiguration const NormalTexture;
		C3D_API static TextureConfiguration const HeightTexture;
		C3D_API static TextureConfiguration const OcclusionTexture;
		C3D_API static TextureConfiguration const TransmittanceTexture;
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
	C3D_API bool operator==( TextureConfiguration const & lhs, TextureConfiguration const & rhs );
	C3D_API bool operator!=( TextureConfiguration const & lhs, TextureConfiguration const & rhs );
	/**@}*/
	C3D_API TextureFlags getFlags( TextureConfiguration const & config );
	C3D_API castor::PixelComponents getPixelComponents( uint32_t mask );

	struct PassTextureConfig
	{
		ashes::ImageCreateInfo imageInfo{ {} };
		TextureConfiguration config{};
	};
}

#endif
