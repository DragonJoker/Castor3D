/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureConfiguration_H___
#define ___C3D_TextureConfiguration_H___
#pragma once

#include "Castor3D/Castor3DPrerequisites.hpp"

namespace castor3d
{
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
		*name
		*	Masks, used to filter image components (AABBGGRR).
		*\~french
		*name
		*	Masques, utilisés pour filtrer les composantes d'une image (AABBGGRR).
		*/
		/**@{*/
		castor::Point2ui colourMask{ 0u, 0u };
		castor::Point2ui specularMask{ 0u, 0u };
		castor::Point2ui glossinessMask{ 0u, 0u };
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
		uint32_t environment{ 0u };
		uint32_t needsGammaCorrection{ 0u };
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
		/**
		*\~english
		*name
		*	Environment masks.
		*\~french
		*name
		*	Masques d'environnement.
		*/
		/**@{*/
		static uint32_t constexpr ReflectionMask = 0x00000001u;
		static uint32_t constexpr RefractionMask = 0x00000002u;
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
		C3D_API static castor3d::TextureConfiguration const DiffuseTexture;
		C3D_API static castor3d::TextureConfiguration const AlbedoTexture;
		C3D_API static castor3d::TextureConfiguration const SpecularTexture;
		C3D_API static castor3d::TextureConfiguration const MetalnessTexture;
		C3D_API static castor3d::TextureConfiguration const GlossinessTexture;
		C3D_API static castor3d::TextureConfiguration const ShininessTexture;
		C3D_API static castor3d::TextureConfiguration const RoughnessTexture;
		C3D_API static castor3d::TextureConfiguration const OpacityTexture;
		C3D_API static castor3d::TextureConfiguration const EmissiveTexture;
		C3D_API static castor3d::TextureConfiguration const NormalTexture;
		C3D_API static castor3d::TextureConfiguration const HeightTexture;
		C3D_API static castor3d::TextureConfiguration const OcclusionTexture;
		C3D_API static castor3d::TextureConfiguration const TransmittanceTexture;
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
}

#endif
