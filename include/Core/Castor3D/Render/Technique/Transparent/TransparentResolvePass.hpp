/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TransparentResolvePass_H___
#define ___C3D_TransparentResolvePass_H___

#include "TransparentModule.hpp"

#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Render/Technique/TechniqueModule.hpp"
#include "Castor3D/Render/Technique/Transparent/TransparentPass.hpp"

#include <ShaderAST/Shader.hpp>

namespace castor3d
{
	struct TransparentResolveProgram
	{
		TransparentResolveProgram( TransparentResolveProgram const & rhs ) = delete;
		TransparentResolveProgram & operator=( TransparentResolveProgram const & rhs ) = delete;
		TransparentResolveProgram( TransparentResolveProgram && rhs ) = default;
		TransparentResolveProgram & operator=( TransparentResolveProgram && rhs ) = delete;
		TransparentResolveProgram( Engine & engine
			, RenderDevice const & device
			, ashes::RenderPass const & renderPass
			, RenderPassTimer & timer
			, ashes::DescriptorSetLayout const & uboLayout
			, ashes::DescriptorSetLayout const & texLayout
			, ashes::PipelineVertexInputStateCreateInfo const & vtxLayout
			, FogType fogType );
		~TransparentResolveProgram();
		void prepare( ashes::FrameBuffer const & frameBuffer
			, ashes::DescriptorSet const & uboDescriptorSet
			, ashes::DescriptorSet const & texDescriptorSet
			, ashes::BufferBase const & vbo
			, TransparentPassResult const & wbResult );
		void accept( PipelineVisitorBase & visitor );
		inline ashes::CommandBuffer const & getCommandBuffer()const
		{
			CU_Require( m_commandBuffer );
			return *m_commandBuffer;
		}

	private:
		Engine & m_engine;
		RenderPassTimer & m_timer;
		ashes::RenderPass const & m_renderPass;
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShader;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::GraphicsPipelinePtr m_pipeline;
		ashes::CommandBufferPtr m_commandBuffer;
	};
	using FinalCombineProgramPtr = std::unique_ptr< TransparentResolveProgram >;
	//!\~english	An array of TransparentResolveProgram, one per fog type.
	//!\~french		Un tableau de TransparentResolveProgram, un par type de brouillard.
	using FinalCombineProgramMap = std::map< FogType, FinalCombineProgramPtr >;

	class TransparentResolvePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	device			The GPU device.
		 *\param[in]	size			The render size.
		 *\param[in]	sceneUbo		The scene UBO.
		 *\param[in]	hdrConfigUbo	The HDR UBO.
		 *\param[in]	gpInfoUbo		The geometry pass UBO.
		 *\param[in]	wbResult		The accumulation pass buffers.
		 *\param[in]	colourView		The target colour view.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	size			La taille du rendu.
		 *\param[in]	sceneUbo		L'UBO de la scène.
		 *\param[in]	hdrConfigUbo	L'UBO HDR.
		 *\param[in]	gpInfoUbo		L'UBO de la passe géométrique.
		 *\param[in]	wbResult		Les tampons de la passe d'accumulation.
		 *\param[in]	colourView		La vue couleur cible.
		 */
		TransparentResolvePass( Engine & engine
			, RenderDevice const & device
			, castor::Size const & size
			, SceneUbo & sceneUbo
			, HdrConfigUbo const & hdrConfigUbo
			, GpInfoUbo const & gpInfoUbo
			, TransparentPassResult const & wbResult
			, ashes::ImageView const & colourView );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~TransparentResolvePass();
		/**
		 *\~english
		 *\brief		Renders the combine pass.
		 *\param[in]	device	The GPU device.
		 *\param[in]	fogType	The fog type.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine la passe de combinaison.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	fogType	Le type de brouillard.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		ashes::Semaphore const & render( RenderDevice const & device
			, FogType fogType
			, ashes::Semaphore const & toWait );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( RenderTechniqueVisitor & visitor );

	private:
		TransparentResolveProgram * doGetProgram( RenderDevice const & device
			, FogType type );

	private:
		castor::Size m_size;
		Engine & m_engine;
		SceneUbo & m_sceneUbo;
		GpInfoUbo const & m_gpInfo;
		TransparentPassResult const & m_wbResult;
		SamplerSPtr m_sampler;
		ashes::VertexBufferPtr< TexturedQuad > m_vertexBuffer;
		ashes::PipelineVertexInputStateCreateInfoPtr m_vertexLayout;
		ashes::DescriptorSetLayoutPtr m_uboDescriptorLayout;
		ashes::DescriptorSetPoolPtr m_uboDescriptorPool;
		ashes::DescriptorSetPtr m_uboDescriptorSet;
		ashes::DescriptorSetLayoutPtr m_texDescriptorLayout;
		ashes::DescriptorSetPoolPtr m_texDescriptorPool;
		ashes::DescriptorSetPtr m_texDescriptorSet;
		ashes::RenderPassPtr m_renderPass;
		RenderPassTimerSPtr m_timer;
		FinalCombineProgramMap m_programs;
		ashes::FrameBufferPtr m_frameBuffer;
		ashes::SemaphorePtr m_semaphore;
	};
}

#endif
