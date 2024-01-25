#include "FFTOceanRendering/ProcessFFTPass.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/ComputePipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>

#include <ShaderWriter/Source.hpp>

namespace ocean_fft
{
	//*********************************************************************************************

	namespace procfft
	{
		static castor::String getName( VkFFTResult result )
		{
			switch ( result )
			{
			case VKFFT_SUCCESS:
				return cuT( "SUCCESS" );
			case VKFFT_ERROR_MALLOC_FAILED:
				return cuT( "ERROR_MALLOC_FAILED" );
			case VKFFT_ERROR_INSUFFICIENT_CODE_BUFFER:
				return cuT( "ERROR_INSUFFICIENT_CODE_BUFFER" );
			case VKFFT_ERROR_INSUFFICIENT_TEMP_BUFFER:
				return cuT( "ERROR_INSUFFICIENT_TEMP_BUFFER" );
			case VKFFT_ERROR_PLAN_NOT_INITIALIZED:
				return cuT( "ERROR_PLAN_NOT_INITIALIZED" );
			case VKFFT_ERROR_NULL_TEMP_PASSED:
				return cuT( "ERROR_NULL_TEMP_PASSED" );
			case VKFFT_ERROR_INVALID_PHYSICAL_DEVICE:
				return cuT( "ERROR_INVALID_PHYSICAL_DEVICE" );
			case VKFFT_ERROR_INVALID_DEVICE:
				return cuT( "ERROR_INVALID_DEVICE" );
			case VKFFT_ERROR_INVALID_QUEUE:
				return cuT( "ERROR_INVALID_QUEUE" );
			case VKFFT_ERROR_INVALID_COMMAND_POOL:
				return cuT( "ERROR_INVALID_COMMAND_POOL" );
			case VKFFT_ERROR_INVALID_FENCE:
				return cuT( "ERROR_INVALID_FENCE" );
			case VKFFT_ERROR_ONLY_FORWARD_FFT_INITIALIZED:
				return cuT( "ERROR_ONLY_FORWARD_FFT_INITIALIZED" );
			case VKFFT_ERROR_ONLY_INVERSE_FFT_INITIALIZED:
				return cuT( "ERROR_ONLY_INVERSE_FFT_INITIALIZED" );
			case VKFFT_ERROR_INVALID_CONTEXT:
				return cuT( "ERROR_INVALID_CONTEXT" );
			case VKFFT_ERROR_INVALID_PLATFORM:
				return cuT( "ERROR_INVALID_PLATFORM" );
			case VKFFT_ERROR_ENABLED_saveApplicationToString:
				return cuT( "ERROR_ENABLED_saveApplicationToString" );
			case VKFFT_ERROR_EMPTY_FFTdim:
				return cuT( "ERROR_EMPTY_FFTdim" );
			case VKFFT_ERROR_EMPTY_size:
				return cuT( "ERROR_EMPTY_size" );
			case VKFFT_ERROR_EMPTY_bufferSize:
				return cuT( "ERROR_EMPTY_bufferSize" );
			case VKFFT_ERROR_EMPTY_buffer:
				return cuT( "ERROR_EMPTY_buffer" );
			case VKFFT_ERROR_EMPTY_tempBufferSize:
				return cuT( "ERROR_EMPTY_tempBufferSize" );
			case VKFFT_ERROR_EMPTY_tempBuffer:
				return cuT( "ERROR_EMPTY_tempBuffer" );
			case VKFFT_ERROR_EMPTY_inputBufferSize:
				return cuT( "ERROR_EMPTY_inputBufferSize" );
			case VKFFT_ERROR_EMPTY_inputBuffer:
				return cuT( "ERROR_EMPTY_inputBuffer" );
			case VKFFT_ERROR_EMPTY_outputBufferSize:
				return cuT( "ERROR_EMPTY_outputBufferSize" );
			case VKFFT_ERROR_EMPTY_outputBuffer:
				return cuT( "ERROR_EMPTY_outputBuffer" );
			case VKFFT_ERROR_EMPTY_kernelSize:
				return cuT( "ERROR_EMPTY_kernelSize" );
			case VKFFT_ERROR_EMPTY_kernel:
				return cuT( "ERROR_EMPTY_kernel" );
			case VKFFT_ERROR_EMPTY_applicationString:
				return cuT( "ERROR_EMPTY_applicationString" );
			case VKFFT_ERROR_UNSUPPORTED_RADIX:
				return cuT( "ERROR_UNSUPPORTED_RADIX" );
			case VKFFT_ERROR_UNSUPPORTED_FFT_LENGTH:
				return cuT( "ERROR_UNSUPPORTED_FFT_LENGTH" );
			case VKFFT_ERROR_UNSUPPORTED_FFT_LENGTH_R2C:
				return cuT( "ERROR_UNSUPPORTED_FFT_LENGTH_R2C" );
			case VKFFT_ERROR_UNSUPPORTED_FFT_LENGTH_DCT:
				return cuT( "ERROR_UNSUPPORTED_FFT_LENGTH_DCT" );
			case VKFFT_ERROR_UNSUPPORTED_FFT_OMIT:
				return cuT( "ERROR_UNSUPPORTED_FFT_OMIT" );
			case VKFFT_ERROR_FAILED_TO_ALLOCATE:
				return cuT( "ERROR_FAILED_TO_ALLOCATE" );
			case VKFFT_ERROR_FAILED_TO_MAP_MEMORY:
				return cuT( "ERROR_FAILED_TO_MAP_MEMORY" );
			case VKFFT_ERROR_FAILED_TO_ALLOCATE_COMMAND_BUFFERS:
				return cuT( "ERROR_FAILED_TO_ALLOCATE_COMMAND_BUFFERS" );
			case VKFFT_ERROR_FAILED_TO_BEGIN_COMMAND_BUFFER:
				return cuT( "ERROR_FAILED_TO_BEGIN_COMMAND_BUFFER" );
			case VKFFT_ERROR_FAILED_TO_END_COMMAND_BUFFER:
				return cuT( "ERROR_FAILED_TO_END_COMMAND_BUFFER" );
			case VKFFT_ERROR_FAILED_TO_SUBMIT_QUEUE:
				return cuT( "ERROR_FAILED_TO_SUBMIT_QUEUE" );
			case VKFFT_ERROR_FAILED_TO_WAIT_FOR_FENCES:
				return cuT( "ERROR_FAILED_TO_WAIT_FOR_FENCES" );
			case VKFFT_ERROR_FAILED_TO_RESET_FENCES:
				return cuT( "ERROR_FAILED_TO_RESET_FENCES" );
			case VKFFT_ERROR_FAILED_TO_CREATE_DESCRIPTOR_POOL:
				return cuT( "ERROR_FAILED_TO_CREATE_DESCRIPTOR_POOL" );
			case VKFFT_ERROR_FAILED_TO_CREATE_DESCRIPTOR_SET_LAYOUT:
				return cuT( "ERROR_FAILED_TO_CREATE_DESCRIPTOR_SET_LAYOUT" );
			case VKFFT_ERROR_FAILED_TO_ALLOCATE_DESCRIPTOR_SETS:
				return cuT( "ERROR_FAILED_TO_ALLOCATE_DESCRIPTOR_SETS" );
			case VKFFT_ERROR_FAILED_TO_CREATE_PIPELINE_LAYOUT:
				return cuT( "ERROR_FAILED_TO_CREATE_PIPELINE_LAYOUT" );
			case VKFFT_ERROR_FAILED_SHADER_PREPROCESS:
				return cuT( "ERROR_FAILED_SHADER_PREPROCESS" );
			case VKFFT_ERROR_FAILED_SHADER_PARSE:
				return cuT( "ERROR_FAILED_SHADER_PARSE" );
			case VKFFT_ERROR_FAILED_SHADER_LINK:
				return cuT( "ERROR_FAILED_SHADER_LINK" );
			case VKFFT_ERROR_FAILED_SPIRV_GENERATE:
				return cuT( "ERROR_FAILED_SPIRV_GENERATE" );
			case VKFFT_ERROR_FAILED_TO_CREATE_SHADER_MODULE:
				return cuT( "ERROR_FAILED_TO_CREATE_SHADER_MODULE" );
			case VKFFT_ERROR_FAILED_TO_CREATE_INSTANCE:
				return cuT( "ERROR_FAILED_TO_CREATE_INSTANCE" );
			case VKFFT_ERROR_FAILED_TO_SETUP_DEBUG_MESSENGER:
				return cuT( "ERROR_FAILED_TO_SETUP_DEBUG_MESSENGER" );
			case VKFFT_ERROR_FAILED_TO_FIND_PHYSICAL_DEVICE:
				return cuT( "ERROR_FAILED_TO_FIND_PHYSICAL_DEVICE" );
			case VKFFT_ERROR_FAILED_TO_CREATE_DEVICE:
				return cuT( "ERROR_FAILED_TO_CREATE_DEVICE" );
			case VKFFT_ERROR_FAILED_TO_CREATE_FENCE:
				return cuT( "ERROR_FAILED_TO_CREATE_FENCE" );
			case VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_POOL:
				return cuT( "ERROR_FAILED_TO_CREATE_COMMAND_POOL" );
			case VKFFT_ERROR_FAILED_TO_CREATE_BUFFER:
				return cuT( "ERROR_FAILED_TO_CREATE_BUFFER" );
			case VKFFT_ERROR_FAILED_TO_ALLOCATE_MEMORY:
				return cuT( "ERROR_FAILED_TO_ALLOCATE_MEMORY" );
			case VKFFT_ERROR_FAILED_TO_BIND_BUFFER_MEMORY:
				return cuT( "ERROR_FAILED_TO_BIND_BUFFER_MEMORY" );
			case VKFFT_ERROR_FAILED_TO_FIND_MEMORY:
				return cuT( "ERROR_FAILED_TO_FIND_MEMORY" );
			case VKFFT_ERROR_FAILED_TO_SYNCHRONIZE:
				return cuT( "ERROR_FAILED_TO_SYNCHRONIZE" );
			case VKFFT_ERROR_FAILED_TO_COPY:
				return cuT( "ERROR_FAILED_TO_COPY" );
			case VKFFT_ERROR_FAILED_TO_CREATE_PROGRAM:
				return cuT( "ERROR_FAILED_TO_CREATE_PROGRAM" );
			case VKFFT_ERROR_FAILED_TO_COMPILE_PROGRAM:
				return cuT( "ERROR_FAILED_TO_COMPILE_PROGRAM" );
			case VKFFT_ERROR_FAILED_TO_GET_CODE_SIZE:
				return cuT( "ERROR_FAILED_TO_GET_CODE_SIZE" );
			case VKFFT_ERROR_FAILED_TO_GET_CODE:
				return cuT( "ERROR_FAILED_TO_GET_CODE" );
			case VKFFT_ERROR_FAILED_TO_DESTROY_PROGRAM:
				return cuT( "ERROR_FAILED_TO_DESTROY_PROGRAM" );
			case VKFFT_ERROR_FAILED_TO_LOAD_MODULE:
				return cuT( "ERROR_FAILED_TO_LOAD_MODULE" );
			case VKFFT_ERROR_FAILED_TO_GET_FUNCTION:
				return cuT( "ERROR_FAILED_TO_GET_FUNCTION" );
			case VKFFT_ERROR_FAILED_TO_SET_DYNAMIC_SHARED_MEMORY:
				return cuT( "ERROR_FAILED_TO_SET_DYNAMIC_SHARED_MEMORY" );
			case VKFFT_ERROR_FAILED_TO_MODULE_GET_GLOBAL:
				return cuT( "ERROR_FAILED_TO_MODULE_GET_GLOBAL" );
			case VKFFT_ERROR_FAILED_TO_LAUNCH_KERNEL:
				return cuT( "ERROR_FAILED_TO_LAUNCH_KERNEL" );
			case VKFFT_ERROR_FAILED_TO_EVENT_RECORD:
				return cuT( "ERROR_FAILED_TO_EVENT_RECORD" );
			case VKFFT_ERROR_FAILED_TO_ADD_NAME_EXPRESSION:
				return cuT( "ERROR_FAILED_TO_ADD_NAME_EXPRESSION" );
			case VKFFT_ERROR_FAILED_TO_INITIALIZE:
				return cuT( "ERROR_FAILED_TO_INITIALIZE" );
			case VKFFT_ERROR_FAILED_TO_SET_DEVICE_ID:
				return cuT( "ERROR_FAILED_TO_SET_DEVICE_ID" );
			case VKFFT_ERROR_FAILED_TO_GET_DEVICE:
				return cuT( "ERROR_FAILED_TO_GET_DEVICE" );
			case VKFFT_ERROR_FAILED_TO_CREATE_CONTEXT:
				return cuT( "ERROR_FAILED_TO_CREATE_CONTEXT" );
			case VKFFT_ERROR_FAILED_TO_CREATE_PIPELINE:
				return cuT( "ERROR_FAILED_TO_CREATE_PIPELINE" );
			case VKFFT_ERROR_FAILED_TO_SET_KERNEL_ARG:
				return cuT( "ERROR_FAILED_TO_SET_KERNEL_ARG" );
			case VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_QUEUE:
				return cuT( "ERROR_FAILED_TO_CREATE_COMMAND_QUEUE" );
			case VKFFT_ERROR_FAILED_TO_RELEASE_COMMAND_QUEUE:
				return cuT( "ERROR_FAILED_TO_RELEASE_COMMAND_QUEUE" );
			case VKFFT_ERROR_FAILED_TO_ENUMERATE_DEVICES:
				return cuT( "ERROR_FAILED_TO_ENUMERATE_DEVICES" );
			case VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE:
				return cuT( "ERROR_FAILED_TO_GET_ATTRIBUTE" );
			case VKFFT_ERROR_FAILED_TO_CREATE_EVENT:
				return cuT( "ERROR_FAILED_TO_CREATE_EVENT" );
			default:
				return cuT( "ERROR_UNKNOWN" );
			}
		}

