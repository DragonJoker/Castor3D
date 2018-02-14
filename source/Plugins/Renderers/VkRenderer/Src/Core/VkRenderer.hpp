/**
*\file
*	Renderer.h
*\author
*	Sylvain Doremus
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Core/Renderer.hpp>

namespace vk_renderer
{
	class Renderer
		: public renderer::Renderer
	{
	public:
		/**
		*\brief
		*	Constructeur, initialise l'instance de Vulkan.
		*/
		Renderer();
		/**
		*\brief
		*	Destructeur.
		*/
		~Renderer();
		/**
		*\brief
		*	Crée le périphérique logique.
		*\param[in] connection
		*	La connection avec la fenêtre.
		*/
		renderer::DevicePtr createDevice( renderer::ConnectionPtr && connection )const override;
		/**
		*\brief
		*	Crée une connection.
		*\param[in] deviceIndex
		*	L'indice du périphérique physique.
		*\param[in] handle
		*	Le descripteur de la fenêtre.
		*/
		renderer::ConnectionPtr createConnection( uint32_t deviceIndex
			, renderer::WindowHandle && handle )const override;
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
		*	Ajoute les couches de l'instance aux noms déjà présents dans la liste donnée.
		*\param[in,out] names
		*	La liste à compléter.
		*\~english
		*\brief
		*	Adds the instance layers names to the given names.
		*\param[in,out] names
		*	The liste to fill.
		*/
		void completeLayerNames( std::vector< char const * > & names )const;
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkInstance.
		*\~english
		*\brief
		*	VkInstance implicit cast operator.
		*/
		inline operator VkInstance const &( )const
		{
			return m_instance;
		}

		PFN_vkGetInstanceProcAddr GetInstanceProcAddr;
#define VK_LIB_GLOBAL_FUNCTION( fun ) PFN_##fun fun;
#define VK_LIB_INSTANCE_FUNCTION( fun ) PFN_##fun fun;
#	include "Miscellaneous/VulkanFunctionsList.inl"

	private:
		/**
		*\~french
		*\brief
		*	Enumère les couches disponibles sur le système.
		*\~english
		*\brief
		*	Lists all layers available on the system.
		*/
		void doInitLayersProperties();
		/**
		*\~french
		*\brief
		*	Enumère les extensions disponibles pour la couche donnée.
		*\param[out] layerProps
		*	Reçoit les propriétés des extensions de la couche.
		*\~english
		*\brief
		*	Lists all the extensions supported by given layer.
		*\param[out] layerProps
		*	Receives the extensions properties for the layer.
		*/
		void doInitLayerExtensionProperties( LayerProperties & layerProps );
		/**
		*\~french
		*\brief
		*	Crée l'instance Vulkan.
		*\~english
		*\brief
		*	Creates the Vulkan instance.
		*/
		void doInitInstance();
		/**
		*\~french
		*\brief
		*	Initialise les extensions de débogage.
		*\~english
		*\brief
		*	Initialises debugging extensions.
		*/
		void doSetupDebugging( VkDebugReportFlagsEXT flags );
		/**
		*\~french
		*\brief
		*	Enumère les GPU physiques disponibles.
		*\~english
		*\brief
		*	Lists the available physical GPUs.
		*/
		void doEnumerateDevices();
		PFN_vkVoidFunction getInstanceProcAddr( char const * const name );

	private:
		renderer::DynamicLibrary m_library;
		VkInstance m_instance{ VK_NULL_HANDLE };
		VkDebugReportCallbackEXT m_msgCallback{ VK_NULL_HANDLE };
		std::vector< LayerProperties > m_instanceLayersProperties;
		std::vector< char const * > m_instanceExtensionNames;
		std::vector< char const * > m_instanceLayerNames;
		std::vector< PhysicalDevicePtr > m_gpus;
	};
}
