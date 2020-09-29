/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredLightPass_H___
#define ___C3D_DeferredLightPass_H___

#include "LightingModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"

#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class LightPass
	{
	protected:
		struct RenderPass
		{
			RenderPass( std::string const & name
				, ashes::RenderPassPtr renderPass
				, LightPassResult const & lpResult );
			ashes::RenderPassPtr renderPass;
			ashes::FrameBufferPtr frameBuffer;
		};
		/**
		\author		Sylvain DOREMUS
		\version	0.10.0
		\date		08/06/2017
		\~english
		\brief		Base class for all light pass programs.
		\~french
		\brief		Classe de base pour tous les programmes des passes d'éclairage.
		*/
		struct Program
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\param[in]	engine		The engine.
			 *\param[in]	vtx			The vertex shader source.
			 *\param[in]	pxl			The fragment shader source.
			 *\param[in]	hasShadows	Tells if this program uses shadow map.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	engine		Le moteur.
			 *\param[in]	vtx			Le source du vertex shader.
			 *\param[in]	pxl			Le source du fagment shader.
			 *\param[in]	hasShadows	Dit si ce programme utilise une shadow map.
			 */
			Program( Engine & engine
				, RenderDevice const & device
				, castor::String const & name
				, ShaderModule const & vtx
				, ShaderModule const & pxl
				, bool hasShadows );
			/**
			 *\~english
			 *\brief		Destructor.
			 *\~french
			 *\brief		Destructeur.
			 */
			virtual ~Program() = default;
			/**
			 *\~english
			 *\brief		Initialises the program and its pipeline.
			 *\param[in]	vbo				The vertex buffer containing the object to render.
			 *\param[in]	vertexLayout	The vertex buffer layout.
			 *\param[in]	firstRenderPass	The render pass for a first light source.
			 *\param[in]	blendRenderPass	The render pass for other light sources.
			 *\param[in]	matrixUbo		The matrix UBO.
			 *\param[in]	sceneUbo		The scene UBO.
			 *\param[in]	gpInfoUbo		The geometry pass UBO.
			 *\param[in]	modelMatrixUbo	The optional model matrix UBO.
			 *\~french
			 *\brief		Initialise le programme et son pipeline.
			 *\param[in]	vbo				Le tampon de sommets contenant l'objet à dessiner.
			 *\param[in]	vertexLayout	Le layout du tampon de sommets.
			 *\param[in]	firstRenderPass	La passe de rendu pour la première source lumineuse.
			 *\param[in]	blendRenderPass	La passe de rendu pour les autres sources lumineuses.
			 *\param[in]	matrixUbo		L'UBO des matrices.
			 *\param[in]	sceneUbo		L'UBO de la scène.
			 *\param[in]	gpInfoUbo		L'UBO de la geometry pass.
			 *\param[in]	modelMatrixUbo	L'UBO optionnel de matrices modèle.
			 */
			void initialise( ashes::VertexBufferBase & vbo
				, ashes::PipelineVertexInputStateCreateInfo const & vertexLayout
				, ashes::RenderPass const & firstRenderPass
				, ashes::RenderPass const & blendRenderPass
				, MatrixUbo & matrixUbo
				, SceneUbo & sceneUbo
				, GpInfoUbo const & gpInfoUbo
				, UniformBufferOffsetT< ModelMatrixUboConfiguration > const * modelMatrixUbo );
			/**
			*\~english
			*\brief		Cleans up the program and its pipeline.
			*\~french
			*\brief		Nettoie le programme et son pipeline.
			*/
			void cleanup();
			/**
			 *\~english
			 *\brief		Binds a light.
			 *\param[in]	light	The light.
			 *\~french
			 *\brief		Active une source lumineuse.
			 *\param[in]	light	La lumière.
			 */
			void bind( Light const & light );
			/**
			 *\~english
			 *\brief		Renders the light pass.
			 *\param[in]	commandBuffer	The command buffer.
			 *\param[in]	count			The number of primitives to draw.
			 *\param[in]	first			Tells if this is the first light pass (\p true) or not (\p false).
			 *\param[in]	offset			The VBO offset.
			 *\~french
			 *\brief		Dessine la passe d'éclairage.
			 *\param[in]	commandBuffer	Le tampon de commandes.
			 *\param[in]	count			Le nombre de primitives à dessiner.
			 *\param[in]	first			Dit si cette passe d'éclairage est la première (\p true) ou pas (\p false).
			 *\param[in]	offset			L'offset dans le VBO.
			 */
			void render( ashes::CommandBuffer & commandBuffer
				, uint32_t count
				, bool first
				, uint32_t offset )const;
			/**
			*\~english
			*name
			*	Getters.
			*\~french
			*name
			*	Accesseurs.
			*/
			/**@{*/
			inline ashes::DescriptorSetLayout const & getUboDescriptorLayout()const
			{
				CU_Require( m_uboDescriptorLayout );
				return *m_uboDescriptorLayout;
			}

			inline ashes::DescriptorSetLayout const & getTextureDescriptorLayout()const
			{
				CU_Require( m_textureDescriptorLayout );
				return *m_textureDescriptorLayout;
			}

			inline ashes::DescriptorSetPool const & getUboDescriptorPool()const
			{
				CU_Require( m_uboDescriptorPool );
				return *m_uboDescriptorPool;
			}

			inline ashes::DescriptorSetPool const & getTextureDescriptorPool()const
			{
				CU_Require( m_textureDescriptorPool );
				return *m_textureDescriptorPool;
			}

			inline ashes::PipelineLayout const & getPipelineLayout()const
			{
				CU_Require( m_pipelineLayout );
				return *m_pipelineLayout;
			}
			/**@}*/

		protected:
			/**
			 *\~english
			 *\brief		Renders the light pass.
			 *\param[in]	commandBuffer	The command buffer.
			 *\param[in]	count			The number of primitives to draw.
			 *\param[in]	first			Tells if this is the first light pass (\p true) or not (\p false).
			 *\param[in]	offset			The VBO offset.
			 *\~french
			 *\brief		Dessine la passe d'éclairage.
			 *\param[in]	commandBuffer	Le tampon de commandes.
			 *\param[in]	count			Le nombre de primitives à dessiner.
			 *\param[in]	first			Dit si cette passe d'éclairage est la première (\p true) ou pas (\p false).
			 *\param[in]	offset			L'offset dans le VBO.
			 */
			virtual void doRender( ashes::CommandBuffer & commandBuffer
				, uint32_t count
				, bool first
				, uint32_t offset )const;

		private:
			/**
			 *\~english
			 *\brief		Creates a pipeline.
			 *\param[in]	vertexLayout	The vertex buffer layout.
			 *\param[in]	renderPass		The render pass to use.
			 *\param[in]	blend			Tells if the pipeline must enable blending.
			 *\return		The created pipeline.
			 *\~french
			 *\brief		Crée un pipeline.
			 *\param[in]	vertexLayout	Le layout du tampon de sommets.
			 *\param[in]	renderPass		La passe de rendu à utiliser.
			 *\param[in]	blend			Dit si le pipeline doit activer le blending.
			 *\return		Le pipeline créé.
			 */
			virtual ashes::GraphicsPipelinePtr doCreatePipeline( ashes::PipelineVertexInputStateCreateInfo const & vertexLayout
				, ashes::RenderPass const & renderPass
				, bool blend ) = 0;
			/**
			 *\~english
			 *\brief		Binds a light.
			 *\param[in]	light	The light.
			 *\~french
			 *\brief		Active une source lumineuse.
			 *\param[in]	light	La lumière.
			 */
			virtual void doBind( Light const & light ) = 0;

		public:
			Engine & m_engine;
			RenderDevice const & m_device;
			castor::String m_name;
			ashes::PipelineShaderStageCreateInfoArray m_program;
			ashes::DescriptorSetLayoutPtr m_uboDescriptorLayout;
			ashes::DescriptorSetPoolPtr m_uboDescriptorPool;
			ashes::DescriptorSetLayoutPtr m_textureDescriptorLayout;
			ashes::DescriptorSetPoolPtr m_textureDescriptorPool;
			ashes::PipelineLayoutPtr m_pipelineLayout;
			ashes::GraphicsPipelinePtr m_blendPipeline;
			ashes::GraphicsPipelinePtr m_firstPipeline;
			bool m_shadows;
		};
		using ProgramPtr = std::unique_ptr< Program >;

	public:
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~LightPass() = default;
		/**
		 *\~english
		 *\brief		Initialises the light pass.
		 *\param[in]	scene		The scene.
		 *\param[in]	gp			The geometry pass buffers.
		 *\param[in]	sceneUbo	The scene UBO.
		 *\param[in]	timer		The render pass timer.
		 *\~french
		 *\brief		Initialise la passe d'éclairage.
		 *\param[in]	scene		La scène.
		 *\param[in]	gp			Les tampons de la passe géométries.
		 *\param[in]	sceneUbo	L'UBO de scène.
		 *\param[in]	timer		Le timer de passe.
		 */
		virtual void initialise( Scene const & scene
			, OpaquePassResult const & gp
			, SceneUbo & sceneUbo
			, RenderPassTimer & timer ) = 0;
		/**
		 *\~english
		 *\brief		Cleans up the light pass.
		 *\~french
		 *\brief		Nettoie la passe d'éclairage.
		 */
		virtual void cleanup() = 0;
		/**
		 *\~english
		 *\brief		Updates the light pass.
		 *\param[in]	first			Tells if this is the first pass.
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	light			The light.
		 *\param[in]	camera			The viewing camera.
		 *\param[in]	shadowMap		The optional shadow map.
		 *\param[in]	shadowMapIndex	The shadow map index.
		 *\~french
		 *\brief		Met à jour la passe d'éclairage.
		 *\param[in]	first			Dit s'il s'agit de la première passe.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	light			La source lumineuse.
		 *\param[in]	camera			La caméra.
		 *\param[in]	shadowMap		La texture d'ombres, optionnelle.
		 *\param[in]	shadowMapIndex	L'index de la texture d'ombres.
		 */
		void update( bool first
			, castor::Size const & size
			, Light const & light
			, Camera const & camera
			, ShadowMap const * shadowMap
			, uint32_t shadowMapIndex );
		/**
		 *\~english
		 *\brief		Renders the light pass.
		 *\param[in]	index	The render pass index.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine la passe de rendu.
		 *\param[in]	index	L'indice de la passe de rendu.
		 *\param[in]	toWait	Le sémaphore de la précédente passe de rendu.
		 */
		virtual ashes::Semaphore const & render( uint32_t index
			, ashes::Semaphore const & toWait );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		virtual void accept( PipelineVisitorBase & visitor ) = 0;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		/**
		 *\~english
		 *\return		The number of primitives to draw.
		 *\~french
		 *\return		Le nombre de primitives à dessiner.
		 */
		virtual uint32_t getCount()const = 0;

		inline ashes::Semaphore const & getSemaphore()const
		{
			CU_Require( m_signalImgFinished );
			return *m_signalImgFinished;
		}

		inline castor::String const & getName()const
		{
			return m_name;
		}
		/**@}*/

	public:
		struct Pipeline
		{
			ProgramPtr program;
			ashes::DescriptorSetPtr uboDescriptorSet;
			ashes::WriteDescriptorSetArray textureWrites;
			ashes::DescriptorSetPtr textureDescriptorSet;
			ashes::CommandBufferPtr firstCommandBuffer;
			ashes::CommandBufferPtr blendCommandBuffer;
			bool isFirstSet{ false };
			bool isBlendSet{ false };
		};
		static size_t makeKey( Light const & light
			, ShadowMap const * shadowMap );
		using PipelinePtr = std::unique_ptr< Pipeline >;
		using PipelineMap = std::map< size_t, PipelinePtr >;
		using PipelineArray = std::array< Pipeline, size_t( ShadowType::eCount ) * 2u >; // * 2u for volumetric scattering or not.

		Pipeline createPipeline( LightType lightType
			, ShadowType shadowType
			, bool rsm
			, ShadowMap const * shadowMap );

	protected:
		virtual Pipeline * doGetPipeline( bool first
			, Light const & light
			, ShadowMap const * shadowMap );

	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	firstRenderPass	The render pass for a first light source.
		 *\param[in]	blendRenderPass	The render pass for other light sources.
		 *\param[in]	lpResult		The light pass result.
		 *\param[in]	gpInfoUbo		The geometry pass UBO.
		 *\param[in]	hasShadows		Tells if shadows are enabled for this light pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	firstRenderPass	La passe de rendu pour la première source lumineuse.
		 *\param[in]	blendRenderPass	La passe de rendu pour les autres sources lumineuses.
		 *\param[in]	lpResult		Le résultat de la passe d'éclairage.
		 *\param[in]	gpInfoUbo		L'UBO de la geometry pass.
		 *\param[in]	hasShadows		Dit si les ombres sont activées pour cette passe d'éclairage.
		 */
		LightPass( Engine & engine
			, RenderDevice const & device
			, castor::String const & suffix
			, ashes::RenderPassPtr && firstRenderPass
			, ashes::RenderPassPtr && blendRenderPass
			, LightPassResult const & lpResult
			, GpInfoUbo const & gpInfoUbo
			, bool hasShadows );
		/**
		 *\~english
		 *\brief		Initialises the light pass.
		 *\param[in]	scene			The scene.
		 *\param[in]	gp				The geometry pass buffers.
		 *\param[in]	type			The light source type.
		 *\param[in]	vbo				The VBO.
		 *\param[in]	vertexLayout	The vertex buffer layout.
		 *\param[in]	sceneUbo		The scene UBO.
		 *\param[in]	modelMatrixUbo	The optional model matrix UBO.
		 *\param[in]	timer			The render pass timer.
		 *\~french
		 *\brief		Initialise la passe d'éclairage.
		 *\param[in]	scene			La scène.
		 *\param[in]	gp				Les tampons de la passe géométries.
		 *\param[in]	type			Le type de source lumineuse.
		 *\param[in]	vbo				Le VBO.
		 *\param[in]	vertexLayout	Le layout du tampon de sommets.
		 *\param[in]	sceneUbo		L'UBO de scène.
		 *\param[in]	modelMatrixUbo	L'UBO optionnel de matrices modèle.
		 *\param[in]	timer			Le timer de passe.
		 */
		void doInitialise( Scene const & scene
			, OpaquePassResult const & gp
			, LightType type
			, ashes::VertexBufferBase & vbo
			, ashes::PipelineVertexInputStateCreateInfo const & vertexLayout
			, SceneUbo & sceneUbo
			, UniformBufferOffsetT< ModelMatrixUboConfiguration > const * modelMatrixUbo
			, RenderPassTimer & timer );
		/**
		 *\~english
		 *\brief		Cleans up the light pass.
		 *\~french
		 *\brief		Nettoie la passe d'éclairage.
		 */
		void doCleanup();
		/**
		 *\~english
		 *\brief		Updates the light pass.
		 *\param[in]	first			Tells if this is the first pass.
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	light			The light.
		 *\param[in]	camera			The viewing camera.
		 *\param[in]	shadowMap		The optional shadow map.
		 *\param[in]	shadowMapIndex	The shadow map index.
		 *\~french
		 *\brief		Met à jour la passe d'éclairage.
		 *\param[in]	first			Dit s'il s'agit de la première passe.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	light			La source lumineuse.
		 *\param[in]	camera			La caméra.
		 *\param[in]	shadowMap		La texture d'ombres, optionnelle.
		 *\param[in]	shadowMapIndex	L'index de la texture d'ombres.
		 */
		virtual void doUpdate( bool first
			, castor::Size const & size
			, Light const & light
			, Camera const & camera
			, ShadowMap const * shadowMap
			, uint32_t shadowMapIndex ) = 0;
		/**
		 *\~english
		 *\brief		Prepares the command buffer for given pipeline.
		 *\param[in]	pipeline		The light pass pipeline.
		 *\param[in]	shadowMap		The optional shadow map.
		 *\param[in]	first			Tells if this is the first pass (\p true) or the blend pass (\p false).
		 *\~french
		 *\brief		Prépare le tampon de commandes du pipeline donné.
		 *\param[in]	pipeline		Le pipeline de la passe d'éclairage.
		 *\param[in]	shadowMap		La texture d'ombres, optionnelle.
		 *\param[in]	first			Dit s'il s'agit de la première passe (\p true) ou la passe de mélange (\p false).
		 */
		void doPrepareCommandBuffer( Pipeline & pipeline
			, ShadowMap const * shadowMap
			, bool first );
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source for this light pass.
		 *\param[in]	sceneFlags	The scene flags.
		 *\param[in]	lightType	The light source type.
		 *\param[in]	shadowType	The shadow type.
		 *\param[in]	volumetric	Tells if volumetric light scattering is to be enabled.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du pixel shader pour cette passe lumineuse.
		 *\param[in]	sceneFlags	Les indicateurs de scène.
		 *\param[in]	lightType	Le type de source lumineuse.
		 *\param[in]	shadowType	Le type d'ombres.
		 *\param[in]	volumetric	Dit si le volumetric light scattering doit être activé.
		 *\return		Le source.
		 */
		virtual ShaderPtr doGetPhongPixelShaderSource( SceneFlags const & sceneFlags
			, LightType lightType
			, ShadowType shadowType
			, bool rsm )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source for this light pass.
		 *\param[in]	sceneFlags	The scene flags.
		 *\param[in]	lightType	The light source type.
		 *\param[in]	shadowType	The shadow type.
		 *\param[in]	volumetric	Tells if volumetric light scattering is to be enabled.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du pixel shader pour cette passe lumineuse.
		 *\param[in]	sceneFlags	Les indicateurs de scène.
		 *\param[in]	lightType	Le type de source lumineuse.
		 *\param[in]	shadowType	Le type d'ombres.
		 *\param[in]	volumetric	Dit si le volumetric light scattering doit être activé.
		 *\return		Le source.
		 */
		virtual ShaderPtr doGetPbrMRPixelShaderSource( SceneFlags const & sceneFlags
			, LightType lightType
			, ShadowType shadowType
			, bool rsm )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source for this light pass.
		 *\param[in]	sceneFlags	The scene flags.
		 *\param[in]	lightType	The light source type.
		 *\param[in]	shadowType	The shadow type.
		 *\param[in]	volumetric	Tells if volumetric light scattering is to be enabled.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du pixel shader pour cette passe lumineuse.
		 *\param[in]	sceneFlags	Les indicateurs de scène.
		 *\param[in]	lightType	Le type de source lumineuse.
		 *\param[in]	shadowType	Le type d'ombres.
		 *\param[in]	volumetric	Dit si le volumetric light scattering doit être activé.
		 *\return		Le source.
		 */
		virtual ShaderPtr doGetPbrSGPixelShaderSource( SceneFlags const & sceneFlags
			, LightType lightType
			, ShadowType shadowType
			, bool rsm )const;
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source for this light pass.
		 *\param[in]	sceneFlags	The scene flags.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du vertex shader pour cette passe lumineuse.
		 *\param[in]	sceneFlags	Les indicateurs de scène.
		 *\return		Le source.
		 */
		virtual ShaderPtr doGetVertexShaderSource( SceneFlags const & sceneFlags )const = 0;
		/**
		 *\~english
		 *\brief		Creates a light pass program.
		 *\return		The created program.
		 *\~french
		 *\brief		Crée un programme de passe d'éclairage.
		 *\return		Le programme créé.
		 */
		virtual ProgramPtr doCreateProgram() = 0;

	protected:
		struct Config
		{
			//!\~english	The variable containing the light colour and index.
			//!\~french		La variable contenant la couleur de la lumière et son indice.
			castor::Point4f colourIndex;
			//!\~english	The variable containing the light intensities (RG) and far plane (B).
			//!\~french		La variable contenant les intensités de la lumière (RG) et le plan éloigné (B).
			castor::Point4f intensityFarPlane;
			//!\~english	The variable containing the light volumetric scattering data.
			//!\~french		La variable contenant les données de volumetric scattering.
			castor::Point4f volumetric;
			//!\~english	The variable containing the light shadow data.
			//!\~french		La variable contenant les données d'ombres.
			castor::Point4f shadow;
		};

		Engine & m_engine;
		RenderDevice const & m_device;
		castor::String m_name;
		Scene const * m_scene{ nullptr };
		SceneUbo * m_sceneUbo{ nullptr };
		UniformBufferOffsetT< ModelMatrixUboConfiguration > const * m_mmUbo{ nullptr };
		ashes::PipelineVertexInputStateCreateInfo m_usedVertexLayout{ 0u, {}, {} };
		ashes::PipelineVertexInputStateCreateInfo const * m_pUsedVertexLayout{ nullptr };
		RenderPassTimer * m_timer{ nullptr };
		UniformBufferBase const * m_baseUbo{ nullptr };
		bool m_shadows{};
		MatrixUbo m_matrixUbo;
		RenderPass m_firstRenderPass;
		RenderPass m_blendRenderPass;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		std::array< ashes::CommandBufferPtr, 2u > m_commandBuffers;
		uint32_t m_commandBufferIndex{ 0u };
		PipelineMap m_pipelines;
		Pipeline * m_pipeline{ nullptr };
		SamplerSPtr m_sampler;
		ashes::VertexBufferPtr< float > m_vertexBuffer;
		ashes::PipelineVertexInputStateCreateInfoPtr m_vertexLayout;
		GpInfoUbo const & m_gpInfoUbo;
		uint32_t m_offset{ 0u };
		ashes::SemaphorePtr m_signalImgReady;
		ashes::SemaphorePtr m_signalImgFinished;
		ashes::FencePtr m_fence;
		OpaquePassResult const * m_opaquePassResult{};
	};
}

#endif
