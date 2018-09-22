/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FinalCombinePass_H___
#define ___C3D_FinalCombinePass_H___

#include "Shader/Ubos/GpInfoUbo.hpp"
#include "Shader/Ubos/SceneUbo.hpp"
#include "Technique/RenderTechniqueVisitor.hpp"
#include "TransparentPass.hpp"

#include <GlslShader.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Program used to combine the transparent and opaque passes.
	\~french
	\brief		Programme utilisé pour combiner les passes opaque et transparente.
	*/
	struct FinalCombineProgram
	{
		FinalCombineProgram( FinalCombineProgram const & rhs ) = delete;
		FinalCombineProgram & operator=( FinalCombineProgram const & rhs ) = delete;
		FinalCombineProgram( FinalCombineProgram && rhs ) = default;
		FinalCombineProgram & operator=( FinalCombineProgram && rhs ) = default;
		FinalCombineProgram( Engine & engine
			, ashes::RenderPass const & renderPass
			, RenderPassTimer & timer
			, ashes::DescriptorSetLayout const & uboLayout
			, ashes::DescriptorSetLayout const & texLayout
			, ashes::VertexLayout const & vtxLayout
			, FogType fogType );
		~FinalCombineProgram();
		void prepare( ashes::FrameBuffer const & frameBuffer
			, ashes::DescriptorSet const & uboDescriptorSet
			, ashes::DescriptorSet const & texDescriptorSet
			, ashes::BufferBase const & vbo );
		void accept( RenderTechniqueVisitor & visitor );
		inline ashes::CommandBuffer const & getCommandBuffer()const
		{
			REQUIRE( m_commandBuffer );
			return *m_commandBuffer;
		}

	private:
		RenderPassTimer & m_timer;
		ashes::RenderPass const & m_renderPass;
		glsl::Shader m_vertexShader;
		glsl::Shader m_pixelShader;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::PipelinePtr m_pipeline;
		ashes::CommandBufferPtr m_commandBuffer;
	};
	//!\~english	An array of FinalCombineProgram, one per fog type.
	//!\~french		Un tableau de FinalCombineProgram, un par type de brouillard.
	using FinalCombinePrograms = std::array< FinalCombineProgram, size_t( FogType::eCount ) >;
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Pass used to combine the transparent and opaque passes.
	\~french
	\brief		Passe utilisée pour combiner les passes opaque et transparente.
	*/
	class FinalCombinePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	size		The render size.
		 *\param[in]	sceneUbo	The scene UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	size		La taille du rendu.
		 *\param[in]	sceneUbo	L'UBO de la scène.
		 */
		FinalCombinePass( Engine & engine
			, castor::Size const & size
			, SceneUbo & sceneUbo
			, WeightedBlendTextures const & wbResult
			, ashes::TextureView const & colourView );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~FinalCombinePass();
		/**
		 *\~english
		 *\brief		Updates the UBOs.
		 *\param[in]	camera		The viewing camera.
		 *\param[in]	invViewProj	The inverse view projection matrix.
		 *\param[in]	invView		The inverse view matrix.
		 *\param[in]	invProj		The inverse projection matrix.
		 *\~french
		 *\brief		Met à jour les UBOs.
		 *\param[in]	camera		La caméra.
		 *\param[in]	invViewProj	La matrice vue projection inversée.
		 *\param[in]	invView		La matrice vue inversée.
		 *\param[in]	invProj		La matrice projection inversée.
		 */
		void update( Camera const & camera
			, castor::Matrix4x4r const & invViewProj
			, castor::Matrix4x4r const & invView
			, castor::Matrix4x4r const & invProj );
		/**
		 *\~english
		 *\brief		Renders the combine pass.
		 *\~french
		 *\brief		Dessine la passe de combinaison.
		 */
		ashes::Semaphore const & render( FogType fogType
			, ashes::Semaphore const & toWait );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( RenderTechniqueVisitor & visitor );

	private:
		castor::Size m_size;
		Engine & m_engine;
		SceneUbo & m_sceneUbo;
		GpInfoUbo m_gpInfo;
		SamplerSPtr m_sampler;
		ashes::VertexBufferPtr< TexturedQuad > m_vertexBuffer;
		ashes::VertexLayoutPtr m_vertexLayout;
		ashes::DescriptorSetLayoutPtr m_uboDescriptorLayout;
		ashes::DescriptorSetPoolPtr m_uboDescriptorPool;
		ashes::DescriptorSetPtr m_uboDescriptorSet;
		ashes::DescriptorSetLayoutPtr m_texDescriptorLayout;
		ashes::DescriptorSetPoolPtr m_texDescriptorPool;
		ashes::DescriptorSetPtr m_texDescriptorSet;
		ashes::RenderPassPtr m_renderPass;
		RenderPassTimerSPtr m_timer;
		FinalCombinePrograms m_programs;
		ashes::FrameBufferPtr m_frameBuffer;
		ashes::SemaphorePtr m_semaphore;
	};
}

#endif
