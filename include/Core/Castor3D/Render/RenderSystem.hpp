/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderSystem_H___
#define ___C3D_RenderSystem_H___

#include "RenderModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"

#include "Castor3D/Miscellaneous/DebugCallbacks.hpp"
#include "Castor3D/Miscellaneous/GpuInformations.hpp"
#include "Castor3D/Miscellaneous/GpuObjectTracker.hpp"
#include "Castor3D/Render/RenderDevice.hpp"

#include <ashespp/Core/WindowHandle.hpp>

#include <stack>

namespace castor3d
{
	struct Renderer
	{
		Renderer() = default;
		Renderer( Renderer && ) = default;
		Renderer & operator=( Renderer && ) = default;
		Renderer( Renderer const & ) = delete;
		Renderer & operator=( Renderer const & ) = delete;
		C3D_API Renderer( Engine & engine
			, AshPluginDescription desc
			, Extensions pinstanceExtensions = {}
			, uint32_t gpuIndex = 0u );

		AshPluginDescription desc;
		Extensions instanceExtensions;
		ashes::InstancePtr instance;
		ashes::PhysicalDevicePtrArray gpus;
		ashes::PhysicalDevice * gpu{};
	};

	class RenderSystem
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	desc	The Ashes plugin description.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	desc	The Ashes plugin description.
		 */
		C3D_API RenderSystem( Engine & engine
			, AshPluginDescription desc
			, Extensions instanceExtensions = {}
			, Extensions deviceExtensions = {} );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	desc	The Ashes plugin description.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	desc	The Ashes plugin description.
		 */
		C3D_API RenderSystem( Engine & engine
			, Renderer renderer
			, Extensions deviceExtensions = {} );
		/**
		*\~english
		*\brief
		*	Creates a Vulkan instance usable with Castor3D.
		*\param[in] engine
		*	The engine.
		*\param[in] desc
		*	The Ashes plugin description.
		*\~french
		*\brief
		*	Crée une instance Vulkan compatible avec Castor3D.
		*\param[in] engine
		*	Le moteur.
		*\param[in] desc
		*	The Ashes plugin description.
		*/
		C3D_API static ashes::InstancePtr createInstance( Engine & engine
			, AshPluginDescription const & desc
			, Extensions & instanceExtensions );
		/**
		*\~english
		*\brief
		*	Adds the instance layers names to the given names.
		*\param[in,out] names
		*	The liste to fill.
		*\~french
		*\brief
		*	Ajoute les couches de l'instance aux noms déjà présents dans la liste donnée.
		*\param[in,out] names
		*	La liste à compléter.
		*/
		C3D_API static void completeLayerNames( Engine const & engine
			, ashes::VkLayerPropertiesArray const & layers
			, ashes::StringArray & names );
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
		C3D_API SpirVShader compileShader( VkShaderStageFlagBits stage
			, castor::String const & name
			, ast::Shader const & shader )const;
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
		C3D_API SpirVShader compileShader( VkShaderStageFlagBits stage
			, castor::String const & name
			, castor::String const & glsl )const;
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
		*\~english
		*\name
		*	Getters.
		*\~french
		*\name
		*	Accesseurs.
		*/
		/**@{*/
		AshPluginDescription const & getDescription()const
		{
			return m_renderer.desc;
		}

		GpuInformations const & getGpuInformations()const
		{
			return m_gpuInformations;
		}

		castor::String getRendererType()const
		{
			return getDescription().name;
		}

		bool hasDevice()const
		{
			return m_device != nullptr;
		}

		RenderDevice const & getRenderDevice()const
		{
			CU_Require( hasDevice() );
			return *m_device;
		}

		OverlayRendererSPtr getOverlayRenderer()const
		{
			return m_overlayRenderer;
		}

		castor::Nanoseconds const & getGpuTime()const
		{
			return m_gpuTime;
		}

		bool hasSsbo()const
		{
			return getDescription().features.hasStorageBuffers == VK_TRUE;
		}

		ashes::Instance const & getInstance()const
		{
			return *m_renderer.instance;
		}

		VkPhysicalDeviceProperties const & getProperties()const
		{
			return m_properties;
		}

		VkPhysicalDeviceMemoryProperties const & getMemoryProperties()const
		{
			return m_memoryProperties;
		}

		VkPhysicalDeviceFeatures const & getFeatures()const
		{
			return m_features;
		}

		ashes::PhysicalDevice const & getPhysicalDevice()const
		{
			return *m_renderer.gpu;
		}

		bool hasFeature( GpuFeature feature )const
		{
			return m_gpuInformations.hasFeature( feature );
		}

		bool hasStereoRendering()const
		{
			return m_gpuInformations.hasStereoRendering();
		}

		bool hasShaderStorageBuffers()const
		{
			return m_gpuInformations.hasShaderStorageBuffers();
		}

		bool hasShaderType( VkShaderStageFlagBits type )const
		{
			return m_gpuInformations.hasShaderType( type );
		}

		uint32_t getValue( GpuMin index )const
		{
			return m_gpuInformations.getValue( index );
		}

		uint32_t getValue( GpuMax index )const
		{
			return m_gpuInformations.getValue( index );
		}

		bool hasLLPV()const
		{
			return m_properties.limits.maxDescriptorSetSampledImages > 16u;
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
		template< class Rep, class Period >
		void incGpuTime( std::chrono::duration< Rep, Period > const & time )
		{
			m_gpuTime += std::chrono::duration_cast< castor::Nanoseconds >( time );
		}

		void resetGpuTime()
		{
			m_gpuTime = castor::Nanoseconds( 0 );
		}
		/**@}*/

	private:
		std::recursive_mutex m_mutex;
		Renderer m_renderer;
		GpuInformations m_gpuInformations;
		OverlayRendererSPtr m_overlayRenderer;
		DebugCallbacksPtr m_debug;
		VkPhysicalDeviceMemoryProperties m_memoryProperties;
		VkPhysicalDeviceProperties m_properties;
		VkPhysicalDeviceFeatures m_features;
		RenderDeviceSPtr m_device;
		std::stack< SceneRPtr > m_stackScenes;
		castor::Nanoseconds m_gpuTime;
	};
}

#endif
