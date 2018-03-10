/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#ifndef ___Renderer_PhysicalDevice_HPP___
#define ___Renderer_PhysicalDevice_HPP___
#pragma once

#include "Miscellaneous/PhysicalDeviceFeatures.hpp"
#include "Miscellaneous/PhysicalDeviceMemoryProperties.hpp"
#include "Miscellaneous/PhysicalDeviceProperties.hpp"
#include "Miscellaneous/QueueFamilyProperties.hpp"

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Classe contenant les informations liées au GPU physique.
	*\~english
	*\brief
	*	Wraps informations about physical GPU.
	*/
	class PhysicalDevice
	{
	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] renderer
		*	The renderer instance.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] renderer
		*	L'instance de rendu.
		*/
		PhysicalDevice( Renderer & renderer );
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~PhysicalDevice() = default;
		/**
		*\~english
		*\brief
		*	Initialises the physical device properties.
		*\~french
		*\brief
		*	Initialise les propriétés du périphérique physique.
		*/
		virtual void initialise() = 0;
		/**
		*\~english
		*\brief
		*	Deduces the memory type, from the given informations.
		*\param[in] typeBits
		*	There are 32 memory types described in VkPhysicalDeviceMemoryProperties.
		*	This parameter is a bitwise combinartion describing the properties to look for.
		*\param[in] requirements
		*	The memory quirements.
		*\param[in] typeIndex
		*	Receives the deduced memory type index.
		*\return
		*	\p false if the memory type could not be deduced.
		*\~french
		*\brief
		*	Déduit le type de mémoire à allouer, en fonction des informations données.
		*\param[in] typeBits
		*	Il y a 32 types de mémoire décrits dans VkPhysicalDeviceMemoryProperties.
		*	Ce paramètre est une combinaison bit à bit décrivant les propriétés dans lesquelles
		*	il faut chercher.
		*\param[in] requirements
		*	Les pré-requis pour le type de mémoire.
		*\param[in] typeIndex
		*	Reçoit l'index du type de mémoire déduit.
		*\return
		*	\p true si le type a pu être déduit, \p false sinon.
		*/ 
		bool deduceMemoryType( uint32_t typeBits
			, MemoryPropertyFlags requirements
			, uint32_t & typeIndex )const;
		/**
		*\~english
		*\brief
		*	Tells if SPIR-V is supported.
		*\~french
		*\brief
		*	Dit si le SPIR-V est supporté.
		*/
		virtual bool isSPIRVSupported()const = 0;
		/**
		*\~english
		*\brief
		*	Tells if GLSL is supported.
		*\~french
		*\brief
		*	Dit si le GLSL est supporté.
		*/
		virtual bool isGLSLSupported()const = 0;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline std::vector< QueueFamilyProperties > const & getQueueProperties()const
		{
			return m_queueProperties;
		}

		inline PhysicalDeviceProperties const & getProperties()const
		{
			return m_properties;
		}

		inline PhysicalDeviceMemoryProperties const & getMemoryProperties()const
		{
			return m_memoryProperties;
		}

		inline PhysicalDeviceFeatures const & getFeatures()const
		{
			return m_features;
		}
		/**@}*/

	protected:
		PhysicalDeviceMemoryProperties m_memoryProperties{};
		PhysicalDeviceFeatures m_features{};
		PhysicalDeviceProperties m_properties{};
		std::vector< QueueFamilyProperties > m_queueProperties;

	private:
		Renderer & m_renderer;
	};
}

#endif
