/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredLightPass_H___
#define ___C3D_DeferredLightPass_H___

#include "Castor3DPrerequisites.hpp"
#include "Miscellaneous/BlockTracker.hpp"
#include "Render/Viewport.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/GpInfoUbo.hpp"

#include <Command/CommandBuffer.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/RenderPass.hpp>
#include <Sync/Semaphore.hpp>

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
		CASTOR_SCOPED_ENUM_BOUNDS( eDepth ),
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
	renderer::Format getTextureFormat( DsTexture texture );
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
	/**
	 *\~english
	 *\brief		Declares the GLSL function used to encode the material specifics into a vec4.
	 *\param[in]	writer	The GLSL writer.
	 *\~french
	 *\brief		Déclare la fonction GLSL utilisée pour encoder les spécificités d'un matériau dans un vec4.
	 *\param[in]	writer	Le writer GLSL.
	 */
	void declareEncodeMaterial( glsl::GlslWriter & writer );
	/**
	 *\~english
	 *\brief		Declares the GLSL function used to decode the material specifics from a vec4.
	 *\param[in]	writer	The GLSL writer.
	 *\~french
	 *\brief		Déclare la fonction GLSL utilisée pour décoder les spécificités d'un matériau depuis un vec4.
	 *\param[in]	writer	Le writer GLSL.
	 */
	void declareDecodeMaterial( glsl::GlslWriter & writer );
	/**
	 *\~english
	 *\brief		Declares the GLSL function used to decode the shadow receiver status from a vec4.
	 *\param[in]	writer	The GLSL writer.
	 *\~french
	 *\brief		Déclare la fonction GLSL utilisée pour décoder le statut de receveur d'ombre depuis un vec4.
	 *\param[in]	writer	Le writer GLSL.
	 */
	void declareDecodeReceiver( glsl::GlslWriter & writer );
	/**
	 *\~english
	 *\brief		Calls the GLSL function used to encode the material specifics into a vec4.
	 *\param[in]	writer		The GLSL writer.
	 *\param[in]	receiver	The shadow receiver status.
	 *\param[in]	reflection	The reflection status.
	 *\param[in]	refraction	The refraction status.
	 *\param[in]	envMapIndex	The environment map index.
	 *\param[in]	encoded		The variable that will receive the encoded value.
	 *\~french
	 *\brief		Appelle la fonction GLSL utilisée pour encoder les spécificités d'un matériau dans un vec4.
	 *\param[in]	writer		Le writer GLSL.
	 *\param[in]	receiver	Le statut de receveur d'ombres.
	 *\param[in]	reflection	Le statut de réflexion.
	 *\param[in]	refraction	Le statut de réfraction.
	 *\param[in]	envMapIndex	L'indice de la texture environnementale.
	 *\param[in]	encoded		La variable qui recevra la valeur encodée.
	 */
	void encodeMaterial( glsl::GlslWriter & writer
		, glsl::Int const & receiver
		, glsl::Int const & reflection
		, glsl::Int const & refraction
		, glsl::Int const & envMapIndex
		, glsl::Float const & encoded );
	/**
	 *\~english
	 *\brief		Calls the GLSL function used to dencode the material specifics from a vec4.
	 *\param[in]	writer		The GLSL writer.
	 *\param[in]	encoded		The encoded value.
	 *\param[in]	receiver	The variable that contains the shadow receiver status.
	 *\param[in]	reflection	The variable that contains the reflection status.
	 *\param[in]	refraction	The variable that contains the refraction status.
	 *\param[in]	envMapIndex	The variable that contains the environment map index.
	 *\~french
	 *\brief		Appelle la fonction GLSL utilisée pour décoder les spécificités d'un matériau depuis un vec4.
	 *\param[in]	writer		Le writer GLSL.
	 *\param[in]	encoded		La valeur encodée.
	 *\param[in]	receiver	La variable qui recevra le statut de receveur d'ombres.
	 *\param[in]	reflection	La variable qui recevra le statut de réflexion.
	 *\param[in]	refraction	La variable qui recevra le statut de réfraction.
	 *\param[in]	envMapIndex	La variable qui recevra l'indice de la texture environnementale.
	 */
	void decodeMaterial( glsl::GlslWriter & writer
		, glsl::Float const & encoded
		, glsl::Int const & receiver
		, glsl::Int const & reflection
		, glsl::Int const & refraction
		, glsl::Int const & envMapIndex );
	/**
	 *\~english
	 *\brief		Calls the GLSL function used to decode the shadow receiver status from a vec4.
	 *\param[in]	writer		The GLSL writer.
	 *\param[in]	encoded		The encoded value.
	 *\param[in]	receiver	The variable that contains the shadow receiver status.
	 *\~french
	 *\brief		Appelle la fonction GLSL utilisée pour décoder le statut de receveur d'ombre depuis un vec4.
	 *\param[in]	writer		Le writer GLSL.
	 *\param[in]	encoded		La valeur encodée.
	 *\param[in]	receiver	La variable qui recevra le statut de receveur d'ombres.
	 */
	void decodeReceiver( glsl::GlslWriter & writer
		, glsl::Int & encoded
		, glsl::Int const & receiver );
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
			RenderPass( renderer::RenderPassPtr && renderPass
				, renderer::TextureView const & depthView
				, renderer::TextureView const & diffuseView
				, renderer::TextureView const & specularView );
			renderer::RenderPassPtr renderPass;
			renderer::FrameBufferPtr frameBuffer;
			renderer::CommandBufferPtr commandBuffer;
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
			 *\param[in]	engine	The engine.
			 *\param[in]	vtx		The vertex shader source.
			 *\param[in]	pxl		The fragment shader source.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	engine	Le moteur.
			 *\param[in]	vtx		Le source du vertex shader.
			 *\param[in]	pxl		Le source du fagment shader.
			 */
			Program( Engine & engine
				, glsl::Shader const & vtx
				, glsl::Shader const & pxl
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
			 *\param[in]	matrixUbo		The matrix UBO.
			 *\param[in]	sceneUbo		The scene UBO.
			 *\param[in]	gpInfoUbo		The geometry pass UBO.
			 *\param[in]	modelMatrixUbo	The optional model matrix UBO.
			 *\~french
			 *\brief		Initialise le programme et son pipeline.
			 *\param[in]	vbo				Le tampon de sommets contenant l'objet à dessiner.
			 *\param[in]	matrixUbo		L'UBO des matrices.
			 *\param[in]	sceneUbo		L'UBO de la scène.
			 *\param[in]	gpInfoUbo		L'UBO de la geometry pass.
			 *\param[in]	modelMatrixUbo	L'UBO optionnel de matrices modèle.
			 */
			void initialise( renderer::VertexBufferBase & vbo
				, renderer::VertexLayout const & vertexLayout
				, renderer::RenderPass const & firstRenderPass
				, renderer::RenderPass const & blendRenderPass
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
			 *\param[in]	size	The render area dimensions.
			 *\param[in]	count	The number of primitives to draw.
			 *\param[in]	first	Tells if this is the first light pass (\p true) or not (\p false).
			 *\param[in]	offset	The VBO offset.
			 *\~french
			 *\brief		Dessine la passe d'éclairage.
			 *\param[in]	size	Les dimensions de la zone de rendu.
			 *\param[in]	count	Le nombre de primitives à dessiner.
			 *\param[in]	first	Dit si cette passe d'éclairage est la première (\p true) ou pas (\p false).
			 *\param[in]	offset	L'offset dans le VBO.
			 */
			void render( renderer::CommandBuffer & commandBuffer
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
			inline renderer::DescriptorSetLayout const & getUboDescriptorLayout()const
			{
				REQUIRE( m_uboDescriptorLayout );
				return *m_uboDescriptorLayout;
			}

			inline renderer::DescriptorSetLayout const & getTextureDescriptorLayout()const
			{
				REQUIRE( m_textureDescriptorLayout );
				return *m_textureDescriptorLayout;
			}

			inline renderer::DescriptorSetPool const & getUboDescriptorPool()const
			{
				REQUIRE( m_uboDescriptorPool );
				return *m_uboDescriptorPool;
			}

			inline renderer::DescriptorSetPool const & getTextureDescriptorPool()const
			{
				REQUIRE( m_textureDescriptorPool );
				return *m_textureDescriptorPool;
			}

			inline renderer::PipelineLayout const & getPipelineLayout()const
			{
				REQUIRE( m_pipelineLayout );
				return *m_pipelineLayout;
			}
			/**@}*/

		private:
			/**
			 *\~english
			 *\brief		Creates a pipeline.
			 *\param[in]	blend	Tells if the pipeline must enable blending.
			 *\return		The created pipeline.
			 *\~french
			 *\brief		Crée un pipeline.
			 *\param[in]	blend	Dit si le pipeline doit activer le blending.
			 *\return		Le pipeline créé.
			 */
			virtual renderer::PipelinePtr doCreatePipeline( renderer::VertexLayout const & vertexLayout
				, renderer::RenderPass const & renderPass
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
			renderer::ShaderStageStateArray m_program;
			renderer::DescriptorSetLayoutPtr m_uboDescriptorLayout;
			renderer::DescriptorSetPoolPtr m_uboDescriptorPool;
			renderer::DescriptorSetLayoutPtr m_textureDescriptorLayout;
			renderer::DescriptorSetPoolPtr m_textureDescriptorPool;
			renderer::PipelineLayoutPtr m_pipelineLayout;
			renderer::PipelinePtr m_blendPipeline;
			renderer::PipelinePtr m_firstPipeline;
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
		 *\param[in]	sceneUbo	The scene UBO.
		 *\~french
		 *\brief		Initialise la passe d'éclairage.
		 *\param[in]	scene		La scène.
		 *\param[in]	sceneUbo	L'UBO de scène.
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
		 *\param[in]	size	The render area dimensions.
		 *\param[in]	light	The light.
		 *\param[in]	camera	The viewing camera.
		 *\~french
		 *\brief		Met à jour la passe d'éclairage.
		 *\param[in]	size	Les dimensions de la zone de rendu.
		 *\param[in]	light	La source lumineuse.
		 *\param[in]	camera	La caméra.
		 */
		virtual void update( castor::Size const & size
			, Light const & light
			, Camera const & camera ) = 0;
		/**
		 *\~english
		 *\brief		Renders the light pass.
		 *\~french
		 *\brief		Dessine la passe de rendu.
		 */
		virtual void render( bool first
			, renderer::Semaphore const & toWait
			, TextureUnit * shadowMapOpt );
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

		inline renderer::Semaphore const & getSemaphore()const
		{
			REQUIRE( m_signalReady );
			return *m_signalReady;
		}
		/**@}*/

	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	frameBuffer	The target framebuffer.
		 *\param[in]	depthAttach	The depth buffer attach.
		 *\param[in]	gpInfoUbo	The geometry pass UBO.
		 *\param[in]	hasShadows	Tells if shadows are enabled for this light pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	frameBuffer	Le tampon d'image cible.
		 *\param[in]	depthAttach	L'attache du tampon de profondeur.
		 *\param[in]	gpInfoUbo	L'UBO de la geometry pass.
		 *\param[in]	hasShadows	Dit si les ombres sont activées pour cette passe d'éclairage.
		 */
		LightPass( Engine & engine
			, renderer::RenderPassPtr && firstRenderPass
			, renderer::RenderPassPtr && blendRenderPass
			, renderer::TextureView const & depthView
			, renderer::TextureView const & diffuseView
			, renderer::TextureView const & specularView
			, GpInfoUbo & gpInfoUbo
			, bool hasShadows );
		/**
		 *\~english
		 *\brief		Initialises the light pass.
		 *\param[in]	scene			The scene.
		 *\param[in]	type			The light source type.
		 *\param[in]	vbo				The VBO.
		 *\param[in]	sceneUbo		The scene UBO.
		 *\param[in]	modelMatrixUbo	The optional model matrix UBO.
		 *\~french
		 *\brief		Initialise la passe d'éclairage.
		 *\param[in]	scene			La scène.
		 *\param[in]	type			Le type de source lumineuse.
		 *\param[in]	vbo				Le VBO.
		 *\param[in]	sceneUbo		L'UBO de scène.
		 *\param[in]	modelMatrixUbo	L'UBO optionnel de matrices modèle.
		 */
		void doInitialise( Scene const & scene
			, GeometryPassResult const & gp
			, LightType type
			, renderer::VertexBufferBase & vbo
			, renderer::VertexLayout const & vertexLayout
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
		void doPrepareCommandBuffer( TextureUnit const * shadowMap
			, bool first );
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source for this light pass.
		 *\param[in]	sceneFlags	The scene flags.
		 *\param[in]	type		The light source type.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du pixel shader pour cette passe lumineuse.
		 *\param[in]	sceneFlags	Les indicateurs de scène.
		 *\param[in]	type		Le type de source lumineuse.
		 *\return		Le source.
		 */
		virtual glsl::Shader doGetLegacyPixelShaderSource( SceneFlags const & sceneFlags
			, LightType type )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source for this light pass.
		 *\param[in]	sceneFlags	The scene flags.
		 *\param[in]	type		The light source type.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du pixel shader pour cette passe lumineuse.
		 *\param[in]	sceneFlags	Les indicateurs de scène.
		 *\param[in]	type		Le type de source lumineuse.
		 *\return		Le source.
		 */
		virtual glsl::Shader doGetPbrMRPixelShaderSource( SceneFlags const & sceneFlags
			, LightType type )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source for this light pass.
		 *\param[in]	sceneFlags	The scene flags.
		 *\param[in]	type		The light source type.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du pixel shader pour cette passe lumineuse.
		 *\param[in]	sceneFlags	Les indicateurs de scène.
		 *\param[in]	type		Le type de source lumineuse.
		 *\return		Le source.
		 */
		virtual glsl::Shader doGetPbrSGPixelShaderSource( SceneFlags const & sceneFlags
			, LightType type )const;
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
		virtual glsl::Shader doGetVertexShaderSource( SceneFlags const & sceneFlags )const = 0;
		/**
		 *\~english
		 *\brief		Creates a light pass program.
		 *\param[in]	vtx		The vertex shader source.
		 *\param[in]	pxl		The fragment shader source.
		 *\return		The created program.
		 *\~french
		 *\brief		Crée un programme de passe d'éclairage.
		 *\param[in]	vtx		Le source du vertex shader.
		 *\param[in]	pxl		Le source du fagment shader.
		 *\return		Le programme créé.
		 */
		virtual ProgramPtr doCreateProgram( glsl::Shader const & vtx
			, glsl::Shader const & pxl ) = 0;

	protected:
		struct Config
		{
			//!\~english	The variable containing the light colour.
			//!\~french		La variable contenant la couleur de la lumière.
			renderer::Vec4 colourIndex;
			//!\~english	The variable containing the light intensities (RG) and far plane (B).
			//!\~french		La variable contenant les intensités de la lumière (RG) et le plan éloigné (B).
			renderer::Vec4 intensityFarPlane;
		};
		Engine & m_engine;
		RenderPass m_firstRenderPass;
		RenderPass m_blendRenderPass;
		RenderPassTimer * m_timer{ nullptr };
		renderer::DescriptorSetPtr m_uboDescriptorSet;
		renderer::WriteDescriptorSetArray m_textureWrites;
		renderer::DescriptorSetPtr m_textureDescriptorSet;
		renderer::UniformBufferBase const * m_baseUbo{ nullptr };
		bool m_shadows;
		MatrixUbo m_matrixUbo;
		ProgramPtr m_program;
		SamplerSPtr m_sampler;
		renderer::VertexBufferPtr< float > m_vertexBuffer;
		renderer::VertexLayoutPtr m_vertexLayout;
		GpInfoUbo & m_gpInfoUbo;
		uint32_t m_offset{ 0u };
		renderer::SemaphorePtr m_signalReady;
		renderer::FencePtr m_fence;
		GeometryPassResult const * m_geometryPassResult;
	};
}

#endif
