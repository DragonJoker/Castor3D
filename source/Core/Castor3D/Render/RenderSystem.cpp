#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Engine.hpp"

#include <ashespp/Core/Instance.hpp>
#include <ashespp/Core/RendererList.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		void addOptionalValidationLayer( std::string const & layer
			, std::string description
			, ashes::StringArray & names )
		{
			if ( layer.find( "validation" ) != std::string::npos
				|| description.find( "LunarG Validation" ) != std::string::npos )
			{
				names.push_back( layer );
			}
		}

		bool isExtensionAvailable( std::vector< VkExtensionProperties > const & available
			, std::string const & requested )
		{
			return available.end() != std::find_if( available.begin()
				, available.end()
				, [&requested]( VkExtensionProperties const & extension )
				{
					return extension.extensionName == requested;
				} );
		}

		void addOptionalDebugReportLayer( std::vector< VkExtensionProperties > const & available
			, ashes::StringArray & names )
		{
#	if VK_EXT_debug_utils

			if ( isExtensionAvailable( available, VK_EXT_DEBUG_UTILS_EXTENSION_NAME ) )
			{
				names.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
			}

#	endif
#	if VK_EXT_debug_report

			if ( isExtensionAvailable( available, VK_EXT_DEBUG_REPORT_EXTENSION_NAME ) )
			{
				names.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
			}

#	endif
#	if VK_EXT_debug_marker

			if ( isExtensionAvailable( available, VK_EXT_DEBUG_MARKER_EXTENSION_NAME ) )
			{
				names.push_back( VK_EXT_DEBUG_MARKER_EXTENSION_NAME );
			}

#	endif
		}

		void checkExtensionsAvailability( std::vector< VkExtensionProperties > const & available
			, ashes::StringArray const & requested )
		{
			for ( auto const & name : requested )
			{
				if ( !isExtensionAvailable( available, name ) )
				{
					throw std::runtime_error{ "Extension " + name + " is not supported." };
				}
			}
		}

		ashes::VkLayerPropertiesArray enumerateLayerProperties( PFN_vkEnumerateInstanceLayerProperties enumLayerProperties )
		{
			if ( !enumLayerProperties )
			{
				return {};
			}

			uint32_t count;
			std::vector< VkLayerProperties > result;
			VkResult res;

			do
			{
				res = enumLayerProperties( &count, nullptr );

				if ( count )
				{
					result.resize( count );
					res = enumLayerProperties( &count, result.data() );
				}
			}
			while ( res == VK_INCOMPLETE );

			if ( res != VK_SUCCESS )
			{
				throw ashes::Exception{ res, "Instance layers retrieval" };
			}

			return result;
		}

		ashes::VkExtensionPropertiesArray enumerateExtensionProperties( PFN_vkEnumerateInstanceExtensionProperties enumInstanceExtensionProperties
			, std::string const & layerName )
		{
			if ( !enumInstanceExtensionProperties )
			{
				return {};
			}

			uint32_t count;
			std::vector< VkExtensionProperties > result;
			VkResult res;

			do
			{
				res = enumInstanceExtensionProperties( layerName.empty() ? nullptr : layerName.c_str()
					, &count
					, nullptr );

				if ( count )
				{
					result.resize( count );
					res = enumInstanceExtensionProperties( layerName.empty() ? nullptr : layerName.c_str()
						, &count
						, result.data() );
				}
			}
			while ( res == VK_INCOMPLETE );

			if ( res != VK_SUCCESS )
			{
				throw ashes::Exception{ res, "Instance layer extensions retrieval" };
			}

			return result;
		}

