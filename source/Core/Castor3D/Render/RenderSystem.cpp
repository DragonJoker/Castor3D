#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Shader/GlslToSpv.hpp"

#include <CastorUtils/Data/BinaryFile.hpp>
#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ashespp/Core/Instance.hpp>
#include <ashespp/Core/RendererList.hpp>

#include <ShaderWriter/Source.hpp>
#include <CompilerGlsl/compileGlsl.hpp>
#include <CompilerSpirV/compileSpirV.hpp>

#if C3D_HasSPIRVCross
#	include "spirv_cpp.hpp"
#	include "spirv_cross_util.hpp"
#	include "spirv_glsl.hpp"
#endif

#define C3D_DebugSpirV 0

namespace castor3d
{
	//*************************************************************************

	namespace
	{
#if C3D_HasSPIRVCross

		struct BlockLocale
		{
			BlockLocale()
				: m_prvLoc{ std::locale( "" ) }
			{
				if ( m_prvLoc.name() != "C" )
				{
					std::locale::global( std::locale{ "C" } );
				}
			}

			~BlockLocale()
			{
				if ( m_prvLoc.name() != "C" )
				{
					std::locale::global( m_prvLoc );
				}
			}

		private:
			std::locale m_prvLoc;
		};

		spv::ExecutionModel getExecutionModel( VkShaderStageFlagBits stage )
		{
			spv::ExecutionModel result{};

			switch ( stage )
			{
			case VK_SHADER_STAGE_VERTEX_BIT:
				result = spv::ExecutionModelVertex;
				break;
			case VK_SHADER_STAGE_GEOMETRY_BIT:
				result = spv::ExecutionModelGeometry;
				break;
			case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
				result = spv::ExecutionModelTessellationControl;
				break;
			case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
				result = spv::ExecutionModelTessellationEvaluation;
				break;
			case VK_SHADER_STAGE_FRAGMENT_BIT:
				result = spv::ExecutionModelFragment;
				break;
			case VK_SHADER_STAGE_COMPUTE_BIT:
				result = spv::ExecutionModelGLCompute;
				break;
			default:
				assert( false && "Unsupported shader stage flag" );
				break;
			}

			return result;
		}

		void doSetEntryPoint( VkShaderStageFlagBits stage
			, spirv_cross::CompilerGLSL & compiler )
		{
			auto model = getExecutionModel( stage );
			std::string entryPoint;

			for ( auto & e : compiler.get_entry_points_and_stages() )
			{
				if ( entryPoint.empty() && e.execution_model == model )
				{
					entryPoint = e.name;
				}
			}

			if ( entryPoint.empty() )
			{
				throw std::runtime_error{ "Could not find an entry point with stage: " + ashes::getName( stage ) };
			}

			compiler.set_entry_point( entryPoint, model );
		}

		void doSetupOptions( castor3d::RenderDevice const & device
			, spirv_cross::CompilerGLSL & compiler )
		{
			auto options = compiler.get_common_options();
			options.version = device->getShaderVersion();
			options.es = false;
			options.separate_shader_objects = true;
			options.enable_420pack_extension = true;
			options.vertex.fixup_clipspace = false;
			options.vertex.flip_vert_y = true;
			options.vertex.support_nonzero_base_instance = true;
			compiler.set_common_options( options );
		}

		std::string compileSpvToGlsl( castor3d::RenderDevice const & device
			, ashes::UInt32Array const & spv
			, VkShaderStageFlagBits stage )
		{
			BlockLocale guard;
			auto compiler = std::make_unique< spirv_cross::CompilerGLSL >( spv );
			doSetEntryPoint( stage, *compiler );
			doSetupOptions( device, *compiler );
			return compiler->compile();
		}

#endif

		//*************************************************************************

		bool isValidationLayer( std::string const & name
			, std::string const & description )
		{
			static std::set< std::string > const validNames
			{
				"VK_LAYER_KHRONOS_validation",
			};
			return validNames.find( name ) != validNames.end();
		}

