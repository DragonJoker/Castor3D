/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_HPP___
#define ___Renderer_HPP___
#pragma once

#include "Core/PhysicalDevice.hpp"
#include "Core/WindowHandle.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	The class initialising the rendering instance.
	*\~french
	*\brief
	*	Classe initialisant l'instance de rendu.
	*/
	class Renderer
	{
	public:
		struct Configuration
		{
			//!\~french		Le nom de l'application.
			//!\~english	The application name.
			std::string appName;
			//!\~french		Le nom du moteur utilisé par l'application.
			//!\~english	The name of the engine used by the application.
			std::string engineName;
			//!\~french		Dit si la couche de validation doit être activée.
			//!\~english	Tells if the validation layer must be enabled.
			bool enableValidation;
		};

	protected:
		/**
		*\~french
		*\brief
		*	Constructor, initialises the rendering instance.
		*\~french
		*\brief
		*	Constructeur, initialise l'instance de rendu.
		*/
		Renderer( ClipDirection clipDirection
			, std::string const & name
			, Configuration const & configuration );

	public:
		/**
		*\~french
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~Renderer() = default;
		/**
		*\~french
		*\brief
		*	Récupère le GPU à l'index donné.
		*\param[in] gpuIndex
		*	L'index du GPU physique.
		*\~english
		*\brief
		*	Retrieves the GPU at given index.
		*\param[in] gpuIndex
		*	The physical GPU index.
		*/
		PhysicalDevice & getPhysicalDevice( uint32_t gpuIndex )const;
		/**
		*\~french
		*\brief
		*	Creates a logical device.
		*\param[in] connection
		*	The connection to the window.
		*\~french
		*\brief
		*	Crée un périphérique logique.
		*\param[in] connection
		*	La connection avec la fenêtre.
		*/
		virtual DevicePtr createDevice( ConnectionPtr && connection )const = 0;
		/**
		*\~french
		*\brief
		*	Crée une connection entre un périphérique physique et une fenêtre.
		*\param[in] deviceIndex
		*	L'indice du périphérique physique.
		*\param[in] handle
		*	Le descripteur de la fenêtre.
		*\~french
		*\brief
		*	Creates a connection between a physical device and a window.
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
			return m_configuration.enableValidation;
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
		*	The number of available GPUs.
		*\~french
		*\return
		*	Le nombre de GPU disponibles.
		*/
		inline uint32_t getGpuCount()const
		{
			return uint32_t( m_gpus.size() );
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

	protected:
		Configuration m_configuration;
		std::vector< PhysicalDevicePtr > m_gpus;

	private:
		ClipDirection m_clipDirection;
		std::string m_name;
	};
}

#endif
