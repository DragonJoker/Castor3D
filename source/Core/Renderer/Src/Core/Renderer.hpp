/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_HPP___
#define ___Renderer_HPP___
#pragma once

#include "Core/WindowHandle.hpp"

namespace renderer
{
	class Renderer
	{
	protected:
		/**
		*\brief
		*	Constructeur, initialise l'instance de renderer.
		*/
		Renderer( ClipDirection clipDirection
			, std::string const & name
			, bool enableValidation );

	public:
		/**
		*\brief
		*	Destructeur.
		*/
		virtual ~Renderer() = default;
		/**
		*\brief
		*	Crée le périphérique logique.
		*\param[in] connection
		*	La connection avec la fenêtre.
		*/
		virtual DevicePtr createDevice( ConnectionPtr && connection )const = 0;
		/**
		*\brief
		*	Constructeur.
		*\param[in] renderer
		*	L'instance de Renderer.
		*\param[in] deviceIndex
		*	L'indice du périphérique physique.
		*\param[in] handle
		*	Le descripteur de la fenêtre.
		*/
		virtual ConnectionPtr createConnection( uint32_t deviceIndex
			, WindowHandle && handle )const = 0;
		/**
		*\~english
		*\return
		*	The validation activation status.
		*\~french
		*\return
		*	Le statut d'activation de la validation.
		*/
		inline bool isValidationEnabled()const
		{
			return m_enableValidation;
		}
		/**
		*\~english
		*\return
		*	The clip direction for the rendering API.
		*\~french
		*\return
		*	La direction de clipping pour l'API de rendu.
		*/
		inline ClipDirection getClipDirection()const
		{
			return m_clipDirection;
		}
		/**
		*\~english
		*\return
		*	The internal renderer name.
		*\~french
		*\return
		*	La nom interne du renderer.
		*/
		inline std::string const & getName()const
		{
			return m_name;
		}

	private:
		ClipDirection m_clipDirection;
		std::string m_name;
		bool m_enableValidation;
	};
}

#endif
