/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LineariseDepthPass_H___
#define ___C3D_LineariseDepthPass_H___

#include "Render/Viewport.hpp"
#include "Texture/TextureUnit.hpp"

#include <Buffer/PushConstantsBuffer.hpp>
#include <Miscellaneous/PushConstantRange.hpp>

namespace castor3d
{
	class SsaoConfigUbo;
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		18/12/2017
	\~english
	\brief		Depth linearisation pass, with mipmaps generation.
	\~french
	\brief		Passe linéarisation de profondeur, avec génération des mipmaps.
	*/
	class LineariseDepthPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	ssaoConfigUbo	The SSAO configuration UBO.
		 *\param[in]	depthBuffer		The non linearised depth buffer.
		 *\param[in]	viewport		The viewport from which clip infos are retrieved.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	ssaoConfigUbo	L'UBO de configuration SSAO.
		 *\param[in]	depthBuffer		Le tampon de profondeur non linéarisé.
		 *\param[in]	viewport		Le viewport depuis lequel on récupère les information de clip.
		 */
		LineariseDepthPass( Engine & engine
			, renderer::Extent2D const & size
			, SsaoConfigUbo & ssaoConfigUbo
			, renderer::TextureView const & depthBuffer
			, Viewport const & viewport );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~LineariseDepthPass();
		/**
		 *\~english
		 *\brief		Linearises depth buffer.
		 *\~french
		 *\brief		Linéarise le tampon de profondeur.
		 */
		void linearise( renderer::Semaphore const & toWait )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline TextureUnit const & getResult()const
		{
			return m_result;
		}

		inline renderer::Semaphore const & getSemaphore()const
		{
			REQUIRE( m_finished );
			return *m_finished;
		}
		/**@}*/

	private:
		void doInitialiseLinearisePass();
		void doInitialiseMinifyPass();
		void doCleanupLinearisePass();
		void doCleanupMinifyPass();
		void doPrepareFrame();

	public:
		static constexpr uint32_t MaxMipLevel = 5u;

	private:
		Engine & m_engine;
		SsaoConfigUbo & m_ssaoConfigUbo;
		renderer::TextureView const & m_depthBuffer;
		renderer::Extent2D m_size;
		TextureUnit m_result;
		RenderPassTimerSPtr m_timer;
		renderer::RenderPassPtr m_renderPass;
		renderer::VertexBufferPtr< NonTexturedQuad > m_vertexBuffer;
		renderer::VertexLayoutPtr m_vertexLayout;
		renderer::SamplerPtr m_lineariseSampler;
		renderer::SamplerPtr m_minifySampler;
		renderer::CommandBufferPtr m_commandBuffer;
		renderer::SemaphorePtr m_finished;
		/**
		*name
		*	Linearisation.
		*/
		/**@{*/
		renderer::ShaderStageStateArray m_lineariseProgram;
		renderer::TextureViewPtr m_linearisedView;
		renderer::FrameBufferPtr m_lineariseFrameBuffer;
		renderer::DescriptorSetLayoutPtr m_lineariseDescriptorLayout;
		renderer::DescriptorSetPoolPtr m_lineariseDescriptorPool;
		renderer::DescriptorSetPtr m_lineariseDescriptor;
		renderer::PipelineLayoutPtr m_linearisePipelineLayout;
		renderer::PipelinePtr m_linearisePipeline;
		renderer::PushConstantsBuffer< castor::Point3f > m_clipInfo;
		/**@}*/
		/**
		*name
		*	Minification.
		*/
		/**@{*/
		struct MinifyPipeline
		{
			struct Configuration
			{
				castor::Point2i textureSize;
				int previousLevel;
			};

			renderer::TextureView const * sourceView;
			renderer::TextureViewPtr targetView;
			renderer::DescriptorSetPtr descriptor;
			renderer::FrameBufferPtr frameBuffer;
			renderer::PushConstantsBufferPtr< Configuration > previousLevel;
			renderer::PushConstantRange pushConstants;
			renderer::PipelinePtr pipeline;
		};

		renderer::ShaderStageStateArray m_minifyProgram;
		renderer::DescriptorSetLayoutPtr m_minifyDescriptorLayout;
		renderer::PipelineLayoutPtr m_minifyPipelineLayout;
		renderer::DescriptorSetPoolPtr m_minifyDescriptorPool;
		std::array< MinifyPipeline, MaxMipLevel > m_minifyPipelines;
		/**@}*/

	};
}

#endif
