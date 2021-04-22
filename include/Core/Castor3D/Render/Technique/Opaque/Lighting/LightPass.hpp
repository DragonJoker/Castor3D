/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredLightPass_H___
#define ___C3D_DeferredLightPass_H___

#include "LightingModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"

#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#include <CastorUtils/Design/Named.hpp>

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
		: public castor::Named
	{
	protected:
		struct RenderPass
		{
			RenderPass( std::string const & name
				, ashes::RenderPassPtr renderPass
				, LightPassResult const & lpResult
				, bool generatesIndirect );
			ashes::RenderPassPtr renderPass;
			ashes::FrameBufferPtr frameBuffer;
		};
		/**
		\~english
		\brief		Base class for all light pass programs.
		\~french
		\brief		Classe de base pour tous les programmes des passes d'éclairage.
		*/
		struct Program
			: public castor::Named
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\param[in]	engine				The engine.
			 *\param[in]	device				The GPU device.
			 *\param[in]	name				The pass name.
			 *\param[in]	vtx					The vertex shader source.
			 *\param[in]	pxl					The fragment shader source.
			 *\param[in]	hasShadows			Tells if this program uses shadow map.
			 *\param[in]	hasVoxels			Tells if this program uses voxellisation result.
			 *\param[in]	generatesIndirect	Tells if this program generates indirect lighting.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	engine				Le moteur.
			 *\param[in]	device				Le device GPU.
			 *\param[in]	name				Le nom de la passe.
			 *\param[in]	vtx					Le source du vertex shader.
			 *\param[in]	pxl					Le source du fagment shader.
			 *\param[in]	hasShadows			Dit si ce programme utilise une shadow map.
			 *\param[in]	hasVoxels			Dit si ce programme utilise le résultat de la voxellisation.
			 *\param[in]	generatesIndirect	Dit si ce programme genère de l'éclairage indirect.
			 */
			Program( Engine & engine
				, RenderDevice const & device
				, castor::String const & name
				, ShaderModule const & vtx
				, ShaderModule const & pxl
				, bool hasShadows
				, bool hasVoxels
				, bool generatesIndirect );
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
			 *\param[in]	modelUbo		The optional model matrix UBO.
			 *\param[in]	voxelUbo		The optional voxelizer UBO.
			 *\~french
			 *\brief		Initialise le programme et son pipeline.
			 *\param[in]	vbo				Le tampon de sommets contenant l'objet à dessiner.
			 *\param[in]	vertexLayout	Le layout du tampon de sommets.
			 *\param[in]	firstRenderPass	La passe de rendu pour la première source lumineuse.
			 *\param[in]	blendRenderPass	La passe de rendu pour les autres sources lumineuses.
			 *\param[in]	matrixUbo		L'UBO des matrices.
			 *\param[in]	sceneUbo		L'UBO de la scène.
			 *\param[in]	gpInfoUbo		L'UBO de la geometry pass.
			 *\param[in]	modelUbo		L'UBO optionnel de matrices modèle.
			 *\param[in]	voxelUbo		L'UBO optionnel du voxelizer.
			 */
			void initialise( ashes::VertexBufferBase & vbo
				, ashes::PipelineVertexInputStateCreateInfo const & vertexLayout
				, ashes::RenderPass const & firstRenderPass
				, ashes::RenderPass const & blendRenderPass
				, MatrixUbo & matrixUbo
				, SceneUbo & sceneUbo
				, GpInfoUbo const & gpInfoUbo
				, UniformBufferT< ModelUboConfiguration > const * modelUbo
				, VoxelizerUbo const * voxelUbo );
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
			virtual ashes::GraphicsPipelinePtr doCreatePipeline( ashes::PipelineVertexInputStateCreateInfo const & vertexLayout
				, ashes::RenderPass const & renderPass
				, bool blend ) = 0;
			virtual void doBind( Light const & light ) = 0;

		public:
			Engine & m_engine;
			RenderDevice const & m_device;
			ashes::PipelineShaderStageCreateInfoArray m_program;
			ashes::DescriptorSetLayoutPtr m_uboDescriptorLayout;
			ashes::DescriptorSetPoolPtr m_uboDescriptorPool;
			ashes::DescriptorSetLayoutPtr m_textureDescriptorLayout;
			ashes::DescriptorSetPoolPtr m_textureDescriptorPool;
			ashes::PipelineLayoutPtr m_pipelineLayout;
			ashes::GraphicsPipelinePtr m_blendPipeline;
			ashes::GraphicsPipelinePtr m_firstPipeline;
			bool m_shadows;
			bool m_voxels;
			bool m_generatesIndirect;
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
		 *\param[in]	first				Tells if this is the first pass.
		 *\param[in]	size				The render area dimensions.
		 *\param[in]	light				The light.
		 *\param[in]	camera				The viewing camera.
		 *\param[in]	shadowMap			The optional shadow map.
		 *\param[in]	vctFirstBounce		The VCT first bounce result.
		 *\param[in]	vctSecondaryBounce	The VCT secondary bounce result.
		 *\~french
		 *\brief		Met à jour la passe d'éclairage.
		 *\param[in]	first				Dit s'il s'agit de la première passe.
		 *\param[in]	size				Les dimensions de la zone de rendu.
		 *\param[in]	light				La source lumineuse.
		 *\param[in]	camera				La caméra.
		 *\param[in]	shadowMap			La texture d'ombres, optionnelle.
		 *\param[in]	vctFirstBounce		Le résultat du premier rebond de VCT.
		 *\param[in]	vctSecondaryBounce	Le résultat du second rebond de VCT.
		 */
		void update( bool first
			, castor::Size const & size
			, Light const & light
			, Camera const & camera
			, ShadowMap const * shadowMap
			, TextureUnit const * vctFirstBounce
			, TextureUnit const * vctSecondaryBounce );
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
			, ShadowMap const * shadowMap
			, TextureUnit const * vctFirstBounce
			, TextureUnit const * vctSecondaryBounce );
		using PipelinePtr = std::unique_ptr< Pipeline >;
		using PipelineMap = std::map< size_t, PipelinePtr >;
		using PipelineArray = std::array< Pipeline, size_t( ShadowType::eCount ) * 2u >; // * 2u for volumetric scattering or not.

		Pipeline createPipeline( LightType lightType
			, ShadowType shadowType
			, bool rsm
			, ShadowMap const * shadowMap
			, TextureUnit const * vctFirstBounce
			, TextureUnit const * vctSecondaryBounce );

	protected:
		virtual Pipeline * doGetPipeline( bool first
			, Light const & light
			, ShadowMap const * shadowMap
			, TextureUnit const * vctFirstBounce
			, TextureUnit const * vctSecondaryBounce );

	private:
		virtual VkClearValue doGetIndirectClearColor()const;

	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device			The GPU device.
		 *\param[in]	suffix			The pass name's suffix.
		 *\param[in]	firstRenderPass	The render pass for a first light source.
		 *\param[in]	blendRenderPass	The render pass for other light sources.
		 *\param[in]	lpConfig		The light pass configuration.
		 *\param[in]	vctConfig		The voxelizer UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	suffix			Le suffixe du nom de la passe.
		 *\param[in]	firstRenderPass	La passe de rendu pour la première source lumineuse.
		 *\param[in]	blendRenderPass	La passe de rendu pour les autres sources lumineuses.
		 *\param[in]	lpConfig		La configuration de la passe d'éclairage.
		 *\param[in]	vctConfig		L'UBO du voxelizer.
		 */
		LightPass( RenderDevice const & device
			, castor::String const & suffix
			, ashes::RenderPassPtr firstRenderPass
			, ashes::RenderPassPtr blendRenderPass
			, LightPassConfig const & lpConfig
			, VoxelizerUbo const * vctConfig );
		/**
		 *\~english
		 *\brief		Initialises the light pass.
		 *\param[in]	scene			The scene.
		 *\param[in]	gp				The geometry pass buffers.
		 *\param[in]	type			The light source type.
		 *\param[in]	vbo				The VBO.
		 *\param[in]	vertexLayout	The vertex buffer layout.
		 *\param[in]	sceneUbo		The scene UBO.
		 *\param[in]	modelUbo		The optional model matrix UBO.
		 *\param[in]	timer			The render pass timer.
		 *\~french
		 *\brief		Initialise la passe d'éclairage.
		 *\param[in]	scene			La scène.
		 *\param[in]	gp				Les tampons de la passe géométries.
		 *\param[in]	type			Le type de source lumineuse.
		 *\param[in]	vbo				Le VBO.
		 *\param[in]	vertexLayout	Le layout du tampon de sommets.
		 *\param[in]	sceneUbo		L'UBO de scène.
		 *\param[in]	modelUbo		L'UBO optionnel de matrices modèle.
		 *\param[in]	timer			Le timer de passe.
		 */
		void doInitialise( Scene const & scene
			, OpaquePassResult const & gp
			, LightType type
			, ashes::VertexBufferBase & vbo
			, ashes::PipelineVertexInputStateCreateInfo const & vertexLayout
			, SceneUbo & sceneUbo
			, UniformBufferT< ModelUboConfiguration > const * modelUbo
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
		 *\param[in]	first	Tells if this is the first pass.
		 *\param[in]	size	The render area dimensions.
		 *\param[in]	light	The light.
		 *\param[in]	camera	The viewing camera.
		 *\~french
		 *\brief		Met à jour la passe d'éclairage.
		 *\param[in]	first	Dit s'il s'agit de la première passe.
		 *\param[in]	size	Les dimensions de la zone de rendu.
		 *\param[in]	light	La source lumineuse.
		 *\param[in]	camera	La caméra.
		 */
		virtual void doUpdate( bool first
			, castor::Size const & size
			, Light const & light
			, Camera const & camera ) = 0;
		/**
		 *\~english
		 *\brief		Prepares the command buffer for given pipeline.
		 *\param[in]	pipeline	The light pass pipeline.
		 *\param[in]	first		Tells if this is the first pass (\p true) or the blend pass (\p false).
		 *\~french
		 *\brief		Prépare le tampon de commandes du pipeline donné.
		 *\param[in]	pipeline	Le pipeline de la passe d'éclairage.
		 *\param[in]	first		Dit s'il s'agit de la première passe (\p true) ou la passe de mélange (\p false).
		 */
		void doPrepareCommandBuffer( Pipeline & pipeline
			, bool first );
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source for this light pass.
		 *\param[in]	sceneFlags	The scene flags.
		 *\param[in]	lightType	The light source type.
		 *\param[in]	shadowType	The shadow type.
		 *\param[in]	rsm			Tells if RSM are to be generated.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du pixel shader pour cette passe lumineuse.
		 *\param[in]	sceneFlags	Les indicateurs de scène.
		 *\param[in]	lightType	Le type de source lumineuse.
		 *\param[in]	shadowType	Le type d'ombres.
		 *\param[in]	rsm			Dit si les RSM doivent être générées.
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
		 *\param[in]	rsm			Tells if RSM are to be generated.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du pixel shader pour cette passe lumineuse.
		 *\param[in]	sceneFlags	Les indicateurs de scène.
		 *\param[in]	lightType	Le type de source lumineuse.
		 *\param[in]	shadowType	Le type d'ombres.
		 *\param[in]	rsm			Dit si les RSM doivent être générées.
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
		 *\param[in]	rsm			Tells if RSM are to be generated.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du pixel shader pour cette passe lumineuse.
		 *\param[in]	sceneFlags	Les indicateurs de scène.
		 *\param[in]	lightType	Le type de source lumineuse.
		 *\param[in]	shadowType	Le type d'ombres.
		 *\param[in]	rsm			Dit si les RSM doivent être générées.
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
			//!\~english	The variable containing the light raw and PCF shadow data.
			//!\~french		La variable contenant les données d'ombres raw et PCF.
			castor::Point4f shadowsOffsets;
			//!\~english	The variable containing the light VSM shadow data.
			//!\~french		La variable contenant les données d'ombres VSM.
			castor::Point4f shadowsVariances;
		};

		Engine & m_engine;
		RenderDevice const & m_device;
		Scene const * m_scene{ nullptr };
		SceneUbo * m_sceneUbo{ nullptr };
		UniformBufferT< ModelUboConfiguration > const * m_optModelUbo{ nullptr };
		VoxelizerUbo const * m_vctUbo{ nullptr };
		ashes::PipelineVertexInputStateCreateInfo m_usedVertexLayout{ 0u, {}, {} };
		ashes::PipelineVertexInputStateCreateInfo const * m_pUsedVertexLayout{ nullptr };
		RenderPassTimer * m_timer{ nullptr };
		UniformBufferBase const * m_baseUbo{ nullptr };
		bool m_shadows;
		bool m_voxels;
		bool m_generatesIndirect;
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
