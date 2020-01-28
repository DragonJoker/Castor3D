/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderSystem_H___
#define ___C3D_RenderSystem_H___

#include "Castor3D/Miscellaneous/DebugCallbacks.hpp"
#include "Castor3D/Miscellaneous/GpuInformations.hpp"
#include "Castor3D/Miscellaneous/GpuObjectTracker.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Render/RenderDevice.hpp"

#include <stack>

#include <CastorUtils/Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\~english
	\brief		This class links Castor3D to the rendering API.
	\~french
	\brief		Cette classe fait le lien entre Castor3D et l'API de rendu.
	*/
	class RenderSystem
		: public castor::OwnedBy< Engine >
	{
		friend class GpuBufferPool;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	desc			The Ashes plugin description.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	desc			The Ashes plugin description.
		 */
		C3D_API RenderSystem( Engine & engine
			, AshPluginDescription desc );
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
		 *\param[in]	informations	The GPU informations.
		 *\~french
		 *\brief		Initialise le render system
		 *\param[in]	informations	Les informations deu GPU.
		 */
		C3D_API void initialise( GpuInformations informations );
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
		C3D_API Scene * getTopScene()const;
		/**
		 *\~english
		 *\brief		Compiles a shader module to the necessary shader language.
		 *\param[in]	module	The shader to compile.
		 *\return		The compiled shader.
		 *\~french
		 *\brief		Compile un shader dans le langage shader nécessaire.
		 *\param[in]	module	Le shader à compiler.
		 *\return		Le shader compilé.
		 */
		C3D_API SpirVShader compileShader( ShaderModule const & module )const;
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
		C3D_API GpuBufferOffset getBuffer( VkBufferUsageFlagBits target
			, uint32_t size
			, VkMemoryPropertyFlags flags );
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
		C3D_API void putBuffer( VkBufferUsageFlagBits target
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
		 *\param[in]	handle	The native window handle.
		 *\param[in]	gpu		The GPU index.
		 *\return		The created device.
		 *\~french
		 *\brief		Crée un périphérique logique lié à un GPU physique.
		 *\param[in]	handle	Le handle de la fenêtre native.
		 *\param[in]	gpu		L'indice du GPU.
		 *\return		Le périphérique logique créé.
		 */
		C3D_API RenderDeviceSPtr createDevice( ashes::WindowHandle handle
			, uint32_t gpuIndex = 0u );
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
		C3D_API castor::Matrix4x4f getFrustum( float left
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
		C3D_API castor::Matrix4x4f getPerspective( castor::Angle const & fovy
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
		C3D_API castor::Matrix4x4f getOrtho( float left
			, float right
			, float bottom
			, float top
			, float zNear
			, float zFar )const;
		/**
		*\~english
		*	Computes a perspective projection matrix with no far plane clipping.
		*\param[in] radiansFovY
		*	The vertical aperture angle.
		*\param[in] aspect
		*	The width / height ratio.
		*\param[in] zNear
		*	The near plane position.
		*\~french
		*\brief
		*	Calcule une matrice de projection en perspective sans clipping
		*	d'arrière plan.
		*\param[in] radiansFovY
		*	L'angle d'ouverture verticale.
		*\param[in] aspect
		*	Le ratio largeur / hauteur.
		*\param[in] zNear
		*	La position du premier plan (pour le clipping).
		*/
		C3D_API castor::Matrix4x4f getInfinitePerspective( float radiansFovY
			, float aspect
			, float zNear )const;
		/**
		*\~french
		*\brief
		*	Ajoute les couches de l'instance aux noms d�j� pr�sents dans la liste donn�e.
		*\param[in,out] names
		*	La liste � compl�ter.
		*\~english
		*\brief
		*	Adds the instance layers names to the given names.
		*\param[in,out] names
		*	The liste to fill.
		*/
		void completeLayerNames( ashes::StringArray & names )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline RenderDevice const & getCurrentRenderDevice()const
		{
			CU_Require( m_currentDevice );
			return *m_currentDevice;
		}

		inline bool hasCurrentRenderDevice()const
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

		inline castor::String getRendererType()const
		{
			return m_desc.name;
		}

		inline bool hasMainDevice()const
		{
			return m_mainDevice != nullptr;
		}

		inline RenderDeviceSPtr getMainRenderDevice()const
		{
			CU_Require( hasMainDevice() );
			return m_mainDevice;
		}

		inline OverlayRendererSPtr getOverlayRenderer()const
		{
			return m_overlayRenderer;
		}

		inline castor::Nanoseconds const & getGpuTime()const
		{
			return m_gpuTime;
		}

		inline bool hasSsbo()const
		{
			return m_desc.features.hasStorageBuffers;
		}

		inline ashes::Instance const & getInstance()const
		{
			return *m_instance;
		}

		inline ashes::StringArray const & getLayerNames()const
		{
			return m_layerNames;
		}

		inline ashes::StringArray const & getExtensionNames()const
		{
			return m_extensionNames;
		}

		inline VkPhysicalDeviceProperties const & getProperties()const
		{
			return m_properties;
		}

		inline VkPhysicalDeviceMemoryProperties const & getMemoryProperties()const
		{
			return m_memoryProperties;
		}

		inline VkPhysicalDeviceFeatures const & getFeatures()const
		{
			return m_features;
		}

		inline AshPluginDescription const & getDescription()const
		{
			return m_desc;
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
		inline void setMainDevice( RenderDeviceSPtr device )
		{
			m_mainDevice = device;
		}

		inline void setCurrentRenderDevice( RenderDevice const * device )
		{
			m_currentDevice = device;
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

	private:
		std::recursive_mutex m_mutex;
		bool m_initialised;
		AshPluginDescription const m_desc;
		GpuInformations m_gpuInformations;
		OverlayRendererSPtr m_overlayRenderer;
		ashes::InstancePtr m_instance;
		DebugCallbacksPtr m_debug;
		ashes::PhysicalDevicePtrArray m_gpus;
		VkPhysicalDeviceMemoryProperties m_memoryProperties;
		VkPhysicalDeviceProperties m_properties;
		VkPhysicalDeviceFeatures m_features;
		uint32_t presentQueueFamilyIndex;
		uint32_t graphicsQueueFamilyIndex;
		uint32_t computeQueueFamilyIndex;
		uint32_t transferQueueFamilyIndex;
		VkLayerProperties m_globalLayer{};
		ashes::VkLayerPropertiesArray m_layers;
		ashes::StringArray m_layerNames;
		ashes::StringArray m_extensionNames;
		ashes::VkExtensionPropertiesArray m_globalLayerExtensions;
		std::map< std::string, ashes::VkExtensionPropertiesArray > m_layersExtensions;
		RenderDeviceSPtr m_mainDevice;
		RenderDevice const * m_currentDevice{ nullptr };
		std::stack< SceneRPtr > m_stackScenes;
		castor::Nanoseconds m_gpuTime;
		GpuBufferPool m_gpuBufferPool;
	};
}

#endif
