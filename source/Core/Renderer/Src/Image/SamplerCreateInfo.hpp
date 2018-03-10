/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_SamplerCreateInfo_HPP___
#define ___Renderer_SamplerCreateInfo_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	struct SamplerCreateInfo
	{
		/**
		*\~french
		*\brief
		*	The magnification filter to apply to lookups.
		*\~english
		*\brief
		*	Le filtre de magnification.
		*/
		Filter magFilter;
		/**
		*\~french
		*\brief
		*	The minification filter to apply to lookups.
		*\~english
		*\brief
		*	Le filtre de minification.
		*/
		Filter minFilter;
		/**
		*\~french
		*\brief
		*	Le filtre de mipmap.
		*\~english
		*\brief
		*	The mipmapping mode.
		*/
		MipmapMode mipmapMode;
		/**
		*\~french
		*\brief
		*	Le mode d'adressage pour les valeurs de U hors de l'intervalle [0..1].
		*\~english
		*\brief
		*	Constructor.
		*	The addressing mode for outside [0..1] range for U coordinate.
		*/
		WrapMode addressModeU;
		/**
		*\~french
		*\brief
		*	Le mode d'adressage pour les valeurs de V hors de l'intervalle [0..1].
		*\~english
		*\brief
		*	Constructor.
		*	The addressing mode for outside [0..1] range for V coordinate.
		*/
		WrapMode addressModeV;
		/**
		*\~french
		*\brief
		*	Le mode d'adressage pour les valeurs de W hors de l'intervalle [0..1].
		*\~english
		*\brief
		*	Constructor.
		*	The addressing mode for outside [0..1] range for W coordinate.
		*/
		WrapMode addressModeW;
		/**
		*\~french
		*\brief
		*	Le décalage à ajouter au calcul de LOD.
		*\~english
		*\brief
		*	The bias to be added to mipmap LOD (level-of-detail) calculation.
		*/
		float mipLodBias;
		/**
		*\~french
		*\brief
		*	\p true pour activer le filtrage anisotropique, \p false sinon.
		*\~english
		*\brief
		*	\p true to enable anisotropic filtering, \p false otherwise.
		*/
		bool anisotropyEnable;
		/**
		*\~french
		*\brief
		*	Valeur maximale pour le filtrage anisotropique.
		*\~english
		*\brief
		*	Maximal anisotropic filtering value.
		*/
		float maxAnisotropy;
		/**
		*\~french
		*\brief
		*	\p true pour activer la comparaison à une valeur de référence durant les lookups, \p false sinon.
		*\~english
		*\brief
		*	\p true to enable comparison against a reference value during lookups, \p false otherwise.
		*/
		bool compareEnable;
		/**
		*\~french
		*\brief
		*	L'opérateur de comparaison, pour les textures de profondeur.
		*\~english
		*\brief
		*	The comparison operator, for depth maps.
		*/
		CompareOp compareOp;
		/**
		*\~french
		*\brief
		*	Niveau de LOD minimal.
		*\~english
		*\brief
		*	Minimal LOD Level.
		*/
		float minLod;
		/**
		*\~french
		*\brief
		*	Niveau de LOD maximal.
		*\~english
		*\brief
		*	Maximal LOD Level.
		*/
		float maxLod;
		/**
		*\~french
		*\brief
		*	Couleur des bords de la texture.
		*\~english
		*\brief
		*	Texture border colour.
		*/
		BorderColour borderColor;
		/**
		*\~french
		*\brief
		*	Contrôle l'utilisation de coordonnées de texel normalisées ou dénormalisées pour adresser les texels d'une image.
		*	Si défini à \p true, l'intervalle des coordonnées de l'image lors du lookup de texel est entre zéro et les dimensions de l'image pour x, y, et z.
		*	Si défini à \p false, l'ingervalle des coordonnées de l'image est entre zéro et un.
		*\~english
		*\brief
		*	Controls whether to use unnormalized or normalized texel coordinates to address texels of the image.
		*	When set to \p true, the range of the image coordinates used to lookup the texel is in the range of zero to the image dimensions for x, y and z.
		*	When set to \p false the range of image coordinates is zero to one.
		*/
		bool unnormalizedCoordinates;
	};
}

#endif
