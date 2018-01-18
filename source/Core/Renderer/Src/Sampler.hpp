/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Sampler_HPP___
#define ___Renderer_Sampler_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Un échantillonneur.
	*/
	class Sampler
	{
	protected:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] wrapS, wrapT, wrapR
		*	Les modes de wrap de texture.
		*\param[in] minFilter, magFilter
		*	Les filtres de minification et magnification.
		*\param[in] mipFilter
		*	Le filtre de mipmap.
		*\param[in] minLod
		*	Niveau de LOD minimal.
		*\param[in] maxLod
		*	Niveau de LOD maximal.
		*\param[in] lodBias
		*	Le décalage de LOD de la texture.
		*\param[in] borderColour
		*	Couleur des bords de la texture.
		*\param[in] maxAnisotropy
		*	Valeur maximale pour le filtrage anisotropique.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] minFilter, magFilter
		*\param[in] wrapS, wrapT, wrapR
		*	The  S, T and R address mode.
		*	The minification and magnification filter.
		*\param[in] mipFilter
		*	The mipmapping mode.
		*\param[in] minLod
		*	Minimal LOD Level.
		*\param[in] maxLod
		*	Maximal LOD Level.
		*\param[in] lodBias
		*	The texture LOD offset.
		*\param[in] borderColour
		*	Texture border colour.
		*\param[in] maxAnisotropy
		*	Maximal anisotropic filtering value.
		*/
		Sampler( Device const & device
			, WrapMode wrapS
			, WrapMode wrapT
			, WrapMode wrapR
			, Filter minFilter
			, Filter magFilter
			, MipmapMode mipFilter = MipmapMode::eNearest
			, float minLod = -1000.0f
			, float maxLod = 1000.0f
			, float lodBias = 0.0f
			, BorderColour borderColour = BorderColour::eFloatOpaqueBlack
			, float maxAnisotropy = 1.0f
			, CompareOp compareOp = CompareOp::eAlways );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~Sampler() = default;
		/**
		*\~english
		*\return
		*	The wrap mode on S axis.
		*\~french
		*\return
		*	Le mode de wrap sur l'axe S.
		*/
		inline WrapMode getWrapS()const
		{
			return m_wrapS;
		}
		/**
		*\~english
		*\return
		*	The wrap mode on T axis.
		*\~french
		*\return
		*	Le mode de wrap sur l'axe T.
		*/
		inline WrapMode getWrapT()const
		{
			return m_wrapT;
		}
		/**
		*\~english
		*\return
		*	The wrap mode on R axis.
		*\~french
		*\return
		*	Le mode de wrap sur l'axe R.
		*/
		inline WrapMode getWrapR()const
		{
			return m_wrapR;
		}
		/**
		*\~english
		*\return
		*	The minification filter.
		*\~french
		*\return
		*	Le filtre de minification.
		*/
		inline Filter getMinFilter()const
		{
			return m_minFilter;
		}
		/**
		*\~english
		*\return
		*	The magnification filter.
		*\~french
		*\return
		*	Le filtre de magnification.
		*/
		inline Filter getMagFilter()const
		{
			return m_magFilter;
		}
		/**
		*\~english
		*\return
		*	The mipmap filter.
		*\~french
		*\return
		*	Le filtre de mipmap.
		*/
		inline MipmapMode getMipFilter()const
		{
			return m_mipFilter;
		}
		/**
		*\~english
		*\return
		*	The minimal LOD level.
		*\~french
		*\return
		*	Le niveau minimal pour le LOD.
		*/
		inline float getMinLod()const
		{
			return m_minLod;
		}
		/**
		*\~english
		*\return
		*	The maximal LOD level.
		*\~french
		*\return
		*	Le niveau maximal pour le LOD.
		*/
		inline float getMaxLod()const
		{
			return m_maxLod;
		}
		/**
		*\~english
		*\return
		*	The LOD bias.
		*\~french
		*\return
		*	Le décalage LOD.
		*/
		inline float getLodBias()const
		{
			return m_lodBias;
		}
		/**
		*\~english
		*\return
		*	The border colour.
		*\~french
		*\brief
		*\return
		*	La couleur de la bordure.
		*/
		inline BorderColour getBorderColour()const
		{
			return m_borderColour;
		}
		/**
		*\~english
		*\return
		*	The maximal anisotropy filtering value.
		*\~french
		*\return
		*	La valeur maximale de filtrage anisotropique.
		*/
		inline float getMaxAnisotropy()const
		{
			return m_maxAnisotropy;
		}
		/**
		*\~english
		*\return
		*	The comparison operator.
		*\~french
		*\brief
		*\return
		*	L'opérateur de comparaison.
		*/
		inline CompareOp getCompareOp()const
		{
			return m_compareOp;
		}

	private:
		WrapMode m_wrapS;
		WrapMode m_wrapT;
		WrapMode m_wrapR;
		Filter m_minFilter;
		Filter m_magFilter;
		MipmapMode m_mipFilter;
		float m_minLod;
		float m_maxLod;
		float m_lodBias;
		BorderColour m_borderColour;
		float m_maxAnisotropy;
		CompareOp m_compareOp;
	};
}

#endif
