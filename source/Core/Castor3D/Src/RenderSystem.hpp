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

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	\remark
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
	\remark
	*/
	template< class Ty > struct RendererAdder;
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		The render system representation
	\remark		This is the class which is the link between Castor3D and the renderer driver (OpenGL or Direct3D)
				<br />Hence it is also the only class which can create the renderers
	\~french
	\brief		Représentation du système de rendu
	\remark		Cette classe fait le lien entre Castor3D et l'api de rendu (OpenGL ou Direct3D)
				<br />Ainsi c'est aussi la seule classe à même de créer les renderers
	*/
	class C3D_API RenderSystem
	{
	protected:
		template< class Ty > friend struct RendererCreator;
		template< class Ty > friend struct RendererAdder;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pEngine		The engine
		 *\param[in]	p_eRendererType	The render API
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pEngine		Le moteur
		 *\param[in]	p_eRendererType	L'API de rendu
		 */
		RenderSystem( Engine * p_pEngine, eRENDERER_TYPE p_eRendererType );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~RenderSystem();
		/**
		 *\~english
		 *\brief		Initialises the render system
		 *\~french
		 *\brief		Initialise le render system
		 */
		void Initialise();
		/**
		 *\~english
		 *\brief		Cleans the render system up
		 *\~french
		 *\brief		Nettoie le render system
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Puts the cleanable renderers to be cleaned
		 *\~french
		 *\brief		Met les renderers vidables à vider
		 */
		virtual	void PrepareRenderersCleanup();
		/**
		 *\~english
		 *\brief		Cleans the cleanable renderers
		 *\~french
		 *\brief		Vide les renderers vidables
		 */
		virtual	void CleanupRenderers();
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
		virtual	bool CheckSupport( eSHADER_MODEL p_eProfile ) = 0;
		/**
		 *\~english
		 *\brief		Checks if the loaded render API needs matrix transposition for shader variables
		 *\remark		Needed because of Direct3D 11
		 *\return		\p false if the doesn't need it
		 *\~french
		 *\brief		Vérifie si l'API de rendu nécessite de transposer les matrices pour les variables de shaders
		 *\remark		Nécessaire à cause de Direct3D 11
		 *\return		\p false s'il la transposition n'est pas nécessaire
		 */
		virtual	bool NeedsMatrixTransposition()const = 0;
		/**
		 *\~english
		 *\brief		Creates a renderer
		 *\remark		Only the render system can do that
		 *\return		The created renderer
		 *\~french
		 *\brief		Crée un renderer
		 *\remark		Seul le render system peut faire ça
		 *\return		Le renderer créé
		 */
		template< class Ty > std::shared_ptr< Ty > CreateRenderer();
		/**
		 *\~english
		 *\brief		Creates an overlay renderer
		 *\return		The created renderer
		 *\~french
		 *\brief		Crée un renderer d'incrustations
		 *\return		Le renderer créé
		 */
		OverlayRendererSPtr CreateOverlayRenderer();
		/**
		 *\~english
		 *\brief		Creates a vertex buffer, given a buffer declaration
		 *\remark		Only the render system can do that
		 *\param[in]	p_elements	The buffer declaration
		 *\param[in]	p_pBuffer	The hardware buffer to which the vertex buffer will be linked
		 *\return		The created vertex buffer, dependant of current API
		 *\~french
		 *\brief		Crée un tampon de sommets, selon une déclaration de tampon
		 *\remark		Seul le render system peut faire ça
		 *\param[in]	p_elements	La déclaration de tampon
		 *\param[in]	p_pBuffer	Le tampon hardware auquel sera lié le tampon de sommets
		 *\return		Le tampon de sommets créé, dépendant de l'API actuelle
		 */
		std::shared_ptr< GpuBuffer< uint8_t > > CreateVertexBuffer( BufferDeclaration const & p_elements, CpuBuffer< uint8_t > * p_pBuffer );
		/**
		 *\~english
		 *\brief		Creates an index buffer
		 *\remark		Only the render system can do that
		 *\param[in]	p_pBuffer	The hardware buffer to which the index buffer will be linked
		 *\return		The created index buffer, dependant of current API
		 *\~french
		 *\brief		Crée un tampon d'indices
		 *\remark		Seul le render system peut faire ça
		 *\param[in]	p_pBuffer	Le tampon hardware auquel sera lié le tampon d'indices
		 *\return		Le tampon d'indices créé, dépendant de l'API actuelle
		 */
		std::shared_ptr< GpuBuffer< uint32_t > > CreateIndexBuffer( CpuBuffer< uint32_t > * p_pBuffer );
		/**
		 *\~english
		 *\brief		Creates a matrix buffer
		 *\remark		Only the render system can do that
		 *\param[in]	p_pBuffer	The hardware buffer to which the buffer will be linked
		 *\return		The created buffer, dependant of current API
		 *\~french
		 *\brief		Crée un tampon de matrices
		 *\remark		Seul le render system peut faire ça
		 *\param[in]	p_pBuffer	Le tampon hardware auquel sera lié le tampon
		 *\return		Le tampon créé, dépendant de l'API actuelle
		 */
		std::shared_ptr< GpuBuffer< real > > CreateMatrixBuffer( CpuBuffer< real > * p_pBuffer );
		/**
		 *\~english
		 *\brief		Creates a texture buffer
		 *\remark		Only the render system can do that
		 *\param[in]	p_pBuffer	The hardware buffer to which the texture buffer will be linked
		 *\return		The created texture buffer, dependant of current API
		 *\~french
		 *\brief		Crée un tampon detexture
		 *\remark		Seul le render system peut faire ça
		 *\param[in]	p_pBuffer	Le tampon hardware auquel sera lié le tampon de texture
		 *\return		Le tampon de texture créé, dépendant de l'API actuelle
		 */
		std::shared_ptr< GpuBuffer< uint8_t > > CreateTextureBuffer( CpuBuffer< uint8_t > * p_pBuffer );
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
		virtual GeometryBuffersSPtr CreateGeometryBuffers( VertexBufferUPtr p_pVertexBuffer, IndexBufferUPtr p_pIndexBuffer, MatrixBufferUPtr p_pMatrixBuffer ) = 0;
		/**
		 *\~english
		 *\brief		Creates a dynamic texture
		 *\remark		Only the render system can do that
		 *\return		The created texture, dependant of current API
		 *\~french
		 *\brief		Crée une texture dynamique
		 *\remark		Seul le render system peut faire ça
		 *\return		La texture créée, dépendante de l'API actuelle
		 */
		DynamicTextureSPtr CreateDynamicTexture();
		/**
		 *\~english
		 *\brief		Creates a sttatic texture
		 *\remark		Only the render system can do that
		 *\return		The created texture, dependant of current API
		 *\~french
		 *\brief		Crée une texture statique
		 *\remark		Seul le render system peut faire ça
		 *\return		La texture créée, dépendante de l'API actuelle
		 */
		StaticTextureSPtr CreateStaticTexture();
		/**
		 *\~english
		 *\brief		Creates a ShaderProgram (GLSL or HLSL only)
		 *\remark		Only the render system can do that
		 *\return		The created ShaderProgram
		 *\~french
		 *\brief		Crée un ShaderProgram (GLSL ou HLSL seulement)
		 *\remark		Seul le render system peut faire ça
		 *\return		Le ShaderProgram créé
		 */
		ShaderProgramBaseSPtr CreateShaderProgram();
		/**
		 *\~english
		 *\brief		Creates a ShaderProgram in a given language
		 *\remark		Only the render system can do that
		 *\param[in]	p_eLanguage	The shader language
		 *\return		The created ShaderProgram
		 *\~french
		 *\brief		Crée un ShaderProgram dans un langage donné
		 *\remark		Seul le render system peut faire ça
		 *\param[in]	p_eLanguage	Le langage du shader
		 *\return		Le ShaderProgram créé
		 */
		ShaderProgramBaseSPtr CreateShaderProgram( eSHADER_LANGUAGE p_eLanguage );
		/**
		 *\~english
		 *\brief		Creates a IPipelineImpl, dependant of a shader language
		 *\param[in]	p_pPipeline	The parent pipeline
		 *\param[in]	p_eLanguage	The shader language
		 *\return		The create IPipelineImpl, NULL if language is unsupported
		 *\~french
		 *\brief		Crée un IPipelineImpl, dépendant d'un langage de shader
		 *\param[in]	p_pPipeline	Le pipeline parent
		 *\param[in]	p_eLanguage	Le langage de shader
		 *\return		Le IPipelineImpl créé, NULL si le langage n'est pas supporté
		 */
		IPipelineImpl * CreatePipeline( Pipeline * p_pPipeline, eSHADER_LANGUAGE p_eLanguage );
		/**
		 *\~english
		 *\brief		Destroys a language specific IPipelineImpl
		 *\param[in]	p_eLanguage	The shader language
		 *\param[in]	p_pPipeline	The IPipelineImpl
		 *\~french
		 *\brief		Détruit un IPipelineImpl spécifique à un langage
		 *\param[in]	p_eLanguage	Le langage de shader
		 *\param[in]	p_pPipeline	Le IPipelineImpl
		 */
		void DestroyPipeline( eSHADER_LANGUAGE p_eLanguage, IPipelineImpl * p_pPipeline );
		/**
		 *\~english
		 *\brief		Begins the overlay (2d elements) rendering
		 *\param[in]	p_size	The render target size
		 *\~french
		 *\brief		Démarre le rendu des overlays (éléments 2D)
		 *\param[in]	p_size	Les dimensions de la cible du rendu
		 */
		virtual	void BeginOverlaysRendering( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Ends the overlay (2d elements) rendering
		 *\~french
		 *\brief		Termine le rendu des overlays (éléments 2D)
		 */
		virtual	void EndOverlaysRendering();
		/**
		 *\~english
		 *\brief		Renders the scene ambient lighting
		 *\param[in]	p_clColour	The light colour
		 *\param[in]	p_pProgram	The ShaderProgram, if any
		 *\~french
		 *\brief		Rend la lumière ambiante de la scène
		 *\param[in]	p_clColour	La couleur de la lumière
		 *\param[in]	p_pProgram	Le ShaderProgram, le cas échéant
		 */
		void RenderAmbientLight( Castor::Colour const & p_clColour, ShaderProgramBase * p_pProgram );
		/**
		 *\~english
		 *\brief		Locks a light index
		 *\remark		8 lights can be rendered at a time on non shader mode
		 *\return		The locked light index, -1 if no more available
		 *\~french
		 *\brief		Verrouille l'index d'une lumière
		 *\remark		8 lumières peuvent être rendues à la fois en mode non shader
		 *\return		L'index verrouillé, -1 s'il n'y en a plus de disponible
		 */
		virtual	int LockLight() = 0;
		/**
		 *\~english
		 *\brief		Unlocks a light index
		 *\param[in]	p_iIndex	The light index
		 *\~french
		 *\brief		Déverrouille un index de lumière
		 *\param[in]	p_iIndex	L'index
		 */
		virtual	void UnlockLight( int p_iIndex ) = 0;
		/**
		 *\~english
		 *\brief		Pushes a scene on th stack
		 *\param[in]	p_pScene	The scene
		 *\~french
		 *\brief		Met une scène sur la pile
		 *\param[in]	p_pScene	La scène
		 */
		void PushScene( Scene * p_pScene );
		/**
		 *\~english
		 *\brief		Pops a scene from the stack
		 *\~french
		 *\brief		Enlève la scène du haut de la pile
		 */
		void PopScene();
		/**
		 *\~english
		 *\brief		Retrieves the top scene from the stack
		 *\return		The scene, NULL if the stack is void
		 *\~french
		 *\brief		Récupère la scène du haut de la pile
		 *\return		La scène, NULL si la pile est vide
		 */
		Scene * GetTopScene();
		/**
		 *\~english
		 *\brief		Retrieves the currently active camera
		 *\return		The camera
		 *\~french
		 *\brief		Récupère la caméra actuellement active
		 *\return		La caméra
		 */
		Camera * GetCurrentCamera()const;
		/**
		 *\~english
		 *\brief		Defines the currently active camera
		 *\param[in]	p_pCamera	The camera
		 *\~french
		 *\brief		Définit la caméra actuellement active
		 *\param[in]	p_pCamera	La caméra
		 */
		void SetCurrentCamera( Camera * p_pCamera );
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
		 *\param[in]	p_eType	The shader type
		 *\~french
		 *\brief		Dit si le RenderSystem supporte le type de shader donné
		 *\param[in]	p_eType	Le type de shader
		 */
		inline bool HasShaderType( eSHADER_TYPE p_eType )const
		{
			return m_useShader[p_eType];
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
		 *\brief		Retrieves the engine
		 *\~french
		 *\brief		Récupère le moteur
		 */
		inline Engine * GetEngine()const
		{
			return m_pEngine;
		}
		/**
		 *\~english
		 *\brief		Retrieves the pipeline
		 *\~french
		 *\brief		Récupère le pipeline
		 */
		inline Pipeline * GetPipeline()const
		{
			return m_pPipeline;
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
		 *\brief		Creates a rendering context
		 *\return		The created context
		 *\~french
		 *\brief		Crée un contexte de rendu
		 *\return		Le contexte créé
		 */
		virtual ContextSPtr CreateContext() = 0;
		/**
		 *\~english
		 *\brief		Creates a FrameVariableBuffer
		 *\return		The created FrameVariableBuffer
		 *\~french
		 *\brief		Crée un FrameVariableBuffer
		 *\return		Le FrameVariableBuffer créé
		 */
		virtual FrameVariableBufferSPtr CreateFrameVariableBuffer( Castor::String const & p_strName ) = 0;
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
		/**
		 *\~english
		 *\brief		Tells if the renderer API supports depth buffer for main FBO
		 *\return		The support status
		 *\~french
		 *\brief		Dit si l'API de rendu supporte les tampons de profondeur pour le FBO principal
		 *\return		Le statut du support
		 */
		virtual bool SupportsDepthBuffer()const = 0;
		/**
		 *\~english
		 *\brief		Create a depth and stencil states object
		 *\return		The object
		 *\~french
		 *\brief		Crée un objet d'états de depth et stencil
		 *\return		L'objet
		 */
		virtual DepthStencilStateSPtr CreateDepthStencilState() = 0;
		/**
		 *\~english
		 *\brief		Create a rasteriser states object
		 *\return		The object
		 *\~french
		 *\brief		Crée un objet d'états de rasteriser
		 *\return		L'objet
		 */
		virtual RasteriserStateSPtr CreateRasteriserState() = 0;
		/**
		 *\~english
		 *\brief		Create a blender states object
		 *\return		The object
		 *\~french
		 *\brief		Crée un objet d'états de blend
		 *\return		L'objet
		 */
		virtual BlendStateSPtr CreateBlendState() = 0;
		/**
		 *\~english
		 *\brief		Create a billboards list
		 *\param[in]	p_pScene		The parent scene
		 *\return		The object
		 *\~french
		 *\brief		Crée une liste de billboards
		 *\param[in]	p_pScene		La scène parente
		 *\return		L'objet
		 */
		virtual BillboardListSPtr CreateBillboardsList( SceneSPtr p_pScene ) = 0;

	protected:
		/**
		 *\~english
		 *\brief		Initialises the render system
		 *\~french
		 *\brief		Initialise le render system
		 */
		virtual void DoInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans the render system up
		 *\~french
		 *\brief		Nettoie le render system
		 */
		virtual void DoCleanup() = 0;
		/**
		 *\~english
		 *\brief		Renders the scene ambient lighting, if it can't be rendered through a shader
		 *\param[in]	p_clColour	The light colour
		 *\~french
		 *\brief		Rend la lumière ambiante de la scène, si elle ne peut être rendue au travers d'un shader
		 *\param[in]	p_clColour	La couleur de la lumière
		 */
		virtual	void DoRenderAmbientLight( Castor::Colour const & p_clColour ) = 0;
		/**
		 *\~english
		 *\brief		Creates a GLSL ShaderProgram
		 *\return		The created ShaderProgram
		 *\~french
		 *\brief		Crée un ShaderProgram GLSL
		 *\return		Le ShaderProgram créé
		 */
		virtual ShaderProgramBaseSPtr DoCreateGlslShaderProgram() = 0;
		/**
		 *\~english
		 *\brief		Creates a HLSL ShaderProgram
		 *\return		The created ShaderProgram
		 *\~french
		 *\brief		Crée un ShaderProgram HLSL
		 *\return		Le ShaderProgram créé
		 */
		virtual ShaderProgramBaseSPtr DoCreateHlslShaderProgram() = 0;
		/**
		 *\~english
		 *\brief		Creates a ShaderProgram (GLSL or HLSL only)
		 *\return		The created ShaderProgram
		 *\~french
		 *\brief		Crée un ShaderProgram (GLSL ou HLSL seulement)
		 *\return		Le ShaderProgram créé
		 */
		virtual ShaderProgramBaseSPtr DoCreateShaderProgram() = 0;
		/**
		 *\~english
		 *\brief		Creates a submesh renderer
		 *\return		The created renderer
		 *\~french
		 *\brief		Crée un renderer de submesh
		 *\return		Le renderer créé
		 */
		virtual SubmeshRendererSPtr DoCreateSubmeshRenderer() = 0;
		/**
		 *\~english
		 *\brief		Creates a texture renderer
		 *\return		The created renderer
		 *\~french
		 *\brief		Crée un renderer de texture
		 *\return		Le renderer créé
		 */
		virtual TextureRendererSPtr DoCreateTextureRenderer() = 0;
		/**
		 *\~english
		 *\brief		Creates a pass renderer
		 *\return		The created renderer
		 *\~french
		 *\brief		Crée un renderer de passe
		 *\return		Le renderer créé
		 */
		virtual PassRendererSPtr DoCreatePassRenderer() = 0;
		/**
		 *\~english
		 *\brief		Creates an overlay renderer
		 *\return		The created renderer
		 *\~french
		 *\brief		Crée un renderer d'incrustations
		 *\return		Le renderer créé
		 */
		virtual OverlayRendererSPtr DoCreateOverlayRenderer() = 0;
		/**
		 *\~english
		 *\brief		Creates a camera renderer
		 *\return		The created renderer
		 *\~french
		 *\brief		Crée un renderer de caméra
		 *\return		Le renderer créé
		 */
		virtual CameraRendererSPtr DoCreateCameraRenderer() = 0;
		/**
		 *\~english
		 *\brief		Creates a light renderer
		 *\return		The created renderer
		 *\~french
		 *\brief		Crée un renderer de lumière
		 *\return		Le renderer créé
		 */
		virtual LightRendererSPtr DoCreateLightRenderer() = 0;
		/**
		 *\~english
		 *\brief		Creates a window renderer
		 *\return		The created renderer
		 *\~french
		 *\brief		Crée un renderer de fenêtre
		 *\return		Le renderer créé
		 */
		virtual WindowRendererSPtr DoCreateWindowRenderer() = 0;
		/**
		 *\~english
		 *\brief		Creates an overlay renderer
		 *\return		The created renderer
		 *\~french
		 *\brief		Crée un renderer d'overlay
		 *\return		Le renderer créé
		 */
		virtual TargetRendererSPtr DoCreateTargetRenderer() = 0;
		/**
		 *\~english
		 *\brief		Creates a sampler renderer
		 *\return		The created renderer
		 *\~french
		 *\brief		Crée un renderer de sampler
		 *\return		Le renderer créé
		 */
		virtual SamplerRendererSPtr DoCreateSamplerRenderer() = 0;
		/**
		 *\~english
		 *\brief		Creates a vertex buffer, given a buffer declaration
		 *\remark		Only the render system can do that
		 *\param[in]	p_elements	The buffer declaration
		 *\param[in]	p_pBuffer	The hardware buffer to which the vertex buffer will be linked
		 *\return		The created vertex buffer, dependant of current API
		 *\~french
		 *\brief		Crée un tampon de sommets, selon une déclaration de tampon
		 *\remark		Seul le render system peut faire ça
		 *\param[in]	p_elements	La déclaration de tampon
		 *\param[in]	p_pBuffer	Le tampon hardware auquel sera lié le tampon de sommets
		 *\return		Le tampon de sommets créé, dépendant de l'API actuelle
		 */
		virtual std::shared_ptr< GpuBuffer< uint8_t > > DoCreateVertexBuffer( BufferDeclaration const & p_elements, CpuBuffer< uint8_t > * p_pBuffer ) = 0;
		/**
		 *\~english
		 *\brief		Creates an index buffer
		 *\remark		Only the render system can do that
		 *\param[in]	p_pBuffer	The hardware buffer to which the index buffer will be linked
		 *\return		The created index buffer, dependant of current API
		 *\~french
		 *\brief		Crée un tampon d'indices
		 *\remark		Seul le render system peut faire ça
		 *\param[in]	p_pBuffer	Le tampon hardware auquel sera lié le tampon d'indices
		 *\return		Le tampon d'indices créé, dépendant de l'API actuelle
		 */
		virtual std::shared_ptr< GpuBuffer< uint32_t > > DoCreateIndexBuffer( CpuBuffer< uint32_t > * p_pBuffer ) = 0;
		/**
		 *\~english
		 *\brief		Creates a matrix buffer
		 *\remark		Only the render system can do that
		 *\param[in]	p_pBuffer	The hardware buffer to which the buffer will be linked
		 *\return		The created buffer, dependant of current API
		 *\~french
		 *\brief		Crée un tampon de matrices
		 *\remark		Seul le render system peut faire ça
		 *\param[in]	p_pBuffer	Le tampon hardware auquel sera lié le tampon
		 *\return		Le tampon créé, dépendant de l'API actuelle
		 */
		virtual std::shared_ptr< GpuBuffer< real > > DoCreateMatrixBuffer( CpuBuffer< real > * p_pBuffer ) = 0;
		/**
		 *\~english
		 *\brief		Creates a texture buffer
		 *\remark		Only the render system can do that
		 *\param[in]	p_pBuffer	The hardware buffer to which the texture buffer will be linked
		 *\return		The created texture buffer, dependant of current API
		 *\~french
		 *\brief		Crée un tampon detexture
		 *\remark		Seul le render system peut faire ça
		 *\param[in]	p_pBuffer	Le tampon hardware auquel sera lié le tampon de texture
		 *\return		Le tampon de texture créé, dépendant de l'API actuelle
		 */
		virtual std::shared_ptr< GpuBuffer< uint8_t > > DoCreateTextureBuffer( CpuBuffer< uint8_t > * p_pBuffer ) = 0;
		/**
		 *\~english
		 *\brief		Creates a texture
		 *\remark		Only the render system can do that
		 *\param[in]	p_eType	The texture type
		 *\return		The created texture, dependant of current API
		 *\~french
		 *\brief		Crée une texture
		 *\remark		Seul le render system peut faire ça
		 *\param[in]	p_eType	Le type de texture
		 *\return		La texture créée, dépendante de l'API actuelle
		 */
		virtual StaticTextureSPtr DoCreateStaticTexture() = 0;
		/**
		 *\~english
		 *\brief		Creates a texture
		 *\remark		Only the render system can do that
		 *\param[in]	p_eType	The texture type
		 *\return		The created texture, dependant of current API
		 *\~french
		 *\brief		Crée une texture
		 *\remark		Seul le render system peut faire ça
		 *\param[in]	p_eType	Le type de texture
		 *\return		La texture créée, dépendante de l'API actuelle
		 */
		virtual DynamicTextureSPtr DoCreateDynamicTexture() = 0;

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
		//!\~english Submesh renderers list	\~french Liste des renderer de submesh
		SubmeshRendererPtrArray m_submeshRenderers;
		//!\~english Texture renderers list	\~french Liste des renderer de texture
		TextureRendererPtrArray m_textureRenderers;
		//!\~english Pass renderers list	\~french Liste des renderer de passe
		PassRendererPtrArray m_passRenderers;
		//!\~english Light renderers list	\~french Liste des renderer de lumière
		LightRendererPtrArray m_lightRenderers;
		//!\~english Window renderers list	\~french Liste des renderer de fenêtre
		WindowRendererPtrArray m_windowRenderers;
		//!\~english Camera renderers list	\~french Liste des renderer de caméra
		CameraRendererPtrArray m_cameraRenderers;
		//!\~english Overlay renderers list	\~french Liste des renderer d'overlay
		OverlayRendererPtrArray m_overlayRenderers;
		//!\~english render target renderers list	\~french Liste des renderer de render target
		TargetRendererPtrArray m_targetRenderers;
		//!\~english Sampler renderers list	\~french Liste des renderer de samplers
		SamplerRendererPtrArray m_arraySamplerRenderers;
		//!\~english Destroyable submesh renderers list	\~french Liste des renderer de submesh à détruire
		SubmeshRendererPtrArray m_submeshRenderersToCleanup;
		//!\~english Destroyable texture renderers list	\~french Liste des renderer de texture à détruire
		TextureRendererPtrArray m_textureRenderersToCleanup;
		//!\~english Destroyable pass renderers list	\~french Liste des renderer de passe à détruire
		PassRendererPtrArray m_passRenderersToCleanup;
		//!\~english Destroyable light renderers list	\~french Liste des renderer de lumière à détruire
		LightRendererPtrArray m_lightRenderersToCleanup;
		//!\~english Destroyable window renderers list	\~french Liste des renderer de fenêtre à détruire
		WindowRendererPtrArray m_windowRenderersToCleanup;
		//!\~english Destroyable camera renderers list	\~french Liste des renderer de caméra à détruire
		CameraRendererPtrArray m_cameraRenderersToCleanup;
		//!\~english Destroyable overlay renderers list	\~french Liste des renderer d'overlay à détruire
		OverlayRendererPtrArray m_overlayRenderersToCleanup;
		//!\~english Destroyable render target renderers list	\~french Liste des renderer de render target à détruire
		TargetRendererPtrArray m_targetRenderersToCleanup;
		//!\~english Destroyable sampler renderers list	\~french Liste des renderer de samplers à détruire
		SamplerRendererPtrArray m_arraySamplerRenderersToCleanup;
		//!\~english The overlay renderer	\~french Le renderer d'overlays
		OverlayRendererSPtr m_overlayRenderer;
		//!\~english The main render context	\~french Le contexte de rendu principal
		ContextWPtr m_wpMainContext;
		//!\~english The currently active render context	\~french Le contexte de rendu actuellement actif
		ContextRPtr m_pCurrentContext;
		//!\~english The core engine	\~french Le moteur
		EngineRPtr m_pEngine;
		//!\~english The matrix pipeline	\~french Le pipeline contenant les matrices
		Pipeline * m_pPipeline;
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
	};

#include "RenderSystem.inl"
}

#pragma warning( pop )

#endif