#if VK_EXT_debug_utils

		std::ostream & operator<<( std::ostream & stream, VkDebugUtilsObjectNameInfoEXT const & value )
		{
			stream << "(" << std::hex << value.objectHandle << ") " << ashes::getName( value.objectType );

			if ( value.pObjectName )
			{
				stream << " " << value.pObjectName;
			}

			return stream;
		}

		std::ostream & operator<<( std::ostream & stream, VkDebugUtilsLabelEXT const & value )
		{
			stream << "(" << value.color[0]
				<< ", " << value.color[1]
				<< ", " << value.color[2]
				<< ", " << value.color[3] << ")";

			if ( value.pLabelName )
			{
				stream << " " << value.pLabelName;
			}

			return stream;
		}

		template< typename ObjectT >
		void print( std::ostream & stream
			, std::string const & name
			, uint32_t count
			, ObjectT const * objects )
		{
			stream << "    " << name << ": " << count << "\n";

			for ( uint32_t i = 0u; i < count; ++i, ++objects )
			{
				stream << "      " << *objects << "\n";
			}
		}

		VkBool32 VKAPI_PTR debugMessageCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity
			, VkDebugUtilsMessageTypeFlagsEXT messageTypes
			, const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData
			, void * pUserData )
		{
			// Select prefix depending on flags passed to the callback
			// Note that multiple flags may be set for a single validation message
			std::locale loc{ "C" };
			std::stringstream stream;
			stream.imbue( loc );
			stream << "Vulkan ";

			// Error that may result in undefined behaviour
			switch ( messageSeverity )
			{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				stream << "Error";
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				stream << "Warning";
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				stream << "Info";
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				stream << "Verbose";
				break;
			}

			if ( ashes::checkFlag( messageTypes, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT ) )
			{
				stream << " - General";
			}
			if ( ashes::checkFlag( messageTypes, VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT ) )
			{
				stream << " - Validation";
			}
			if ( ashes::checkFlag( messageTypes, VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT ) )
			{
				stream << " - Performance";
			}

			stream << ":\n";
			stream << "    Message ID: " << pCallbackData->pMessageIdName << "\n";
			stream << "    Code: 0x" << std::hex << pCallbackData->messageIdNumber << "\n";
			stream << "    Message: " << pCallbackData->pMessage << "\n";
			print( stream, "Objects", pCallbackData->objectCount, pCallbackData->pObjects );
			print( stream, "Queue Labels", pCallbackData->queueLabelCount, pCallbackData->pQueueLabels );
			print( stream, "CommmandBuffer Labels", pCallbackData->cmdBufLabelCount, pCallbackData->pCmdBufLabels );

			VkBool32 result = VK_FALSE;

			switch ( messageSeverity )
			{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				castor::Logger::logError( stream );
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				castor::Logger::logWarning( stream );
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				//castor::Logger::logTrace( stream );
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				castor::Logger::logDebug( stream );
				break;
			}

			// The return value of this callback controls wether the Vulkan call that caused
			// the validation message will be aborted or not
			// Return VK_FALSE if we DON'T want Vulkan calls that cause a validation message 
			// (and return a VkResult) to abort
			// Return VK_TRUE if you instead want to have calls abort, and the function will 
			// return VK_ERROR_VALIDATION_FAILED_EXT 
			return result;
		}

