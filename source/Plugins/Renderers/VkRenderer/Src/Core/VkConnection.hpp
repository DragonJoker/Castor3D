/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Core/Connection.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Encapsulation de la connexion à une application dépendant de la plateforme.
	*/
	class Connection
		: public renderer::Connection
	{
	public:
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
		Connection( Renderer const & renderer
			, uint32_t deviceIndex
			, renderer::WindowHandle && handle );
		/**
		*\brief
		*	Destructeur, détruit la surface KHR.
		*/
		~Connection();
		/**
		*\~french
		*\return
		*	Les capacités de la surface de présentation.
		*\~english
		*\return
		*	The presentation surface's capabilites.
		*/
		VkSurfaceCapabilitiesKHR getSurfaceCapabilities()const;
		/**
		*\~french
		*\return
		*	La surface de présentation.
		*\~english
		*\return
		*	The presentation surface.
		*/
		inline auto getPresentSurface()const
		{
			return m_presentSurface;
		}
		/**
		*\~french
		*\return
		*	L'index du type de file graphique.
		*\~english
		*\return
		*	The graphic queue's family index.
		*/
		inline auto getGraphicsQueueFamilyIndex()const
		{
			return m_graphicsQueueFamilyIndex;
		}
		/**
		*\~french
		*\return
		*	L'index du type de file de présentation.
		*\~english
		*\return
		*	The presentation queue's family index.
		*/
		inline auto getPresentQueueFamilyIndex()const
		{
			return m_graphicsQueueFamilyIndex;
		}
		/**
		*\~french
		*\return
		*	Le périphérique physique.
		*\~english
		*\return
		*	The physical device.
		*/
		inline PhysicalDevice const & getGpu()const
		{
			return m_gpu;
		}

	private:
		/**
		*\~french
		*\brief
		*	Crée la surface liée à la fenêtre.
		*\~english
		*\brief
		*	Creates the surface associated to the window.
		*/
		void doCreatePresentSurface();
		/**
		*\~french
		*\brief
		*	Récupère les informations de présentation.
		*\~english
		*\brief
		*	Retrieves the presentation informations.
		*/
		void doRetrievePresentationInfos();

	private:
		Renderer const & m_renderer;
		PhysicalDevice const & m_gpu;
		VkSurfaceKHR m_presentSurface{ VK_NULL_HANDLE };
		uint32_t m_graphicsQueueFamilyIndex{ std::numeric_limits< uint32_t >::max() };
		uint32_t m_presentQueueFamilyIndex{ std::numeric_limits< uint32_t >::max() };
	};
}
