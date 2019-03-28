/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredLightPass_H___
#define ___C3D_DeferredLightPass_H___

#include "Castor3D/Castor3DPrerequisites.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Technique/RenderTechniqueVisitor.hpp"

#include <CastorUtils/Miscellaneous/BlockTracker.hpp>

#include <Ashes/Command/CommandBuffer.hpp>
#include <Ashes/Descriptor/DescriptorSet.hpp>
#include <Ashes/Pipeline/VertexLayout.hpp>
#include <Ashes/RenderPass/FrameBuffer.hpp>
#include <Ashes/RenderPass/RenderPass.hpp>
#include <Ashes/Sync/Semaphore.hpp>

#include <ShaderWriter/Shader.hpp>

namespace castor3d
{
	class GeometryPassResult;
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Enumerator of textures used in deferred rendering.
	\~french
	\brief		Enumération des textures utilisées lors du rendu différé.
	*/
	enum class DsTexture
		: uint8_t
	{
		eDepth,
		eData1, // RGB => Normal, A => Object Material flags
		eData2, // RGB => Diffuse/Albedo - SSR: A => Shininess - PBRMR: A => Unused - PBRSG: A => Glossiness
		eData3, // A => AO - SSR/PBRSG: RGB => Specular - PBRMR: R => Metallic, G => Roughness, B => Unused
		eData4, // RGB => Emissive, A => Transmittance
		eData5, // RG => Velocity, B => Material Index, A => Unused
		CU_ScopedEnumBounds( eDepth ),
	};
	/**
	 *\~english
	 *\brief		Retrieve the name for given texture enum value.
	 *\param[in]	texture	The value.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère le nom pour la valeur d'énumeration de texture.
	 *\param[in]	texture	La valeur.
	 *\return		Le nom.
	 */
	castor::String getTextureName( DsTexture texture );
	/**
	 *\~english
	 *\brief		Retrieve the pixel format for given texture enum value.
	 *\param[in]	texture	The value.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère le format de pixels pour la valeur d'énumeration de texture.
	 *\param[in]	texture	La valeur.
	 *\return		Le nom.
	 */
	ashes::Format getTextureFormat( DsTexture texture );
	/**
	 *\~english
	 *\brief		Retrieve the attachment index for given texture enum value.
	 *\param[in]	texture	The value.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère l'indice d'attache pour la valeur d'énumeration de texture.
	 *\param[in]	texture	La valeur.
	 *\return		Le nom.
	 */
	uint32_t getTextureAttachmentIndex( DsTexture texture );
	/**
	 *\~english
	 *\brief		Retrieve the maximum litten distance for given light and attenuation.
	 *\param[in]	light		The light source.
	 *\param[in]	attenuation	The attenuation values.
	 *\return		The value.
	 *\~french
	 *\brief		Récupère l'indice d'attache pour la valeur d'énumeration de texture.
	 *\param[in]	light		La source lumineuse.
	 *\param[in]	attenuation	Les valeurs d'atténuation.
	 *\return		La valeur.
	 */
	float getMaxDistance( LightCategory const & light
		, castor::Point3f const & attenuation );
	/**
	 *\~english
	 *\brief		Retrieve the maximum litten distance for given light and attenuation.
	 *\param[in]	light		The light source.
	 *\param[in]	attenuation	The attenuation values.
	 *\param[in]	max			The viewer max value.
	 *\return		The value.
	 *\~french
	 *\brief		Récupère l'indice d'attache pour la valeur d'énumeration de texture.
	 *\param[in]	light		La source lumineuse.
	 *\param[in]	attenuation	Les valeurs d'atténuation.
	 *\param[in]	max			La valeur maximale de l'observateur.
	 *\return		La valeur.
	 */
	float getMaxDistance( LightCategory const & light
		, castor::Point3f const & attenuation
		, float max );
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Base class for all light passes.
	\remarks	The result of each light pass is blended with the previous one.
	\~french
	\brief		Classe de base pour toutes les passes d'éclairage.
	\remarks	Le résultat de chaque passe d'éclairage est mélangé avec celui de la précédente.
	*/
	class LightPass
	{
	protected:
		struct RenderPass
		{
			RenderPass( ashes::RenderPassPtr && renderPass
				, ashes::TextureView const & depthView
				, ashes::TextureView const & diffuseView
				, ashes::TextureView const & specularView );
			ashes::RenderPassPtr renderPass;
			ashes::FrameBufferPtr frameBuffer;
		};
		/*!
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
				, ashes::VertexLayout const & vertexLayout
				, ashes::RenderPass const & firstRenderPass
				, ashes::RenderPass const & blendRenderPass
				, MatrixUbo & matrixUbo
				, SceneUbo & sceneUbo
				, GpInfoUbo & gpInfoUbo
				, ModelMatrixUbo * modelMatrixUbo );
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
			virtual ashes::PipelinePtr doCreatePipeline( ashes::VertexLayout const & vertexLayout
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
			ashes::ShaderStageStateArray m_program;
			ashes::DescriptorSetLayoutPtr m_uboDescriptorLayout;
			ashes::DescriptorSetPoolPtr m_uboDescriptorPool;
			ashes::DescriptorSetLayoutPtr m_textureDescriptorLayout;
			ashes::DescriptorSetPoolPtr m_textureDescriptorPool;
			ashes::PipelineLayoutPtr m_pipelineLayout;
			ashes::PipelinePtr m_blendPipeline;
			ashes::PipelinePtr m_firstPipeline;
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
			, GeometryPassResult const & gp
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
		virtual void accept( RenderTechniqueVisitor & visitor ) = 0;
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
			CU_Require( m_signalReady );
			return *m_signalReady;
		}
		/**@}*/

