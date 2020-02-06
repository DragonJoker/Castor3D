﻿/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_RENDER_SYSTEM_H___
#define ___C3D_RENDER_SYSTEM_H___

#include "Castor3DPrerequisites.hpp"
#include "Context.hpp"

#include <stack>

#include <Colour.hpp>
#include <OwnedBy.hpp>

#ifndef C3D_TRACE_OBJECTS
#	define C3D_TRACE_OBJECTS 1
#endif

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Template class used to create a renderer
	\~french
	\brief		Classe template utilisée pour créer un renderer
	*/
	template< class Ty > struct RendererCreator;
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Template class used to add a renderer to the list
	\~french
	\brief		Classe template utilisée pour ajouter un renderer à la liste
	*/
	template< class Ty > struct RendererAdder;
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		The render system representation
	\remarks	This is the class which is the link between Castor3D and the renderer driver (OpenGL or Direct3D)
				<br />Hence it is also the only class which can create the renderers
	\~french
	\brief		Représentation du système de rendu
	\remarks	Cette classe fait le lien entre Castor3D et l'api de rendu (OpenGL ou Direct3D)
				<br />Ainsi c'est aussi la seule classe à même de créer les renderers
	*/
	class RenderSystem
		: public Castor::OwnedBy< Engine >
	{
	protected:
		template< class Ty > friend struct RendererCreator;
		template< class Ty > friend struct RendererAdder;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_engine		The engine
		 *\param[in]	p_eRendererType	The render API
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine		Le moteur
		 *\param[in]	p_eRendererType	L'API de rendu
		 */
		C3D_API RenderSystem( Engine & p_engine, eRENDERER_TYPE p_eRendererType );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~RenderSystem();
		/**
		 *\~english
		 *\brief		Initialises the render system
		 *\~french
		 *\brief		Initialise le render system
		 */
		C3D_API void Initialise();
		/**
		 *\~english
		 *\brief		Cleans the render system up
		 *\~french
		 *\brief		Nettoie le render system
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Creates a ShaderProgram in a given language
		 *\remarks		Only the render system can do that
		 *\param[in]	p_langage	The shader language
		 *\return		The created ShaderProgram
		 *\~french
		 *\brief		Crée un ShaderProgram dans un langage donné
		 *\remarks		Seul le render system peut faire ça
		 *\param[in]	p_langage	Le langage du shader
		 *\return		Le ShaderProgram créé
		 */
		C3D_API ShaderProgramSPtr CreateShaderProgram( eSHADER_LANGUAGE p_langage );
		/**
		 *\~english
		 *\brief		Renders the scene ambient lighting
		 *\param[in]	p_clColour			The light colour
		 *\param[in]	p_variableBuffer	The variable buffer that receives the ambient light
		 *\~french
		 *\brief		Rend la lumière ambiante de la scène
		 *\param[in]	p_clColour			La couleur de la lumière
		 *\param[in]	p_variableBuffer	Le buffer the variables, qui reçoit la lumière ambient
		 */
		C3D_API void RenderAmbientLight( Castor::Colour const & p_clColour, FrameVariableBuffer & p_variableBuffer );
		/**
		 *\~english
		 *\brief		Pushes a scene on th stack
		 *\param[in]	p_scene	The scene
		 *\~french
		 *\brief		Met une scène sur la pile
		 *\param[in]	p_scene	La scène
		 */
		C3D_API void PushScene( Scene * p_scene );
		/**
		 *\~english
		 *\brief		Pops a scene from the stack
		 *\~french
		 *\brief		Enlève la scène du haut de la pile
		 */
		C3D_API void PopScene();
		/**
		 *\~english
		 *\brief		Retrieves the top scene from the stack
		 *\return		The scene, nullptr if the stack is void
		 *\~french
		 *\brief		Récupère la scène du haut de la pile
		 *\return		La scène, nullptr si la pile est vide
		 */
		C3D_API Scene * GetTopScene();
		/**
		 *\~english
		 *\brief		Retrieves the currently active camera
		 *\return		The camera
		 *\~french
		 *\brief		Récupère la caméra actuellement active
		 *\return		La caméra
		 */
		C3D_API Camera * GetCurrentCamera()const;
		/**
		 *\~english
		 *\brief		Defines the currently active camera
		 *\param[in]	p_pCamera	The camera
		 *\~french
		 *\brief		Définit la caméra actuellement active
		 *\param[in]	p_pCamera	La caméra
		 */
		C3D_API void SetCurrentCamera( Camera * p_pCamera );
		/**
		 *\~english
		 *\return		A pre-configured GlslWriter instance.
		 *\~french
		 *\brief		Une instance pré-configurée de GlslWriter.
		 */
		C3D_API GLSL::GlslWriter CreateGlslWriter();
		/**
		 *\~english
		 *\brief		Creates a shader program for billboards rendering use.
		 *\return		The created program.
		 *\~french
		 *\brief		Crée un programme shader pour les rendu de billboards.
		 *\return		Le programme créé.
		 */
		C3D_API ShaderProgramSPtr CreateBillboardsProgram( RenderTechnique const & p_technique, uint32_t p_flags );
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags
		 *\param[in]	p_programFlags	Bitwise ORed ePROGRAM_FLAG
		 *\~french
		 *\brief		Récupère le source du vertex shader qui correspond aux flags donnés
		 *\param[in]	p_programFlags	Combinaison de ePROGRAM_FLAG
		 */
		C3D_API Castor::String GetVertexShaderSource( uint32_t p_programFlags );
		/**
		 *\~english
		 *\brief		Checks support for given shader model
		 *\param[in]	p_model	The shader model
		 *\return		\p false if the given model is not supported by current API
		 *\~french
		 *\brief		Vérifie le support d'un modèle de shaders
		 *\param[in]	p_model	Le modèle de shaders
		 *\return		\p false si le modèle donné n'est pas supporté par l'API actuelle
		 */
		inline bool CheckSupport( eSHADER_MODEL p_model )
		{
			return p_model <= m_maxShaderModel;
		}
		/**
		 *\~english
		 *\return		The maximum supported shader model.
		 *\~french
		 *\return		Le modèle de shader maximal supporté.
		 */
		inline eSHADER_MODEL GetMaxShaderModel()
		{
			return m_maxShaderModel;
		}
		/**
		 *\~english
		 *\brief		Creates a geometries buffer holder.
		 *\param[in]	p_topology	The buffers topology.
		 *\param[in]	p_program	The shader program.
		 *\param[in]	p_vtx		The vertex buffer.
		 *\param[in]	p_idx		The index buffer.
		 *\param[in]	p_bones		The bones data buffer.
		 *\param[in]	p_inst		The instances matrices buffer.
		 *\~french
		 *\brief		Crée un conteneur de buffers de géométrie.
		 *\param[in]	p_topology	La topologie des tampons.
		 *\param[in]	p_program	Le programme shader.
		 *\param[in]	p_vtx		Le tampon de sommets.
		 *\param[in]	p_idx		Le tampon d'indices.
		 *\param[in]	p_bones		Le tampon de données de bones.
		 *\param[in]	p_inst		Le tampon de matrices d'instances.
		 */
		C3D_API virtual GeometryBuffersSPtr CreateGeometryBuffers( eTOPOLOGY p_topology, ShaderProgram const & p_program, VertexBuffer * p_vtx, IndexBuffer * p_idx, VertexBuffer * p_bones, VertexBuffer * p_inst ) = 0;
		/**
		 *\~english
		 *\brief		Creates a rendering context
		 *\return		The created context
		 *\~french
		 *\brief		Crée un contexte de rendu
		 *\return		Le contexte créé
		 */
		C3D_API virtual ContextSPtr CreateContext() = 0;
		/**
		 *\~english
		 *\brief		Creates a FrameVariableBuffer
		 *\return		The created FrameVariableBuffer
		 *\~french
		 *\brief		Crée un FrameVariableBuffer
		 *\return		Le FrameVariableBuffer créé
		 */
		C3D_API virtual FrameVariableBufferSPtr CreateFrameVariableBuffer( Castor::String const & p_name ) = 0;
		/**
		 *\~english
		 *\brief		Create a depth and stencil states object
		 *\return		The object
		 *\~french
		 *\brief		Crée un objet d'états de depth et stencil
		 *\return		L'objet
		 */
		C3D_API virtual DepthStencilStateSPtr CreateDepthStencilState() = 0;
		/**
		 *\~english
		 *\brief		Create a rasteriser states object
		 *\return		The object
		 *\~french
		 *\brief		Crée un objet d'états de rasteriser
		 *\return		L'objet
		 */
		C3D_API virtual RasteriserStateSPtr CreateRasteriserState() = 0;
		/**
		 *\~english
		 *\brief		Create a blender states object
		 *\return		The object
		 *\~french
		 *\brief		Crée un objet d'états de blend
		 *\return		L'objet
		 */
		C3D_API virtual BlendStateSPtr CreateBlendState() = 0;
		/**
		 *\~english
		 *\brief		Create a sampler
		 *\param[in]	p_name	The sampler name
		 *\return		The object
		 *\~french
		 *\brief		Crée un échantillonneur
		 *\param[in]	p_name	Le nom de l'échantillonneur
		 *\return		L'objet
		 */
		C3D_API virtual SamplerSPtr CreateSampler( Castor::String const & p_name ) = 0;
		/**
		 *\~english
		 *\brief		Creates a ShaderProgram (GLSL or HLSL only)
		 *\return		The created ShaderProgram
		 *\~french
		 *\brief		Crée un ShaderProgram (GLSL ou HLSL seulement)
		 *\return		Le ShaderProgram créé
		 */
		C3D_API virtual ShaderProgramSPtr CreateShaderProgram() = 0;
		/**
		 *\~english
		 *\brief		Creates a texture.
		 *\return		The created texture, dependant of current API.
		 *\~french
		 *\brief		Crée une texture.
		 *\return		La texture créée, dépendante de l'API actuelle.
		 */
		C3D_API virtual StaticTextureSPtr CreateStaticTexture() = 0;
		/**
		 *\~english
		 *\brief		Creates a texture.
		 *\param[in]	p_cpuAccess		The required CPU access (combination of eACCESS_TYPE).
		 *\param[in]	p_gpuAccess		The required GPU access (combination of eACCESS_TYPE).
		 *\return		The created texture, dependant of current API.
		 *\~french
		 *\brief		Crée une texture.
		 *\param[in]	p_cpuAccess		Les accès requis pour le CPU (combinaison de eACCESS_TYPE).
		 *\param[in]	p_gpuAccess		Les accès requis pour le GPU (combinaison de eACCESS_TYPE).
		 *\return		La texture créée, dépendante de l'API actuelle.
		 */
		C3D_API virtual DynamicTextureSPtr CreateDynamicTexture( uint8_t p_cpuAccess, uint8_t p_gpuAccess ) = 0;
		/**
		 *\~english
		 *\brief		Creates a vertex buffer
		 *\remarks		Only the render system can do that
		 *\param[in]	p_buffer	The hardware buffer to which the vertex buffer will be linked
		 *\return		The created vertex buffer, dependant of current API
		 *\~french
		 *\brief		Crée un tampon de sommets
		 *\remarks		Seul le render system peut faire ça
		 *\param[in]	p_buffer	Le tampon hardware auquel sera lié le tampon de sommets
		 *\return		Le tampon de sommets créé, dépendant de l'API actuelle
		 */
		C3D_API virtual std::shared_ptr< GpuBuffer< uint8_t > > CreateVertexBuffer( CpuBuffer< uint8_t > * p_buffer ) = 0;
		/**
		 *\~english
		 *\brief		Creates an index buffer
		 *\remarks		Only the render system can do that
		 *\param[in]	p_buffer	The hardware buffer to which the index buffer will be linked
		 *\return		The created index buffer, dependant of current API
		 *\~french
		 *\brief		Crée un tampon d'indices
		 *\remarks		Seul le render system peut faire ça
		 *\param[in]	p_buffer	Le tampon hardware auquel sera lié le tampon d'indices
		 *\return		Le tampon d'indices créé, dépendant de l'API actuelle
		 */
		C3D_API virtual std::shared_ptr< GpuBuffer< uint32_t > > CreateIndexBuffer( CpuBuffer< uint32_t > * p_buffer ) = 0;
		/**
		 *\~english
		 *\return		A pipeline implementation, depending on loaded API
		 *\~french
		 *\return		Une implémentation de pipeline, en fonction de l'API chargée.
		 */
		C3D_API virtual IPipelineImplSPtr GetPipelineImpl() = 0;
		/**
		 *\~english
		 *\brief		Creates a frame buffer.
		 *\return		The created frame buffer.
		 *\~french
		 *\brief		Crée un tampon d'image.
		 *\return		Le tampon d'image créé.
		 */
		C3D_API virtual FrameBufferSPtr CreateFrameBuffer() = 0;
		/**
		 *\~english
		 *\brief		Creates the window back buffers.
		 *\return		The created back buffers.
		 *\~french
		 *\brief		Crée les tampons d'image de la fenêtre.
		 *\return		Les tampons d'image créés.
		 */
		C3D_API virtual BackBuffersSPtr CreateBackBuffers() = 0;
		/**
		 *\~english
		 *\brief		Creates a GPU query.
		 *\param[in]	p_type	The query type.
		 *\return		The created GPU query.
		 *\~french
		 *\brief		Crée une requête GPU.
		 *\param[in]	p_type	Le type de requête.
		 *\return		La requête GPU créée.
		 */
		C3D_API virtual GpuQuerySPtr CreateQuery( eQUERY_TYPE p_type ) = 0;
		/**
		 *\~english
		 *\brief		Tells if the RenderSystem supports given shader type
		 *\param[in]	p_type	The shader type
		 *\~french
		 *\brief		Dit si le RenderSystem supporte le type de shader donné
		 *\param[in]	p_type	Le type de shader
		 */
		inline bool HasShaderType( eSHADER_TYPE p_type )const
		{
			return m_useShader[p_type];
		}
		/**
		 *\~english
		 *\brief		Tells if the RenderSystem is initialised
		 *\~french
		 *\brief		Dit si le RenderSystem est initialisé
		 */
		inline bool IsInitialised()const
		{
			return m_initialised;
		}
		/**
		 *\~english
		 *\brief		Tells if the RenderSystem supports stereo
		 *\~french
		 *\brief		Dit si le RenderSystem supporte la stéréo
		 */
		inline bool IsStereoAvailable()const
		{
			return m_stereoAvailable;
		}
		/**
		 *\~english
		 *\brief		Defines if the RenderSystem supports stereo
		 *\~french
		 *\brief		Définit si le RenderSystem supporte la stéréo
		 */
		inline void SetStereoAvailable( bool p_bStereo )
		{
			m_stereoAvailable = p_bStereo;
		}
		/**
		 *\~english
		 *\brief		Retrieves the pipeline
		 *\~french
		 *\brief		Récupère le pipeline
		 */
		inline Pipeline const & GetPipeline()const
		{
			return *m_pipeline;
		}
		/**
		 *\~english
		 *\brief		Retrieves the pipeline
		 *\~french
		 *\brief		Récupère le pipeline
		 */
		inline Pipeline & GetPipeline()
		{
			return *m_pipeline;
		}
		/**
		 *\~english
		 *\brief		Retrieves the renderer API
		 *\~french
		 *\brief		Récupère l'API de rendu
		 */
		inline eRENDERER_TYPE GetRendererType()const
		{
			return m_rendererType;
		}
		/**
		 *\~english
		 *\brief		Retrieves the instanced draw calls support
		 *\return		The value
		 *\~french
		 *\brief		Récupère le support de l'instanciation
		 *\return		La valeur
		 */
		inline bool HasInstancing()const
		{
			return m_instancing;
		}
		/**
		 *\~english
		 *\brief		Retrieves the accumulation buffer support
		 *\return		The value
		 *\~french
		 *\brief		Récupère le support du buffer d'accumulation
		 *\return		La valeur
		 */
		inline bool HasAccumulationBuffer()const
		{
			return m_accumBuffer;
		}
		/**
		 *\~english
		 *\brief		Retrieves the non power of two textures support
		 *\return		The value
		 *\~french
		 *\brief		Récupère le support des textures non puissance de deux
		 *\return		La valeur
		 */
		inline bool HasNonPowerOfTwoTextures()const
		{
			return m_nonPowerOfTwoTextures;
		}
		/**
		 *\~english
		 *\brief		Sets the main render context
		 *\param[in]	p_context	The context
		 *\~french
		 *\brief		Définit le contexte de rendu principal
		 *\param[in]	p_context	Le contexte
		 */
		inline void SetMainContext( ContextSPtr p_context )
		{
			m_mainContext = p_context;
		}
		/**
		 *\~english
		 *\brief		Retrieves the main render context
		 *\return		The context
		 *\~french
		 *\brief		Récupère le contexte de rendu principal
		 *\return		Le contexte
		 */
		inline ContextSPtr GetMainContext()
		{
			return m_mainContext;
		}
		/**
		 *\~english
		 *\brief		Sets the currently active render context
		 *\param[in]	p_context	The context
		 *\~french
		 *\brief		Définit le contexte de rendu actuellement actif
		 *\param[in]	p_context	Le contexte
		 */
		inline void SetCurrentContext( Context * p_context )
		{
			m_currentContext = p_context;
		}
		/**
		 *\~english
		 *\brief		Retrieves the currently active render context
		 *\return		The context
		 *\~french
		 *\brief		Récupère le contexte de rendu actuellement actif
		 *\return		Le contexte
		 */
		inline Context * GetCurrentContext()
		{
			return m_currentContext;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay renderer
		 *\return		The value
		 *\~french
		 *\brief		Récupère le renderer d'overlays
		 *\return		La valeur
		 */
		inline OverlayRendererSPtr GetOverlayRenderer()
		{
			return m_overlayRenderer;
		}
		/**
		 *\~english
		 *\brief		Increments the GPU time.
		 *\param[in]	p_time	The increment value.
		 *\~french
		 *\brief		Incrémente le temps CPU.
		 *\param[in]	p_time	La valeur d'incrément.
		 */
		template< class Rep, class Period >
		inline void IncGpuTime( std::chrono::duration< Rep, Period > const & p_time )
		{
			m_gpuTime += std::chrono::duration_cast< std::chrono::milliseconds >( p_time );
		}
		/**
		 *\~english
		 *\brief		Resets the GPU time.
		 *\~french
		 *\brief		Réinitialise le temps CPU.
		 */
		inline void ResetGpuTime()
		{
			m_gpuTime = std::chrono::milliseconds( 0 );
		}
		/**
		 *\~english
		 *\brief		Retrieves the GPU time.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le temps CPU.
		 *\return		La valeur.
		 */
		inline std::chrono::milliseconds GetGpuTime()const
		{
			return m_gpuTime;
		}
		/**
		 *\~english
		 *\return		The shader language version.
		 *\~french
		 *\return		La version du langage shader.
		 */
		inline uint32_t GetShaderLanguageVersion()const
		{
			return m_shaderLanguageVersion;
		}
		/**
		 *\~english
		 *\return		The constant buffers support status.
		 *\~french
		 *\return		Le statut du support des tampons de constantes.
		 */
		inline bool HasConstantsBuffers()const
		{
			return m_hasConstantsBuffers;
		}
		/**
		 *\~english
		 *\return		The texture buffers support status.
		 *\~french
		 *\return		Le statut du support des tampons de textures.
		 */
		inline bool HasTextureBuffers()const
		{
			return m_hasTextureBuffers;
		}

	protected:
		/**
		 *\~english
		 *\brief		Initialises the render system
		 *\~french
		 *\brief		Initialise le render system
		 */
		C3D_API virtual void DoInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans the render system up
		 *\~french
		 *\brief		Nettoie le render system
		 */
		C3D_API virtual void DoCleanup() = 0;

	protected:
		//!\~english Mutex used to make this class thread safe	\~french Mutex pour rendre cette classe thread safe
		std::recursive_mutex m_mutex;
		//!\~english Tells whether or not it is initialised	\~french Dit si le render system est initialisé
		bool m_initialised;
		//!\~english Tells whether or not the selected render API supports instanced draw calls	\~french Dit si l'API de rendu choisie supporte le dessin instancié
		bool m_instancing;
		//!\~english Tells whether or not the selected render API supports accumulation buffers	\~french Dit si l'API de rendu choisie supporte le buffer d'accumulation
		bool m_accumBuffer;
		//!\~english Tells whether or not the selected render API supports non power of two textures	\~french Dit si l'API de rendu choisie supporte les textures non puissance de 2
		bool m_nonPowerOfTwoTextures;
		//!\~english Tells the RenderSystem supports stereo	\~french Dit si le RenderSystem supporte la stéréo
		bool m_stereoAvailable;
		//!\~english Tells which types of shaders are supported	\~french Dit quel type de shaders sont supportés
		bool m_useShader[eSHADER_TYPE_COUNT];
		//!\~english The maximum supported shader model.	\~french Le modèle de shader maximum supporté.
		eSHADER_MODEL m_maxShaderModel;
		//!\~english The overlay renderer	\~french Le renderer d'overlays
		OverlayRendererSPtr m_overlayRenderer;
		//!\~english The main render context	\~french Le contexte de rendu principal
		ContextSPtr m_mainContext;
		//!\~english The currently active render context	\~french Le contexte de rendu actuellement actif
		ContextRPtr m_currentContext;
		//!\~english The matrix pipeline	\~french Le pipeline contenant les matrices
		std::unique_ptr< Pipeline > m_pipeline;
		//!\~english Scene stack	\~french Pile des scènes
		std::stack< SceneRPtr > m_stackScenes;
		//!\~english The current loaded renderer api type	\~french Le type de l'api de rendu actuellement chargée
		eRENDERER_TYPE m_rendererType;
		//!\~english The currently active camera	\~french La caméra actuellement active
		CameraRPtr m_pCurrentCamera;
		//!\~english The time spent on GPU for current frame.	\~french Le temps passé sur le GPU pour l'image courante.
		std::chrono::milliseconds m_gpuTime;
		//!\~english The shader language version.	\~french La version du langage de shader.
		uint32_t m_shaderLanguageVersion;
		//!\~english The constants buffers support status.	\~french Le statut du support de tampons de constantes.
		bool m_hasConstantsBuffers;
		//!\~english The texture buffers support status.	\~french Le statut du support de tampons de textures.
		bool m_hasTextureBuffers;

#if C3D_TRACE_OBJECTS

		struct ObjectDeclaration
		{
			uint32_t m_id;
			std::string m_name;
			void * m_object;
			std::string m_file;
			int m_line;
			int m_ref;
			std::string m_stack;
		};

		uint32_t m_id = 0;
		std::list< ObjectDeclaration > m_allocated;

		template< typename T >
		bool DoTrack( T * p_object, std::string const & p_type, std::string const & p_file, int p_line, std::string & p_name )
		{
			auto && l_it = std::find_if( m_allocated.begin(), m_allocated.end(), [p_object]( ObjectDeclaration const & l_object )
			{
				return p_object == l_object.m_object;
			} );

			bool l_return = l_it == m_allocated.end();

			std::stringstream l_ptr;
			l_ptr.width( 16 );
			l_ptr.fill( '0' );
			l_ptr << std::hex << std::right << uint64_t( p_object );
			std::stringstream l_type;
			l_type.width( 20 );
			l_type << std::left << p_type;
			std::stringstream l_name;
			l_name << "(" << m_id << ") " << l_type.str() << " [0x" << l_ptr.str() << "]";

			if ( l_return )
			{
				std::stringstream l_stream;
				l_stream << Castor::Debug::Backtrace();
				m_allocated.push_back( { ++m_id, p_type, p_object, p_file, p_line, 1, l_stream.str() } );
				Castor::Logger::LogDebug( l_name );
				p_name = l_name.str();
			}
			else
			{
				if ( l_it->m_ref > 0 )
				{
					Castor::Logger::LogDebug( std::stringstream() << "Rereferencing object: " << l_type.str() << " [0x" << l_ptr.str() << "] => " << l_it->m_ref );
				}
				else
				{
					Castor::Logger::LogDebug( l_name );
				}

				++l_it->m_ref;
			}

			return l_return;
		}

		inline bool DoTrack( Castor::Named * p_object, std::string const & p_type, std::string const & p_file, int p_line, std::string & p_name )
		{
			return DoTrack( reinterpret_cast< void * >( p_object ), p_type + ": " + Castor::string::string_cast< char >( p_object->GetName() ), p_file, p_line, p_name );
		}

		template< typename T >
		bool DoUntrack( T * p_object, ObjectDeclaration & p_declaration )
		{
			auto l_it = std::find_if( m_allocated.begin(), m_allocated.end(), [&p_object]( ObjectDeclaration p_decl )
			{
				return p_object == p_decl.m_object;
			} );

			bool l_return = false;
			char l_szName[1024] = { 0 };
			std::stringstream l_ptr;
			l_ptr.width( 16 );
			l_ptr.fill( '0' );
			l_ptr << std::hex << std::right << uint64_t( p_object );

			if ( l_it != m_allocated.end() )
			{
				std::stringstream l_type;
				l_type.width( 20 );
				l_type << std::left << l_it->m_name;

				if ( !--l_it->m_ref )
				{
					p_declaration = *l_it;
					l_return = true;
					Castor::Logger::LogWarning( std::stringstream() << "Released " << l_type.str() << " [0x" << l_ptr.str() << "] => " << p_declaration.m_ref );
				}
				else if ( l_it->m_ref < 0 )
				{
					Castor::Logger::LogError( std::stringstream() << "Trying to release an already released object: " << l_type.str() << " [0x" << l_ptr.str() << "] => " << l_it->m_ref );
				}
			}
			else
			{
				Castor::Logger::LogWarning( std::stringstream() << "Untracked [0x" << l_ptr.str() << cuT( "]" ) );
			}

			return l_return;
		}

		C3D_API void DoReportTracked();

#else

		inline void DoReportTracked()
		{
		}

#endif
	};
}

#endif
