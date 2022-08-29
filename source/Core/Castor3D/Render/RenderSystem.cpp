#include "Castor3D/Render/RenderSystem.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Shader/GlslToSpv.hpp"

#include <CastorUtils/Data/BinaryFile.hpp>
#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>
#include <CastorUtils/Align/Aligned.hpp>

#include <ashespp/Core/Instance.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Core/RendererList.hpp>
#include <ashespp/Core/Surface.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/StagingTexture.hpp>

#include <ShaderWriter/Source.hpp>
#include <CompilerSpirV/compileSpirV.hpp>
#if C3D_HasGLSL
#	include <CompilerGlsl/compileGlsl.hpp>
#endif

#if C3D_HasSPIRVCross
#	include "spirv_cpp.hpp"
#	include "spirv_cross_util.hpp"
#	include "spirv_glsl.hpp"
#endif

#include <atomic>

#if !defined( NDEBUG )
#	define C3D_DebugSpirV 0
#endif

CU_ImplementCUSmartPtr( castor3d, RenderSystem )

#pragma warning( disable: 4191 ) // Unsafe conversion from 'PFN_vkVoidFunction' to 'PFN_vkAnyOtherFunction'

namespace castor3d
{
	//*************************************************************************

	namespace rendsys
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

		static spv::ExecutionModel getExecutionModel( VkShaderStageFlagBits stage )
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

