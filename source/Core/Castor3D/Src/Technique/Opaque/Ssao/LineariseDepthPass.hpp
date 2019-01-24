/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LineariseDepthPass_H___
#define ___C3D_LineariseDepthPass_H___

#include "Render/Viewport.hpp"
#include "Technique/RenderTechniqueVisitor.hpp"
#include "Texture/TextureUnit.hpp"

#include <ShaderWriter/Shader.hpp>

#include <Design/ChangeTracked.hpp>

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
			, ashes::Extent2D const & size
			, SsaoConfigUbo & ssaoConfigUbo
			, ashes::TextureView const & depthBuffer );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~LineariseDepthPass();
		/**
		 *\~english
		 *\brief		Updates clip info.
		 *\~french
		 *\brief		Met à jour les informations de clipping.
		 */
		void update( Camera const & camera );
		/**
		 *\~english
		 *\brief		Linearises depth buffer.
		 *\~french
		 *\brief		Linéarise le tampon de profondeur.
		 */
		ashes::Semaphore const & linearise( ashes::Semaphore const & toWait )const;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor );
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
		/**@}*/

	private:
		void doInitialiseLinearisePass();
		void doInitialiseMinifyPass();
		void doCleanupLinearisePass();
		void doCleanupMinifyPass();
		void doPrepareFrame();

	public:
		static constexpr uint32_t MaxMipLevel = 5u;
		static constexpr uint32_t LogMaxOffset = 3u;
		static constexpr float MinRadius = 1.0f;

	private:
		Engine & m_engine;
		SsaoConfigUbo & m_ssaoConfigUbo;
		ashes::TextureView const & m_depthBuffer;
		ashes::Extent2D m_size;
		TextureUnit m_result;
		RenderPassTimerSPtr m_timer;
		ashes::RenderPassPtr m_renderPass;
		ashes::VertexBufferPtr< NonTexturedQuad > m_vertexBuffer;
		ashes::VertexLayoutPtr m_vertexLayout;
		ashes::SamplerPtr m_lineariseSampler;
		ashes::SamplerPtr m_minifySampler;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::SemaphorePtr m_finished;
		/**
		*name
		*	Linearisation.
		*/
		/**@{*/
		ShaderModule m_lineariseVertexShader;
		ShaderModule m_linearisePixelShader;
		ashes::ShaderStageStateArray m_lineariseProgram;
		ashes::TextureViewPtr m_linearisedView;
		ashes::FrameBufferPtr m_lineariseFrameBuffer;
		ashes::DescriptorSetLayoutPtr m_lineariseDescriptorLayout;
		ashes::DescriptorSetPoolPtr m_lineariseDescriptorPool;
		ashes::DescriptorSetPtr m_lineariseDescriptor;
		ashes::PipelineLayoutPtr m_linearisePipelineLayout;
		ashes::PipelinePtr m_linearisePipeline;
		ashes::UniformBufferPtr< castor::Point3f > m_clipInfo;
		castor::ChangeTracked< castor::Point3f > m_clipInfoValue;
		/**@}*/
		/**
		*name
		*	Minification.
		*/
		/**@{*/
		struct MinifyConfiguration
		{
			castor::Point2i textureSize;
			int previousLevel;
		};
		struct MinifyPipeline
		{
			ashes::TextureView const * sourceView;
			ashes::TextureViewPtr targetView;
			ashes::DescriptorSetPtr descriptor;
			ashes::FrameBufferPtr frameBuffer;
			ashes::PipelinePtr pipeline;
		};

		ashes::UniformBufferPtr< MinifyConfiguration > m_previousLevel;
		ShaderModule m_minifyVertexShader;
		ShaderModule m_minifyPixelShader;
		ashes::ShaderStageStateArray m_minifyProgram;
		ashes::DescriptorSetLayoutPtr m_minifyDescriptorLayout;
		ashes::PipelineLayoutPtr m_minifyPipelineLayout;
		ashes::DescriptorSetPoolPtr m_minifyDescriptorPool;
		std::array< MinifyPipeline, MaxMipLevel > m_minifyPipelines;
		/**@}*/

	};
}

#endif
