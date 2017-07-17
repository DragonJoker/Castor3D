/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_DeferredLightPass_H___
#define ___C3D_DeferredLightPass_H___

#include "Mesh/Buffer/BufferDeclaration.hpp"
#include "Render/Viewport.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/MatrixUbo.hpp"
#include "Shader/GpInfoUbo.hpp"

#include <Miscellaneous/BlockTracker.hpp>

namespace Castor3D
{
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
		eData1,
		eData2,
		eData3,
		eData4,
		CASTOR_SCOPED_ENUM_BOUNDS( eDepth ),
	};
	/**
	 *\~english
	 *\brief		Retrieve the name for given texture enum value.
	 *\param[in]	p_texture	The value.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère le nom pour la valeur d'énumeration de texture.
	 *\param[in]	p_texture	La valeur.
	 *\return		Le nom.
	 */
	Castor::String GetTextureName( DsTexture p_texture );
	/**
	 *\~english
	 *\brief		Retrieve the pixel format for given texture enum value.
	 *\param[in]	p_texture	The value.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère le format de pixels pour la valeur d'énumeration de texture.
	 *\param[in]	p_texture	La valeur.
	 *\return		Le nom.
	 */
	Castor::PixelFormat GetTextureFormat( DsTexture p_texture );
	/**
	 *\~english
	 *\brief		Retrieve the attachment point for given texture enum value.
	 *\param[in]	p_texture	The value.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère le point d'attache pour la valeur d'énumeration de texture.
	 *\param[in]	p_texture	La valeur.
	 *\return		Le nom.
	 */
	AttachmentPoint GetTextureAttachmentPoint( DsTexture p_texture );
	/**
	 *\~english
	 *\brief		Retrieve the attachment index for given texture enum value.
	 *\param[in]	p_texture	The value.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère l'indice d'attache pour la valeur d'énumeration de texture.
	 *\param[in]	p_texture	La valeur.
	 *\return		Le nom.
	 */
	uint32_t GetTextureAttachmentIndex( DsTexture p_texture );
	/**
	 *\~english
	 *\brief		Retrieve the maximum litten distance for given light and attenuation.
	 *\param[in]	p_light			The light source.
	 *\param[in]	p_attenuation	The attenuation values.
	 *\param[in]	p_max			The viewer max value.
	 *\return		The value.
	 *\~french
	 *\brief		Récupère l'indice d'attache pour la valeur d'énumeration de texture.
	 *\param[in]	p_light			La source lumineuse.
	 *\param[in]	p_attenuation	Les valeurs d'atténuation.
	 *\param[in]	p_max			La valeur maximale de l'observateur.
	 *\return		La valeur.
	 */
	float GetMaxDistance( LightCategory const & p_light
		, Castor::Point3f const & p_attenuation
		, float p_max );
	/**
	 *\~english
	 *\brief		Declares the GLSL function used to encode the material specifics into a vec4.
	 *\param[in]	p_writer	The GLSL writer.
	 *\~french
	 *\brief		Déclare la fonction GLSL utilisée pour encoder les spécificités d'un matériau dans un vec4.
	 *\param[in]	p_writer	Le writer GLSL.
	 */
	void Declare_EncodeMaterial( GLSL::GlslWriter & p_writer );
	/**
	 *\~english
	 *\brief		Declares the GLSL function used to decode the material specifics from a vec4.
	 *\param[in]	p_writer	The GLSL writer.
	 *\~french
	 *\brief		Déclare la fonction GLSL utilisée pour décoder les spécificités d'un matériau depuis un vec4.
	 *\param[in]	p_writer	Le writer GLSL.
	 */
	void Declare_DecodeMaterial( GLSL::GlslWriter & p_writer );
	/**
	 *\~english
	 *\brief		Declares the GLSL function used to decode the shadow receiver status from a vec4.
	 *\param[in]	p_writer	The GLSL writer.
	 *\~french
	 *\brief		Déclare la fonction GLSL utilisée pour décoder le statut de receveur d'ombre depuis un vec4.
	 *\param[in]	p_writer	Le writer GLSL.
	 */
	void Declare_DecodeReceiver( GLSL::GlslWriter & p_writer );
	/**
	 *\~english
	 *\brief		Calls the GLSL function used to encode the material specifics into a vec4.
	 *\param[in]	p_writer		The GLSL writer.
	 *\param[in]	p_receiver		The shadow receiver status.
	 *\param[in]	p_reflection	The reflection status.
	 *\param[in]	p_refraction	The refraction status.
	 *\param[in]	p_envMapIndex	The environment map index.
	 *\param[in]	p_encoded		The variable that will receive the encoded value.
	 *\~french
	 *\brief		Appelle la fonction GLSL utilisée pour encoder les spécificités d'un matériau dans un vec4.
	 *\param[in]	p_writer		Le writer GLSL.
	 *\param[in]	p_receiver		Le statut de receveur d'ombres.
	 *\param[in]	p_reflection	Le statut de réflexion.
	 *\param[in]	p_refraction	Le statut de réfraction.
	 *\param[in]	p_envMapIndex	L'indice de la texture environnementale.
	 *\param[in]	p_encoded		La variable qui recevra la valeur encodée.
	 */
	void EncodeMaterial( GLSL::GlslWriter & p_writer
		, GLSL::Int const & p_receiver
		, GLSL::Int const & p_reflection
		, GLSL::Int const & p_refraction
		, GLSL::Int const & p_envMapIndex
		, GLSL::Float const & p_encoded );
	/**
	 *\~english
	 *\brief		Calls the GLSL function used to dencode the material specifics from a vec4.
	 *\param[in]	p_writer		The GLSL writer.
	 *\param[in]	p_encoded		The encoded value.
	 *\param[in]	p_receiver		The variable that contains the shadow receiver status.
	 *\param[in]	p_reflection	The variable that contains the reflection status.
	 *\param[in]	p_refraction	The variable that contains the refraction status.
	 *\param[in]	p_envMapIndex	The variable that contains the environment map index.
	 *\~french
	 *\brief		Appelle la fonction GLSL utilisée pour décoder les spécificités d'un matériau depuis un vec4.
	 *\param[in]	p_writer		Le writer GLSL.
	 *\param[in]	p_encoded		La valeur encodée.
	 *\param[in]	p_receiver		La variable qui recevra le statut de receveur d'ombres.
	 *\param[in]	p_reflection	La variable qui recevra le statut de réflexion.
	 *\param[in]	p_refraction	La variable qui recevra le statut de réfraction.
	 *\param[in]	p_envMapIndex	La variable qui recevra l'indice de la texture environnementale.
	 */
	void DecodeMaterial( GLSL::GlslWriter & p_writer
		, GLSL::Float const & p_encoded
		, GLSL::Int const & p_receiver
		, GLSL::Int const & p_reflection
		, GLSL::Int const & p_refraction
		, GLSL::Int const & p_envMapIndex );
	/**
	 *\~english
	 *\brief		Calls the GLSL function used to decode the shadow receiver status from a vec4.
	 *\param[in]	p_writer		The GLSL writer.
	 *\param[in]	p_encoded		The encoded value.
	 *\param[in]	p_receiver		The variable that contains the shadow receiver status.
	 *\~french
	 *\brief		Appelle la fonction GLSL utilisée pour décoder le statut de receveur d'ombre depuis un vec4.
	 *\param[in]	p_writer		Le writer GLSL.
	 *\param[in]	p_encoded		La valeur encodée.
	 *\param[in]	p_receiver		La variable qui recevra le statut de receveur d'ombres.
	 */
	void DecodeReceiver( GLSL::GlslWriter & p_writer
		, GLSL::Int & p_encoded
		, GLSL::Int const & p_receiver );
	//!\~english	The geometry pass result.
	//!\~french		Le résultat de la geometry pass.
	using GeometryPassResult = std::array< TextureUnitUPtr, size_t( DsTexture::eCount ) >;
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
			 *\param[in]	p_engine	The engine.
			 *\param[in]	p_vtx		The vertex shader source.
			 *\param[in]	p_pxl		The fragment shader source.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	p_engine	Le moteur.
			 *\param[in]	p_vtx		Le source du vertex shader.
			 *\param[in]	p_pxl		Le source du fagment shader.
			 */
			Program( Engine & p_engine
				, GLSL::Shader const & p_vtx
				, GLSL::Shader const & p_pxl );
			/**
			 *\~english
			 *\brief		Destructor.
			 *\~french
			 *\brief		Destructeur.
			 */
			virtual ~Program()noexcept;
			/**
			 *\~english
			 *\brief		Initialises the program and its pipeline.
			 *\param[in]	p_vbo				The vertex buffer containing the object to render.
			 *\param[in]	p_ibo				An optional index buffer.
			 *\param[in]	p_matrixUbo			The matrix UBO.
			 *\param[in]	p_sceneUbo			The scene UBO.
			 *\param[in]	p_gpInfo			The geometry pass UBO.
			 *\param[in]	p_modelMatrixUbo	The optional model matrix UBO.
			 *\~french
			 *\brief		Initialise le programme et son pipeline.
			 *\param[in]	p_vbo				Le tampon de sommets contenant l'objet à dessiner.
			 *\param[in]	p_ibo				Un tampon d'indices optionnel.
			 *\param[in]	p_matrixUbo			L'UBO des matrices.
			 *\param[in]	p_sceneUbo			L'UBO de la scène.
			 *\param[in]	p_gpInfo			L'UBO de la geometry pass.
			 *\param[in]	p_modelMatrixUbo	L'UBO optionnel de matrices modèle.
			 */
			void Initialise( VertexBuffer & p_vbo
				, IndexBufferSPtr p_ibo
				, MatrixUbo & p_matrixUbo
				, SceneUbo & p_sceneUbo
				, UniformBuffer & p_gpInfoUbo
				, ModelMatrixUbo * p_modelMatrixUbo );
			/**
			*\~english
			*\brief		Cleans up the program and its pipeline.
			*\~french
			*\brief		Nettoie le programme et son pipeline.
			*/
			void Cleanup();
			/**
			 *\~english
			 *\brief		Binds a light.
			 *\param[in]	p_light	The light.
			 *\~french
			 *\brief		Active une source lumineuse.
			 *\param[in]	p_light	La lumière.
			 */
			void Bind( Light const & p_light );
			/**
			 *\~english
			 *\brief		Renders the light pass.
			 *\param[in]	p_size		The render area dimensions.
			 *\param[in]	p_colour	The light colour.
			 *\param[in]	p_count		The number of primitives to draw.
			 *\param[in]	p_first		Tells if this is the first light pass (\p true) or not (\p false).
			 *\~french
			 *\brief		Dessine la passe d'éclairage.
			 *\param[in]	p_size		Les dimensions de la zone de rendu.
			 *\param[in]	p_colour	La couleur de la souce lumineuse.
			 *\param[in]	p_count		Le nombre de primitives à dessiner.
			 *\param[in]	p_first		Dit si cette passe d'éclairage est la première (\p true) ou pas (\p false).
			 */
			void Render( Castor::Size const & p_size
				, Castor::Point3f const & p_colour
				, uint32_t p_count
				, bool p_first )const;

		private:
			/**
			 *\~english
			 *\brief		Creates a pipeline.
			 *\param[in]	p_blend	Tells if the pipeline must enable blending.
			 *\return		The created pipeline.
			 *\~french
			 *\brief		Crée un pipeline.
			 *\param[in]	p_blend	Dit si le pipeline doit activer le blending.
			 *\return		Le pipeline créé.
			 */
			virtual RenderPipelineUPtr DoCreatePipeline( bool p_blend ) = 0;
			/**
			 *\~english
			 *\brief		Binds a light.
			 *\param[in]	p_light	The light.
			 *\~french
			 *\brief		Active une source lumineuse.
			 *\param[in]	p_light	La lumière.
			 */
			virtual void DoBind( Light const & p_light ) = 0;

		public:
			//!\~english	The shader program used to render lights.
			//!\~french		Le shader utilisé pour rendre les lumières.
			ShaderProgramSPtr m_program;
			//!\~english	Geometry buffers holder.
			//!\~french		Conteneur de buffers de géométries.
			GeometryBuffersSPtr m_geometryBuffers;
			//!\~english	The pipeline used by the light pass.
			//!\~french		Le pipeline utilisé par la passe lumières.
			RenderPipelineSPtr m_blendPipeline;
			//!\~english	The pipeline used by the light pass.
			//!\~french		Le pipeline utilisé par la passe lumières.
			RenderPipelineSPtr m_firstPipeline;
			//!\~english	The variable containing the light colour.
			//!\~french		La variable contenant la couleur de la lumière.
			PushUniform3fSPtr m_lightColour;
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
		 *\param[in]	p_scene		The scene.
		 *\param[in]	p_sceneUbo	The scene UBO.
		 *\~french
		 *\brief		Initialise la passe d'éclairage.
		 *\param[in]	p_scene		La scène.
		 *\param[in]	p_sceneUbo	L'UBO de scène.
		 */
		virtual void Initialise( Scene const & p_scene
			, SceneUbo & p_sceneUbo ) = 0;
		/**
		 *\~english
		 *\brief		Cleans up the light pass.
		 *\~french
		 *\brief		Nettoie la passe d'éclairage.
		 */
		virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\brief		Renders the light pass on currently bound framebuffer.
		 *\param[in]	p_size			The render area dimensions.
		 *\param[in]	p_gp			The geometry pass result.
		 *\param[in]	p_light			The light.
		 *\param[in]	p_camera		The viewing camera.
		 *\param[in]	p_invViewProj	The inverse view projection matrix.
		 *\param[in]	p_invView		The inverse view matrix.
		 *\param[in]	p_invProj		The inverse projection matrix.
		 *\param[in]	p_first			Tells if this is the first light pass (\p true) or not (\p false).
		 *\~french
		 *\brief		Dessine la passe d'éclairage sur le tampon d'image donné.
		 *\param[in]	p_size			Les dimensions de la zone de rendu.
		 *\param[in]	p_gp			Le résultat de la geometry pass.
		 *\param[in]	p_light			La source lumineuse.
		 *\param[in]	p_camera		La caméra.
		 *\param[in]	p_invViewProj	La matrice vue projection inversée.
		 *\param[in]	p_invView		La matrice vue inversée.
		 *\param[in]	p_invProj		La matrice projection inversée.
		 *\param[in]	p_first			Dit si cette passe d'éclairage est la première (\p true) ou pas (\p false).
		 */
		virtual void Render( Castor::Size const & p_size
			, GeometryPassResult const & p_gp
			, Light const & p_light
			, Camera const & p_camera
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj
			, bool p_first );
		/**
		 *\~english
		 *\return		The number of primitives to draw.
		 *\~french
		 *\return		Le nombre de primitives à dessiner.
		 */
		virtual uint32_t GetCount()const = 0;

	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine		The engine.
		 *\param[in]	p_frameBuffer	The target framebuffer.
		 *\param[in]	p_depthAttach	The depth buffer attach.
		 *\param[in]	p_shadows		Tells if shadows are enabled for this light pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine		Le moteur.
		 *\param[in]	p_frameBuffer	Le tampon d'image cible.
		 *\param[in]	p_depthAttach	L'attache du tampon de profondeur.
		 *\param[in]	p_shadows		Dit si les ombres sont activées pour cette passe d'éclairage.
		 */
		LightPass( Engine & p_engine
			, FrameBuffer & p_frameBuffer
			, FrameBufferAttachment & p_depthAttach
			, bool p_shadows );
		/**
		 *\~english
		 *\brief		Initialises the light pass.
		 *\param[in]	p_scene				The scene.
		 *\param[in]	p_type				The light source type.
		 *\param[in]	p_vtx				The vertex shader source.
		 *\param[in]	p_pxl				The fragment shader source.
		 *\param[in]	p_sceneUbo			The scene UBO.
		 *\param[in]	p_modelMatrixUbo	The optional model matrix UBO.
		 *\~french
		 *\brief		Initialise la passe d'éclairage.
		 *\param[in]	p_scene				La scène.
		 *\param[in]	p_type				Le type de source lumineuse.
		 *\param[in]	p_vtx				Le source du vertex shader.
		 *\param[in]	p_pxl				Le source du fagment shader.
		 *\param[in]	p_sceneUbo			L'UBO de scène.
		 *\param[in]	p_modelMatrixUbo	L'UBO optionnel de matrices modèle.
		 */
		void DoInitialise( Scene const & p_scene
			, LightType p_type
			, VertexBuffer & p_vbo
			, IndexBufferSPtr p_ibo
			, SceneUbo & p_sceneUbo
			, ModelMatrixUbo * p_modelMatrixUbo );
		/**
		 *\~english
		 *\brief		Cleans up the light pass.
		 *\~french
		 *\brief		Nettoie la passe d'éclairage.
		 */
		void DoCleanup();
		/**
		 *\~english
		 *\brief		Updates the light pass.
		 *\param[in]	p_size		The render area dimensions.
		 *\param[in]	p_light		The light.
		 *\param[in]	p_camera	The viewing camera.
		 *\~french
		 *\brief		Met à jour la passe d'éclairage.
		 *\param[in]	p_size		Les dimensions de la zone de rendu.
		 *\param[in]	p_light		La source lumineuse.
		 *\param[in]	p_camera	La caméra.
		 */
		virtual void DoUpdate( Castor::Size const & p_size
			, Light const & p_light
			, Camera const & p_camera ) = 0;
		/**
		 *\~english
		 *\brief		Renders the light pass on currently bound framebuffer.
		 *\param[in]	p_size		The render area dimensions.
		 *\param[in]	p_gp		Le résultat de la geometry pass.
		 *\param[in]	p_colour	La couleur de la souce lumineuse.
		 *\param[in]	p_first		Tells if this is the first light pass (\p true) or not (\p false).
		 *\~french
		 *\brief		Dessine la passe d'éclairage sur le tampon d'image donné.
		 *\param[in]	p_size		Les dimensions de la zone de rendu.
		 *\param[in]	p_gp		Le résultat de la geometry pass.
		 *\param[in]	p_colour	La couleur de la souce lumineuse.
		 *\param[in]	p_first		Dit si cette passe d'éclairage est la première (\p true) ou pas (\p false).
		 */
		void DoRender( Castor::Size const & p_size
			, GeometryPassResult const & p_gp
			, Castor::Point3f const & p_colour
			, bool p_first );
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source for this light pass.
		 *\param[in]	p_sceneFlags	The scene flags.
		 *\param[in]	p_type			The light source type.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du pixel shader pour cette passe lumineuse.
		 *\param[in]	p_sceneFlags	Les indicateurs de scène.
		 *\param[in]	p_type			Le type de source lumineuse.
		 *\return		Le source.
		 */
		virtual GLSL::Shader DoGetLegacyPixelShaderSource( SceneFlags const & p_sceneFlags
			, LightType p_type )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source for this light pass.
		 *\param[in]	p_sceneFlags	The scene flags.
		 *\param[in]	p_type			The light source type.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du pixel shader pour cette passe lumineuse.
		 *\param[in]	p_sceneFlags	Les indicateurs de scène.
		 *\param[in]	p_type			Le type de source lumineuse.
		 *\return		Le source.
		 */
		virtual GLSL::Shader DoGetPbrMRPixelShaderSource( SceneFlags const & p_sceneFlags
			, LightType p_type )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source for this light pass.
		 *\param[in]	p_sceneFlags	The scene flags.
		 *\param[in]	p_type			The light source type.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du pixel shader pour cette passe lumineuse.
		 *\param[in]	p_sceneFlags	Les indicateurs de scène.
		 *\param[in]	p_type			Le type de source lumineuse.
		 *\return		Le source.
		 */
		virtual GLSL::Shader DoGetPbrSGPixelShaderSource( SceneFlags const & p_sceneFlags
			, LightType p_type )const;
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source for this light pass.
		 *\param[in]	p_sceneFlags	The scene flags.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du vertex shader pour cette passe lumineuse.
		 *\param[in]	p_sceneFlags	Les indicateurs de scène.
		 *\return		Le source.
		 */
		virtual GLSL::Shader DoGetVertexShaderSource( SceneFlags const & p_sceneFlags )const = 0;
		/**
		 *\~english
		 *\brief		Creates a light pass program.
		 *\param[in]	p_vtx		The vertex shader source.
		 *\param[in]	p_pxl		The fragment shader source.
		 *\return		The created program.
		 *\~french
		 *\brief		Crée un programme de passe d'éclairage.
		 *\param[in]	p_vtx		Le source du vertex shader.
		 *\param[in]	p_pxl		Le source du fagment shader.
		 *\return		Le programme créé.
		 */
		virtual ProgramPtr DoCreateProgram( GLSL::Shader const & p_vtx
			, GLSL::Shader const & p_pxl )const = 0;

	protected:
		//!\~english	The engine.
		//!\~french		Le moteur.
		Engine & m_engine;
		//!\~english	Tells if shadows are enabled.
		//!\~french		Dit si les ombres sont activées.
		bool m_shadows;
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		MatrixUbo m_matrixUbo;
		//!\~english	The target FBO.
		//!\~french		Le FBO cible.
		FrameBuffer & m_frameBuffer;
		//!\~english	The target RBO attach.
		//!\~french		L'attache de RBO cible.
		FrameBufferAttachment & m_depthAttach;
		//!\~english	The light pass program.
		//!\~french		Le programme de la passe de lumière.
		ProgramPtr m_program;
		//!\~english	The geometry pass informations.
		//!\~french		Les informations de la passe de géométrie.
		std::unique_ptr< GpInfoUbo > m_gpInfo;
	};
}

#endif
