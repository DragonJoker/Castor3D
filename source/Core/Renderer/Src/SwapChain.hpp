/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_SwapChain_HPP___
#define ___Renderer_SwapChain_HPP___
#pragma once

#include "RendererPrerequisites.hpp"
#include "RenderingResources.hpp"

#include <Design/Signal.hpp>
#include <Graphics/PixelFormat.hpp>

namespace renderer
{
	class SwapChain
	{
	protected:
		/**
		*\brief
		*	Constructeur.
		*/
		SwapChain( Device const & device
			, IVec2 const & size );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~SwapChain() = default;
		/**
		*\return
		*	Récupère les ressources de rendu actives.
		*/
		virtual RenderingResources * getResources() = 0;
		/**
		*\brief
		*	Réinitialise la swap chain.
		*/
		virtual void reset( IVec2 const & size ) = 0;
		/**
		*\brief
		*	Crée les tampons d'image des back buffers, compatibles avec la passe de rendu donnée.
		*\param[in] renderPass
		*	La passe de rendu.
		*\return
		*	Les tampons d'images.
		*/
		virtual FrameBufferPtrArray createFrameBuffers( RenderPass const & renderPass )const = 0;
		/**
		*\brief
		*	Crée les tampons d'image des back buffers, compatibles avec la passe de rendu donnée.
		*\param[in] renderPass
		*	La passe de rendu.
		*\return
		*	Les tampons d'images.
		*/
		virtual CommandBufferPtrArray createCommandBuffers()const = 0;
		/**
		*\brief
		*	Enregistre des commandes de pré-rendu.
		*\param[in] index
		*	L'indice de l'image.
		*\param[in] commandBuffer
		*	Le tampon de commandes recevant les commandes.
		*/
		virtual void preRenderCommands( uint32_t index
			, CommandBuffer const & commandBuffer )const = 0;
		/**
		*\brief
		*	Enregistre des commandes de post-rendu.
		*\param[in] index
		*	L'indice de l'image.
		*\param[in] commandBuffer
		*	Le tampon de commandes recevant les commandes.
		*/
		virtual void postRenderCommands( uint32_t index
			, CommandBuffer const & commandBuffer )const = 0;
		/**
		*\return
		*	Présente les ressources de rendu.
		*/
		virtual void present( RenderingResources & resources ) = 0;
		/**
		*\brief
		*	Définit la couleur de vidage de la swapchain.
		*\param[in] value
		*	La nouvelle valeur.
		*/
		virtual void setClearColour( RgbaColour const & value ) = 0;
		/**
		*\brief
		*	Définit la couleur de vidage de la swapchain.
		*\param[in] value
		*	La nouvelle valeur.
		*/
		virtual RgbaColour getClearColour()const = 0;
		/**
		*\return
		*	Les dimensions de la swap chain.
		*/
		virtual IVec2 getDimensions()const = 0;
		/**
		*\return
		*	Les format des pixels de la swap chain.
		*/
		virtual PixelFormat getFormat()const = 0;
		/**
		*\return
		*	Récupère les ressources de rendu par défaut.
		*/
		inline RenderingResources const & getDefaultResources()const
		{
			return *m_renderingResources[0];
		}

	public:
		//! Le signal levé lorsque la swap chain est recréée.
		using OnResetFunc = std::function< void() >;
		using OnReset = castor::Signal< OnResetFunc >;
		OnReset onReset;

	protected:
		Device const & m_device;
		//! Les dimensions de la swap chain
		IVec2 m_dimensions;
		//! Les ressources de rendu liées à la swap chain.
		std::vector< RenderingResourcesPtr > m_renderingResources;
		//! L'indice de la ressource de rendu active.
		mutable size_t m_resourceIndex{ 0 };
	};
}

#endif
