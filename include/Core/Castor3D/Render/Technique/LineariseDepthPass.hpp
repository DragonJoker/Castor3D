/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LineariseDepthPass_H___
#define ___C3D_LineariseDepthPass_H___

#include "Castor3D/Render/PostEffect/PostEffectModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/PostEffect/PostEffect.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <ShaderAST/Shader.hpp>

#include <CastorUtils/Design/ChangeTracked.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Image/Sampler.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#define C3D_DebugLinearisePass 0

namespace castor3d
{
	class LineariseDepthPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	depthBuffer		The non linearised depth buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	depthBuffer		Le tampon de profondeur non linéarisé.
		 */
		C3D_API LineariseDepthPass( Engine & engine
			, castor::String prefix
			, VkExtent2D const & size
			, ashes::ImageView const & depthBuffer );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~LineariseDepthPass();
		/**
		 *\~english
		 *\brief		Updates clipping info.
		 *\param[in]	viewport	The viewport containing the clipping data.
		 *\~french
		 *\brief		Met à jour les informations de clipping.
		 *\param[in]	viewport	Le viewport contenant les données de clipping.
		 */
		C3D_API void update( Viewport const & viewport
			, ashes::CommandBuffer * cb );
		/**
		 *\~english
		 *\brief		Updates clipping info.
		 *\param[in]	viewport	The viewport containing the clipping data.
		 *\~french
		 *\brief		Met à jour les informations de clipping.
		 *\param[in]	viewport	Le viewport contenant les données de clipping.
		 */
		C3D_API void update( Viewport const & viewport );
		/**
		 *\~english
		 *\brief		Linearises depth buffer.
		 *\~french
		 *\brief		Linéarise le tampon de profondeur.
		 */
		C3D_API ashes::Semaphore const & linearise( ashes::Semaphore const & toWait )const;
		/**
		 *\~english
		 *\param[in]	timer	The render timer.
		 *\return		The commands used to render the pass.
		 *\~french
		 *\param[in]	timer	Le timer de rendu.
		 *\return		Les commandes utilisées pour rendre la passe.
		 */
		C3D_API CommandsSemaphore getCommands( RenderPassTimer const & timer
			, uint32_t index )const;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( PipelineVisitorBase & visitor );
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

		inline ashes::CommandBuffer const & getCommands()const
		{
			return *m_commandBuffer;
		}
		/**@}*/

	private:
		void doInitialiseLinearisePass();
		void doInitialiseMinifyPass();
		void doCleanupLinearisePass();
		void doCleanupMinifyPass();
		void doPrepareFrame( ashes::CommandBuffer & cb
			, RenderPassTimer const & timer
			, uint32_t index )const;

	public:
		static constexpr uint32_t MaxMipLevel = 5u;
		static constexpr uint32_t LogMaxOffset = 3u;
		static constexpr float MinRadius = 1.0f;

	private:
		Engine & m_engine;
		ashes::ImageView const & m_depthBuffer;
		castor::String m_prefix;
		VkExtent2D m_size;
		TextureUnit m_result;
		RenderPassTimerSPtr m_timer;
		ashes::RenderPassPtr m_renderPass;
		ashes::VertexBufferPtr< NonTexturedQuad > m_vertexBuffer;
		ashes::PipelineVertexInputStateCreateInfoPtr m_vertexLayout;
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
		ashes::PipelineShaderStageCreateInfoArray m_lineariseProgram;
		ashes::ImageView m_linearisedView;
		ashes::FrameBufferPtr m_lineariseFrameBuffer;
		ashes::DescriptorSetLayoutPtr m_lineariseDescriptorLayout;
		ashes::DescriptorSetPoolPtr m_lineariseDescriptorPool;
		ashes::DescriptorSetPtr m_lineariseDescriptor;
		ashes::PipelineLayoutPtr m_linearisePipelineLayout;
		ashes::GraphicsPipelinePtr m_linearisePipeline;
		UniformBufferUPtr< castor::Point3f > m_clipInfo;
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
			ashes::ImageView const * sourceView;
			ashes::ImageView targetView;
			ashes::DescriptorSetPtr descriptor;
			ashes::FrameBufferPtr frameBuffer;
			ashes::GraphicsPipelinePtr pipeline;
		};

		UniformBufferUPtr< MinifyConfiguration > m_previousLevel;
		ShaderModule m_minifyVertexShader;
		ShaderModule m_minifyPixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_minifyProgram;
		ashes::DescriptorSetLayoutPtr m_minifyDescriptorLayout;
		ashes::PipelineLayoutPtr m_minifyPipelineLayout;
		ashes::DescriptorSetPoolPtr m_minifyDescriptorPool;
		std::array< MinifyPipeline, MaxMipLevel > m_minifyPipelines;
		/**@}*/

	};
}

#endif
