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

#include <ShaderAST/ShaderAllocator.hpp>
#include <ShaderAST/Visitors/SelectEntryPoint.hpp>

#include <stack>

namespace castor3d
{
	struct Renderer
	{
		Renderer()noexcept = default;
		Renderer( Renderer && ) = default;
		Renderer & operator=( Renderer && ) = default;
		Renderer( Renderer const & ) = delete;
		Renderer & operator=( Renderer const & ) = delete;
		~Renderer()noexcept = default;

		C3D_API Renderer( Engine & engine
			, AshPluginDescription desc
			, Extensions pinstanceExtensions = {}
			, uint32_t gpuIndex = 0u );

		AshPluginDescription desc{};
		Extensions instanceExtensions{};
		ashes::InstancePtr instance{};
		ashes::PhysicalDevicePtrArray gpus{};
		ashes::PhysicalDevice * gpu{};
	};

	class RenderSystem
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		*\~english
		*	Constructor.
		*\param[in] engine
		*	The engine.
		*\param[in] desc
		*	The Ashes plugin description.
		*\param[in] instanceExtensions
		*	The instance extensions.
		*\param[in] deviceExtensions
		*	The device extensions.
		*\~french
		*	Constructeur.
		*\param[in] engine
		*	Le moteur.
		*\param[in] desc
		*	The Ashes plugin description.
		*\param[in] instanceExtensions
		*	Les extensions d'instance.
		*\param[in] deviceExtensions
		*	Les extensions de device.
		*/
		C3D_API RenderSystem( Engine & engine
			, AshPluginDescription desc
			, Extensions instanceExtensions = {}
			, Extensions deviceExtensions = {} );
		/**
		*\~english
		*	Constructor.
		*\param[in] engine
		*	The engine.
		*\param[in] renderer
		*	The selected renderer.
		*\param[in] deviceExtensions
		*	The device extensions.
		*\~french
		*	Constructeur.
		*\param[in] engine
		*	Le moteur.
		*\param[in] renderer
		*	Le renderer sélectionné.
		*\param[in] deviceExtensions
		*	Les extensions de device.
		*/
		C3D_API RenderSystem( Engine & engine
			, Renderer renderer
			, Extensions deviceExtensions = {} );
		/**
		*\~english
		*	Creates a Vulkan instance usable with Castor3D.
		*\param[in] engine
		*	The engine.
		*\param[in] desc
		*	The Ashes plugin description.
		*\param[in] instanceExtensions
		*	The instance extensions.
		*\~french
		*	Crée une instance Vulkan compatible avec Castor3D.
		*\param[in] engine
		*	Le moteur.
		*\param[in] desc
		*	La description du plugin Ashes.
		*\param[in] instanceExtensions
		*	Les extensions d'instance.
		*/
		C3D_API static ashes::InstancePtr createInstance( Engine const & engine
			, AshPluginDescription const & desc
			, Extensions & instanceExtensions );
		/**
		*\~english
		*	Adds the instance layers names to the given names.
		*\param[in] engine
		*	The engine.
		*\param[in] layers
		*	The available layers.
		*\param[in,out] names
		*	The liste to fill.
		*\~french
		*	Ajoute les couches de l'instance aux noms déjà présents dans la liste donnée.
		*\param[in] engine
		*	Le moteur.
		*\param[in,out] layers
		*	Les couches présentes.
		*\param[in,out] names
		*	La liste à compléter.
		*/
		C3D_API static void completeLayerNames( Engine const & engine
			, ashes::VkLayerPropertiesArray const & layers
			, ashes::StringArray & names );
		/**
		*\~english
		*	Compiles a shader module to the necessary shader language.
		*\param[in] stage
		*	The shader stage.
		*\param[in] name
		*	The shader name.
		*\param[in] shader
		*	The shader to compile.
		*\param[in] entryPoint
		*	The shader entry point to compile.
		*\return
		*	The compiled shader.
		*\~french
		*	Compile un shader dans le langage shader nécessaire.
		*\param[in] stage
		*	Le stage du shader.
		*\param[in] name
		*	Le nom du shader.
		*\param[in] shader
		*	Le shader à compiler.
		*\param[in] entryPoint
		*	Le point d'entrée de shader à compiler.
		*\return
		*	Le shader compilé.
		*/
		C3D_API SpirVShader compileShader( VkShaderStageFlagBits stage
			, castor::String const & name
			, ast::Shader const & shader
			, ast::EntryPointConfig const & entryPoint );
		/**
		*\~english
		*	Compiles a shader module to the necessary shader language.
		*\param[in] stage
		*	The shader stage.
		*\param[in] name
		*	The shader name.
		*\param[in] glsl
		*	The shader to compile.
		*\return
		*	The compiled shader.
		*\~french
		*	Compile un shader dans le langage shader nécessaire.
		*\param[in] stage
		*	Le stage du shader.
		*\param[in] name
		*	Le nom du shader.
		*\param[in] glsl
		*	Le shader à compiler.
		*\return
		*	Le shader compilé.
		*/
		C3D_API SpirVShader compileShader( VkShaderStageFlagBits stage
			, castor::String const & name
			, castor::MbString const & glsl )const;
		/**
		*\~english
		*	Compiles a shader module to the necessary shader language.
		*\param[in] shaderModule
		*	The shader to compile.
		*\return
		*	The compiled shader.
		*\~french
		*	Compile un shader dans le langage shader nécessaire.
		*\param[in] shaderModule
		*	Le shader à compiler.
		*\return
		*	Le shader compilé.
		*/
		C3D_API SpirVShader const & compileShader( ShaderModule & shaderModule );
		/**
		*\~english
		*	Compiles a shader module to the necessary shader language.
		*\param[in] shaderModule
		*	The shader to compile.
		*\param[in] entryPoint
		*	The shader entry point to compile.
		*\return
		*	The compiled shader.
		*\~french
		*	Compile un shader dans le langage shader nécessaire.
		*\param[in] shaderModule
		*	Le shader à compiler.
		*\param[in] entryPoint
		*	Le point d'entrée de shader à compiler.
		*\return
		*	Le shader compilé.
		*/
		C3D_API SpirVShader const & compileShader( ProgramModule & shaderModule
			, ast::EntryPointConfig const & entryPoint );
		/**
		*\~english
		*	Computes an frustum projection matrix.
		*\param[in] left, right
		*	The left and right planes position.
		*\param[in] top, bottom
		*	The top and bottom planes position.
		*\param[in] zNear, zFar
		*	The near and far planes position.
		*\~french
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
		*	Computes an orthographic projection matrix.
		*\param[in] left, right
		*	The left and right planes position.
		*\param[in] top, bottom
		*	The top and bottom planes position.
		*\param[in] zNear, zFar
		*	The near and far planes position.
		*\~french
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
		*\param[in] fovy
		*	The vertical aperture angle.
		*\param[in] aspect
		*	The width / height ratio.
		*\param[in] zNear
		*	The near plane position.
		*\~french
		*	Calcule une matrice de projection en perspective sans clipping d'arrière plan.
		*\param[in] fovy
		*	L'angle d'ouverture verticale.
		*\param[in] aspect
		*	Le ratio largeur / hauteur.
		*\param[in] zNear
		*	La position du premier plan (pour le clipping).
		*/
		C3D_API castor::Matrix4x4f getInfinitePerspective( castor::Angle const & fovy
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
		C3D_API Texture const & getPrefilteredBrdfTexture()const;

		AshPluginDescription const & getDescription()const noexcept
		{
			return m_renderer.desc;
		}

		GpuInformations const & getGpuInformations()const noexcept
		{
			return m_gpuInformations;
		}

		castor::String getRendererType()const noexcept
		{
			return castor::makeString( getDescription().name );
		}

		bool hasDevice()const noexcept
		{
			return m_device != nullptr;
		}

		RenderDevice & getRenderDevice()const noexcept
		{
			CU_Require( hasDevice() );
			return *m_device;
		}

		bool hasSsbo()const noexcept
		{
			return getDescription().features.hasStorageBuffers == VK_TRUE;
		}

		ashes::Instance const & getInstance()const noexcept
		{
			return *m_renderer.instance;
		}

		VkPhysicalDeviceProperties const & getProperties()const noexcept
		{
			return m_properties;
		}

		VkPhysicalDeviceMemoryProperties const & getMemoryProperties()const noexcept
		{
			return m_memoryProperties;
		}

		VkPhysicalDeviceFeatures const & getFeatures()const noexcept
		{
			return m_features;
		}

		ashes::PhysicalDevice const & getPhysicalDevice()const noexcept
		{
			return *m_renderer.gpu;
		}

		bool hasFeature( GpuFeature feature )const noexcept
		{
			return m_gpuInformations.hasFeature( feature );
		}

		bool hasStereoRendering()const noexcept
		{
			return m_gpuInformations.hasStereoRendering();
		}

		bool hasShaderStorageBuffers()const noexcept
		{
			return m_gpuInformations.hasShaderStorageBuffers();
		}

		bool hasShaderType( VkShaderStageFlagBits type )const noexcept
		{
			return m_gpuInformations.hasShaderType( type );
		}

		uint32_t getValue( GpuMin index )const noexcept
		{
			return m_gpuInformations.getValue( index );
		}

		uint32_t getValue( GpuMax index )const noexcept
		{
			return m_gpuInformations.getValue( index );
		}

		bool hasLLPV()const noexcept
		{
			return m_properties.limits.maxDescriptorSetSampledImages > 16u;
		}

		ashes::Buffer< castor::Point4f > const & getRandomStorage()const noexcept
		{
			return *m_randomStorage;
		}
		/**@}*/

	private:
		bool doCreateRandomStorage( RenderDevice const & device );
		ast::ShaderAllocator & doGetShaderAllocator();

	private:
		castor::RecursiveMutex m_mutex;
		Renderer m_renderer{};
		GpuInformations m_gpuInformations{};
		DebugCallbacksPtr m_debug{};
		VkPhysicalDeviceMemoryProperties m_memoryProperties{};
		VkPhysicalDeviceProperties m_properties{};
		VkPhysicalDeviceFeatures m_features{};
		RenderDeviceUPtr m_device{};
		castor::Stack< SceneRPtr > m_stackScenes{};
		ashes::BufferPtr< castor::Point4f > m_randomStorage{};
		castor::Mutex m_allocMutex;
		castor::UnorderedMap< std::thread::id, castor::RawUniquePtr< ast::ShaderAllocator > > m_shaderCompileAllocator{};
	};
}

#endif