		static castor::String getErrorText( castor::xchar const * action
			, VkFFTResult result )
		{
			auto stream = castor::makeStringStream();
			stream << cuT( "Error during " ) << action << cuT( ": " ) << getName( result );
			return stream.str();
		}

		static VkFFTApplication createApp( VkFFTConfig const & config
			, castor3d::RenderDevice const & device
			, VkExtent2D const & extent
			, VkDeviceSize & inBufferSize
			, VkBuffer & vkInput
			, VkDeviceSize & outBufferSize
			, castor::Array< VkBuffer, 2u > & vkOutput )
		{
			static castor::Mutex mutex;
			auto lock( castor::makeUniqueLock( mutex ) );
			auto graphics = device.graphicsData();
			VkQueue vkQueue = *graphics->queue;
			VkCommandPool vkCommandPool = *graphics->commandPool;
			VkFFTConfiguration fftConfig{};

			fftConfig.physicalDevice = &config.vkPhysicalDevice;
			fftConfig.device = &config.vkDevice;
			fftConfig.queue = &vkQueue;
			fftConfig.commandPool = &vkCommandPool;
			fftConfig.fence = &config.vkFence;
			fftConfig.isCompilerInitialized = 0;

			fftConfig.FFTdim = 2;
			fftConfig.size[0] = extent.width;
			fftConfig.size[1] = extent.height;
			fftConfig.bufferNum = 1u;
			fftConfig.buffer = &vkOutput[0];
			fftConfig.bufferSize = &outBufferSize;
			fftConfig.inputBufferNum = 1u;
			fftConfig.inputBuffer = &vkInput;
			fftConfig.isInputFormatted = 1u;
			fftConfig.inputBufferSize = &inBufferSize;

			VkFFTApplication result{};
			checkFFTResultMandat( cuT( "VkFFT initialisation" )
				, initializeVkFFT( &result, fftConfig ) );
			return result;
		}
	}

