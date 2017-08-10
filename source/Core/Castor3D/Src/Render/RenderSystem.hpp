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
#ifndef ___C3D_RENDER_SYSTEM_H___
#define ___C3D_RENDER_SYSTEM_H___

#include "Miscellaneous/GpuInformations.hpp"
#include "Miscellaneous/GpuObjectTracker.hpp"

#include <stack>

#include <Design/OwnedBy.hpp>

namespace castor3d
{
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
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	p_name		The renderer name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	p_name		Le nom du renderer.
		 */
		C3D_API RenderSystem( Engine & engine, castor::String const & p_name );
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
		C3D_API void initialise( GpuInformations && p_informations );
		/**
		 *\~english
		 *\brief		Cleans the render system up
		 *\~french
		 *\brief		Nettoie le render system
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Pushes a scene on the stack
		 *\param[in]	p_scene	The scene
		 *\~french
		 *\brief		Met une scène sur la pile
		 *\param[in]	p_scene	La scène
		 */
		C3D_API void pushScene( Scene * p_scene );
		/**
		 *\~english
		 *\brief		Pops a scene from the stack
		 *\~french
		 *\brief		Enlève la scène du haut de la pile
		 */
		C3D_API void popScene();
		/**
		 *\~english
		 *\brief		Retrieves the top scene from the stack
		 *\return		The scene, nullptr if the stack is void
		 *\~french
		 *\brief		Récupère la scène du haut de la pile
		 *\return		La scène, nullptr si la pile est vide
		 */
		C3D_API Scene * getTopScene();
		/**
		 *\~english
		 *\return		A pre-configured GlslWriter instance.
		 *\~french
		 *\brief		Une instance pré-configurée de GlslWriter.
		 */
		C3D_API GLSL::GlslWriter createGlslWriter();
		/**
		 *\~english
		 *\brief		sets the currently active render context
		 *\param[in]	p_context	The context
		 *\~french
		 *\brief		Définit le contexte de rendu actuellement actif
		 *\param[in]	p_context	Le contexte
		 */
		C3D_API void setCurrentContext( Context * p_context );
		/**
		 *\~english
		 *\brief		Retrieves the currently active render context
		 *\return		The context
		 *\~french
		 *\brief		Récupère le contexte de rendu actuellement actif
		 *\return		Le contexte
		 */
		C3D_API Context * getCurrentContext();
		/**
		 *\~english
		 *\return		The GPU informations.
		 *\~french
		 *\return		Les informations sur le GPU.
		 */
		inline GpuInformations const & getGpuInformations()const
		{
			return m_gpuInformations;
		}
		/**
		 *\~english
		 *\brief		Tells if the RenderSystem is initialised
		 *\~french
		 *\brief		Dit si le RenderSystem est initialisé
		 */
		inline bool isInitialised()const
		{
			return m_initialised;
		}
		/**
		 *\~english
		 *\brief		Retrieves the renderer API
		 *\~french
		 *\brief		Récupère l'API de rendu
		 */
		inline castor::String const & getRendererType()const
		{
			return m_name;
		}
		/**
		 *\~english
		 *\brief		sets the main render context
		 *\param[in]	p_context	The context
		 *\~french
		 *\brief		Définit le contexte de rendu principal
		 *\param[in]	p_context	Le contexte
		 */
		inline void setMainContext( ContextSPtr p_context )
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
		inline ContextSPtr getMainContext()
		{
			return m_mainContext;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay renderer
		 *\return		The value
		 *\~french
		 *\brief		Récupère le renderer d'overlays
		 *\return		La valeur
		 */
		inline OverlayRendererSPtr getOverlayRenderer()
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
		inline void incGpuTime( std::chrono::duration< Rep, Period > const & p_time )
		{
			m_gpuTime += std::chrono::duration_cast< castor::Nanoseconds >( p_time );
		}
		/**
		 *\~english
		 *\brief		Resets the GPU time.
		 *\~french
		 *\brief		Réinitialise le temps CPU.
		 */
		inline void resetGpuTime()
		{
			m_gpuTime = castor::Nanoseconds( 0 );
		}
		/**
		 *\~english
		 *\brief		Retrieves the GPU time.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le temps CPU.
		 *\return		La valeur.
		 */
		inline castor::Nanoseconds const & getGpuTime()const
		{
			return m_gpuTime;
		}
		/**
		 *\~english
		 *\brief		Creates a ShaderProgram.
		 *\return		The created ShaderProgram.
		 *\~french
		 *\brief		Crée un ShaderProgram.
		 *\return		Le ShaderProgram créé.
		 */
		C3D_API virtual ShaderProgramSPtr createShaderProgram() = 0;
		/**
		 *\~english
		 *\brief		Creates a UniformBufferBinding.
		 *\param[in]	p_ubo		The parent uniform buffer.
		 *\param[in]	p_program	The parent program.
		 *\return		The created UniformBufferBinding.
		 *\~french
		 *\brief		Crée un UniformBufferBinding.
		 *\param[in]	p_ubo		le tampon d'uniformes parent.
		 *\param[in]	p_program	Le programme parent.
		 *\return		Le UniformBufferBinding créé.
		 */
		C3D_API virtual UniformBufferBindingUPtr createUniformBufferBinding( UniformBuffer & p_ubo
			, ShaderProgram const & p_program ) = 0;
		/**
		 *\~english
		 *\brief		Creates a geometries buffer holder.
		 *\param[in]	p_topology	The buffers topology.
		 *\param[in]	p_program	The shader program.
		 *\~french
		 *\brief		Crée un conteneur de buffers de géométrie.
		 *\param[in]	p_topology	La topologie des tampons.
		 *\param[in]	p_program	Le programme shader.
		 */
		C3D_API virtual GeometryBuffersSPtr createGeometryBuffers( Topology p_topology
			, ShaderProgram const & p_program ) = 0;
		/**
		 *\~english
		 *\brief		Creates a rendering context
		 *\return		The created context
		 *\~french
		 *\brief		Crée un contexte de rendu
		 *\return		Le contexte créé
		 */
		C3D_API virtual ContextSPtr createContext() = 0;
		/**
		 *\~english
		 *\brief		create a render pipeline.
		 *\param[in]	p_dsState	The depth stencil state.
		 *\param[in]	p_rsState	The rateriser state.
		 *\param[in]	p_bdState	The blend state.
		 *\param[in]	p_msState	The multisample state.
		 *\param[in]	p_program	The shader program.
		 *\param[in]	p_flags		The creation flags.
		 *\return		The pipeline.
		 *\~french
		 *\brief		Crée un pipeline de rendu.
		 *\param[in]	p_dsState	L'état de stencil et profondeur.
		 *\param[in]	p_rsState	L'état de rastériseur.
		 *\param[in]	p_bdState	L'état de mélange.
		 *\param[in]	p_msState	L'état de multi-échantillonnage.
		 *\param[in]	p_program	Le programme shader.
		 *\param[in]	p_flags		Les indicateurs de création.
		 *\return		Le pipeline.
		 */
		C3D_API virtual RenderPipelineUPtr createRenderPipeline( DepthStencilState && p_dsState
			, RasteriserState && p_rsState
			, BlendState && p_bdState
			, MultisampleState && p_msState
			, ShaderProgram & p_program
			, PipelineFlags const & p_flags ) = 0;
		/**
		 *\~english
		 *\brief		Creates a compute pipeline.
		 *\param[in]	p_program	The shader program.
		 *\return		The pipeline.
		 *\~french
		 *\brief		Crée un pipeline de pipeline.
		 *\param[in]	p_program	Le programme shader.
		 *\return		Le pipeline.
		 */
		C3D_API virtual ComputePipelineUPtr createComputePipeline( ShaderProgram & p_program ) = 0;
		/**
		 *\~english
		 *\brief		create a sampler
		 *\param[in]	p_name	The sampler name
		 *\return		The object
		 *\~french
		 *\brief		Crée un échantillonneur
		 *\param[in]	p_name	Le nom de l'échantillonneur
		 *\return		L'objet
		 */
		C3D_API virtual SamplerSPtr createSampler( castor::String const & p_name ) = 0;
		/**
		 *\~english
		 *\brief		Creates a texture.
		 *\param[in]	p_type		The texture type.
		 *\param[in]	p_cpuAccess	The required CPU access (combination of AccessType).
		 *\param[in]	p_gpuAccess	The required GPU access (combination of AccessType).
		 *\return		The created texture, depending of current API.
		 *\~french
		 *\brief		Crée une texture.
		 *\param[in]	p_type		Le type de texture.
		 *\param[in]	p_cpuAccess	Les accès requis pour le CPU (combinaison de AccessType).
		 *\param[in]	p_gpuAccess	Les accès requis pour le GPU (combinaison de AccessType).
		 *\return		La texture créée, dépendante de l'API actuelle.
		 */
		C3D_API virtual TextureLayoutSPtr createTexture( TextureType p_type
			, AccessTypes const & p_cpuAccess
			, AccessTypes const & p_gpuAccess ) = 0;
		/**
		 *\~english
		 *\brief		Creates a texture.
		 *\param[in]	p_type		The texture type.
		 *\param[in]	p_cpuAccess	The required CPU access (combination of AccessType).
		 *\param[in]	p_gpuAccess	The required GPU access (combination of AccessType).
		 *\return		The created texture, depending of current API.
		 *\param[in]	p_format	The texture format.
		 *\param[in]	p_size		The texture dimensions.
		 *\~french
		 *\brief		Crée une texture.
		 *\param[in]	p_type		Le type de texture.
		 *\param[in]	p_cpuAccess	Les accès requis pour le CPU (combinaison de AccessType).
		 *\param[in]	p_gpuAccess	Les accès requis pour le GPU (combinaison de AccessType).
		 *\param[in]	p_format	Le format de la texture.
		 *\param[in]	p_size		Les dimensions de la texture.
		 *\return		La texture créée, dépendante de l'API actuelle.
		 */
		C3D_API virtual TextureLayoutSPtr createTexture( TextureType p_type
			, AccessTypes const & p_cpuAccess
			, AccessTypes const & p_gpuAccess
			, castor::PixelFormat p_format
			, castor::Size const & p_size ) = 0;
		/**
		 *\~english
		 *\brief		Creates a texture.
		 *\param[in]	p_type		The texture type.
		 *\param[in]	p_cpuAccess	The required CPU access (combination of AccessType).
		 *\param[in]	p_gpuAccess	The required GPU access (combination of AccessType).
		 *\return		The created texture, depending of current API.
		 *\param[in]	p_format	The texture format.
		 *\param[in]	p_size		The texture dimensions.
		 *\~french
		 *\brief		Crée une texture.
		 *\param[in]	p_type		Le type de texture.
		 *\param[in]	p_cpuAccess	Les accès requis pour le CPU (combinaison de AccessType).
		 *\param[in]	p_gpuAccess	Les accès requis pour le GPU (combinaison de AccessType).
		 *\param[in]	p_format	Le format de la texture.
		 *\param[in]	p_size		Les dimensions de la texture.
		 *\return		La texture créée, dépendante de l'API actuelle.
		 */
		C3D_API virtual TextureLayoutSPtr createTexture( TextureType p_type
			, AccessTypes const & p_cpuAccess
			, AccessTypes const & p_gpuAccess
			, castor::PixelFormat p_format
			, castor::Point3ui const & p_size ) = 0;
		/**
		 *\~english
		 *\brief		Creates a texture storage.
		 *\param[in]	p_type		The storage type.
		 *\param[in]	p_layout	The texture layout.
		 *\param[in]	p_cpuAccess	The required CPU access (combination of AccessType).
		 *\param[in]	p_gpuAccess	The required GPU access (combination of AccessType).
		 *\return		The created storage, depending on current API.
		 *\~french
		 *\brief		Crée un stockage de texture.
		 *\param[in]	p_type		Le type de stockage.
		 *\param[in]	p_layout	Le layout de la texture.
		 *\param[in]	p_cpuAccess	Les accès requis pour le CPU (combinaison de AccessType).
		 *\param[in]	p_gpuAccess	Les accès requis pour le GPU (combinaison de AccessType).
		 *\return		Le stockage créé, dépendant de l'API actuelle.
		 */
		C3D_API virtual TextureStorageUPtr createTextureStorage( TextureStorageType p_type
			, TextureLayout & p_layout
			, AccessTypes const & p_cpuAccess
			, AccessTypes const & p_gpuAccess ) = 0;
		/**
		 *\~english
		 *\brief		Creates a GPU buffer.
		 *\param[in]	p_type	The buffer type.
		 *\return		The created buffer, depending on current API.
		 *\~french
		 *\brief		Crée un tampon GPU.
		 *\param[in]	p_type	Le type de tampon.
		 *\return		Le tampon créé, dépendant de l'API actuelle.
		 */
		C3D_API virtual GpuBufferUPtr createBuffer( BufferType p_type ) = 0;
		/**
		 *\~english
		 *\brief		Creates a transform feedback instance.
		 *\param[in]	p_computed	The computed elements description.
		 *\param[in]	p_topology	The topology.
		 *\param[in]	p_program	The shader program.
		 *\return		The created transform feedback, depending on current API.
		 *\~french
		 *\brief		Crée une instance de transform feedback.
		 *\param[in]	p_computed	La description des éléments calculés.
		 *\param[in]	p_topology	La topologie.
		 *\param[in]	p_program	Le programm shader.
		 *\return		Le tampon de transform feedback créé, dépendant de l'API actuelle.
		 */
		C3D_API virtual TransformFeedbackUPtr createTransformFeedback( BufferDeclaration const & p_computed
			, Topology p_topology
			, ShaderProgram & p_program ) = 0;
		/**
		 *\~english
		 *\brief		Creates a frame buffer.
		 *\return		The created frame buffer.
		 *\~french
		 *\brief		Crée un tampon d'image.
		 *\return		Le tampon d'image créé.
		 */
		C3D_API virtual FrameBufferSPtr createFrameBuffer() = 0;
		/**
		 *\~english
		 *\brief		Creates the window back buffers.
		 *\return		The created back buffers.
		 *\~french
		 *\brief		Crée les tampons d'image de la fenêtre.
		 *\return		Les tampons d'image créés.
		 */
		C3D_API virtual BackBuffersSPtr createBackBuffers() = 0;
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
		C3D_API virtual GpuQueryUPtr createQuery( QueryType p_type ) = 0;
		/**
		 *\~english
		 *\brief		Creates a viewport render API specific implementation.
		 *\param[in]	p_viewport	The parent vieport.
		 *\return		The created implementation.
		 *\~french
		 *\brief		Crée une implémentation de viewport spécifique à l'API de rendu.
		 *\param[in]	p_viewport	Le voewport parent.
		 *\return		L'implémentation créée.
		 */
		C3D_API virtual IViewportImplUPtr createViewport( Viewport & p_viewport ) = 0;

	protected:
		/**
		 *\~english
		 *\brief		Initialises the render system
		 *\~french
		 *\brief		Initialise le render system
		 */
		C3D_API virtual void doInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans the render system up
		 *\~french
		 *\brief		Nettoie le render system
		 */
		C3D_API virtual void doCleanup() = 0;

	protected:
		//!\~english	Mutex used to make this class thread safe.
		//!\~french		Mutex pour rendre cette classe thread safe.
		std::recursive_mutex m_mutex;
		//!\~english	Tells whether or not it is initialised.
		//!\~french		Dit si le render system est initialisé.
		bool m_initialised;
		//!\~english	The GPU informations.
		//!\~french		Les informations sur le GPU.
		GpuInformations m_gpuInformations;
		//!\~english	The overlay renderer.
		//!\~french		Le renderer d'overlays.
		OverlayRendererSPtr m_overlayRenderer;
		//!\~english	The main render context.
		//!\~french		Le contexte de rendu principal.
		ContextSPtr m_mainContext;
		//!\~english	The currently active render context.
		//!\~french		Le contexte de rendu actuellement actif.
		std::map< std::thread::id, ContextRPtr > m_currentContexts;
		//!\~english	Scene stack.
		//!\~french		Pile des scènes.
		std::stack< SceneRPtr > m_stackScenes;
		//!\~english	The current loaded renderer api type.
		//!\~french		Le type de l'api de rendu actuellement chargée.
		castor::String m_name;
		//!\~english	The time spent on GPU for current frame.
		//!\~french		Le temps passé sur le GPU pour l'image courante.
		castor::Nanoseconds m_gpuTime;

#if C3D_TRACE_OBJECTS

		GpuObjectTracker m_tracker;

#endif
	};
}

#endif
