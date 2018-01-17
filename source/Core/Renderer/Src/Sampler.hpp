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
		*/
		Sampler( Device const & device
			, WrapMode wrapS
			, WrapMode wrapT
			, WrapMode wrapR
			, Filter minFilter
			, Filter magFilter
			, MipmapMode mipFilter = MipmapMode::eNearest );

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
		*\return
		*	Le mode de wrap sur l'axe S.
		*/
		inline WrapMode getWrapS()const
		{
			return m_wrapS;
		}
		/**
		*\return
		*	Le mode de wrap sur l'axe T.
		*/
		inline WrapMode getWrapT()const
		{
			return m_wrapT;
		}
		/**
		*\return
		*	Le mode de wrap sur l'axe R.
		*/
		inline WrapMode getWrapR()const
		{
			return m_wrapR;
		}
		/**
		*\return
		*	Le filtre de minification.
		*/
		inline Filter getMinFilter()const
		{
			return m_minFilter;
		}
		/**
		*\return
		*	Le filtre de magnification.
		*/
		inline Filter getMagFilter()const
		{
			return m_magFilter;
		}
		/**
		*\return
		*	Le filtre de mipmap.
		*/
		inline MipmapMode getMipFilter()const
		{
			return m_mipFilter;
		}

	private:
		WrapMode m_wrapS;
		WrapMode m_wrapT;
		WrapMode m_wrapR;
		Filter m_minFilter;
		Filter m_magFilter;
		MipmapMode m_mipFilter;
	};
}

#endif