	//************************************************************************************************

	bool checkFFTResult( castor::xchar const * action
		, VkFFTResult result )
	{
		if ( result == VKFFT_SUCCESS )
		{
			return true;
		}

		castor3d::log::error << procfft::getErrorText( action, result ) << std::endl;
		return false;
	}

	void checkFFTResultMandat( castor::xchar const * action
		, VkFFTResult result )
	{
		if ( !checkFFTResult( action, result ) )
		{
			CU_Exception( castor::toUtf8( procfft::getErrorText( action, result ) ) );
		}
	}

	//************************************************************************************************

	VkFFTConfig::VkFFTConfig( castor3d::RenderDevice const & device
			, VkExtent2D const & dimensions )
		: device{ device }
		, fence{ device->createFence( "OceanFFT" ) }
		, vkPhysicalDevice{ device->getPhysicalDevice() }
		, vkDevice{ *device }
		, vkFence{ *fence }
	{
	}

	//************************************************************************************************

	castor::String const ProcessFFTPass::Name{ cuT( "GenerateHeightmap" ) };

	ProcessFFTPass::ProcessFFTPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, castor3d::RenderDevice const & device
		, VkFFTConfig const & config
		, VkExtent2D const & extent
		, ashes::BufferBase const & input
		, castor::Array< ashes::BufferBasePtr, 2u > const & output
		, crg::RunnablePass::IsEnabledCallback isEnabled )
		: crg::RunnablePass{ pass
			, context
			, graph
			, { []( uint32_t index ){}
				, GetPipelineStateCallback( [](){ return crg::getPipelineState( VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT ); } )
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doRecordInto( context, cb, i ); }
				, GetPassIndexCallback( [this](){ return doGetPassIndex(); } )
				, isEnabled
				, IsComputePassCallback( [this](){ return doIsComputePass(); } ) }
			, { 1u } }
		, m_device{ device }
		, m_extent{ extent }
		, m_inBufferSize{ input.getSize() }
		, m_vkInput{ input }
		, m_outBufferSize{ output[0]->getSize() }
		, m_vkOutput{ *output[0], *output[1] }
		, m_app{ procfft::createApp( config
			, device
			, m_extent
			, m_inBufferSize
			, m_vkInput
			, m_outBufferSize
			, m_vkOutput ) }
	{
	}

	ProcessFFTPass::~ProcessFFTPass()
	{
		deleteVkFFT( &m_app );
	}

	void ProcessFFTPass::accept( castor3d::RenderTechniqueVisitor & visitor )
	{
	}

	void ProcessFFTPass::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		VkFFTLaunchParams launchParams{};
		launchParams.commandBuffer = &commandBuffer;
		launchParams.inputBuffer = &m_vkInput;
		launchParams.buffer = &m_vkOutput[0] ;
		checkFFTResult( cuT( "VkFFT recording" )
			, VkFFTAppend( &m_app, -1, &launchParams ) );
	}

	uint32_t ProcessFFTPass::doGetPassIndex()const
	{
		return 0u;
	}

	bool ProcessFFTPass::doIsComputePass()const
	{
		return true;
	}

	//************************************************************************************************

	crg::FramePass const & createProcessFFTPass( castor::String const & name
		, castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, VkExtent2D const & extent
		, VkFFTConfig const & config
		, ashes::BufferBase const & input
		, castor::Array< ashes::BufferBasePtr, 2u > const & output )
	{
		auto mbName = castor::toUtf8( name );
		auto & result = graph.createPass( "Process" + mbName
			, [&device, extent, &input, &output, &config]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = castor::make_unique< ProcessFFTPass >( framePass
					, context
					, runnableGraph
					, device
					, config
					, extent
					, input
					, output
					, crg::RunnablePass::IsEnabledCallback( [](){ return true; } ) );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		result.addDependency( previousPass );
		result.addInputStorageBuffer( { input, mbName + "Frequency" }
			, ProcessFFTPass::eInput
			, 0u
			, input.getSize() );
		result.addOutputStorageBuffer( { *output.front(), mbName + "FFTResult" }
			, ProcessFFTPass::eOutput
			, 0u
			, output.front()->getSize() );
		return result;
	}

	//************************************************************************************************
}
