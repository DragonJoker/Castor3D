/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RENDER_SYSTEM_H___
#define ___C3D_RENDER_SYSTEM_H___

#include "Miscellaneous/GpuInformations.hpp"
#include "Miscellaneous/GpuObjectTracker.hpp"
#include "Mesh/Buffer/GpuBufferPool.hpp"

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
		friend class GpuBufferPool;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	name	The renderer name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	name	Le nom du renderer.
		 */
		C3D_API RenderSystem( Engine & engine, castor::String const & name );
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
		C3D_API void initialise( GpuInformations && informations );
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
		 *\param[in]	scene	The scene
		 *\~french
		 *\brief		Met une scène sur la pile
		 *\param[in]	scene	La scène
		 */
		C3D_API void pushScene( Scene * scene );
		/**
		 *\~english
		 *\brief		Pops a scene from the stack
		 *\~french
		 *\brief		Enlève la scène du haut de la pile
		 */
		C3D_API void popScene();
		/**
		 *\~english
		 *\return		The top scene from the stack, nullptr if the stack is empty.
		 *\~french
		 *\return		La scène du haut de la pile, nullptr si la pile est vide.
		 */
		C3D_API Scene * getTopScene();
		/**
		 *\~english
		 *\return		A pre-configured GlslWriter instance.
		 *\~french
		 *\brief		Une instance pré-configurée de GlslWriter.
		 */
		C3D_API glsl::GlslWriter createGlslWriter();
		/**
		 *\~english
		 *\brief		Sets the currently active render context
		 *\param[in]	context	The context
		 *\~french
		 *\brief		Définit le contexte de rendu actuellement actif
		 *\param[in]	context	Le contexte
		 */
		C3D_API void setCurrentContext( Context * context );
		/**
		 *\~english
		 *\return		The currently active render context.
		 *\~french
		 *\return		Le contexte de rendu actuellement actif.
		 */
		C3D_API Context * getCurrentContext();
		/**
		 *\~english
		 *\brief		Retrieves a GPU buffer with the given size.
		 *\param[in]	target	The buffer type.
		 *\param[in]	size	The wanted buffer size.
		 *\param[in]	flags	The buffer memory flags.
		 *\return		The created buffer, depending on current API.
		 *\~french
		 *\brief		Récupère un tampon GPU avec la taille donnée.
		 *\param[in]	target	Le type de tampon.
		 *\param[in]	size	La taille voulue pour le tampon.
		 *\param[in]	flags	Les indicateurs de mémoire du tampon.
		 *\return		Le tampon créé.
		 */
		C3D_API GpuBufferOffset getBuffer( renderer::BufferTarget target
			, uint32_t size
			, renderer::MemoryPropertyFlags flags );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	target			The buffer type.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	target			Le type de tampon.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		C3D_API void putBuffer( renderer::BufferTarget target
			, GpuBufferOffset const & bufferOffset );
		/**
		 *\~english
		 *\brief		Cleans up the buffer pool.
		 *\~french
		 *\brief		Nettoie le pool de tampons.
		 */
		C3D_API void cleanupPool();
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
		 *\brief		Sets the main render context
		 *\param[in]	context	The context
		 *\~french
		 *\brief		Définit le contexte de rendu principal
		 *\param[in]	context	Le contexte
		 */
		inline void setMainContext( ContextSPtr context )
		{
			m_mainContext = context;
		}
		/**
		 *\~english
		 *\return		The main render context.
		 *\~french
		 *\return		Le contexte de rendu principal.
		 */
		inline ContextSPtr getMainContext()
		{
			return m_mainContext;
		}
		/**
		 *\~english
		 *\return		The overlay renderer.
		 *\~french
		 *\return		Le renderer d'overlays.
		 */
		inline OverlayRendererSPtr getOverlayRenderer()
		{
			return m_overlayRenderer;
		}
		/**
		 *\~english
		 *\brief		Increments the GPU time.
		 *\param[in]	time	The increment value.
		 *\~french
		 *\brief		Incrémente le temps CPU.
		 *\param[in]	time	La valeur d'incrément.
		 */
		template< class Rep, class Period >
		inline void incGpuTime( std::chrono::duration< Rep, Period > const & time )
		{
			m_gpuTime += std::chrono::duration_cast< castor::Nanoseconds >( time );
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
		 *\return		The GPU time.
		 *\~french
		 *\return		Le temps CPU.
		 */
		inline castor::Nanoseconds const & getGpuTime()const
		{
			return m_gpuTime;
		}
		/**
		 *\~english
		 *\brief		Creates a rendering context
		 *\return		The created context
		 *\~french
		 *\brief		Crée un contexte de rendu
		 *\return		Le contexte créé
		 */
		C3D_API virtual ContextSPtr createContext() = 0;

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
		//!\~english	The GPU buffer pool.
		//!\~french		Le pool de tampons GPU.
		GpuBufferPool m_gpuBufferPool;

#if C3D_TRACE_OBJECTS

		GpuObjectTracker m_tracker;

#endif
	};
}

#endif
