/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RENDER_SYSTEM_H___
#define ___C3D_RENDER_SYSTEM_H___

#include "Miscellaneous/GpuInformations.hpp"
#include "Miscellaneous/GpuObjectTracker.hpp"
#include "Buffer/GpuBufferPool.hpp"

#include <stack>

#include <Design/OwnedBy.hpp>

#include <Miscellaneous/PhysicalDeviceFeatures.hpp>
#include <Miscellaneous/PhysicalDeviceMemoryProperties.hpp>
#include <Miscellaneous/PhysicalDeviceProperties.hpp>

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
		C3D_API RenderSystem( Engine & engine
			, castor::String const & name
			, bool topDown );
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
		 *\brief		Connects to the device onEnabled/onDisabled signals.
		 *\param[in]	device	The device to register.
		 *\~french
		 *\brief		Se connecte aux signaux onEnablle/onDisable du périphérique.
		 *\param[in]	device	Le périphérique à enregistrer.
		 */
		C3D_API void registerDevice( renderer::Device & device );
		/**
		 *\~english
		 *\brief		Disconnects from the device onEnabled/onDisabled signals.
		 *\param[in]	device	The device to unregister.
		 *\~french
		 *\brief		Se déconnecte du signaux onEnablle/onDisable du périphérique.
		 *\param[in]	device	Le périphérique à désenregistrer.
		 */
		C3D_API void unregisterDevice( renderer::Device & device );
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
		C3D_API Scene * getTopScene()const;
		/**
		 *\~english
		 *\return		A pre-configured GlslWriter instance.
		 *\~french
		 *\brief		Une instance pré-configurée de GlslWriter.
		 */
		C3D_API virtual glsl::GlslWriter createGlslWriter();
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
		 *\brief		Creates a logical device bound to a physical GPU.
		 *\param[in]	gpu	The GPU index.
		 *\return		The created device.
		 *\~french
		 *\brief		Crée un périphérique logique lié à un GPU physique.
		 *\param[in]	gpu	L'indice du GPU.
		 *\return		Le périphérique logique créé.
		 */
		C3D_API renderer::DevicePtr createDevice( renderer::WindowHandle && handle
			, uint32_t gpu = 0u );
		/**
		*\~english
		*\brief
		*	Computes an frustum projection matrix.
		*\param[in] left, right
		*	The left and right planes position.
		*\param[in] top, bottom
		*	The top and bottom planes position.
		*\param[in] zNear, zFar
		*	The near and far planes position.
		*\~french
		*\brief
		*	Calcule une matrice de projection frustum.
		*\param[in] left, right
		*	La position des plans gauche et droite.
		*\param[in] top, bottom
		*	La position des plans haut et bas.
		*\param[in] zNear, zFar
		*	La position des premier et arrière plans.
		*/
		C3D_API castor::Matrix4x4r getFrustum( float left
			, float right
			, float bottom
			, float top
			, float zNear
			, float zFar )const;
		/**
		*\~english
		*	Computes a perspective projection matrix.
		*\param[in] fovy
		*	The vertical aperture angle.
		*\param[in] aspect
		*	The width / height ratio.
		*\param[in] zNear
		*	The near plane position.
		*\param[in] zFar
		*	The far plane position.
		*\~french
		*\brief
		*	Calcule une matrice de projection en perspective.
		*\param[in] fovy
		*	L'angle d'ouverture verticale.
		*\param[in] aspect
		*	Le ratio largeur / hauteur.
		*\param[in] zNear
		*	La position du premier plan (pour le clipping).
		*\param[in] zFar
		*	La position de l'arrière plan (pour le clipping).
		*/
		C3D_API castor::Matrix4x4r getPerspective( float radiansFovY
			, float aspect
			, float zNear
			, float zFar )const;
		/**
		*\~english
		*\brief
		*	Computes an orthographic projection matrix.
		*\param[in] left, right
		*	The left and right planes position.
		*\param[in] top, bottom
		*	The top and bottom planes position.
		*\param[in] zNear, zFar
		*	The near and far planes position.
		*\~french
		*\brief
		*	Calcule une matrice de projection orthographique.
		*\param[in] left, right
		*	La position des plans gauche et droite.
		*\param[in] top, bottom
		*	La position des plans haut et bas.
		*\param[in] zNear, zFar
		*	La position des premier et arrière plans.
		*/
		C3D_API castor::Matrix4x4r getOrtho( float left
			, float right
			, float bottom
			, float top
			, float zNear
			, float zFar )const;
		/**
		*\~english
		*	Computes a perspective projection matrix with no far plane clipping.
		*\param[in] fovy
		*	The vertical aperture angle.
		*\param[in] aspect
		*	The width / height ratio.
		*\param[in] zNear
		*	The near plane position.
		*\~french
		*\brief
		*	Calcule une matrice de projection en perspective sans clipping
		*	d'arrière plan.
		*\param[in] fovy
		*	L'angle d'ouverture verticale.
		*\param[in] aspect
		*	Le ratio largeur / hauteur.
		*\param[in] zNear
		*	La position du premier plan (pour le clipping).
		*/
		C3D_API castor::Matrix4x4r getInfinitePerspective( float radiansFovY
			, float aspect
			, float zNear )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline renderer::PhysicalDeviceProperties const & getProperties()const
		{
			return m_properties;
		}

		inline renderer::PhysicalDeviceMemoryProperties const & getMemoryProperties()const
		{
			return m_memoryProperties;
		}

		inline renderer::PhysicalDeviceFeatures const & getFeatures()const
		{
			return m_features;
		}

		inline renderer::Device const * getCurrentDevice()const
		{
			REQUIRE( m_currentDevice );
			return m_currentDevice;
		}

		inline bool hasCurrentDevice()const
		{
			return m_currentDevice != nullptr;
		}

		inline GpuInformations const & getGpuInformations()const
		{
			return m_gpuInformations;
		}

		inline bool isInitialised()const
		{
			return m_initialised;
		}

		inline castor::String const & getRendererType()const
		{
			return m_name;
		}

		inline bool hasMainDevice()
		{
			return m_mainDevice != nullptr;
		}

		inline renderer::DevicePtr getMainDevice()
		{
			REQUIRE( hasMainDevice() );
			return m_mainDevice;
		}

		inline OverlayRendererSPtr getOverlayRenderer()
		{
			return m_overlayRenderer;
		}

		inline castor::Nanoseconds const & getGpuTime()const
		{
			return m_gpuTime;
		}

		inline bool isTopDown()const
		{
			return m_topDown;
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		inline void setMainDevice( renderer::DevicePtr device )
		{
			m_mainDevice = device;
		}

		template< class Rep, class Period >
		inline void incGpuTime( std::chrono::duration< Rep, Period > const & time )
		{
			m_gpuTime += std::chrono::duration_cast< castor::Nanoseconds >( time );
		}

		inline void resetGpuTime()
		{
			m_gpuTime = castor::Nanoseconds( 0 );
		}
		/**@}*/

	protected:
		std::recursive_mutex m_mutex;
		bool m_initialised;
		bool const m_topDown;
		GpuInformations m_gpuInformations;
		OverlayRendererSPtr m_overlayRenderer;
		renderer::RendererPtr m_renderer;
		renderer::DevicePtr m_mainDevice;
		renderer::Device const * m_currentDevice{ nullptr };
		std::stack< SceneRPtr > m_stackScenes;
		castor::String m_name;
		castor::Nanoseconds m_gpuTime;
		GpuBufferPool m_gpuBufferPool;
		std::map< renderer::Device *, renderer::DeviceEnabledConnection > m_deviceEnabledConnections;
		std::map< renderer::Device *, renderer::DeviceDisabledConnection > m_deviceDisabledConnections;
		renderer::PhysicalDeviceMemoryProperties m_memoryProperties{};
		renderer::PhysicalDeviceFeatures m_features{};
		renderer::PhysicalDeviceProperties m_properties{};

#if C3D_TRACE_OBJECTS

		GpuObjectTracker m_tracker;

#endif
	};
}

#endif
