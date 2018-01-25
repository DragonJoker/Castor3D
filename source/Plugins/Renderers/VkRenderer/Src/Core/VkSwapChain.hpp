/**
*\file
*	Renderer.h
*\author
*	Sylvain Doremus
*/
#pragma once

#include "VkRendererPrerequisites.hpp"
#include "Core/VkRenderingResources.hpp"

#include <Core/SwapChain.hpp>

namespace vk_renderer
{
	class SwapChain
		: public renderer::SwapChain
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	La connexion logique au GPU.
		*\param[in] size
		*	Les dimensions de la surface de rendu.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] size
		*	The render surface dimensions.
		*/
		SwapChain( Device const & device
			, renderer::UIVec2 const & size );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~SwapChain();
		/**
		*\~french
		*\brief
		*	R�initialise la swap chain.
		*\~english
		*\brief
		*	Resets the swap chain.
		*/
		void reset( renderer::UIVec2 const & size )override;
		/**
		*\~french
		*\brief
		*	Cr�e les tampons d'image des back buffers, compatibles avec la passe de rendu donn�e.
		*\param[in] renderPass
		*	La passe de rendu.
		*\return
		*	Les tampons d'images.
		*\~english
		*\brief
		*	Creates the back buffers' frame buffers, compatible with given render pass.
		*\param[in] renderPass
		*	The render pass.
		*\return
		*	The frame buffers.
		*/
		renderer::FrameBufferPtrArray createFrameBuffers( renderer::RenderPass const & renderPass )const override;
		/**
		*\~french
		*\brief
		*	Cr�e les tampons de commandes des back buffers.
		*\return
		*	Les tampons de commandes.
		*\~english
		*\brief
		*	Creates the back buffers' command buffers.
		*\return
		*	The command buffers.
		*/
		renderer::CommandBufferPtrArray createCommandBuffers()const override;
		/**
		*\~french
		*\brief
		*	Enregistre des commandes de pr�-rendu.
		*\param[in] index
		*	L'indice de l'image.
		*\param[in] commandBuffer
		*	Le tampon de commandes recevant les commandes.
		*\~english
		*\brief
		*	Registers pre-render commands.
		*\param[in] index
		*	The index of the backbuffer.
		*\param[in] commandBuffer
		*	The command buffer receiving the commands.
		*/
		void preRenderCommands( uint32_t index
			, renderer::CommandBuffer const & commandBuffer )const override;
		/**
		*\~french
		*\brief
		*	Enregistre des commandes de post-rendu.
		*\param[in] index
		*	L'indice de l'image.
		*\param[in] commandBuffer
		*	Le tampon de commandes recevant les commandes.
		*\~english
		*\brief
		*	Registers post-render commands.
		*\param[in] index
		*	The index of the backbuffer.
		*\param[in] commandBuffer
		*	The command buffer receiving the commands.
		*/
		void postRenderCommands( uint32_t index
			, renderer::CommandBuffer const & commandBuffer )const override;;
		/**
		*\~french
		*\return
		*	R�cup�re les ressources de rendu actives.
		*\~english
		*\return
		*	The active rendering resources.
		*/
		renderer::RenderingResources * getResources()override;
		/**
		*\~french
		*\brief
		*	Rend l'image utilis�e � la swap chain, pour la dessiner.
		*\param[in] resources
		*	Les ressources de rendu.
		*\~english
		*\brief
		*	Gives back the backbuffer to the swap chain, to present it.
		*\param[in] resources
		*	The rendering resources.
		*/
		void present( renderer::RenderingResources & resources )override;
		/**
		*\~french
		*\return
		*	Les tampons d'images.
		*\~english
		*\return
		*	The back buffers.
		*/
		inline auto const & getBackBuffers()const
		{
			return m_backBuffers;
		}
		/**
		*\~french
		*\brief
		*	D�finit couleur de vidage.
		*\param[in] colour
		*	La nouvelle valeur.
		*\~english
		*\brief
		*	Defines the clear colour.
		*\param[in] colour
		*	The new value.
		*/
		inline void setClearColour( renderer::RgbaColour const & value )override
		{
			m_clearColour = convert( value );
		}
		/**
		*\~french
		*\return
		*	La couleur de vidage.
		*\~english
		*\return
		*	The clear colour.
		*/
		inline renderer::RgbaColour getClearColour()const override
		{
			return convert( m_clearColour );
		}
		/**
		*\~french
		*\return
		*	Les dimensions de la swap chain.
		*\~french
		*\return
		*	The swap chain dimensions.
		*/
		inline renderer::UIVec2 getDimensions()const override
		{
			return m_dimensions;
		}
		/**
		*\~french
		*\return
		*	Le format des images de la swap chain.
		*\~english
		*\return
		*	The swap chain's images pixels format.
		*/
		inline renderer::PixelFormat getFormat()const override
		{
			return m_format;
		}
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkSwapchainKHR.
		*\~english
		*\brief
		*	VkSwapchainKHR implicit cast operator.
		*/
		inline operator VkSwapchainKHR const &()const
		{
			return m_swapChain;
		}

	private:
		uint32_t doGetImageCount();
		void doSelectFormat( VkPhysicalDevice gpu );
		VkPresentModeKHR doSelectPresentMode();
		void doCreateSwapChain();
		void doCreateBackBuffers();
		bool doCheckNeedReset( VkResult errCode
			, bool acquisition
			, char const * const action );
		void doResetSwapChain();

	protected:
		Device const & m_device;
		renderer::PixelFormat m_format{};
		VkColorSpaceKHR m_colorSpace;
		VkSwapchainKHR m_swapChain{};
		VkSurfaceKHR m_surface{};
		VkSurfaceCapabilitiesKHR m_surfaceCapabilities{};
		uint32_t m_currentBuffer{};
		BackBufferPtrArray m_backBuffers;
		VkClearColorValue m_clearColour{};
	};
}
