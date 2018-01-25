#include "Core/VkRenderer.hpp"

#include "Core/VkConnection.hpp"
#include "Core/VkDevice.hpp"
#include "Core/VkPhysicalDevice.hpp"
#include "Core/VkSwapChain.hpp"

# if VKRENDERER_GLSL_TO_SPV
#	include <glslang/Public/ShaderLang.h>
#endif

namespace vk_renderer
{
	namespace
	{
#ifdef _WIN32
		static char const * const VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#else
		static char const * const VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME = VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
#endif
		VKAPI_ATTR VkBool32 VKAPI_CALL MyDebugReportCallback( VkDebugReportFlagsEXT flags
			, VkDebugReportObjectTypeEXT objectType
			, uint64_t object
			, size_t location
			, int32_t messageCode
			, const char * pLayerPrefix
			, const char * pMessage
			, void* pUserData )
		{
			// Select prefix depending on flags passed to the callback
			// Note that multiple flags may be set for a single validation message
			std::string prefix( "" );

			// Error that may result in undefined behaviour
			if ( renderer::checkFlag( flags, VK_DEBUG_REPORT_ERROR_BIT_EXT ) )
			{
				prefix += "ERROR:";
			};
			// Warnings may hint at unexpected / non-spec API usage
			if ( renderer::checkFlag( flags, VK_DEBUG_REPORT_WARNING_BIT_EXT ) )
			{
				prefix += "WARNING:";
			};
			// May indicate sub-optimal usage of the API
			if ( renderer::checkFlag( flags, VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ) )
			{
				prefix += "PERFORMANCE:";
			};
			// Informal messages that may become handy during debugging
			if ( renderer::checkFlag( flags, VK_DEBUG_REPORT_INFORMATION_BIT_EXT ) )
			{
				prefix += "INFO:";
			}
			// Diagnostic info from the Vulkan loader and layers
			// Usually not helpful in terms of API usage, but may help to debug layer and loader problems 
			if ( renderer::checkFlag( flags, VK_DEBUG_REPORT_DEBUG_BIT_EXT ) )
			{
				prefix += "DEBUG:";
			}

			// Display message to default output (console/logcat)
			std::stringstream debugMessage;
			debugMessage << prefix << " [" << pLayerPrefix << "] Code " << messageCode << " : " << pMessage;

#if defined( __ANDROID__ )

			if ( renderer::checkFlag( flags, VK_DEBUG_REPORT_ERROR_BIT_EXT ) )
			{
				LOGE( "%s", debugMessage.str().c_str() );
			}
			else
			{
				LOGD( "%s", debugMessage.str().c_str() );
			}

			fflush( stdout );

#else

			if ( renderer::checkFlag( flags, VK_DEBUG_REPORT_ERROR_BIT_EXT ) )
			{
				std::cerr << debugMessage.str() << std::endl;
			}
			else
			{
				std::cout << debugMessage.str() << std::endl;
			}

#endif

			// The return value of this callback controls wether the Vulkan call that caused
			// the validation message will be aborted or not
			// We return VK_FALSE as we DON'T want Vulkan calls that cause a validation message 
			// (and return a VkResult) to abort
			// If you instead want to have calls abort, pass in VK_TRUE and the function will 
			// return VK_ERROR_VALIDATION_FAILED_EXT 
			return VK_FALSE;
		}
	}

	Renderer::Renderer()
		: renderer::Renderer{}
	{
# if VKRENDERER_GLSL_TO_SPV

		glslang::InitializeProcess();

#endif
		doInitLayersProperties();
		doInitInstance();
		doEnumerateDevices();
	}

	Renderer::~Renderer()
	{
		if ( m_instance != VK_NULL_HANDLE )
		{
			if ( m_msgCallback != VK_NULL_HANDLE )
			{
				m_destroyDebugReportCallback( m_instance, m_msgCallback, nullptr );
				m_createDebugReportCallback = VK_NULL_HANDLE;
				m_destroyDebugReportCallback = VK_NULL_HANDLE;
				m_debugReportMessage = VK_NULL_HANDLE;
				m_msgCallback = VK_NULL_HANDLE;
			}

			m_instanceLayersProperties.clear();
			m_instanceExtensionNames.clear();
			m_instanceLayerNames.clear();
			m_gpus.clear();
			vk::DestroyInstance( m_instance, nullptr );
			m_instance = VK_NULL_HANDLE;
		}

# if VKRENDERER_GLSL_TO_SPV

		glslang::FinalizeProcess();

#endif

		DEBUG_WRITE( "VkRenderer.log" );
	}

