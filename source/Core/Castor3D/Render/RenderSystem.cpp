#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Engine.hpp"

#include <ashespp/Core/Instance.hpp>
#include <ashespp/Core/RendererList.hpp>

#include <ShaderWriter/Source.hpp>

#if !defined( NDEBUG )
#	define LOAD_VALIDATION_LAYERS 1
#else
#	define LOAD_VALIDATION_LAYERS 0
#endif

using namespace castor;

namespace castor3d
{
	namespace
	{
		void addOptionalValidationLayer( std::string const & layer
			, std::string description
			, ashes::StringArray & names )
		{
#if LOAD_VALIDATION_LAYERS
			if ( layer.find( "validation" ) != std::string::npos
				|| description.find( "LunarG Validation" ) != std::string::npos )
			{
				names.push_back( layer );
			}
#endif
		}

		void addOptionalDebugReportLayer( ashes::StringArray & names )
		{
#if LOAD_VALIDATION_LAYERS
			names.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
			//names.push_back( VK_EXT_DEBUG_MARKER_EXTENSION_NAME );
#endif
		}

		void checkExtensionsAvailability( std::vector< VkExtensionProperties > const & available
			, ashes::StringArray const & requested )
		{
			for ( auto const & name : requested )
			{
				if ( available.end() == std::find_if( available.begin()
					, available.end()
					, [&name]( VkExtensionProperties const & extension )
					{
						return extension.extensionName == name;
					} ) )
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

		char const * const getName( VkDebugReportObjectTypeEXT value )
		{
			switch ( value )
			{
			case VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT:
				return "Unknown";
			case VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT:
				return "Instance";
			case VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT:
				return "PhysicalDevice";
			case VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT:
				return "Device";
			case VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT:
				return "Queue";
			case VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT:
				return "Semaphore";
			case VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT:
				return "CommandBuffer";
			case VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT:
				return "Fence";
			case VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT:
				return "DeviceMemory";
			case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT:
				return "Buffer";
			case VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT:
				return "Image";
			case VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT:
				return "Event";
			case VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT:
				return "QueryPool";
			case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT:
				return "BufferView";
			case VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT:
				return "ImageView";
			case VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT:
				return "ShaderModule";
			case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT:
				return "PipelineCache";
			case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT:
				return "PipelineLayout";
			case VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT:
				return "RenderPass";
			case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT:
				return "Pipeline";
			case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT:
				return "DescriptorSetLayout";
			case VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT:
				return "Sampler";
			case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT:
				return "DescriptorPool";
			case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT:
				return "DescriptorSet";
			case VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT:
				return "Framebuffer";
			case VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT:
				return "CommandPool";
			case VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT:
				return "Surface";
			case VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT:
				return "Swapchain";
			case VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_EXT:
				return "DebugReport";

			default:
				assert( false && "Unsupported VkDebugReportObjectTypeEXT." );
				return "Undefined";
			}
		}

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
			stream << "    Object: (" << std::hex << object << ") " << getName( objectType ) << "\n";
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
				castor::Logger::logTrace( stream );
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

		VkDebugReportCallbackEXT setupDebugging( ashes::Instance const & instance
			, void * userData )
		{
			// The report flags determine what type of messages for the layers will be displayed
			// For validating (debugging) an appplication the error and warning bits should suffice
			VkDebugReportFlagsEXT debugReportFlags = 0u
				| VK_DEBUG_REPORT_DEBUG_BIT_EXT
				//| VK_DEBUG_REPORT_INFORMATION_BIT_EXT
				| VK_DEBUG_REPORT_WARNING_BIT_EXT
				| VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
				| VK_DEBUG_REPORT_ERROR_BIT_EXT;

			VkDebugReportCallbackCreateInfoEXT dbgCreateInfo
			{
				VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
				nullptr,
				debugReportFlags,
				debugReportCallback,
				userData
			};

			return instance.createDebugReportCallback( dbgCreateInfo );
		}

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
		, m_debugCallback{ VK_NULL_HANDLE }
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
		addOptionalDebugReportLayer( m_extensionNames );
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

#if LOAD_VALIDATION_LAYERS
		m_debugCallback = setupDebugging( *m_instance
			, this );
#endif

		m_gpus = m_instance->enumeratePhysicalDevices();

		if ( m_gpus.empty() )
		{
			CU_Exception( "No Physical device found." );
		}

		auto & gpu = m_gpus[0];
		m_memoryProperties = gpu.getMemoryProperties();
		m_properties = gpu.getProperties();
		m_features = gpu.getFeatures();
	}

	RenderSystem::~RenderSystem()
	{
		if ( m_debugCallback != VK_NULL_HANDLE )
		{
			m_instance->vkDestroyDebugReportCallbackEXT( *m_instance, m_debugCallback, nullptr );
		}
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

#if C3D_TRACE_OBJECTS

		m_tracker.reportTracked();

#endif
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
		for ( auto const & props : m_layers )
		{
			addOptionalValidationLayer( props.layerName
				, props.description
				, names );
		}
	}
}