		bool isApiTraceLayer( std::string const & name
			, std::string const & description )
		{
			static std::set< std::string > const validNames
			{
				"VK_LAYER_LUNARG_api_dump",
			};
			return validNames.find( name ) != validNames.end();
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
#if VK_EXT_debug_utils
			if ( isExtensionAvailable( available, VK_EXT_DEBUG_UTILS_EXTENSION_NAME ) )
			{
				names.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
			}
#endif
#if VK_EXT_debug_report
			if ( isExtensionAvailable( available, VK_EXT_DEBUG_REPORT_EXTENSION_NAME ) )
			{
				names.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
			}
#endif
#if VK_EXT_debug_marker
			if ( isExtensionAvailable( available, VK_EXT_DEBUG_MARKER_EXTENSION_NAME ) )
			{
				names.push_back( VK_EXT_DEBUG_MARKER_EXTENSION_NAME );
			}
#endif
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
				VK_API_VERSION_1_2,
			};
		}
	}

	//*************************************************************************

	RenderSystem::RenderSystem( Engine & engine
		, AshPluginDescription desc )
		: OwnedBy< Engine >{ engine }
		, m_desc{ std::move( desc ) }
		, m_initialised{ false }
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
			static std::map< uint32_t, castor::String > vendors
			{
				{ 0x1002, cuT( "AMD" ) },
				{ 0x10DE, cuT( "NVIDIA" ) },
				{ 0x8086, cuT( "INTEL" ) },
				{ 0x13B5, cuT( "ARM" ) },
			};
			auto & device = *getMainRenderDevice();
			auto & features = device.features;
			auto & properties = device.properties;
			auto & limits = properties.limits;
			castor::StringStream stream( castor::makeStringStream() );
			stream << ( properties.apiVersion >> 22 ) << cuT( "." ) << ( ( properties.apiVersion >> 12 ) & 0x0FFF );
			m_gpuInformations.setVendor( vendors[properties.vendorID] );
			m_gpuInformations.setRenderer( properties.deviceName );
			m_gpuInformations.setVersion( stream.str() );
			m_gpuInformations.setShaderLanguageVersion( device.device->getShaderVersion() );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eShaderStorageBuffers, m_desc.features.hasStorageBuffers );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eStereoRendering, limits.maxViewports > 1u );

			m_gpuInformations.useShaderType( VK_SHADER_STAGE_COMPUTE_BIT, device.device->getInstance().getFeatures().hasComputeShaders );
			m_gpuInformations.useShaderType( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, features.tessellationShader );
			m_gpuInformations.useShaderType( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, features.tessellationShader );
			m_gpuInformations.useShaderType( VK_SHADER_STAGE_GEOMETRY_BIT, features.geometryShader );
			m_gpuInformations.useShaderType( VK_SHADER_STAGE_FRAGMENT_BIT, true );
			m_gpuInformations.useShaderType( VK_SHADER_STAGE_VERTEX_BIT, true );

			m_gpuInformations.setMaxValue( GpuMax::eTexture1DSize, limits.maxImageDimension1D );
			m_gpuInformations.setMaxValue( GpuMax::eTexture2DSize, limits.maxImageDimension2D );
			m_gpuInformations.setMaxValue( GpuMax::eTexture3DSize, limits.maxImageDimension3D );
			m_gpuInformations.setMaxValue( GpuMax::eTextureCubeSize, limits.maxImageDimensionCube );
			m_gpuInformations.setMaxValue( GpuMax::eTextureLayers, limits.maxImageArrayLayers );
			m_gpuInformations.setMaxValue( GpuMax::eSamplerLodBias, int32_t( limits.maxSamplerLodBias ) );
			m_gpuInformations.setMaxValue( GpuMax::eClipDistances, limits.maxClipDistances  );
			m_gpuInformations.setMaxValue( GpuMax::eFramebufferWidth, limits.maxFramebufferWidth );
			m_gpuInformations.setMaxValue( GpuMax::eFramebufferHeight, limits.maxFramebufferHeight );
			m_gpuInformations.setMaxValue( GpuMax::eFramebufferLayers, limits.maxFramebufferLayers );
			m_gpuInformations.setMaxValue( GpuMax::eFramebufferSamples, limits.framebufferColorSampleCounts );
			m_gpuInformations.setMaxValue( GpuMax::eTexelBufferSize, limits.maxTexelBufferElements );
			m_gpuInformations.setMaxValue( GpuMax::eUniformBufferSize, limits.maxUniformBufferRange );
			m_gpuInformations.setMaxValue( GpuMax::eStorageBufferSize, limits.maxStorageBufferRange );
			m_gpuInformations.setMaxValue( GpuMax::eViewportWidth, limits.maxViewportDimensions[0] );
			m_gpuInformations.setMaxValue( GpuMax::eViewportHeight, limits.maxViewportDimensions[1] );
			m_gpuInformations.setMaxValue( GpuMax::eViewports, limits.maxViewports );

			log::info << m_gpuInformations << std::endl;

			m_initialised = true;
		}
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

	SpirVShader RenderSystem::compileShader( castor3d::ShaderModule const & module )const
	{
		SpirVShader result;

		if ( module.shader )
		{
			result.spirv = spirv::serialiseSpirv( *module.shader );

#if !defined( NDEBUG )
#	if C3D_HasGLSL
			std::string glsl = glsl::compileGlsl( *module.shader
				, ast::SpecialisationInfo{}
				, glsl::GlslConfig
				{
					module.shader->getType(),
					460,
					true,
					false,
					true,
					true,
					true,
					true,
				} );
#	else
			std::string glsl;
#	endif

			result.text = glsl + "\n" + spirv::writeSpirv( *module.shader );

#	if C3D_HasSPIRVCross && C3D_DebugSpirV && C3D_HasGlslang
			std::string name = module.name + "_" + ashes::getName( module.stage );

			try
			{
				auto glslFromSpv = compileSpvToGlsl( *getMainRenderDevice()
					, result.spirv
					, module.stage );
				const_cast< castor3d::ShaderModule & >( module ).source += "\n" + glslFromSpv;
			}
			catch ( std::exception & exc )
			{
				std::cerr << module.source << std::endl;
				std::cerr << exc.what() << std::endl;
				{
					castor::BinaryFile file{ castor::File::getExecutableDirectory() / ( name + "_sdw.spv" )
						, castor::File::OpenMode::eWrite };
					file.writeArray( result.spirv.data()
						, result.spirv.size() );
				}

				auto ref = castor3d::compileGlslToSpv( *getMainRenderDevice()
					, module.stage
					, glsl );
				{
					castor::BinaryFile file{ castor::File::getExecutableDirectory() / ( name + "_glslang.spv" )
						, castor::File::OpenMode::eWrite };
					file.writeArray( ref.data()
						, ref.size() );
				}
			}

#	endif
#endif
		}
		else
		{
			CU_Require( !module.source.empty() );
			result.text = module.source;
			result.spirv = castor3d::compileGlslToSpv( *getMainRenderDevice()
				, module.stage
				, module.source );
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
		CU_Require( gpuIndex < m_gpus.size()
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
			if ( ( getEngine()->isValidationEnabled()
					&& isValidationLayer( props.layerName, props.description ) )
				|| ( getEngine()->isApiTraceEnabled()
					&& isApiTraceLayer( props.layerName, props.description ) ) )
			{
				names.push_back( props.layerName );
			}
		}
	}

	//*************************************************************************
}
