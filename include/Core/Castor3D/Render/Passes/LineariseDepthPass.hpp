/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LineariseDepthPass_H___
#define ___C3D_LineariseDepthPass_H___

#include "PassesModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>

#include <ShaderAST/Shader.hpp>

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
			, castor::String const & prefix
			, castor::Size const & size
			, ashes::ImageView const & depthBuffer );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~LineariseDepthPass() = default;
		C3D_API void initialise( RenderDevice const & device );
		C3D_API void cleanup( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Updates clipping info.
		 *\param[in]	viewport	The viewport containing the clipping data.
		 *\~french
		 *\brief		Met à jour les informations de clipping.
		 *\param[in]	viewport	Le viewport contenant les données de clipping.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Updates clipping info.
		 *\~french
		 *\brief		Met à jour les informations de clipping.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Linearises depth buffer.
		 *\~french
		 *\brief		Linéarise le tampon de profondeur.
		 */
		C3D_API ashes::Semaphore const & linearise( RenderDevice const & device
			, ashes::Semaphore const & toWait )const;
		/**
		 *\~english
		 *\param[in]	timer	The render timer.
		 *\return		The commands used to render the pass.
		 *\~french
		 *\param[in]	timer	Le timer de rendu.
		 *\return		Les commandes utilisées pour rendre la passe.
		 */
		C3D_API CommandsSemaphore getCommands( RenderDevice const & device
			, RenderPassTimer const & timer
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
		void doInitialiseLinearisePass( RenderDevice const & device );
		void doInitialiseMinifyPass( RenderDevice const & device );
		void doCleanupLinearisePass( RenderDevice const & device );
		void doCleanupMinifyPass( RenderDevice const & device );
		void doPrepareFrame( ashes::CommandBuffer & cb
			, RenderPassTimer const & timer
			, uint32_t index )const;

	public:
		static constexpr uint32_t MaxMipLevel = 5u;

	private:
		Engine & m_engine;
		ashes::ImageView const & m_srcDepthBuffer;
		ashes::ImageView m_depthBuffer;
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
		bool m_initialised{ false };
		/**
		*name
		*	Common.
		*/
		/**@{*/
		struct Layout
		{
			ashes::DescriptorSetLayoutPtr descriptorLayout;
			ashes::DescriptorSetPoolPtr descriptorPool;
			ashes::PipelineLayoutPtr pipelineLayout;
		};
		/**@}*/
		/**
		*name
		*	Linearisation.
		*/
		/**@{*/
		ShaderModule m_lineariseVertexShader;
		ShaderModule m_linearisePixelShader;
		ashes::ImageView m_linearisedView;
		Layout m_lineariseLayout;
		ashes::DescriptorSetPtr m_lineariseDescriptor;
		ashes::FrameBufferPtr m_lineariseFrameBuffer;
		ashes::GraphicsPipelinePtr m_linearisePipeline;
		UniformBufferOffsetT< castor::Point3f > m_clipInfo;
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

		std::vector< UniformBufferOffsetT< MinifyConfiguration > > m_previousLevel;
		ShaderModule m_minifyVertexShader;
		ShaderModule m_minifyPixelShader;
		Layout m_minifyLayout;
		std::array< MinifyPipeline, MaxMipLevel > m_minifyPipelines;
		/**@}*/

	};
}

#endif
