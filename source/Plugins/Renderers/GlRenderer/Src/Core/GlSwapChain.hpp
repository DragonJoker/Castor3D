/**
*\file
*	Renderer.h
*\author
*	Sylvain Doremus
*/
#pragma once

#include "GlRendererPrerequisites.hpp"
#include "Core/GlRenderingResources.hpp"

#include <Core/SwapChain.hpp>

namespace gl_renderer
{
	class SwapChain
		: public renderer::SwapChain
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*/
		SwapChain( renderer::Device const & device
			, renderer::UIVec2 const & size );
		/**
		*\brief
		*	R�initialise la swap chain.
		*/
		void reset( renderer::UIVec2 const & size )override;
		/**
		*\brief
		*	Cr�e les tampons d'image des back buffers, compatibles avec la passe de rendu donn�e.
		*\param[in] renderPass
		*	La passe de rendu.
		*\return
		*	Les tampons d'images.
		*/
		renderer::FrameBufferPtrArray createFrameBuffers( renderer::RenderPass const & renderPass )const override;
		/**
		*\brief
		*	Cr�e les tampons d'image des back buffers, compatibles avec la passe de rendu donn�e.
		*\param[in] renderPass
		*	La passe de rendu.
		*\return
		*	Les tampons d'images.
		*/
		renderer::CommandBufferPtrArray createCommandBuffers()const override;
		/**
		*\brief
		*	Enregistre des commandes de pr�-rendu.
		*\param[in] index
		*	L'indice de l'image.
		*\param[in] commandBuffer
		*	Le tampon de commandes recevant les commandes.
		*/
		void preRenderCommands( uint32_t index
			, renderer::CommandBuffer const & commandBuffer )const override;
		/**
		*\brief
		*	Enregistre des commandes de post-rendu.
		*\param[in] index
		*	L'indice de l'image.
		*\param[in] commandBuffer
		*	Le tampon de commandes recevant les commandes.
		*/
		void postRenderCommands( uint32_t index
			, renderer::CommandBuffer const & commandBuffer )const override;;
		/**
		*\return
		*	R�cup�re les ressources de rendu actives.
		*/
		renderer::RenderingResources * getResources()override;
		/**
		*\return
		*	Pr�sente les ressources de rendu.
		*/
		void present( renderer::RenderingResources & resources )override;
		/**
		*\brief
		*	D�finit la couleur de vidage de la swapchain.
		*\param[in] value
		*	La nouvelle valeur.
		*/
		inline void setClearColour( renderer::RgbaColour const & value )override
		{
			m_clearColour = value;
		}
		/**
		*\brief
		*	D�finit la couleur de vidage de la swapchain.
		*\param[in] value
		*	La nouvelle valeur.
		*/
		inline renderer::RgbaColour getClearColour()const override
		{
			return m_clearColour;
		}
		/**
		*\return
		*	Les dimensions de la swap chain.
		*/
		inline renderer::UIVec2 getDimensions()const override
		{
			return m_dimensions;
		}
		/**
		*\return
		*	Les format des pixels de la swap chain.
		*/
		inline renderer::PixelFormat getFormat()const override
		{
			return m_format;
		}

	private:
		void doResetSwapChain();

	private:
		renderer::RgbaColour m_clearColour;
		renderer::PixelFormat m_format;
	};
}