#endif
#if VK_EXT_debug_report

		VkBool32 VKAPI_PTR debugReportCallback( VkDebugReportFlagsEXT flags
			, VkDebugReportObjectTypeEXT objectType
			, uint64_t object
			, size_t location
			, int32_t messageCode
			, const char * pLayerPrefix
			, const char * pMessage
			, void * pUserData )
		{
			// Select prefix depending on flags passed to the callback
			// Note that multiple flags may be set for a single validation message
			std::locale loc{ "C" };
			std::stringstream stream;
			stream.imbue( loc );
			stream << "Vulkan ";

			// Error that may result in undefined behaviour
			if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_ERROR_BIT_EXT ) )
			{
				stream << "Error:\n";
			};
			// Warnings may hint at unexpected / non-spec API usage
			if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_WARNING_BIT_EXT ) )
			{
				stream << "Warning:\n";
			};
			// May indicate sub-optimal usage of the API
			if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ) )
			{
				stream << "Performance:\n";
			};
			// Informal messages that may become handy during debugging
			if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_INFORMATION_BIT_EXT ) )
			{
				stream << "Info:\n";
			}
			// Diagnostic info from the Vulkan loader and layers
			// Usually not helpful in terms of API usage, but may help to debug layer and loader problems 
			if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_DEBUG_BIT_EXT ) )
			{
				stream << "Debug:\n";
			}

			// Display message to default output (console/logcat)
			stream << "    Layer: " << pLayerPrefix << "\n";
			stream << "    Code: 0x" << std::hex << messageCode << "\n";
			stream << "    Object: (" << std::hex << object << ") " << ashes::getName( objectType ) << "\n";
			stream << "    Message: " << pMessage;

			VkBool32 result = VK_FALSE;

			if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_ERROR_BIT_EXT ) )
			{
				castor::Logger::logError( stream );
			}
			else if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_WARNING_BIT_EXT )
				|| ashes::checkFlag( flags, VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ) )
			{
				castor::Logger::logWarning( stream );
			}
			else if ( ashes::checkFlag( flags, VK_DEBUG_REPORT_INFORMATION_BIT_EXT ) )
			{
				//castor::Logger::logTrace( stream );
			}
			else
			{
				castor::Logger::logDebug( stream );
			}

			// The return value of this callback controls wether the Vulkan call that caused
			// the validation message will be aborted or not
			// Return VK_FALSE if we DON'T want Vulkan calls that cause a validation message 
			// (and return a VkResult) to abort
			// Return VK_TRUE if you instead want to have calls abort, and the function will 
			// return VK_ERROR_VALIDATION_FAILED_EXT 
			return result;
		}