		static void doSetEntryPoint( VkShaderStageFlagBits stage
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

		static void doSetupOptions( castor3d::RenderDevice const & device
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

		static std::string compileSpvToGlsl( castor3d::RenderDevice const & device
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
#if !defined( NDEBUG )
#	if C3D_HasGLSL

		static glsl::GlslExtensionSet getGLSLExtensions( uint32_t glslVersion )
		{
			glsl::GlslExtensionSet result;

			if ( glslVersion >= glsl::v4_6 )
			{
				result.insert( glsl::EXT_shader_atomic_float );
				result.insert( glsl::EXT_ray_tracing );
				result.insert( glsl::EXT_ray_query );
				result.insert( glsl::EXT_scalar_block_layout );
				result.insert( glsl::KHR_shader_subgroup_basic );
				result.insert( glsl::KHR_shader_subgroup_ballot );
			}

			if ( glslVersion >= glsl::v4_5 )
			{
				result.insert( glsl::ARB_shader_ballot );
				result.insert( glsl::ARB_shader_viewport_layer_array );
				result.insert( glsl::NV_stereo_view_rendering );
				result.insert( glsl::NVX_multiview_per_view_attributes );
				result.insert( glsl::EXT_nonuniform_qualifier );
				result.insert( glsl::NV_mesh_shader );
				result.insert( glsl::EXT_buffer_reference2 );
			}

			if ( glslVersion >= glsl::v4_3 )
			{
				result.insert( glsl::NV_viewport_array2 );
				result.insert( glsl::NV_shader_atomic_fp16_vector );
			}

			if ( glslVersion >= glsl::v4_2 )
			{
				result.insert( glsl::ARB_compute_shader );
				result.insert( glsl::ARB_explicit_uniform_location );
				result.insert( glsl::ARB_shading_language_420pack );
				result.insert( glsl::NV_shader_atomic_float );
			}

			if ( glslVersion >= glsl::v4_1 )
			{
				result.insert( glsl::ARB_shading_language_packing );
			}

			if ( glslVersion >= glsl::v4_0 )
			{
				result.insert( glsl::ARB_separate_shader_objects );
				result.insert( glsl::ARB_texture_cube_map_array );
				result.insert( glsl::ARB_texture_gather );
			}

			if ( glslVersion >= glsl::v3_3 )
			{
				result.insert( glsl::ARB_shader_stencil_export );
				result.insert( glsl::KHR_vulkan_glsl );
				result.insert( glsl::EXT_shader_explicit_arithmetic_types_int64 );
				result.insert( glsl::EXT_multiview );
				result.insert( glsl::ARB_explicit_attrib_location );
				result.insert( glsl::ARB_shader_image_load_store );
				result.insert( glsl::EXT_gpu_shader4 );
				result.insert( glsl::ARB_gpu_shader5 );
				result.insert( glsl::EXT_gpu_shader4_1 );
				result.insert( glsl::ARB_texture_query_lod );
				result.insert( glsl::ARB_texture_query_levels );
				result.insert( glsl::ARB_shader_draw_parameters );
				result.insert( glsl::ARB_fragment_layer_viewport );
				result.insert( glsl::ARB_tessellation_shader );
			}

			return result;
		}

#	endif
#endif

		//*************************************************************************

#if C3D_UseAllocationCallbacks

		struct AlignedBuddyAllocatorTraits
		{
			using PointerType = uint8_t *;
			struct Block
			{
				PointerType data;
			};
			/**
			 *\~english
			 *\brief		Constructor.
			 *\param[in]	size	The allocator size.
			 *\param[in]	align	The allocator base alignment.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	size	La taille de l'allocateur.
			 *\param[in]	align	LL'alignement de base de l'allocateur.
			 */
			inline AlignedBuddyAllocatorTraits( size_t size
				, uint32_t align )
				: m_align{ align }
				, m_size{ ashes::getAlignedSize( size, m_align ) }
				, m_memory{ reinterpret_cast< uint8_t * >( castor::alignedAlloc( m_align, m_size ) ) }
			{
				CU_Require( m_memory );
				log::info << cuT( "Vulkan memory allocator, allocating " ) << m_size << cuT( " bytes" ) << std::endl;
			}

			inline ~AlignedBuddyAllocatorTraits()
			{
				log::info << cuT( "Vulkan memory allocator" ) << std::endl;
				log::info << cuT( "	Total Allocations: " ) << m_totalAllocated << cuT( " bytes" ) << std::endl;
				log::info << cuT( "	Allocated at most: " ) << m_maxAllocated << cuT( " bytes" ) << std::endl;
				log::info << cuT( "	Spoiled memory: " ) << m_totalSpoilt << cuT( " bytes" ) << std::endl;
				log::info << cuT( "	Leaked memory: " ) << m_currentAllocated << cuT( " bytes" ) << std::endl;
				castor::alignedFree( m_memory );
			}
			/**
			*\~copydoc
			*	ashes::BuddyAllocatorTraits::getSize
			*/
			inline size_t getSize()const
			{
				return m_size;
			}
			/**
			*\~copydoc
			*	ashes::BuddyAllocatorTraits::getPointer
			*/
			inline PointerType getPointer( uint32_t offset )
			{
				CU_Require( ashes::getAlignedSize( offset, m_align ) == offset );
				return m_memory + offset;
			}
			/**
			*\~copydoc
			*	ashes::BuddyAllocatorTraits::getOffset
			*/
			inline size_t getOffset( PointerType pointer )const
			{
				auto offset = size_t( pointer - m_memory );
				CU_Ensure( ashes::getAlignedSize( offset, m_align ) == offset );
				return offset;
			}
			/**
			*\~copydoc
			*	ashes::BuddyAllocatorTraits::getNull
			*/
			inline Block getNull()const
			{
				static Block result{ nullptr };
				return result;
			}
			/**
			*\~copydoc
			*	ashes::BuddyAllocatorTraits::isNull
			*/
			inline bool isNull( PointerType pointer )const
			{
				return pointer == getNull().data;
			}

			inline void registerAllocation( PointerType pointer
				, VkDeviceSize wantedSize
				, VkDeviceSize pointerSize )
			{
				m_currentAllocated += pointerSize;
				m_totalAllocated += pointerSize;
				m_totalSpoilt += ( pointerSize - wantedSize );
				m_maxAllocated = std::max< uint64_t >( m_maxAllocated, m_currentAllocated );
			}

			inline void registerDeallocation( PointerType pointer
				, VkDeviceSize pointerSize )
			{
				m_currentAllocated -= pointerSize;
			}

		private:
			uint32_t m_align;
			size_t m_size;
			uint8_t * m_memory;
			std::atomic_ullong m_currentAllocated{};
			std::atomic_ullong m_totalAllocated{};
			std::atomic_ullong m_maxAllocated{};
			std::atomic_ullong m_totalSpoilt{};
		};

#endif

		uint32_t constexpr vk1_0 = ashes::makeVersion( 1, 0, 0 );
		uint32_t constexpr vk1_1 = ashes::makeVersion( 1, 1, 0 );
		uint32_t constexpr vk1_2 = ashes::makeVersion( 1, 2, 0 );

		//*************************************************************************

		static bool isValidationLayer( std::string const & name
			, std::string const & description )
		{
			static std::set< std::string > const validNames
			{
				"VK_LAYER_KHRONOS_validation",
			};
			return validNames.find( name ) != validNames.end();
		}

		static bool isSynchronisationLayer( std::string const & name
			, std::string const & description )
		{
			static std::set< std::string > const validNames
			{
				"VK_LAYER_KHRONOS_synchronization2",
			};
			return validNames.find( name ) != validNames.end();
		}

		static bool isApiTraceLayer( std::string const & name
			, std::string const & description )
		{
			static std::set< std::string > const validNames
			{
				"VK_LAYER_LUNARG_api_dump",
			};
			return validNames.find( name ) != validNames.end();
		}

		static bool isExtensionAvailable( std::vector< VkExtensionProperties > const & available
			, std::string const & requested )
		{
			return available.end() != std::find_if( available.begin()
				, available.end()
				, [&requested]( VkExtensionProperties const & extension )
				{
					return extension.extensionName == requested;
				} );
		}

		static void addOptionalDebugLayers( std::vector< VkExtensionProperties > const & available
			, Extensions & extensions )
		{
#if VK_KHR_synchronization2
			log::debug << "Adding synchronization layer" << std::endl;
			if ( isExtensionAvailable( available, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME ) )
			{
				extensions.addExtension( VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME );
			}
#endif
#if VK_EXT_debug_utils
			log::debug << "Adding debug_utils layer" << std::endl;
			if ( isExtensionAvailable( available, VK_EXT_DEBUG_UTILS_EXTENSION_NAME ) )
			{
				extensions.addExtension( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
			}
#else
#	if VK_EXT_debug_report
			log::debug << "Adding debug_report layer" << std::endl;
			if ( isExtensionAvailable( available, VK_EXT_DEBUG_REPORT_EXTENSION_NAME ) )
			{
				extensions.addExtension( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
			}
#	endif
#	if VK_EXT_debug_marker
			log::debug << "Adding debug_marker layer" << std::endl;
			if ( isExtensionAvailable( available, VK_EXT_DEBUG_MARKER_EXTENSION_NAME ) )
			{
				extensions.addExtension( VK_EXT_DEBUG_MARKER_EXTENSION_NAME );
			}
#	endif
#endif
		}

		static void checkExtensionsAvailability( std::vector< VkExtensionProperties > const & available
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

		static ashes::VkLayerPropertiesArray enumerateLayerProperties( PFN_vkEnumerateInstanceLayerProperties enumLayerProperties )
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

		static ashes::VkExtensionPropertiesArray enumerateExtensionProperties( PFN_vkEnumerateInstanceExtensionProperties enumInstanceExtensionProperties
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

		static ashes::ApplicationInfo createApplicationInfo( Engine & engine
			, uint32_t vkApiVersion )
		{
			return ashes::ApplicationInfo
			{
				engine.getAppName(),
				engine.getAppVersion().getVkVersion(),
				"Castor3D",
				Version{}.getVkVersion(),
				vkApiVersion,
			};
		}

		static uint32_t getSpirVVersion( uint32_t vkApiVersion )
		{
			uint32_t result{ spirv::v1_0 };

			if ( vkApiVersion >= vk1_2 )
			{
				result = spirv::v1_5;
			}
			else if ( vkApiVersion >= vk1_1 )
			{
				result = spirv::v1_3;
			}
			else if ( vkApiVersion >= vk1_0 )
			{
				result = spirv::v1_0;
			}

			return result;
		}

		static spirv::SpirVExtensionSet listSpirVExtensions( RenderDevice const & device )
		{
			spirv::SpirVExtensionSet result;

			if ( device.hasExtension( VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME )
				|| device.gpu.getProperties().apiVersion >= ashes::makeVersion( 1, 1, 0 ) )
			{
				result.insert( spirv::KHR_shader_draw_parameters );
			}

			if ( device.hasExtension( VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME )
				|| device.gpu.getProperties().apiVersion >= ashes::makeVersion( 1, 2, 0 ) )
			{
				result.insert( spirv::EXT_descriptor_indexing );
			}

			if ( device.hasExtension( VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME )
				&& device.hasTerminateInvocation() )
			{
				result.insert( spirv::KHR_terminate_invocation );
			}

			if ( device.hasExtension( VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME )
				&& device.hasDemoteToHelperInvocation() )
			{
				result.insert( spirv::EXT_demote_to_helper_invocation );
			}

			if ( device.hasExtension( VK_NV_MESH_SHADER_EXTENSION_NAME )
				&& device.hasMeshAndTaskShaders() )
			{
				result.insert( spirv::NV_mesh_shader );
			}

			if ( device.hasExtension( VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME )
				&& device.hasAtomicFloatAdd() )
			{
				result.insert( spirv::EXT_shader_atomic_float_add );
			}

			if ( device.hasExtension( VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME )
				&& device.hasBufferDeviceAddress() )
			{
				result.insert( spirv::EXT_physical_storage_buffer );
			}

			if ( device.hasExtension( VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME )
				&& device.hasExtension( VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME )
				&& device.hasRayTracing() )
			{
				result.insert( spirv::KHR_ray_tracing );
			}

			if ( device.gpu.getProperties().apiVersion >= ashes::makeVersion( 1, 2, 0 ) )
			{
				result.insert( spirv::KHR_shader_subgroup );
			}

			return result;
		}
	}

	//*************************************************************************

	Renderer::Renderer( Engine & engine
		, AshPluginDescription pdesc
		, Extensions pinstanceExtensions
		, uint32_t gpuIndex )
		: desc{ std::move( pdesc ) }
		, instanceExtensions{ std::move( pinstanceExtensions ) }
		, instance{ RenderSystem::createInstance( engine, desc, instanceExtensions ) }
		, gpus{ instance->enumeratePhysicalDevices() }
	{
		if ( gpus.empty() )
		{
			instance.reset();
			CU_Exception( "No Physical device found." );
		}

		gpu = gpus[gpuIndex].get();
	}

	//*************************************************************************

	RenderSystem::RenderSystem( Engine & engine
		, Renderer renderer
		, Extensions pdeviceExtensions )
		: OwnedBy< Engine >{ engine }
		, m_renderer{ std::move( renderer ) }
		, m_gpuInformations{}
	{
		if ( !m_renderer.gpu )
		{
			m_renderer.gpus.clear();
			m_renderer.instance.reset();
			CU_Exception( "No GPU selected." );
		}

		if ( getEngine()->isValidationEnabled() )
		{
			m_debug = std::make_unique< DebugCallbacks >( *m_renderer.instance
				, this );
		}

		auto & gpu = *m_renderer.gpu;
		m_memoryProperties = gpu.getMemoryProperties();
		m_properties = gpu.getProperties();
		m_features = gpu.getFeatures();

		getOwner()->getImageLoader().setCompressionSupport( { m_features.textureCompressionBC == VK_TRUE
			, m_features.textureCompressionBC == VK_TRUE
			, m_features.textureCompressionBC == VK_TRUE
			, m_features.textureCompressionBC == VK_TRUE
			, false } );

		m_device = std::make_shared< RenderDevice >( *this
			, gpu
			, m_renderer.desc
			, std::move( pdeviceExtensions ) );

		static std::map< uint32_t, castor::String > vendors
		{
			{ 0x1002, cuT( "AMD" ) },
			{ 0x10DE, cuT( "NVIDIA" ) },
			{ 0x8086, cuT( "INTEL" ) },
			{ 0x13B5, cuT( "ARM" ) },
		};
		auto & device = *m_device;
		auto & features = device.features;
		auto & properties = device.properties;
		auto & limits = properties.limits;
		castor::StringStream stream( castor::makeStringStream() );
		stream << ( properties.apiVersion >> 22 ) << cuT( "." ) << ( ( properties.apiVersion >> 12 ) & 0x0FFF );
		m_gpuInformations.setVendor( vendors[properties.vendorID] );
		m_gpuInformations.setRenderer( properties.deviceName );
		m_gpuInformations.setVersion( stream.str() );
		m_gpuInformations.updateFeature( castor3d::GpuFeature::eStereoRendering, limits.maxViewports > 1u );
		m_gpuInformations.updateFeature( castor3d::GpuFeature::eShaderStorageBuffers, m_renderer.desc.features.hasStorageBuffers != 0 );
		m_gpuInformations.updateFeature( castor3d::GpuFeature::eBindless, m_device->hasBindless() );
		m_gpuInformations.updateFeature( castor3d::GpuFeature::eGeometry, features.geometryShader != 0 );
		m_gpuInformations.updateFeature( castor3d::GpuFeature::eTessellation, features.tessellationShader != 0 );
		m_gpuInformations.updateFeature( castor3d::GpuFeature::eRayTracing, m_device->hasRayTracing() );
		m_gpuInformations.updateFeature( castor3d::GpuFeature::eMesh, m_device->hasMeshAndTaskShaders() );

		m_gpuInformations.useShaderType( VK_SHADER_STAGE_COMPUTE_BIT, device.device->getInstance().getFeatures().hasComputeShaders != 0 );
		m_gpuInformations.useShaderType( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, features.tessellationShader != 0 );
		m_gpuInformations.useShaderType( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, features.tessellationShader != 0 );
		m_gpuInformations.useShaderType( VK_SHADER_STAGE_GEOMETRY_BIT, features.geometryShader != 0 );
		m_gpuInformations.useShaderType( VK_SHADER_STAGE_FRAGMENT_BIT, true );
		m_gpuInformations.useShaderType( VK_SHADER_STAGE_VERTEX_BIT, true );
		m_gpuInformations.useShaderType( VK_SHADER_STAGE_MESH_BIT_NV, m_device->hasMeshAndTaskShaders() );
		m_gpuInformations.useShaderType( VK_SHADER_STAGE_TASK_BIT_NV, m_device->hasMeshAndTaskShaders() );
		m_gpuInformations.useShaderType( VK_SHADER_STAGE_RAYGEN_BIT_KHR, m_device->hasRayTracing() );
		m_gpuInformations.useShaderType( VK_SHADER_STAGE_ANY_HIT_BIT_KHR, m_device->hasRayTracing() );
		m_gpuInformations.useShaderType( VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, m_device->hasRayTracing() );
		m_gpuInformations.useShaderType( VK_SHADER_STAGE_MISS_BIT_KHR, m_device->hasRayTracing() );
		m_gpuInformations.useShaderType( VK_SHADER_STAGE_INTERSECTION_BIT_KHR, m_device->hasRayTracing() );
		m_gpuInformations.useShaderType( VK_SHADER_STAGE_CALLABLE_BIT_KHR, m_device->hasRayTracing() );

		m_gpuInformations.setValue( GpuMin::eBufferMapSize, uint32_t( limits.nonCoherentAtomSize ) );
		m_gpuInformations.setValue( GpuMin::eUniformBufferOffsetAlignment, uint32_t( limits.minUniformBufferOffsetAlignment ) );

		m_gpuInformations.setValue( GpuMax::eTexture1DSize, limits.maxImageDimension1D );
		m_gpuInformations.setValue( GpuMax::eTexture2DSize, limits.maxImageDimension2D );
		m_gpuInformations.setValue( GpuMax::eTexture3DSize, limits.maxImageDimension3D );
		m_gpuInformations.setValue( GpuMax::eTextureCubeSize, limits.maxImageDimensionCube );
		m_gpuInformations.setValue( GpuMax::eTextureLayers, limits.maxImageArrayLayers );
		m_gpuInformations.setValue( GpuMax::eSamplerLodBias, uint32_t( limits.maxSamplerLodBias ) );
		m_gpuInformations.setValue( GpuMax::eClipDistances, limits.maxClipDistances );
		m_gpuInformations.setValue( GpuMax::eFramebufferWidth, limits.maxFramebufferWidth );
		m_gpuInformations.setValue( GpuMax::eFramebufferHeight, limits.maxFramebufferHeight );
		m_gpuInformations.setValue( GpuMax::eFramebufferLayers, limits.maxFramebufferLayers );
		m_gpuInformations.setValue( GpuMax::eFramebufferSamples, limits.framebufferColorSampleCounts );
		m_gpuInformations.setValue( GpuMax::eTexelBufferSize, limits.maxTexelBufferElements );
		m_gpuInformations.setValue( GpuMax::eUniformBufferSize, limits.maxUniformBufferRange );
		m_gpuInformations.setValue( GpuMax::eStorageBufferSize, limits.maxStorageBufferRange );
		m_gpuInformations.setValue( GpuMax::eViewportWidth, limits.maxViewportDimensions[0] );
		m_gpuInformations.setValue( GpuMax::eViewportHeight, limits.maxViewportDimensions[1] );
		m_gpuInformations.setValue( GpuMax::eViewports, limits.maxViewports );

		log::info << m_gpuInformations << std::endl;
	}

	RenderSystem::RenderSystem( Engine & engine
		, AshPluginDescription desc
		, Extensions instanceExtensions
		, Extensions deviceExtensions )
		: RenderSystem{ engine
			, Renderer{ engine, std::move( desc ), std::move( instanceExtensions ), 0u }
			, std::move( deviceExtensions ) }
	{
	}

	RenderSystem::~RenderSystem()
	{
	}

	ashes::InstancePtr RenderSystem::createInstance( Engine & engine
		, AshPluginDescription const & desc
		, Extensions & instanceExtensions )
	{
		auto & rendererList = engine.getRenderersList();
		auto plugin = rendererList.selectPlugin( desc.name );
		PFN_vkEnumerateInstanceLayerProperties enumLayerProperties;
		enumLayerProperties = reinterpret_cast< PFN_vkEnumerateInstanceLayerProperties >( plugin.getInstanceProcAddr( VK_NULL_HANDLE,
			"vkEnumerateInstanceLayerProperties" ) );
		PFN_vkEnumerateInstanceExtensionProperties enumInstanceExtensionProperties;
		enumInstanceExtensionProperties = reinterpret_cast< PFN_vkEnumerateInstanceExtensionProperties >( plugin.getInstanceProcAddr( VK_NULL_HANDLE,
			"vkEnumerateInstanceExtensionProperties" ) );

		auto layers = rendsys::enumerateLayerProperties( enumLayerProperties );
		VkLayerProperties globalLayer{};
		auto globalLayerExtensions = rendsys::enumerateExtensionProperties( enumInstanceExtensionProperties
			, globalLayer.layerName );

		// On récupère la liste d'extensions pour chaque couche de l'instance.
		std::map< std::string, ashes::VkExtensionPropertiesArray > layersExtensions;
		for ( auto layerProperties : layers )
		{
			layersExtensions.emplace( layerProperties.layerName
				, rendsys::enumerateExtensionProperties( enumInstanceExtensionProperties
					, layerProperties.layerName ) );
		}

		uint32_t apiVersion{ rendsys::vk1_0 };
		PFN_vkEnumerateInstanceVersion enumerateInstanceVersion;
		enumerateInstanceVersion = reinterpret_cast< PFN_vkEnumerateInstanceVersion >( plugin.getInstanceProcAddr( VK_NULL_HANDLE,
			"vkEnumerateInstanceVersion" ) );

		if ( enumerateInstanceVersion )
		{
			enumerateInstanceVersion( &apiVersion );
		}

		ashes::StringArray layerNames;
		completeLayerNames( engine, layers, layerNames );

		if ( rendsys::isExtensionAvailable( globalLayerExtensions, VK_KHR_SURFACE_EXTENSION_NAME ) )
		{
			instanceExtensions.addExtension( VK_KHR_SURFACE_EXTENSION_NAME );
		}
#if defined( VK_KHR_get_physical_device_properties2 )
		if ( rendsys::isExtensionAvailable( globalLayerExtensions, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME ) )
		{
			instanceExtensions.addExtension( VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME );
		}
#endif
#if defined( VK_USE_PLATFORM_ANDROID_KHR )
		if ( rendsys::isExtensionAvailable( globalLayerExtensions, VK_KHR_ANDROID_SURFACE_EXTENSION_NAME ) )
		{
			instanceExtensions.addExtension( VK_KHR_ANDROID_SURFACE_EXTENSION_NAME );
		}
#endif
#if defined( VK_USE_PLATFORM_MIR_KHR )
		if ( rendsys::isExtensionAvailable( globalLayerExtensions, VK_KHR_MIR_SURFACE_EXTENSION_NAME ) )
		{
			instanceExtensions.addExtension( VK_KHR_MIR_SURFACE_EXTENSION_NAME );
		}
#endif
#if defined( VK_USE_PLATFORM_FUCHSIA )
		if ( rendsys::isExtensionAvailable( globalLayerExtensions, VK_FUCHSIA_IMAGEPIPE_SURFACE_EXTENSION_NAME ) )
		{
			instanceExtensions.addExtension( VK_FUCHSIA_IMAGEPIPE_SURFACE_EXTENSION_NAME );
		}
#endif
#if defined( VK_USE_PLATFORM_IOS_MVK )
		if ( rendsys::isExtensionAvailable( globalLayerExtensions, VK_MVK_IOS_SURFACE_EXTENSION_NAME ) )
		{
			instanceExtensions.addExtension( VK_MVK_IOS_SURFACE_EXTENSION_NAME );
		}
#endif
#if defined( VK_USE_PLATFORM_MACOS_MVK )
		if ( rendsys::isExtensionAvailable( globalLayerExtensions, VK_MVK_MACOS_SURFACE_EXTENSION_NAME ) )
		{
			instanceExtensions.addExtension( VK_MVK_MACOS_SURFACE_EXTENSION_NAME );
		}
#endif
#if defined( VK_USE_PLATFORM_VI_NN )
		if ( rendsys::isExtensionAvailable( globalLayerExtensions, VK_NN_VI_SURFACE_EXTENSION_NAME ) )
		{
			instanceExtensions.addExtension( VK_NN_VI_SURFACE_EXTENSION_NAME );
		}
#endif
#if defined( VK_USE_PLATFORM_XCB_KHR )
		if ( rendsys::isExtensionAvailable( globalLayerExtensions, VK_KHR_XCB_SURFACE_EXTENSION_NAME ) )
		{
			instanceExtensions.addExtension( VK_KHR_XCB_SURFACE_EXTENSION_NAME );
		}
#endif
#if defined( VK_USE_PLATFORM_XLIB_KHR )
		if ( rendsys::isExtensionAvailable( globalLayerExtensions, VK_KHR_XLIB_SURFACE_EXTENSION_NAME ) )
		{
			instanceExtensions.addExtension( VK_KHR_XLIB_SURFACE_EXTENSION_NAME );
		}
#endif
#if defined( VK_USE_PLATFORM_WAYLAND_KHR )
		if ( rendsys::isExtensionAvailable( globalLayerExtensions, VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME ) )
		{
			instanceExtensions.addExtension( VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME );
		}
#endif
#if defined( VK_USE_PLATFORM_WIN32_KHR )
		if ( rendsys::isExtensionAvailable( globalLayerExtensions, VK_KHR_WIN32_SURFACE_EXTENSION_NAME ) )
		{
			instanceExtensions.addExtension( VK_KHR_WIN32_SURFACE_EXTENSION_NAME );
		}
#endif
		if ( engine.isValidationEnabled() )
		{
			rendsys::addOptionalDebugLayers( globalLayerExtensions, instanceExtensions );
		}

		auto & extensionNames = instanceExtensions.getExtensionsNames();
		rendsys::checkExtensionsAvailability( globalLayerExtensions, extensionNames );
		ashes::InstanceCreateInfo createInfo
		{
			0u,
			rendsys::createApplicationInfo( engine, apiVersion ),
			layerNames,
			extensionNames,
		};
		log::debug << "Enabled layers count: " << uint32_t( layerNames.size() ) << std::endl;
		log::debug << "Enabled extensions count: " << uint32_t( extensionNames.size() ) << std::endl;
		return std::make_unique< ashes::Instance >( std::move( plugin )
#if C3D_UseAllocationCallbacks
			, ashes::makeAllocator< AlignedBuddyAllocatorTraits >()
#else
			, nullptr
#endif
			, std::move( createInfo ) );
	}

	void RenderSystem::completeLayerNames( Engine const & engine
		, ashes::VkLayerPropertiesArray const & layers
		, ashes::StringArray & names )
	{
		for ( auto const & props : layers )
		{
			if ( ( engine.isValidationEnabled()
				&& ( rendsys::isValidationLayer( props.layerName, props.description )
					|| rendsys::isSynchronisationLayer( props.layerName, props.description ) ) )
				|| ( engine.isApiTraceEnabled()
					&& rendsys::isApiTraceLayer( props.layerName, props.description ) ) )
			{
				names.push_back( props.layerName );
			}
		}
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
		if ( module.shader )
		{
			return compileShader( module.stage
				, module.name
				, *module.shader );
		}

		return compileShader( module.stage
			, module.name
			, module.source );
	}

	SpirVShader RenderSystem::compileShader( VkShaderStageFlagBits stage
		, castor::String const & name
		, ast::Shader const & shader )const
	{
		log::debug << "Compiling " << ashes::getName( stage ) << " shader [" << name << "] ...";
		SpirVShader result;
		auto availableExtensions = rendsys::listSpirVExtensions( *m_device );
		spirv::SpirVConfig spirvConfig{ rendsys::getSpirVVersion( m_properties.apiVersion )
			, &availableExtensions };
		log::debug << " SPV ...";
		result.spirv = spirv::serialiseSpirv( shader, spirvConfig );

#if !defined( NDEBUG )
#	if C3D_HasGLSL
		log::debug << " GLSL ...";
		glsl::GlslConfig config{ shader.getType()
			, glsl::v4_6
			, rendsys::getGLSLExtensions( glsl::v4_6 )
			, true
			, false
			, true
			, true
			, true
			, true };
		std::string glsl = glsl::compileGlsl( shader
			, ast::SpecialisationInfo{}
			, config );
#	else
		std::string glsl;
#	endif

		result.text = glsl + "\n" + spirv::writeSpirv( shader, spirvConfig );
		//log::trace << result.text << std::endl;

#	if C3D_HasSPIRVCross && C3D_DebugSpirV && C3D_HasGlslang
		std::string name = name + "_" + ashes::getName( stage );

		try
		{
			log::debug << " Validation ...";
			auto glslFromSpv = rendsys::compileSpvToGlsl( getRenderDevice()
				, result.spirv
				, stage );
			const_cast< castor3d::ShaderModule & >( module ).source += "\n" + glslFromSpv;
		}
		catch ( std::exception & exc )
		{
			log::error << result.text << std::endl;
			log::error << exc.what() << std::endl;
			{
				castor::BinaryFile file{ castor::File::getExecutableDirectory() / ( name + "_sdw.spv" )
					, castor::File::OpenMode::eWrite };
				file.writeArray( result.spirv.data()
					, result.spirv.size() );
			}

			auto ref = castor3d::compileGlslToSpv( getRenderDevice()
				, stage
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

		log::debug << " Done." << std::endl;
		return result;
	}

	SpirVShader RenderSystem::compileShader( VkShaderStageFlagBits stage
		, castor::String const & name
		, castor::String const & glsl )const
	{
		SpirVShader result;
		CU_Require( !glsl.empty() );
		result.text = glsl;
		result.spirv = castor3d::compileGlslToSpv( getRenderDevice()
			, stage
			, glsl );
		return result;
	}

	castor::Matrix4x4f RenderSystem::getFrustum( float left
		, float right
		, float bottom
		, float top
		, float zNear
		, float zFar )const
	{
		return convert( m_renderer.instance->frustum( left, right, bottom, top, zNear, zFar ) );
	}

	castor::Matrix4x4f RenderSystem::getPerspective( castor::Angle const & fovy
		, float aspect
		, float zNear
		, float zFar )const
	{
		return convert( m_renderer.instance->perspective( fovy.radians(), aspect, zNear, zFar ) );
	}

	castor::Matrix4x4f RenderSystem::getOrtho( float left
		, float right
		, float bottom
		, float top
		, float zNear
		, float zFar )const
	{
		return convert( m_renderer.instance->ortho( left, right, bottom, top, zNear, zFar ) );
	}

	castor::Matrix4x4f RenderSystem::getInfinitePerspective( float radiansFovY
		, float aspect
		, float zNear )const
	{
		return convert( m_renderer.instance->infinitePerspective( radiansFovY, aspect, zNear ) );
	}

	Texture const & RenderSystem::getPrefilteredBrdfTexture()const
	{
		return getEngine()->getPrefilteredBrdfTexture();
	}

	//*************************************************************************
}
