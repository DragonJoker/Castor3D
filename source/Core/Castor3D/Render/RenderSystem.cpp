#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Engine.hpp"

#include <ashespp/Core/Instance.hpp>
#include <ashespp/Core/RendererList.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	//*************************************************************************

	namespace
	{
		bool isValidationLayer( std::string const & name
			, std::string const & description )
		{
			return ( name.find( "validation" ) != std::string::npos )
				|| ( description.find( "LunarG Validation" ) != std::string::npos );
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

		void addOptionalDebugLayers( std::vector< VkExtensionProperties > const & available
			, ashes::StringArray & names )
		{
#	if VK_EXT_debug_utils

			if ( isExtensionAvailable( available, VK_EXT_DEBUG_UTILS_EXTENSION_NAME ) )
			{
				names.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
			}
			else
#	endif

			{

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
		}

		void checkExtensionsAvailability( std::vector< VkExtensionProperties > const & available
			, ashes::StringArray const & requested )
		{
			for ( auto const & name : requested )
			{
				if ( !isExtensionAvailable( available, name ) )
				{
					CU_Exception( "Extension " + name + " is not supported." );
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
				CU_Exception( "Instance layers retrieval failed: " + ashes::getName( res ) + "." );
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
				CU_Exception( "Instance layer extensions retrieval failed: " + ashes::getName( res ) + "." );
			}

			return result;
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
			addOptionalDebugLayers( m_globalLayerExtensions, m_extensionNames );
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

	castor::Matrix4x4f RenderSystem::getFrustum( float left
		, float right
		, float bottom
		, float top
		, float zNear
		, float zFar )const
	{
		return convert( m_instance->frustum( left, right, bottom, top, zNear, zFar ) );
	}

	castor::Matrix4x4f RenderSystem::getPerspective( castor::Angle const & fovy
		, float aspect
		, float zNear
		, float zFar )const
	{
		return convert( m_instance->perspective( fovy.radians(), aspect, zNear, zFar ) );
	}

	castor::Matrix4x4f RenderSystem::getOrtho( float left
		, float right
		, float bottom
		, float top
		, float zNear
		, float zFar )const
	{
		return convert( m_instance->ortho( left, right, bottom, top, zNear, zFar ) );
	}

	castor::Matrix4x4f RenderSystem::getInfinitePerspective( float radiansFovY
		, float aspect
		, float zNear )const
	{
		return convert( m_instance->infinitePerspective( radiansFovY, aspect, zNear ) );
	}

	void RenderSystem::completeLayerNames( ashes::StringArray & names )const
	{
		for ( auto const & props : m_layers )
		{
			if ( getEngine()->isValidationEnabled()
				&& isValidationLayer( props.layerName, props.description ) )
			{
				names.push_back( props.layerName );
			}
		}
	}

	//*************************************************************************
}