#endif

		ashes::ApplicationInfo createApplicationInfo( Engine & engine
			, AshPluginDescription desc )
		{
			return ashes::ApplicationInfo
			{
				engine.getAppName(),
				engine.getAppVersion().getVkVersion(),
				"Castor3D",
				Version{}.getVkVersion(),
				VK_API_VERSION_1_0,
			};
		}
	}

	//*************************************************************************

	DebugCallbacks::DebugCallbacks( ashes::Instance const & instance
		, void * userData )
		: m_instance{ instance }
		, m_userData{ userData }
	{
#if VK_EXT_debug_utils

		VkDebugUtilsMessageSeverityFlagsEXT severityFlags = 0u
			//| VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		VkDebugUtilsMessageTypeFlagsEXT typeFlags = 0u
			| VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		VkDebugUtilsMessengerCreateInfoEXT createInfo
		{
			VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			nullptr,
			0u,
			severityFlags,
			typeFlags,
			debugMessageCallback,
			userData
		};
		m_messenger = instance.createDebugUtilsMessenger( createInfo );

		if ( m_messenger == VK_NULL_HANDLE )
#endif
		{

#if VK_EXT_debug_report

			VkDebugReportFlagsEXT debugReportFlags = 0u
				| VK_DEBUG_REPORT_DEBUG_BIT_EXT
				//| VK_DEBUG_REPORT_INFORMATION_BIT_EXT
				| VK_DEBUG_REPORT_WARNING_BIT_EXT
				| VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
				| VK_DEBUG_REPORT_ERROR_BIT_EXT;
			VkDebugReportCallbackCreateInfoEXT createInfo
			{
				VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
					nullptr,
					debugReportFlags,
					debugReportCallback,
					userData
			};
			m_callback = instance.createDebugReportCallback( createInfo );

#endif
		}
	}

	DebugCallbacks::~DebugCallbacks()
	{
#if VK_EXT_debug_report

		if ( m_callback != VK_NULL_HANDLE )
		{
			m_instance.vkDestroyDebugReportCallbackEXT( m_instance, m_callback, nullptr );
		}

#endif
#if VK_EXT_debug_utils

		if ( m_messenger != VK_NULL_HANDLE )
		{
			m_instance.vkDestroyDebugUtilsMessengerEXT( m_instance, m_messenger, nullptr );
		}

#endif
	}

	//*************************************************************************

	RenderSystem::RenderSystem( Engine & engine
		, AshPluginDescription desc
		, bool topDown
		, bool zeroToOneDepth
		, bool invertedNormals )
		: OwnedBy< Engine >{ engine }
		, m_desc{ std::move( desc ) }
		, m_initialised{ false }
		, m_topDown{ topDown }
		, m_zeroToOneDepth{ zeroToOneDepth }
		, m_invertedNormals{ invertedNormals }
		, m_gpuInformations{}
		, m_gpuBufferPool{ *this }
	{
		auto & rendererList = engine.getRenderersList();
		auto plugin = rendererList.selectPlugin( desc.name );
		PFN_vkEnumerateInstanceLayerProperties enumLayerProperties;
		enumLayerProperties = ( PFN_vkEnumerateInstanceLayerProperties )plugin.getInstanceProcAddr( VK_NULL_HANDLE,
			"vkEnumerateInstanceLayerProperties" );
		PFN_vkEnumerateInstanceExtensionProperties enumInstanceExtensionProperties;
		enumInstanceExtensionProperties = ( PFN_vkEnumerateInstanceExtensionProperties )plugin.getInstanceProcAddr( VK_NULL_HANDLE,
			"vkEnumerateInstanceExtensionProperties" );

		m_layers = enumerateLayerProperties( enumLayerProperties );
		m_globalLayerExtensions = enumerateExtensionProperties( enumInstanceExtensionProperties
			, m_globalLayer.layerName );

		// On récupère la liste d'extensions pour chaque couche de l'instance.
		for ( auto layerProperties : m_layers )
		{
			m_layersExtensions.emplace( layerProperties.layerName
				, enumerateExtensionProperties( enumInstanceExtensionProperties
					, layerProperties.layerName ) );
		}

		completeLayerNames( m_layerNames );

		m_extensionNames.push_back( VK_KHR_SURFACE_EXTENSION_NAME );
		m_extensionNames.push_back( ashes::KHR_PLATFORM_SURFACE_EXTENSION_NAME );

		if ( getEngine()->isValidationEnabled() )
		{
			addOptionalDebugReportLayer( m_globalLayerExtensions, m_extensionNames );
		}

		checkExtensionsAvailability( m_globalLayerExtensions, m_extensionNames );

		ashes::InstanceCreateInfo createInfo
		{
			0u,
			createApplicationInfo( engine, desc ),
			m_layerNames,
			m_extensionNames,
		};
		m_instance = std::make_unique< ashes::Instance >( std::move( plugin )
			, std::move( createInfo ) );

		if ( getEngine()->isValidationEnabled() )
		{
			m_debug = std::make_unique< DebugCallbacks >( *m_instance
				, this );
		}

		m_gpus = m_instance->enumeratePhysicalDevices();

		if ( m_gpus.empty() )
		{
			CU_Exception( "No Physical device found." );
		}

		auto & gpu = *m_gpus[0];
		m_memoryProperties = gpu.getMemoryProperties();
		m_properties = gpu.getProperties();
		m_features = gpu.getFeatures();
	}

	RenderSystem::~RenderSystem()
	{
	}

	void RenderSystem::initialise( GpuInformations informations )
	{
		m_gpuInformations = std::move( informations );

		if ( !m_initialised )
		{
			static std::map< uint32_t, String > vendors
			{
				{ 0x1002, cuT( "AMD" ) },
				{ 0x10DE, cuT( "NVIDIA" ) },
				{ 0x8086, cuT( "INTEL" ) },
				{ 0x13B5, cuT( "ARM" ) },
			};
			auto & device = *getMainRenderDevice();
			StringStream stream( makeStringStream() );
			stream << ( device.properties.apiVersion >> 22 ) << cuT( "." ) << ( ( device.properties.apiVersion >> 12 ) & 0x0FFF );
			m_gpuInformations.setVendor( vendors[device.properties.vendorID] );
			m_gpuInformations.setRenderer( device.properties.deviceName );
			m_gpuInformations.setVersion( stream.str() );
			m_gpuInformations.setShaderLanguageVersion( device.device->getShaderVersion() );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eShaderStorageBuffers, m_desc.features.hasStorageBuffers );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eStereoRendering, false );

			m_gpuInformations.useShaderType( VK_SHADER_STAGE_COMPUTE_BIT, device.device->getInstance().getFeatures().hasComputeShaders );
			m_gpuInformations.useShaderType( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, device.features.tessellationShader );
			m_gpuInformations.useShaderType( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, device.features.tessellationShader );
			m_gpuInformations.useShaderType( VK_SHADER_STAGE_GEOMETRY_BIT, device.features.geometryShader );
			m_gpuInformations.useShaderType( VK_SHADER_STAGE_FRAGMENT_BIT, true );
			m_gpuInformations.useShaderType( VK_SHADER_STAGE_VERTEX_BIT, true );

			m_initialised = true;
		}

		Logger::logInfo( cuT( "Vendor: " ) + m_gpuInformations.getVendor() );
		Logger::logInfo( cuT( "Renderer: " ) + m_gpuInformations.getRenderer() );
		Logger::logInfo( cuT( "Version: " ) + m_gpuInformations.getVersion() );
		//m_gpuInformations.removeFeature( GpuFeature::eShaderStorageBuffers );
	}

	void RenderSystem::cleanup()
	{
		m_mainDevice.reset();
	}

	void RenderSystem::pushScene( Scene * p_scene )
	{
		m_stackScenes.push( p_scene );
	}

	void RenderSystem::popScene()
	{
		m_stackScenes.pop();
	}

	Scene * RenderSystem::getTopScene()const
	{
		Scene * result = nullptr;

		if ( !m_stackScenes.empty() )
		{
			result = m_stackScenes.top();
		}

		return result;
	}

	GpuBufferOffset RenderSystem::getBuffer( VkBufferUsageFlagBits type
		, uint32_t size
		, VkMemoryPropertyFlags flags )
	{
		return m_gpuBufferPool.getGpuBuffer( type
			, size
			, flags );
	}

	void RenderSystem::putBuffer( VkBufferUsageFlagBits type
		, GpuBufferOffset const & bufferOffset )
	{
		m_gpuBufferPool.putGpuBuffer( type
			, bufferOffset );
	}

	void RenderSystem::cleanupPool()
	{
		m_gpuBufferPool.cleanup();
	}
	
	RenderDeviceSPtr RenderSystem::createDevice( ashes::WindowHandle handle
		, uint32_t gpuIndex )
	{
		assert( gpuIndex < m_gpus.size()
			&& "Invalid Physical Device index." );
		return std::make_shared< RenderDevice >( *this
			, *m_gpus[gpuIndex]
			, m_desc
			, m_instance->createSurface( *m_gpus[gpuIndex], std::move( handle ) ) );
	}

	castor::Matrix4x4r RenderSystem::getFrustum( float left
		, float right
		, float bottom
		, float top
		, float zNear
		, float zFar )const
	{
		return convert( m_instance->frustum( left, right, bottom, top, zNear, zFar ) );
	}

	castor::Matrix4x4r RenderSystem::getPerspective( castor::Angle const & fovy
		, float aspect
		, float zNear
		, float zFar )const
	{
		return convert( m_instance->perspective( fovy.radians(), aspect, zNear, zFar ) );
	}

	castor::Matrix4x4r RenderSystem::getOrtho( float left
		, float right
		, float bottom
		, float top
		, float zNear
		, float zFar )const
	{
		return convert( m_instance->ortho( left, right, bottom, top, zNear, zFar ) );
	}

	castor::Matrix4x4r RenderSystem::getInfinitePerspective( float radiansFovY
		, float aspect
		, float zNear )const
	{
		return convert( m_instance->infinitePerspective( radiansFovY, aspect, zNear ) );
	}

	void RenderSystem::completeLayerNames( ashes::StringArray & names )const
	{
		if ( getEngine()->isValidationEnabled() )
		{
			for ( auto const & props : m_layers )
			{
				addOptionalValidationLayer( props.layerName
					, props.description
					, names );
			}
		}
	}
}