	renderer::DevicePtr Renderer::createDevice( renderer::ConnectionPtr && connection )const
	{
		renderer::DevicePtr result;

		try
		{
			result = std::make_unique< Device >( *this, std::move( connection ) );
		}
		catch ( std::exception & exc )
		{
			std::cerr << "Could not initialise logical device:\n" << exc.what() << std::endl;
		}
		catch ( ... )
		{
			std::cerr << "Could not initialise logical device:\nUnknown error" << std::endl;
		}

		return result;
	}

	renderer::ConnectionPtr Renderer::createConnection( uint32_t deviceIndex
		, renderer::WindowHandle && handle )const
	{
		return std::make_unique< Connection >( *this
			, deviceIndex
			, std::move( handle ) );
	}

	PhysicalDevice & Renderer::getPhysicalDevice( uint32_t gpuIndex )const
	{
		if ( gpuIndex >= m_gpus.size() )
		{
			throw std::logic_error( "Invalid GPU index" );
		}

		return *m_gpus[gpuIndex];
	}

	void Renderer::completeLayerNames( std::vector< char const * > & names )const
	{
		for ( auto const & props : m_instanceLayersProperties )
		{
#if !defined( NDEBUG )
			if ( std::string{ props.m_properties.layerName }.find( "validation" ) != std::string::npos
				|| std::string{ props.m_properties.description }.find( "LunarG Validation" ) != std::string::npos )
			{
				names.push_back( props.m_properties.layerName );
			}
#endif
		}
	}

	void Renderer::doInitLayersProperties()
	{
		VkResult res{ VK_SUCCESS };
		std::vector< VkLayerProperties > vkProperties;
		// Il est possible, bien que rare, que le nombre de couches
		// d'instance change. Par exemple, en installant quelque chose
		// qui ajoute de nouvelles couches que le loader utiliserait
		// entre la requ�te initiale pour le compte et la r�cup�ration
		// des VkLayerProperties. Le loader l'indique en retournant
		// VK_INCOMPLETE et en mettant � jour le compte.
		// Le compte va alors �tre mis � jour avec le nombre d'entr�es
		// charg�es dans le pointeur de donn�es, dans le cas o�
		// le nombre de couches a diminu� ou est inf�rieur � la taille
		// donn�e.
		do
		{
			uint32_t instanceLayerCount{ 0 };
			res = vk::EnumerateInstanceLayerProperties( &instanceLayerCount, nullptr );

			if ( checkError( res ) )
			{
				if ( instanceLayerCount == 0 )
				{
					res = VK_SUCCESS;
				}
				else
				{
					vkProperties.resize( instanceLayerCount );
					res = vk::EnumerateInstanceLayerProperties( &instanceLayerCount
						, vkProperties.data() );
				}
			}
		} while ( res == VK_INCOMPLETE );

		LayerProperties layerProperties{ VkLayerProperties{} };
		doInitLayerExtensionProperties( layerProperties );
		m_instanceLayersProperties.push_back( layerProperties );

		// On r�cup�re la liste d'extensions pour chaque couche de l'instance.
		for ( auto prop : vkProperties )
		{
			LayerProperties layerProperties{ prop };
			doInitLayerExtensionProperties( layerProperties );
			m_instanceLayersProperties.push_back( layerProperties );
		}

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Layer properties retrieval failed: " + getLastError() };
		}
	}

	void Renderer::doInitLayerExtensionProperties( LayerProperties & layerProps )
	{
		VkResult res{ VK_SUCCESS };
		char * name{ layerProps.m_properties.layerName };

		// R�cup�ration des extensions support�es par la couche.
		do
		{
			uint32_t extensionCount{ 0 };
			res = vk::EnumerateInstanceExtensionProperties( name
				, &extensionCount
				, nullptr );

			if ( checkError( res ) )
			{
				if ( extensionCount > 0 )
				{
					layerProps.m_extensions.resize( extensionCount );
					res = vk::EnumerateInstanceExtensionProperties( name
						, &extensionCount
						, layerProps.m_extensions.data() );
				}
			}
		} while ( res == VK_INCOMPLETE );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Layer extensions properties retrieval failed: " + getLastError() };
		}
	}

	void Renderer::doInitInstance()
	{
		completeLayerNames( m_instanceLayerNames );
		m_instanceExtensionNames.push_back( VK_KHR_SURFACE_EXTENSION_NAME );
		m_instanceExtensionNames.push_back( VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME );
#if !defined( NDEBUG )
		m_instanceExtensionNames.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
#endif
		checkExtensionsAvailability( m_instanceLayersProperties.front().m_extensions, m_instanceExtensionNames );

		VkApplicationInfo appInfo
		{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,
			nullptr,
			ShortName.c_str(),                                              // pApplicationName
			VK_MAKE_VERSION( VersionMajor, VersionMinor, VersionBuild ),    // applicationVersion
			ShortName.c_str(),                                              // pEngineName
			VK_MAKE_VERSION( VersionMajor, VersionMinor, VersionBuild ),    // engineVersion
			VulkanVersion                                                   // apiVersion
		};

		VkInstanceCreateInfo instInfo
		{
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			nullptr,
			0,                                                                             // flags
			&appInfo,                                                                      // pApplicationInfo
			static_cast< uint32_t >( m_instanceLayerNames.size() ),                        // enabledLayerCount
			m_instanceLayerNames.empty() ? nullptr : m_instanceLayerNames.data(),          // ppEnabledLayerNames
			static_cast< uint32_t >( m_instanceExtensionNames.size() ),                    // enabledExtensionCount
			m_instanceExtensionNames.empty() ? nullptr : m_instanceExtensionNames.data(),  // ppEnabledExtensionNames
		};
		DEBUG_DUMP( instInfo );

		auto res = vk::CreateInstance( &instInfo, nullptr, &m_instance );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Instance initialisation failed: " + getLastError() };
		}

