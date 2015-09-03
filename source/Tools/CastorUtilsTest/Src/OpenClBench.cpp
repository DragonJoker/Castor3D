#include "OpenClBench.hpp"

#include "Point.hpp"

#if defined( CASTOR_USE_OCL )
#undef max

namespace
{
	void randomInit( float * p_pData1, float * p_pData2, int size )
	{
		for ( int i = 0; i < size; ++i )
		{
			p_pData1[i] = rand() / float( RAND_MAX );
			p_pData2[i] = p_pData1[i];
		}
	}
}

namespace Testing
{
	bool CheckErr( cl_int p_iErr, const char * p_szName )
	{
		bool l_bReturn = true;
		static std::map< cl_int, Castor::String > MapErrors;

		if ( MapErrors.empty() )
		{
			MapErrors[CL_SUCCESS                                  		] = cuT( "CL_SUCCESS";	);
			MapErrors[CL_DEVICE_NOT_FOUND                         		] = cuT( "CL_DEVICE_NOT_FOUND"	);
			MapErrors[CL_DEVICE_NOT_AVAILABLE                     		] = cuT( "CL_DEVICE_NOT_AVAILABLE"	);
			MapErrors[CL_COMPILER_NOT_AVAILABLE                   		] = cuT( "CL_COMPILER_NOT_AVAILABLE"	);
			MapErrors[CL_MEM_OBJECT_ALLOCATION_FAILURE            		] = cuT( "CL_MEM_OBJECT_ALLOCATION_FAILURE"	);
			MapErrors[CL_OUT_OF_RESOURCES                         		] = cuT( "CL_OUT_OF_RESOURCES"	);
			MapErrors[CL_OUT_OF_HOST_MEMORY                       		] = cuT( "CL_OUT_OF_HOST_MEMORY"	);
			MapErrors[CL_PROFILING_INFO_NOT_AVAILABLE             		] = cuT( "CL_PROFILINm_INFO_NOT_AVAILABLE"	);
			MapErrors[CL_MEM_COPY_OVERLAP                         		] = cuT( "CL_MEM_COPY_OVERLAP"	);
			MapErrors[CL_IMAGE_FORMAT_MISMATCH                    		] = cuT( "CL_IMAGE_FORMAT_MISMATCH"	);
			MapErrors[CL_IMAGE_FORMAT_NOT_SUPPORTED               		] = cuT( "CL_IMAGE_FORMAT_NOT_SUPPORTED"	);
			MapErrors[CL_BUILD_PROGRAM_FAILURE                    		] = cuT( "CL_BUILD_PROGRAM_FAILURE"	);
			MapErrors[CL_MAP_FAILURE                              		] = cuT( "CL_MAP_FAILURE"	);
			MapErrors[CL_MISALIGNED_SUB_BUFFER_OFFSET             		] = cuT( "CL_MISALIGNED_SUB_BUFFER_OFFSET"	);
			MapErrors[CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST		] = cuT( "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST"	);
#if defined( CL_COMPILE_PROGRAM_FAILURE )
			MapErrors[CL_COMPILE_PROGRAM_FAILURE                  		] = cuT( "CL_COMPILE_PROGRAM_FAILURE"	);
#endif
#if defined( CL_LINKER_NOT_AVAILABLE )
			MapErrors[CL_LINKER_NOT_AVAILABLE                     		] = cuT( "CL_LINKER_NOT_AVAILABLE"	);
#endif
#if defined( CL_LINK_PROGRAM_FAILURE )
			MapErrors[CL_LINK_PROGRAM_FAILURE                     		] = cuT( "CL_LINK_PROGRAM_FAILURE"	);
#endif
#if defined( CL_DEVICE_PARTITION_FAILED )
			MapErrors[CL_DEVICE_PARTITION_FAILED                  		] = cuT( "CL_DEVICE_PARTITION_FAILED"	);
#endif
#if defined( CL_KERNEL_ARm_INFO_NOT_AVAILABLE )
			MapErrors[CL_KERNEL_ARm_INFO_NOT_AVAILABLE            		] = cuT( "CL_KERNEL_ARm_INFO_NOT_AVAILABLE"	);
#endif
			MapErrors[CL_INVALID_VALUE                            		] = cuT( "CL_INVALID_VALUE"	);
			MapErrors[CL_INVALID_DEVICE_TYPE                      		] = cuT( "CL_INVALID_DEVICE_TYPE"	);
			MapErrors[CL_INVALID_PLATFORM                         		] = cuT( "CL_INVALID_PLATFORM"	);
			MapErrors[CL_INVALID_DEVICE                           		] = cuT( "CL_INVALID_DEVICE"	);
			MapErrors[CL_INVALID_CONTEXT                          		] = cuT( "CL_INVALID_CONTEXT"	);
			MapErrors[CL_INVALID_QUEUE_PROPERTIES                 		] = cuT( "CL_INVALID_QUEUE_PROPERTIES"	);
			MapErrors[CL_INVALID_COMMAND_QUEUE                    		] = cuT( "CL_INVALID_COMMAND_QUEUE"	);
			MapErrors[CL_INVALID_HOST_PTR                         		] = cuT( "CL_INVALID_HOST_PTR"	);
			MapErrors[CL_INVALID_MEM_OBJECT                       		] = cuT( "CL_INVALID_MEM_OBJECT"	);
			MapErrors[CL_INVALID_IMAGE_FORMAT_DESCRIPTOR          		] = cuT( "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR"	);
			MapErrors[CL_INVALID_IMAGE_SIZE                       		] = cuT( "CL_INVALID_IMAGE_SIZE"	);
			MapErrors[CL_INVALID_SAMPLER                          		] = cuT( "CL_INVALID_SAMPLER"	);
			MapErrors[CL_INVALID_BINARY                           		] = cuT( "CL_INVALID_BINARY"	);
			MapErrors[CL_INVALID_BUILD_OPTIONS                    		] = cuT( "CL_INVALID_BUILD_OPTIONS"	);
			MapErrors[CL_INVALID_PROGRAM                          		] = cuT( "CL_INVALID_PROGRAM"	);
			MapErrors[CL_INVALID_PROGRAM_EXECUTABLE               		] = cuT( "CL_INVALID_PROGRAM_EXECUTABLE"	);
			MapErrors[CL_INVALID_KERNEL_NAME                      		] = cuT( "CL_INVALID_KERNEL_NAME"	);
			MapErrors[CL_INVALID_KERNEL_DEFINITION                		] = cuT( "CL_INVALID_KERNEL_DEFINITION"	);
			MapErrors[CL_INVALID_KERNEL                           		] = cuT( "CL_INVALID_KERNEL"	);
			MapErrors[CL_INVALID_ARG_INDEX                        		] = cuT( "CL_INVALID_ARm_INDEX"	);
			MapErrors[CL_INVALID_ARG_VALUE                        		] = cuT( "CL_INVALID_ARm_VALUE"	);
			MapErrors[CL_INVALID_ARG_SIZE                         		] = cuT( "CL_INVALID_ARm_SIZE"	);
			MapErrors[CL_INVALID_KERNEL_ARGS                      		] = cuT( "CL_INVALID_KERNEL_ARGS"	);
			MapErrors[CL_INVALID_WORK_DIMENSION                   		] = cuT( "CL_INVALID_WORK_DIMENSION"	);
			MapErrors[CL_INVALID_WORK_GROUP_SIZE                  		] = cuT( "CL_INVALID_WORK_GROUP_SIZE"	);
			MapErrors[CL_INVALID_WORK_ITEM_SIZE                   		] = cuT( "CL_INVALID_WORK_ITEM_SIZE"	);
			MapErrors[CL_INVALID_GLOBAL_OFFSET                    		] = cuT( "CL_INVALID_GLOBAL_OFFSET"	);
			MapErrors[CL_INVALID_EVENT_WAIT_LIST                  		] = cuT( "CL_INVALID_EVENT_WAIT_LIST"	);
			MapErrors[CL_INVALID_EVENT                            		] = cuT( "CL_INVALID_EVENT"	);
			MapErrors[CL_INVALID_OPERATION                        		] = cuT( "CL_INVALID_OPERATION"	);
			MapErrors[CL_INVALID_GL_OBJECT                        		] = cuT( "CL_INVALID_GL_OBJECT"	);
			MapErrors[CL_INVALID_BUFFER_SIZE                      		] = cuT( "CL_INVALID_BUFFER_SIZE"	);
			MapErrors[CL_INVALID_MIP_LEVEL                        		] = cuT( "CL_INVALID_MIP_LEVEL"	);
			MapErrors[CL_INVALID_GLOBAL_WORK_SIZE                 		] = cuT( "CL_INVALID_GLOBAL_WORK_SIZE"	);
			MapErrors[CL_INVALID_PROPERTY                         		] = cuT( "CL_INVALID_PROPERTY"	);
#if defined( CL_INVALID_IMAGE_DESCRIPTOR )
			MapErrors[CL_INVALID_IMAGE_DESCRIPTOR                 		] = cuT( "CL_INVALID_IMAGE_DESCRIPTOR"	);
#endif
#if defined( CL_INVALID_COMPILER_OPTIONS )
			MapErrors[CL_INVALID_COMPILER_OPTIONS                 		] = cuT( "CL_INVALID_COMPILER_OPTIONS"	);
#endif
#if defined( CL_INVALID_LINKER_OPTIONS )
			MapErrors[CL_INVALID_LINKER_OPTIONS                   		] = cuT( "CL_INVALID_LINKER_OPTIONS"	);
#endif
#if defined( CL_INVALID_DEVICE_PARTITION_COUNT )
			MapErrors[CL_INVALID_DEVICE_PARTITION_COUNT					] = cuT( "CL_INVALID_DEVICE_PARTITION_COUNT"	);
#endif
		};

		if ( p_iErr != CL_SUCCESS )
		{
			std::map< cl_int, Castor::String >::const_iterator l_it = MapErrors.find( p_iErr );

			if ( l_it != MapErrors.end() )
			{
				Castor::Logger::LogWarning( Castor::StringStream() << cuT( "ERROR: " ) << Castor::str_utils::from_str( p_szName ) << cuT( " - 0x" ) << std::hex << p_iErr << cuT( " (" ) << l_it->second << cuT( ")" ) );
			}
			else
			{
				Castor::Logger::LogWarning( Castor::StringStream() << cuT( "ERROR: " ) << Castor::str_utils::from_str( p_szName ) << cuT( " - 0x" ) << std::hex << p_iErr );
			}

			l_bReturn = false;
		}

		return l_bReturn;
	}

