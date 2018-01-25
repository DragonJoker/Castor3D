#pragma once

#include "Prerequisites.hpp"

#include <Core/Device.hpp>
#include <Core/SwapChain.hpp>

#include <Utils/UtilsSignal.hpp>

#include <wx/panel.h>

#include <array>

namespace vkapp
{
	/**
	*\~french
	*\brief
	*	Le panneau de rendu.
	*\~english
	*\brief
	*	The render panel.
	*/
	class RenderPanel
		: public wxPanel
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*	Initialise le device logique.
		*\param parent
		*	La fenêtre parente.
		*\param size
		*	La taille de la fenêtre.
		*\param instance
		*	L'instance Vulkan.
		*\~english
		*\brief
		*	Constructor.
		*	Initialises the logical device.
		*\param parent
		*	The parent window.
		*\param size
		*	The window dimensions.
		*\param instance
		*	The Vulkan instance.
		*/
		RenderPanel( wxWindow * parent
			, wxSize const & size
			, renderer::Renderer const & renderer );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~RenderPanel();

	private:
		/**
		*\~french
		*\brief
		*	Nettoie l'instance.
		*\~english
		*\brief
		*	Cleans up the instance.
		*/
		void doCleanup();
		/**
		*\~french
		*\brief
		*	Crée le device logique.
		*\~english
		*\brief
		*	Creates the logical device.
		*/
		void doCreateDevice( renderer::Renderer const & renderer );
		/**
		*\~french
		*\brief
		*	Crée la swap chain.
		*\~english
		*\brief
		*	Creates the swap chain.
		*/
		void doCreateSwapChain();
		/**
		*\~french
		*\brief
		*	Crée la passe de rendu.
		*\~english
		*\brief
		*	Creates the render pass.
		*/
		void doCreateRenderPass();
		/**
		*\~french
		*\brief
		*	Crée les tampons de commande.
		*\~english
		*\brief
		*	Creates the command buffers.
		*/
		bool doPrepareFrames();
		/**
		*\~french
		*\brief
		*	Dessine la scène.
		*\~english
		*\brief
		*	Draws the scene.
		*/
		void doDraw();
		/**
		*\~french
		*\brief
		*	Réinitialise la swap chain et toutes ses dépendances.
		*\remarks
		*	Peut être nécessaire en cas de redimensionnement de la fenêtre, par exemple.
		*\~english
		*\brief
		*	Resets the swap chain and all needed dependencies.
		*\remarks
		*	Can be necessary, for example when the window is resized.
		*/
		void doResetSwapChain();
		/**
		*\~french
		*\brief
		*	Fonction appelée par le timer de rendu.
		*\~english
		*\brief
		*	Function called by the render timer.
		*/
		void onTimer( wxTimerEvent & event );
		/**
		*\~french
		*\brief
		*	Fonction appelée lors du redimensionnement de la fenêtre.
		*\~english
		*\brief
		*	Function called when the window is resized.
		*/
		void onSize( wxSizeEvent & event );

	private:
		renderer::DevicePtr m_device;
		renderer::SwapChainPtr m_swapChain;
		renderer::RenderPassPtr m_renderPass;
		std::vector< renderer::FrameBufferPtr > m_frameBuffers;
		renderer::CommandBufferPtrArray m_commandBuffers;
		renderer::SignalConnection< renderer::SwapChain::OnReset > m_swapChainReset;
		wxTimer * m_timer{ nullptr };
	};
}