#if !defined( NDEBUG )
		// The report flags determine what type of messages for the layers will be displayed
		// For validating (debugging) an appplication the error and warning bits should suffice
		VkDebugReportFlagsEXT debugReportFlags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		// Additional flags include performance info, loader and layer debug messages, etc.
		doSetupDebugging( debugReportFlags );
#endif
	}

	void Renderer::doSetupDebugging( VkDebugReportFlagsEXT flags )
	{
		m_createDebugReportCallback = reinterpret_cast< PFN_vkCreateDebugReportCallbackEXT >( GetInstanceProcAddr( m_instance
			, "vkCreateDebugReportCallbackEXT" ) );
		m_destroyDebugReportCallback = reinterpret_cast< PFN_vkDestroyDebugReportCallbackEXT >( GetInstanceProcAddr( m_instance
			, "vkDestroyDebugReportCallbackEXT" ) );
		m_debugReportMessage = reinterpret_cast< PFN_vkDebugReportMessageEXT >( GetInstanceProcAddr( m_instance
			, "vkDebugReportMessageEXT" ) );

		VkDebugReportCallbackCreateInfoEXT dbgCreateInfo
		{
			VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT,
			nullptr,
			flags,
			( PFN_vkDebugReportCallbackEXT )MyDebugReportCallback,
			this
		};

		VkResult res = m_createDebugReportCallback(
			m_instance,
			&dbgCreateInfo,
			nullptr,
			&m_msgCallback );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Debug initialisation failed: " + getLastError() };
		}
	}

	void Renderer::doEnumerateDevices()
	{
		uint32_t gpuCount{ 0 };
		// On r�cup�re les GPU physiques.
		auto res = vk::EnumeratePhysicalDevices( m_instance
			, &gpuCount
			, nullptr );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "GPU enumeration failed: " + getLastError() };
		}

		std::vector< VkPhysicalDevice > gpus( gpuCount, VK_NULL_HANDLE );
		res = vk::EnumeratePhysicalDevices( m_instance
			, &gpuCount
			, gpus.data() );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "GPU enumeration failed: " + getLastError() };
		}

		if ( !gpuCount )
		{
			throw std::runtime_error{ "No GPU supporting vulkan." };
		}

		// Et on les stocke dans des PhysicalDevice.
		m_gpus.reserve( gpuCount );

		for ( auto gpu : gpus )
		{
			m_gpus.push_back( std::make_unique< PhysicalDevice >( *this, gpu ) );
		}
	}
}
