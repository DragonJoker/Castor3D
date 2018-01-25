#pragma once

#include "Prerequisites.hpp"

#include <Core/Connection.hpp>
#include <Core/Device.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
#include <Image/Sampler.hpp>
#include <Core/SwapChain.hpp>

#include <Utils/UtilsSignal.hpp>

#include <wx/panel.h>

#include <array>

namespace vkapp
{
	class RenderPanel
		: public wxPanel
	{
	public:
		RenderPanel( wxWindow * parent
			, wxSize const & size
			, renderer::Renderer const & renderer );
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
		*	Met à jour la matrice de projection.
		*\~english
		*\brief
		*	Updates the projection matrix.
		*/
		void doUpdateProjection();
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
		*	Crée la texture.
		*\~english
		*\brief
		*	Creates the texture.
		*/
		void doCreateTexture();
		/**
		*\~french
		*\brief
		*	Crée le tampon de variables uniformes.
		*\~english
		*\brief
		*	Creates the uniform variables buffer.
		*/
		void doCreateUniformBuffer();
		/**
		*\~french
		*\brief
		*	Crée le tampon de transfert.
		*\~english
		*\brief
		*	Creates the transfer buffer.
		*/
		void doCreateStagingBuffer();
		/**
		*\~french
		*\brief
		*	Crée le descriptors layout/pool/set.
		*\~english
		*\brief
		*	Creates the descriptors layout/pool/set.
		*/
		void doCreateOffscreenDescriptorSet();
		/**
		*\~french
		*\brief
		*	Crée la passe de rendu.
		*\~english
		*\brief
		*	Creates the render pass.
		*/
		void doCreateOffscreenRenderPass();
		/**
		*\~french
		*\brief
		*	Crée le tampon d'image.
		*\~english
		*\brief
		*	Creates the frame buffer.
		*/
		void doCreateFrameBuffer();
		/**
		*\~french
		*\brief
		*	Crée le tampon de sommets.
		*\~english
		*\brief
		*	Creates the vertex buffer.
		*/
		void doCreateOffscreenVertexBuffer();
		/**
		*\~french
		*\brief
		*	Crée le pipeline de rendu offscreen.
		*\~english
		*\brief
		*	Creates the offscreen rendering pipeline.
		*/
		void doCreateOffscreenPipeline();
		/**
		*\~french
		*\brief
		*	Enregistre les commandes de dessin d'une frame, pour chaque back buffer.
		*\~english
		*\brief
		*	Records the commands necessary to draw one frame, for each back buffer.
		*/
		void doPrepareOffscreenFrame();
		/**
		*\~french
		*\brief
		*	Crée le descriptors layout/pool/set.
		*\~english
		*\brief
		*	Creates the descriptors layout/pool/set.
		*/
		void doCreateMainDescriptorSet();
		/**
		*\~french
		*\brief
		*	Crée la passe de rendu.
		*\~english
		*\brief
		*	Creates the render pass.
		*/
		void doCreateMainRenderPass();
		/**
		*\~french
		*\brief
		*	Crée le tampon de sommets.
		*\~english
		*\brief
		*	Creates the vertex buffer.
		*/
		void doCreateMainVertexBuffer();
		/**
		*\~french
		*\brief
		*	Crée le pipeline de rendu principal.
		*\~english
		*\brief
		*	Creates the main rendering pipeline.
		*/
		void doCreateMainPipeline();
		/**
		*\~french
		*\brief
		*	Enregistre les commandes de dessin d'une frame, pour chaque back buffer.
		*\~english
		*\brief
		*	Records the commands necessary to draw one frame, for each back buffer.
		*/
		void doPrepareMainFrames();
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
		wxTimer * m_timer{ nullptr };
		renderer::DevicePtr m_device;
		renderer::SwapChainPtr m_swapChain;
		renderer::StagingBufferPtr m_stagingBuffer;
		renderer::TexturePtr m_texture;
		renderer::TextureViewPtr m_view;
		renderer::SamplerPtr m_sampler;
		renderer::TexturePtr m_renderTargetColour;
		renderer::TextureViewPtr m_renderTargetColourView;
		renderer::FrameBufferPtr m_frameBuffer;
		renderer::UniformBufferPtr< renderer::Mat4 > m_matrixUbo;

		renderer::CommandBufferPtr m_commandBuffer;
		renderer::CommandBufferPtr m_updateCommandBuffer;
		renderer::RenderPassPtr m_offscreenRenderPass;
		renderer::ShaderProgramPtr m_offscreenProgram;
		renderer::PipelineLayoutPtr m_offscreenPipelineLayout;
		renderer::PipelinePtr m_offscreenPipeline;
		renderer::VertexBufferPtr< TexturedVertexData > m_offscreenVertexBuffer;
		renderer::BufferPtr< uint16_t > m_offscreenIndexBuffer;
		renderer::VertexLayoutPtr m_offscreenVertexLayout;
		renderer::GeometryBuffersPtr m_offscreenGeometryBuffers;
		renderer::DescriptorSetLayoutPtr m_offscreenDescriptorLayout;
		renderer::DescriptorSetPoolPtr m_offscreenDescriptorPool;
		renderer::DescriptorSetPtr m_offscreenDescriptorSet;
		std::vector< TexturedVertexData > m_offscreenVertexData;
		renderer::UInt16Array m_offscreenIndexData;

		renderer::RenderPassPtr m_mainRenderPass;
		renderer::ShaderProgramPtr m_mainProgram;
		renderer::PipelineLayoutPtr m_mainPipelineLayout;
		renderer::PipelinePtr m_mainPipeline;
		renderer::VertexBufferPtr< TexturedVertexData > m_mainVertexBuffer;
		renderer::VertexLayoutPtr m_mainVertexLayout;
		renderer::GeometryBuffersPtr m_mainGeometryBuffers;
		renderer::DescriptorSetLayoutPtr m_mainDescriptorLayout;
		renderer::DescriptorSetPoolPtr m_mainDescriptorPool;
		renderer::DescriptorSetPtr m_mainDescriptorSet;
		std::vector< TexturedVertexData > m_mainVertexData;

		std::vector< renderer::FrameBufferPtr > m_frameBuffers;
		std::vector< renderer::CommandBufferPtr > m_commandBuffers;
		renderer::SignalConnection< renderer::SwapChain::OnReset > m_swapChainReset;
	};
}