	OpenCLBench::OpenCLBench()
		:	BenchCase( "OpenCLBench" )
	{
		std::string l_prog;
		cl_int l_iErr;
		cl::Platform::get( &m_arrayPlatforms );
		bool l_bContinue = CheckErr( m_arrayPlatforms.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get" );

		if ( l_bContinue )
		{
			Castor::Logger::LogInfo( Castor::StringStream() << cuT( "Platform count is: " ) << m_arrayPlatforms.size() );
			std::string l_strInfo;
			m_platform = m_arrayPlatforms[0];
			m_platform.getInfo( ( cl_platform_info )CL_PLATFORM_NAME, &l_strInfo );
			Castor::Logger::LogInfo( std::stringstream() << "  Name:    " << l_strInfo );
			m_platform.getInfo( ( cl_platform_info )CL_PLATFORM_VENDOR, &l_strInfo );
			Castor::Logger::LogInfo( std::stringstream() << "  Vendor:  " << l_strInfo );
			m_platform.getInfo( ( cl_platform_info )CL_PLATFORM_VERSION, &l_strInfo );
			Castor::Logger::LogInfo( std::stringstream() << "  Version: " << l_strInfo );
			cl_context_properties l_props[3] = { CL_CONTEXT_PLATFORM, ( cl_context_properties )( m_platform )(), 0 };
			m_context = cl::Context( CL_DEVICE_TYPE_CPU, l_props, NULL, NULL, &l_iErr );
			l_bContinue = CheckErr( l_iErr, "Context::Context()" );
		}

		if ( l_bContinue )
		{
			m_arrayDevices = m_context.getInfo< CL_CONTEXT_DEVICES >();
			l_bContinue = CheckErr( m_arrayDevices.size() > 0 ? CL_SUCCESS : -1, "l_arrayDevices.size() > 0" );
		}

		if ( l_bContinue )
		{
			m_queue = cl::CommandQueue( m_context, m_arrayDevices[0], 0, &l_iErr );
			l_bContinue = CheckErr( l_iErr, "CommandQueue::CommandQueue()" );
		}

		if ( l_bContinue )
		{
			m_clBufferMtx4x4A = cl::Buffer( m_context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, 16 * sizeof( float ), m_bufferMtx4x4A, &l_iErr );
			l_bContinue = CheckErr( l_iErr, "Buffer::Buffer() -Mtx4x4 A" );
		}

		if ( l_bContinue )
		{
			m_clBufferMtx4x4B = cl::Buffer( m_context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, 16 * sizeof( float ), m_bufferMtx4x4B, &l_iErr );
			l_bContinue = CheckErr( l_iErr, "Buffer::Buffer() -Mtx4x4 B" );
		}

		if ( l_bContinue )
		{
			m_clBufferMtx4x4C = cl::Buffer( m_context, CL_MEM_READ_WRITE, 16 * sizeof( float ), NULL, &l_iErr );
			l_bContinue = CheckErr( l_iErr, "Buffer::Buffer() -Mtx4x4 C" );
		}

		if ( l_bContinue )
		{
			Castor::Path l_filePath = Castor::File::DirectoryGetCurrent();
			std::string l_path = Castor::str_utils::to_str( l_filePath / cuT( "mtx_ope.cl" ) );
			std::ifstream l_file( l_path );
			l_bContinue = CheckErr( l_file ? CL_SUCCESS : -1, l_path.c_str() );

			if ( l_bContinue )
			{
				l_prog = std::string( std::istreambuf_iterator<char>( l_file ), std::istreambuf_iterator<char>() );
			}
		}

		if ( l_bContinue )
		{
			cl::Program::Sources l_source( 1, cl::string( l_prog ) );
			m_program = cl::Program( m_context, l_source );
			l_iErr = m_program.build( m_arrayDevices );
			l_bContinue = CheckErr( l_iErr, "Program::build()" );

			if ( !l_bContinue )
			{
				cl::string l_strInfo;
				l_iErr = m_program.getBuildInfo( m_arrayDevices[0], CL_PROGRAM_BUILD_LOG, &l_strInfo );

				if ( CheckErr( l_iErr, "Program::getBuildInfo()" ) )
				{
					Castor::Logger::LogInfo( l_strInfo );
				}
			}
		}

		if ( l_bContinue )
		{
			m_kernel = cl::Kernel( m_program, "mult_4x4", &l_iErr );
			l_bContinue = CheckErr( l_iErr, "Kernel::Kernel()" );
		}

		if ( l_bContinue )
		{
			l_iErr = m_kernel.setArg( 0, m_clBufferMtx4x4C );
			l_bContinue = CheckErr( l_iErr, "Kernel::setArg() - m_clBufferMtx4x4C" );
		}

		if ( l_bContinue )
		{
			l_iErr = m_kernel.setArg( 1, m_clBufferMtx4x4A );
			l_bContinue = CheckErr( l_iErr, "Kernel::setArg() - m_clBufferMtx4x4A" );
		}

		if ( l_bContinue )
		{
			l_iErr = m_kernel.setArg( 2, m_clBufferMtx4x4B );
			l_bContinue = CheckErr( l_iErr, "Kernel::setArg() - m_clBufferMtx4x4B" );
		}

		if ( l_bContinue )
		{
			m_queue.enqueueNDRangeKernel( m_kernel, cl::NullRange, cl::NDRange( 1, 1 ), cl::NDRange( 4, 4 ), NULL, &m_event );
			m_queue.enqueueReadBuffer( m_clBufferMtx4x4C, CL_TRUE, 0, 16 * sizeof( float ), m_bufferMtx4x4C );
		}

		m_bClInitialised = l_bContinue;
	}

	OpenCLBench::~OpenCLBench()
	{
	}

	void OpenCLBench::Execute()
	{
		BENCHMARK( MtxMultCastor, NB_TESTS );
		BENCHMARK( MtxMultCL, NB_TESTS );
	}

	void OpenCLBench::MtxMultCastor()
	{
		m_mtx4x4CuC = m_mtx4x4CuA * m_mtx4x4CuB;
	}

	void OpenCLBench::MtxMultCL()
	{
		if ( m_bClInitialised )
		{
			m_queue.enqueueNDRangeKernel( m_kernel, cl::NullRange, cl::NDRange( 1, 1 ), cl::NDRange( 4, 4 ), NULL, &m_event );
			m_queue.enqueueReadBuffer( m_clBufferMtx4x4C, CL_TRUE, 0, 16 * sizeof( float ), m_bufferMtx4x4C );
		}
	}
}

#endif