	protected:
		struct Pipeline
		{
			ProgramPtr program;
			ashes::DescriptorSetPtr uboDescriptorSet;
			ashes::WriteDescriptorSetArray textureWrites;
			ashes::DescriptorSetPtr textureDescriptorSet;
			ashes::CommandBufferPtr firstCommandBuffer;
			ashes::CommandBufferPtr blendCommandBuffer;
		};
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	firstRenderPass	The render pass for a first light source.
		 *\param[in]	blendRenderPass	The render pass for other light sources.
		 *\param[in]	depthView		The target depth view.
		 *\param[in]	diffuseView		The target diffuse view.
		 *\param[in]	specularView	The target specular view.
		 *\param[in]	gpInfoUbo		The geometry pass UBO.
		 *\param[in]	hasShadows		Tells if shadows are enabled for this light pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	firstRenderPass	La passe de rendu pour la première source lumineuse.
		 *\param[in]	blendRenderPass	La passe de rendu pour les autres sources lumineuses.
		 *\param[in]	depthView		La vue de profondeur cible.
		 *\param[in]	diffuseView		La vue de diffuse cible.
		 *\param[in]	specularView	La vue de spéculaire cible.
		 *\param[in]	gpInfoUbo		L'UBO de la geometry pass.
		 *\param[in]	hasShadows		Dit si les ombres sont activées pour cette passe d'éclairage.
		 */
		LightPass( Engine & engine
			, ashes::RenderPassPtr && firstRenderPass
			, ashes::RenderPassPtr && blendRenderPass
			, ashes::TextureView const & depthView
			, ashes::TextureView const & diffuseView
			, ashes::TextureView const & specularView
			, GpInfoUbo & gpInfoUbo
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
			, GeometryPassResult const & gp
			, LightType type
			, ashes::VertexBufferBase & vbo
			, ashes::VertexLayout const & vertexLayout
			, SceneUbo & sceneUbo
			, ModelMatrixUbo * modelMatrixUbo
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
		 *\param[in]	shadowMapIndex	The shadow map index.
		 *\param[in]	first			Tells if this is the first pass (\p true) or the blend pass (\p false).
		 *\~french
		 *\brief		Prépare le tampon de commandes du pipeline donné.
		 *\param[in]	pipeline		Le pipeline de la passe d'éclairage.
		 *\param[in]	shadowMap		La texture d'ombres, optionnelle.
		 *\param[in]	shadowMapIndex	L'index de la texture d'ombres.
		 *\param[in]	first			Dit s'il s'agit de la première passe (\p true) ou la passe de mélange (\p false).
		 */
		void doPrepareCommandBuffer( Pipeline & pipeline
			, ShadowMap const * shadowMap
			, uint32_t shadowMapIndex
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
		virtual ShaderPtr doGetLegacyPixelShaderSource( SceneFlags const & sceneFlags
			, LightType lightType
			, ShadowType shadowType
			, bool volumetric )const;
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
			, bool volumetric )const;
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
			, bool volumetric )const;
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
		Scene const * m_scene{ nullptr };
		RenderPassTimer * m_timer{ nullptr };
		ashes::UniformBufferBase const * m_baseUbo{ nullptr };
		bool m_shadows;
		MatrixUbo m_matrixUbo;
		RenderPass m_firstRenderPass;
		RenderPass m_blendRenderPass;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::CommandBufferPtr m_commandBuffer;
		std::array< Pipeline, size_t( ShadowType::eCount ) * 2u > m_pipelines; // * 2u for volumetric scattering or not.
		Pipeline * m_pipeline{ nullptr };
		SamplerSPtr m_sampler;
		ashes::VertexBufferPtr< float > m_vertexBuffer;
		ashes::VertexLayoutPtr m_vertexLayout;
		GpInfoUbo & m_gpInfoUbo;
		uint32_t m_offset{ 0u };
		ashes::SemaphorePtr m_signalReady;
		ashes::FencePtr m_fence;
		GeometryPassResult const * m_geometryPassResult;
	};
}

#endif
