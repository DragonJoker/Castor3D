/*
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
		 *\param[in]	p_eLanguage	The shader language
		 *\return		The created ShaderProgram
		 *\~french
		 *\brief		Crée un ShaderProgram dans un langage donné
		 *\remarks		Seul le render system peut faire ça
		 *\param[in]	p_eLanguage	Le langage du shader
		 *\return		Le ShaderProgram créé
		 */
		C3D_API ShaderProgramBaseSPtr CreateShaderProgram( eSHADER_LANGUAGE p_eLanguage );
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
		 *\return		The scene, NULL if the stack is void
		 *\~french
		 *\brief		Récupère la scène du haut de la pile
		 *\return		La scène, NULL si la pile est vide
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
		 *\brief		Checks support for given shader model
		 *\param[in]	p_eProfile	The shader model
		 *\return		\p false if the given model is not supported by current API
		 *\~french
		 *\brief		Vérifie le support d'un modèle de shaders
		 *\param[in]	p_eProfile	Le modèle de shaders
		 *\return		\p false si le modèle donné n'est pas supporté par l'API actuelle
		 */
		C3D_API virtual	bool CheckSupport( eSHADER_MODEL p_eProfile ) = 0;
		/**
		 *\~english
		 *\brief		Checks if the loaded render API needs matrix transposition for shader variables
		 *\remarks		Needed because of Direct3D 11
		 *\return		\p false if the doesn't need it
		 *\~french
		 *\brief		Vérifie si l'API de rendu nécessite de transposer les matrices pour les variables de shaders
		 *\remarks		Nécessaire à cause de Direct3D 11
		 *\return		\p false s'il la transposition n'est pas nécessaire
		 */
		C3D_API virtual	bool NeedsMatrixTransposition()const = 0;
		/**
		 *\~english
		 *\brief		Creates a geometries buffer holder
		 *\param[in]	p_pVertexBuffer	The vertex buffer
		 *\param[in]	p_pIndexBuffer	The index buffer
		 *\param[in]	p_pMatrixBuffer	The matrix buffer
		 *\return		The created geometries buffer holder
		 *\~french
		 *\brief		Crée un conteneur de buffers de géométrie
		 *\param[in]	p_pVertexBuffer	Le tampon de sommets
		 *\param[in]	p_pIndexBuffer	Le tampon d'indices
		 *\param[in]	p_pMatrixBuffer	Le tampon de matrices
		 *\return		Le conteneur de buffers de géométrie
		 */
		C3D_API virtual GeometryBuffersSPtr CreateGeometryBuffers( VertexBufferUPtr p_pVertexBuffer, IndexBufferUPtr p_pIndexBuffer, MatrixBufferUPtr p_pMatrixBuffer ) = 0;
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
		 *\brief		Tells if the renderer API supports depth buffer for main FBO
		 *\return		The support status
		 *\~french
		 *\brief		Dit si l'API de rendu supporte les tampons de profondeur pour le FBO principal
		 *\return		Le statut du support
		 */
		C3D_API virtual bool SupportsDepthBuffer()const = 0;
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
		 *\brief		Create a billboards list
		 *\param[in]	p_scene		The parent scene
		 *\return		The object
		 *\~french
		 *\brief		Crée une liste de billboards
		 *\param[in]	p_scene		La scène parente
		 *\return		L'objet
		 */
		C3D_API virtual BillboardListSPtr CreateBillboardsList( SceneSPtr p_scene ) = 0;
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
		 *\brief		Create a render target
		 *\param[in]	p_type	The render target type
		 *\return		The object
		 *\~french
		 *\brief		Crée une cible de rendu
		 *\param[in]	p_type	Le type de cible de rendu
		 *\return		L'objet
		 */
		C3D_API virtual RenderTargetSPtr CreateRenderTarget( eTARGET_TYPE p_type ) = 0;
		/**
		 *\~english
		 *\brief		Create a render window
		 *\return		The object
		 *\~french
		 *\brief		Crée une fenêtre de rendu
		 *\return		L'objet
		 */
		C3D_API virtual RenderWindowSPtr CreateRenderWindow() = 0;
		/**
		 *\~english
		 *\brief		Creates a ShaderProgram (GLSL or HLSL only)
		 *\return		The created ShaderProgram
		 *\~french
		 *\brief		Crée un ShaderProgram (GLSL ou HLSL seulement)
		 *\return		Le ShaderProgram créé
		 */
		C3D_API virtual ShaderProgramBaseSPtr CreateShaderProgram() = 0;
		/**
		 *\~english
		 *\brief		Creates an overlay renderer
		 *\return		The created renderer
		 *\~french
		 *\brief		Crée un renderer d'incrustations
		 *\return		Le renderer créé
		 */
		C3D_API virtual OverlayRendererSPtr CreateOverlayRenderer() = 0;
		/**
		 *\~english
		 *\brief		Creates a texture
		 *\remarks		Only the render system can do that
		 *\param[in]	p_type	The texture type
		 *\return		The created texture, dependant of current API
		 *\~french
		 *\brief		Crée une texture
		 *\remarks		Seul le render system peut faire ça
		 *\param[in]	p_type	Le type de texture
		 *\return		La texture créée, dépendante de l'API actuelle
		 */
		C3D_API virtual StaticTextureSPtr CreateStaticTexture() = 0;
		/**
		 *\~english
		 *\brief		Creates a texture
		 *\remarks		Only the render system can do that
		 *\param[in]	p_type	The texture type
		 *\return		The created texture, dependant of current API
		 *\~french
		 *\brief		Crée une texture
		 *\remarks		Seul le render system peut faire ça
		 *\param[in]	p_type	Le type de texture
		 *\return		La texture créée, dépendante de l'API actuelle
		 */
		C3D_API virtual DynamicTextureSPtr CreateDynamicTexture() = 0;
		/**
		 *\~english
		 *\brief		Creates a vertex buffer, given a buffer declaration
		 *\remarks		Only the render system can do that
		 *\param[in]	p_elements	The buffer declaration
		 *\param[in]	p_pBuffer	The hardware buffer to which the vertex buffer will be linked
		 *\return		The created vertex buffer, dependant of current API
		 *\~french
		 *\brief		Crée un tampon de sommets, selon une déclaration de tampon
		 *\remarks		Seul le render system peut faire ça
		 *\param[in]	p_elements	La déclaration de tampon
		 *\param[in]	p_pBuffer	Le tampon hardware auquel sera lié le tampon de sommets
		 *\return		Le tampon de sommets créé, dépendant de l'API actuelle
		 */
		C3D_API virtual std::shared_ptr< GpuBuffer< uint8_t > > CreateVertexBuffer( BufferDeclaration const & p_elements, CpuBuffer< uint8_t > * p_pBuffer ) = 0;
		/**
		 *\~english
		 *\brief		Creates an index buffer
		 *\remarks		Only the render system can do that
		 *\param[in]	p_pBuffer	The hardware buffer to which the index buffer will be linked
		 *\return		The created index buffer, dependant of current API
		 *\~french
		 *\brief		Crée un tampon d'indices
		 *\remarks		Seul le render system peut faire ça
		 *\param[in]	p_pBuffer	Le tampon hardware auquel sera lié le tampon d'indices
		 *\return		Le tampon d'indices créé, dépendant de l'API actuelle
		 */
		C3D_API virtual std::shared_ptr< GpuBuffer< uint32_t > > CreateIndexBuffer( CpuBuffer< uint32_t > * p_pBuffer ) = 0;
		/**
		 *\~english
		 *\brief		Creates a matrix buffer
		 *\remarks		Only the render system can do that
		 *\param[in]	p_pBuffer	The hardware buffer to which the buffer will be linked
		 *\return		The created buffer, dependant of current API
		 *\~french
		 *\brief		Crée un tampon de matrices
		 *\remarks		Seul le render system peut faire ça
		 *\param[in]	p_pBuffer	Le tampon hardware auquel sera lié le tampon
		 *\return		Le tampon créé, dépendant de l'API actuelle
		 */
		C3D_API virtual std::shared_ptr< GpuBuffer< real > > CreateMatrixBuffer( CpuBuffer< real > * p_pBuffer ) = 0;
		/**
		 *\~english
		 *\brief		Creates a texture buffer
		 *\remarks		Only the render system can do that
		 *\param[in]	p_pBuffer	The hardware buffer to which the texture buffer will be linked
		 *\return		The created texture buffer, dependant of current API
		 *\~french
		 *\brief		Crée un tampon detexture
		 *\remarks		Seul le render system peut faire ça
		 *\param[in]	p_pBuffer	Le tampon hardware auquel sera lié le tampon de texture
		 *\return		Le tampon de texture créé, dépendant de l'API actuelle
		 */
		C3D_API virtual std::shared_ptr< GpuBuffer< uint8_t > > CreateTextureBuffer( CpuBuffer< uint8_t > * p_pBuffer ) = 0;
		/**
		 *\~english
		 *\return		A pipeline implementation, depending on loaded API
		 *\~french
		 *\return		Une implémentation de pipeline, en fonction de l'API chargée.
		 */
		C3D_API virtual IPipelineImplSPtr GetPipelineImpl() = 0;
		/**
		 *\~english
		 *\brief		Tells if multi-texturing is available
		 *\~french
		 *\brief		Dit si le multi-texturing est disponible
		 */
		inline bool UseMultiTexturing()const
		{
			return m_useMultiTexturing;
		}
		/**
		 *\~english
		 *\brief		Tells if shaders are available
		 *\~french
		 *\brief		Dit si les shaders sont disponibles
		 */
		inline bool UseShaders()const
		{
			return m_useShaders;
		}
		/**
		 *\~english
		 *\brief		Tells if shaders are forced (OpenGL 3.x/4.x)
		 *\~french
		 *\brief		Dit si les shaders sont obligatoires (OpenGL 3.x/4.x)
		 */
		inline bool ForceShaders()const
		{
			return m_forceShaders;
		}
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
			return m_bInitialised;
		}
		/**
		 *\~english
		 *\brief		Tells if the RenderSystem supports stereo
		 *\~french
		 *\brief		Dit si le RenderSystem supporte la stéréo
		 */
		inline bool IsStereoAvailable()const
		{
			return m_bStereoAvailable;
		}
		/**
		 *\~english
		 *\brief		Defines if the RenderSystem supports stereo
		 *\~french
		 *\brief		Définit si le RenderSystem supporte la stéréo
		 */
		inline void SetStereoAvailable( bool p_bStereo )
		{
			m_bStereoAvailable = p_bStereo;
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
			return m_eRendererType;
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
			return m_bInstancing;
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
			return m_bAccumBuffer;
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
			return m_bNonPowerOfTwoTextures;
		}
		/**
		 *\~english
		 *\brief		Sets the main render context
		 *\param[in]	p_pContext	The context
		 *\~french
		 *\brief		Définit le contexte de rendu principal
		 *\param[in]	p_pContext	Le contexte
		 */
		inline void SetMainContext( ContextSPtr p_pContext )
		{
			m_wpMainContext = p_pContext;
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
			return m_wpMainContext.lock();
		}
		/**
		 *\~english
		 *\brief		Sets the currently active render context
		 *\param[in]	p_pContext	The context
		 *\~french
		 *\brief		Définit le contexte de rendu actuellement actif
		 *\param[in]	p_pContext	Le contexte
		 */
		inline void SetCurrentContext( Context * p_pContext )
		{
			m_pCurrentContext = p_pContext;
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
			return m_pCurrentContext;
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
		//!\~english Tells if multi-texturing is supported	\~french Dit si le multi-texturing est supporté
		bool m_useMultiTexturing;
		//!\~english Tells if shaders are supported	\~french Dit si les shaders sont supportés
		bool m_useShaders;
		//!\~english Tells if shaders are forced (OpenGl 3.x/4.x)	\~french Dit si les shaders sont forcés (OpenGl 3.x/4.x)
		bool m_forceShaders;
		//!\~english Tells which types of shaders are supported	\~french Dit quel type de shaders sont supportés
		bool m_useShader[eSHADER_TYPE_COUNT];
		//!\~english Tells whether or not it is initialised	\~french Dit si le render system est initialisé
		bool m_bInitialised;
		//!\~english Tells whether or not the selected render API supports instanced draw calls	\~french Dit si l'API de rendu choisie supporte le dessin instancié
		bool m_bInstancing;
		//!\~english Tells whether or not the selected render API supports accumulation buffers	\~french Dit si l'API de rendu choisie supporte le buffer d'accumulation
		bool m_bAccumBuffer;
		//!\~english Tells whether or not the selected render API supports non power of two textures	\~french Dit si l'API de rendu choisie supporte les textures non puissance de 2
		bool m_bNonPowerOfTwoTextures;
		//!\~english The overlay renderer	\~french Le renderer d'overlays
		OverlayRendererSPtr m_overlayRenderer;
		//!\~english The main render context	\~french Le contexte de rendu principal
		ContextWPtr m_wpMainContext;
		//!\~english The currently active render context	\~french Le contexte de rendu actuellement actif
		ContextRPtr m_pCurrentContext;
		//!\~english The matrix pipeline	\~french Le pipeline contenant les matrices
		std::unique_ptr< Pipeline > m_pipeline;
		//!\~english Scene stack	\~french Pile des scènes
		std::stack< SceneRPtr > m_stackScenes;
		//!\~english The current loaded renderer api type	\~french Le type de l'api de rendu actuellement chargée
		eRENDERER_TYPE m_eRendererType;
		//!\~english Tells the RenderSystem supports stereo	\~french Dit si le RenderSystem supporte la stéréo
		bool m_bStereoAvailable;
		//!\~english The currently active camera	\~french La caméra actuellement active
		CameraRPtr m_pCurrentCamera;
		//!\~english The matrix mode before call to BeginOverlaysRendering	\~french Le mode de matrice avant l'appel à BeginOverlaysRendering
		eMTXMODE m_ePreviousMtxMode;

#if !defined( NDEBUG )

		struct ObjectDeclaration
		{
			uint32_t m_id;
			std::string m_name;
			void * m_object;
			std::string m_file;
			int m_line;
			int m_ref;
			Castor::String m_stack;
		};

		uint32_t m_id = 0;
		std::list< ObjectDeclaration > m_allocated;

		C3D_API bool DoTrack( void * p_object, std::string const & p_type, std::string const & p_file, int p_line, std::string & p_name );
		C3D_API bool DoTrack( Castor::Named * p_object, std::string const & p_type, std::string const & p_file, int p_line, std::string & p_name );
		C3D_API bool DoUntrack( void * p_object, ObjectDeclaration & p_declaration );
		C3D_API bool DoUntrack( Castor::Named * p_object, ObjectDeclaration & p_declaration );
		C3D_API void DoReportTracked();

#endif
	};
}

#endif
