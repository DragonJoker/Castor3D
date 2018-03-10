/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Sampler_HPP___
#define ___Renderer_Sampler_HPP___
#pragma once

#include "SamplerCreateInfo.hpp"

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
		*\param[in] createInfo
		*	Les informations de création.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] createInfo
		*	The creation informations.
		*/
		Sampler( Device const & device
			, SamplerCreateInfo const & createInfo );

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
			return m_info.addressModeU;
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
			return m_info.addressModeV;
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
			return m_info.addressModeW;
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
			return m_info.minFilter;
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
			return m_info.magFilter;
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
			return m_info.mipmapMode;
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
			return m_info.minLod;
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
			return m_info.maxLod;
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
			return m_info.mipLodBias;
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
			return m_info.borderColor;
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
			return m_info.maxAnisotropy;
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
			return m_info.compareOp;
		}

	private:
		SamplerCreateInfo m_info;
	};
}

#endif
