#include "FFTOceanRendering/ProcessFFTPass.hpp"

#include "FFTOceanRendering/OceanFFTRenderPass.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
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

	namespace
	{
		castor::String getName( VkFFTResult result )
		{
			switch ( result )
			{
			case VKFFT_SUCCESS:
				return "SUCCESS";
			case VKFFT_ERROR_MALLOC_FAILED:
				return "ERROR_MALLOC_FAILED";
			case VKFFT_ERROR_INSUFFICIENT_CODE_BUFFER:
				return "ERROR_INSUFFICIENT_CODE_BUFFER";
			case VKFFT_ERROR_INSUFFICIENT_TEMP_BUFFER:
				return "ERROR_INSUFFICIENT_TEMP_BUFFER";
			case VKFFT_ERROR_PLAN_NOT_INITIALIZED:
				return "ERROR_PLAN_NOT_INITIALIZED";
			case VKFFT_ERROR_NULL_TEMP_PASSED:
				return "ERROR_NULL_TEMP_PASSED";
			case VKFFT_ERROR_INVALID_PHYSICAL_DEVICE:
				return "ERROR_INVALID_PHYSICAL_DEVICE";
			case VKFFT_ERROR_INVALID_DEVICE:
				return "ERROR_INVALID_DEVICE";
			case VKFFT_ERROR_INVALID_QUEUE:
				return "ERROR_INVALID_QUEUE";
			case VKFFT_ERROR_INVALID_COMMAND_POOL:
				return "ERROR_INVALID_COMMAND_POOL";
			case VKFFT_ERROR_INVALID_FENCE:
				return "ERROR_INVALID_FENCE";
			case VKFFT_ERROR_ONLY_FORWARD_FFT_INITIALIZED:
				return "ERROR_ONLY_FORWARD_FFT_INITIALIZED";
			case VKFFT_ERROR_ONLY_INVERSE_FFT_INITIALIZED:
				return "ERROR_ONLY_INVERSE_FFT_INITIALIZED";
			case VKFFT_ERROR_INVALID_CONTEXT:
				return "ERROR_INVALID_CONTEXT";
			case VKFFT_ERROR_INVALID_PLATFORM:
				return "ERROR_INVALID_PLATFORM";
			case VKFFT_ERROR_ENABLED_saveApplicationToString:
				return "ERROR_ENABLED_saveApplicationToString";
			case VKFFT_ERROR_EMPTY_FFTdim:
				return "ERROR_EMPTY_FFTdim";
			case VKFFT_ERROR_EMPTY_size:
				return "ERROR_EMPTY_size";
			case VKFFT_ERROR_EMPTY_bufferSize:
				return "ERROR_EMPTY_bufferSize";
			case VKFFT_ERROR_EMPTY_buffer:
				return "ERROR_EMPTY_buffer";
			case VKFFT_ERROR_EMPTY_tempBufferSize:
				return "ERROR_EMPTY_tempBufferSize";
			case VKFFT_ERROR_EMPTY_tempBuffer:
				return "ERROR_EMPTY_tempBuffer";
			case VKFFT_ERROR_EMPTY_inputBufferSize:
				return "ERROR_EMPTY_inputBufferSize";
			case VKFFT_ERROR_EMPTY_inputBuffer:
				return "ERROR_EMPTY_inputBuffer";
			case VKFFT_ERROR_EMPTY_outputBufferSize:
				return "ERROR_EMPTY_outputBufferSize";
			case VKFFT_ERROR_EMPTY_outputBuffer:
				return "ERROR_EMPTY_outputBuffer";
			case VKFFT_ERROR_EMPTY_kernelSize:
				return "ERROR_EMPTY_kernelSize";
			case VKFFT_ERROR_EMPTY_kernel:
				return "ERROR_EMPTY_kernel";
			case VKFFT_ERROR_EMPTY_applicationString:
				return "ERROR_EMPTY_applicationString";
			case VKFFT_ERROR_UNSUPPORTED_RADIX:
				return "ERROR_UNSUPPORTED_RADIX";
			case VKFFT_ERROR_UNSUPPORTED_FFT_LENGTH:
				return "ERROR_UNSUPPORTED_FFT_LENGTH";
			case VKFFT_ERROR_UNSUPPORTED_FFT_LENGTH_R2C:
				return "ERROR_UNSUPPORTED_FFT_LENGTH_R2C";
			case VKFFT_ERROR_UNSUPPORTED_FFT_LENGTH_DCT:
				return "ERROR_UNSUPPORTED_FFT_LENGTH_DCT";
			case VKFFT_ERROR_UNSUPPORTED_FFT_OMIT:
				return "ERROR_UNSUPPORTED_FFT_OMIT";
			case VKFFT_ERROR_FAILED_TO_ALLOCATE:
				return "ERROR_FAILED_TO_ALLOCATE";
			case VKFFT_ERROR_FAILED_TO_MAP_MEMORY:
				return "ERROR_FAILED_TO_MAP_MEMORY";
			case VKFFT_ERROR_FAILED_TO_ALLOCATE_COMMAND_BUFFERS:
				return "ERROR_FAILED_TO_ALLOCATE_COMMAND_BUFFERS";
			case VKFFT_ERROR_FAILED_TO_BEGIN_COMMAND_BUFFER:
				return "ERROR_FAILED_TO_BEGIN_COMMAND_BUFFER";
			case VKFFT_ERROR_FAILED_TO_END_COMMAND_BUFFER:
				return "ERROR_FAILED_TO_END_COMMAND_BUFFER";
			case VKFFT_ERROR_FAILED_TO_SUBMIT_QUEUE:
				return "ERROR_FAILED_TO_SUBMIT_QUEUE";
			case VKFFT_ERROR_FAILED_TO_WAIT_FOR_FENCES:
				return "ERROR_FAILED_TO_WAIT_FOR_FENCES";
			case VKFFT_ERROR_FAILED_TO_RESET_FENCES:
				return "ERROR_FAILED_TO_RESET_FENCES";
			case VKFFT_ERROR_FAILED_TO_CREATE_DESCRIPTOR_POOL:
				return "ERROR_FAILED_TO_CREATE_DESCRIPTOR_POOL";
			case VKFFT_ERROR_FAILED_TO_CREATE_DESCRIPTOR_SET_LAYOUT:
				return "ERROR_FAILED_TO_CREATE_DESCRIPTOR_SET_LAYOUT";
			case VKFFT_ERROR_FAILED_TO_ALLOCATE_DESCRIPTOR_SETS:
				return "ERROR_FAILED_TO_ALLOCATE_DESCRIPTOR_SETS";
			case VKFFT_ERROR_FAILED_TO_CREATE_PIPELINE_LAYOUT:
				return "ERROR_FAILED_TO_CREATE_PIPELINE_LAYOUT";
			case VKFFT_ERROR_FAILED_SHADER_PREPROCESS:
				return "ERROR_FAILED_SHADER_PREPROCESS";
			case VKFFT_ERROR_FAILED_SHADER_PARSE:
				return "ERROR_FAILED_SHADER_PARSE";
			case VKFFT_ERROR_FAILED_SHADER_LINK:
				return "ERROR_FAILED_SHADER_LINK";
			case VKFFT_ERROR_FAILED_SPIRV_GENERATE:
				return "ERROR_FAILED_SPIRV_GENERATE";
			case VKFFT_ERROR_FAILED_TO_CREATE_SHADER_MODULE:
				return "ERROR_FAILED_TO_CREATE_SHADER_MODULE";
			case VKFFT_ERROR_FAILED_TO_CREATE_INSTANCE:
				return "ERROR_FAILED_TO_CREATE_INSTANCE";
			case VKFFT_ERROR_FAILED_TO_SETUP_DEBUG_MESSENGER:
				return "ERROR_FAILED_TO_SETUP_DEBUG_MESSENGER";
			case VKFFT_ERROR_FAILED_TO_FIND_PHYSICAL_DEVICE:
				return "ERROR_FAILED_TO_FIND_PHYSICAL_DEVICE";
			case VKFFT_ERROR_FAILED_TO_CREATE_DEVICE:
				return "ERROR_FAILED_TO_CREATE_DEVICE";
			case VKFFT_ERROR_FAILED_TO_CREATE_FENCE:
				return "ERROR_FAILED_TO_CREATE_FENCE";
			case VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_POOL:
				return "ERROR_FAILED_TO_CREATE_COMMAND_POOL";
			case VKFFT_ERROR_FAILED_TO_CREATE_BUFFER:
				return "ERROR_FAILED_TO_CREATE_BUFFER";
			case VKFFT_ERROR_FAILED_TO_ALLOCATE_MEMORY:
				return "ERROR_FAILED_TO_ALLOCATE_MEMORY";
			case VKFFT_ERROR_FAILED_TO_BIND_BUFFER_MEMORY:
				return "ERROR_FAILED_TO_BIND_BUFFER_MEMORY";
			case VKFFT_ERROR_FAILED_TO_FIND_MEMORY:
				return "ERROR_FAILED_TO_FIND_MEMORY";
			case VKFFT_ERROR_FAILED_TO_SYNCHRONIZE:
				return "ERROR_FAILED_TO_SYNCHRONIZE";
			case VKFFT_ERROR_FAILED_TO_COPY:
				return "ERROR_FAILED_TO_COPY";
			case VKFFT_ERROR_FAILED_TO_CREATE_PROGRAM:
				return "ERROR_FAILED_TO_CREATE_PROGRAM";
			case VKFFT_ERROR_FAILED_TO_COMPILE_PROGRAM:
				return "ERROR_FAILED_TO_COMPILE_PROGRAM";
			case VKFFT_ERROR_FAILED_TO_GET_CODE_SIZE:
				return "ERROR_FAILED_TO_GET_CODE_SIZE";
			case VKFFT_ERROR_FAILED_TO_GET_CODE:
				return "ERROR_FAILED_TO_GET_CODE";
			case VKFFT_ERROR_FAILED_TO_DESTROY_PROGRAM:
				return "ERROR_FAILED_TO_DESTROY_PROGRAM";
			case VKFFT_ERROR_FAILED_TO_LOAD_MODULE:
				return "ERROR_FAILED_TO_LOAD_MODULE";
			case VKFFT_ERROR_FAILED_TO_GET_FUNCTION:
				return "ERROR_FAILED_TO_GET_FUNCTION";
			case VKFFT_ERROR_FAILED_TO_SET_DYNAMIC_SHARED_MEMORY:
				return "ERROR_FAILED_TO_SET_DYNAMIC_SHARED_MEMORY";
			case VKFFT_ERROR_FAILED_TO_MODULE_GET_GLOBAL:
				return "ERROR_FAILED_TO_MODULE_GET_GLOBAL";
			case VKFFT_ERROR_FAILED_TO_LAUNCH_KERNEL:
				return "ERROR_FAILED_TO_LAUNCH_KERNEL";
			case VKFFT_ERROR_FAILED_TO_EVENT_RECORD:
				return "ERROR_FAILED_TO_EVENT_RECORD";
			case VKFFT_ERROR_FAILED_TO_ADD_NAME_EXPRESSION:
				return "ERROR_FAILED_TO_ADD_NAME_EXPRESSION";
			case VKFFT_ERROR_FAILED_TO_INITIALIZE:
				return "ERROR_FAILED_TO_INITIALIZE";
			case VKFFT_ERROR_FAILED_TO_SET_DEVICE_ID:
				return "ERROR_FAILED_TO_SET_DEVICE_ID";
			case VKFFT_ERROR_FAILED_TO_GET_DEVICE:
				return "ERROR_FAILED_TO_GET_DEVICE";
			case VKFFT_ERROR_FAILED_TO_CREATE_CONTEXT:
				return "ERROR_FAILED_TO_CREATE_CONTEXT";
			case VKFFT_ERROR_FAILED_TO_CREATE_PIPELINE:
				return "ERROR_FAILED_TO_CREATE_PIPELINE";
			case VKFFT_ERROR_FAILED_TO_SET_KERNEL_ARG:
				return "ERROR_FAILED_TO_SET_KERNEL_ARG";
			case VKFFT_ERROR_FAILED_TO_CREATE_COMMAND_QUEUE:
				return "ERROR_FAILED_TO_CREATE_COMMAND_QUEUE";
			case VKFFT_ERROR_FAILED_TO_RELEASE_COMMAND_QUEUE:
				return "ERROR_FAILED_TO_RELEASE_COMMAND_QUEUE";
			case VKFFT_ERROR_FAILED_TO_ENUMERATE_DEVICES:
				return "ERROR_FAILED_TO_ENUMERATE_DEVICES";
			case VKFFT_ERROR_FAILED_TO_GET_ATTRIBUTE:
				return "ERROR_FAILED_TO_GET_ATTRIBUTE";
			case VKFFT_ERROR_FAILED_TO_CREATE_EVENT:
				return "ERROR_FAILED_TO_CREATE_EVENT";
			default:
				return "ERROR_UNKNOWN";
			}
		}

		castor::String getErrorText( castor::xchar const * action
			, VkFFTResult result )
		{
			auto stream = castor::makeStringStream();
			stream << cuT( "Error during " ) << action << cuT( ": " ) << getName( result );
			return stream.str();
		}

		VkFFTApplication createApp( FFTConfig const & config
			, VkExtent2D const & extent
			, VkDeviceSize & inBufferSize
			, VkBuffer & vkInput
			, VkDeviceSize & outBufferSize
			, std::array< VkBuffer, 2u > & vkOutput )
		{
			static std::mutex mutex;
			auto lock( castor::makeUniqueLock( mutex ) );

			VkFFTConfiguration fftConfig{};

			fftConfig.physicalDevice = &config.vkPhysicalDevice;
			fftConfig.device = &config.vkDevice;
			fftConfig.queue = &config.vkQueue;
			fftConfig.commandPool = &config.vkCommandPool;
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
			checkFFTResultMandat( "VkFFT initialisation"
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

		castor3d::log::error << getErrorText( action, result ) << std::endl;
		return false;
	}

	void checkFFTResultMandat( castor::xchar const * action
		, VkFFTResult result )
	{
		if ( !checkFFTResult( action, result ) )
		{
			CU_Exception( getErrorText( action, result ) );
		}
	}

	//************************************************************************************************

	FFTConfig::FFTConfig( castor3d::RenderDevice const & device
			, VkExtent2D const & dimensions )
		: device{ device }
		, queueData{ *device.reserveGraphicsData() }
		, fence{ device->createFence( "OceanFFT" ) }
		, vkPhysicalDevice{ device->getPhysicalDevice() }
		, vkDevice{ *device }
		, vkQueue{ *queueData.queue }
		, vkCommandPool{ *queueData.commandPool }
		, vkFence{ *fence }
	{
	}

	//************************************************************************************************

	castor::String const ProcessFFTPass::Name{ "GenerateHeightmap" };

	ProcessFFTPass::ProcessFFTPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, castor3d::RenderDevice const & device
		, FFTConfig const & config
		, VkExtent2D const & extent
		, ashes::BufferBase const & input
		, std::array< ashes::BufferBasePtr, 2u > const & output
		, crg::RunnablePass::IsEnabledCallback isEnabled )
		: crg::RunnablePass{ pass
			, context
			, graph
			, { [this](){ doInitialise(); }
				, GetSemaphoreWaitFlagsCallback( [this](){ return doGetSemaphoreWaitFlags(); } )
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
		, m_app{ createApp( config
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

	void ProcessFFTPass::doInitialise()
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
		checkFFTResult( "VkFFT recording"
			, VkFFTAppend( &m_app, -1, &launchParams ) );
	}

	VkPipelineStageFlags ProcessFFTPass::doGetSemaphoreWaitFlags()const
	{
		return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
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
		, crg::FrameGraph & graph
		, crg::FramePass const & previousPass
		, VkExtent2D const & extent
		, FFTConfig const & config
		, ashes::BufferBase const & input
		, std::array< ashes::BufferBasePtr, 2u > const & output
		, std::shared_ptr< IsRenderPassEnabled > isEnabled )
	{
		auto & result = graph.createPass( OceanFFT::Name + "/Process" + name
			, [&device, extent, isEnabled, &input, &output, &config]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = std::make_unique< ProcessFFTPass >( framePass
					, context
					, runnableGraph
					, device
					, config
					, extent
					, input
					, output
					, crg::RunnablePass::IsEnabledCallback( [isEnabled](){ return ( *isEnabled )(); } ) );
				device.renderSystem.getEngine()->registerTimer( runnableGraph.getName() + "/" + framePass.name
					, res->getTimer() );
				return res;
			} );
		result.addDependency( previousPass );
		result.addInputStorageBuffer( { input, name + "Frequency" }
			, ProcessFFTPass::eInput
			, 0u
			, input.getSize() );
		result.addOutputStorageBuffer( { *output.front(), name + "FFTResult" }
			, ProcessFFTPass::eOutput
			, 0u
			, output.front()->getSize() );
		return result;
	}

	//************************************************************************************************
}
