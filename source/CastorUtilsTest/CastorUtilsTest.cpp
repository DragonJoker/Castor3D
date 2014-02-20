#include "CastorUtilsTest/Benchmark.hpp"
#include "CastorUtilsTest/Point.hpp"

#include <CastorUtils/Matrix.hpp>
#include <CastorUtils/TransformationMatrix.hpp>

#include <glm/glm.hpp>

#include <iostream>
#include <limits>

#if defined( NDEBUG )
#	define NB_TESTS 1000000
#else
#	define NB_TESTS 1000
#endif

using namespace Castor;
using namespace CastorUtilsTest;

inline bool CheckErr( cl_int p_iErr, const char * p_szName )
{
	bool l_bReturn = true;

	static std::map< cl_int, String > MapErrors;
	
	if( MapErrors.empty() )
	{
		MapErrors[CL_SUCCESS                                  		] = cuT( "CL_SUCCESS";									);
		MapErrors[CL_DEVICE_NOT_FOUND                         		] = cuT( "CL_DEVICE_NOT_FOUND"							);
		MapErrors[CL_DEVICE_NOT_AVAILABLE                     		] = cuT( "CL_DEVICE_NOT_AVAILABLE"						);
		MapErrors[CL_COMPILER_NOT_AVAILABLE                   		] = cuT( "CL_COMPILER_NOT_AVAILABLE"					);
		MapErrors[CL_MEM_OBJECT_ALLOCATION_FAILURE            		] = cuT( "CL_MEM_OBJECT_ALLOCATION_FAILURE"				);
		MapErrors[CL_OUT_OF_RESOURCES                         		] = cuT( "CL_OUT_OF_RESOURCES"							);
		MapErrors[CL_OUT_OF_HOST_MEMORY                       		] = cuT( "CL_OUT_OF_HOST_MEMORY"						);
		MapErrors[CL_PROFILING_INFO_NOT_AVAILABLE             		] = cuT( "CL_PROFILING_INFO_NOT_AVAILABLE"				);
		MapErrors[CL_MEM_COPY_OVERLAP                         		] = cuT( "CL_MEM_COPY_OVERLAP"							);
		MapErrors[CL_IMAGE_FORMAT_MISMATCH                    		] = cuT( "CL_IMAGE_FORMAT_MISMATCH"						);
		MapErrors[CL_IMAGE_FORMAT_NOT_SUPPORTED               		] = cuT( "CL_IMAGE_FORMAT_NOT_SUPPORTED"				);
		MapErrors[CL_BUILD_PROGRAM_FAILURE                    		] = cuT( "CL_BUILD_PROGRAM_FAILURE"						);
		MapErrors[CL_MAP_FAILURE                              		] = cuT( "CL_MAP_FAILURE"								);
		MapErrors[CL_MISALIGNED_SUB_BUFFER_OFFSET             		] = cuT( "CL_MISALIGNED_SUB_BUFFER_OFFSET"				);
		MapErrors[CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST		] = cuT( "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST"	);
#if defined( CL_COMPILE_PROGRAM_FAILURE )
		MapErrors[CL_COMPILE_PROGRAM_FAILURE                  		] = cuT( "CL_COMPILE_PROGRAM_FAILURE"					);
#endif
#if defined( CL_LINKER_NOT_AVAILABLE )
		MapErrors[CL_LINKER_NOT_AVAILABLE                     		] = cuT( "CL_LINKER_NOT_AVAILABLE"						);
#endif
#if defined( CL_LINK_PROGRAM_FAILURE )
		MapErrors[CL_LINK_PROGRAM_FAILURE                     		] = cuT( "CL_LINK_PROGRAM_FAILURE"						);
#endif
#if defined( CL_DEVICE_PARTITION_FAILED )
		MapErrors[CL_DEVICE_PARTITION_FAILED                  		] = cuT( "CL_DEVICE_PARTITION_FAILED"					);
#endif
#if defined( CL_KERNEL_ARG_INFO_NOT_AVAILABLE )
		MapErrors[CL_KERNEL_ARG_INFO_NOT_AVAILABLE            		] = cuT( "CL_KERNEL_ARG_INFO_NOT_AVAILABLE"				);
#endif
		MapErrors[CL_INVALID_VALUE                            		] = cuT( "CL_INVALID_VALUE"								);
		MapErrors[CL_INVALID_DEVICE_TYPE                      		] = cuT( "CL_INVALID_DEVICE_TYPE"						);
		MapErrors[CL_INVALID_PLATFORM                         		] = cuT( "CL_INVALID_PLATFORM"							);
		MapErrors[CL_INVALID_DEVICE                           		] = cuT( "CL_INVALID_DEVICE"							);
		MapErrors[CL_INVALID_CONTEXT                          		] = cuT( "CL_INVALID_CONTEXT"							);
		MapErrors[CL_INVALID_QUEUE_PROPERTIES                 		] = cuT( "CL_INVALID_QUEUE_PROPERTIES"					);
		MapErrors[CL_INVALID_COMMAND_QUEUE                    		] = cuT( "CL_INVALID_COMMAND_QUEUE"						);
		MapErrors[CL_INVALID_HOST_PTR                         		] = cuT( "CL_INVALID_HOST_PTR"							);
		MapErrors[CL_INVALID_MEM_OBJECT                       		] = cuT( "CL_INVALID_MEM_OBJECT"						);
		MapErrors[CL_INVALID_IMAGE_FORMAT_DESCRIPTOR          		] = cuT( "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR"			);
		MapErrors[CL_INVALID_IMAGE_SIZE                       		] = cuT( "CL_INVALID_IMAGE_SIZE"						);
		MapErrors[CL_INVALID_SAMPLER                          		] = cuT( "CL_INVALID_SAMPLER"							);
		MapErrors[CL_INVALID_BINARY                           		] = cuT( "CL_INVALID_BINARY"							);
		MapErrors[CL_INVALID_BUILD_OPTIONS                    		] = cuT( "CL_INVALID_BUILD_OPTIONS"						);
		MapErrors[CL_INVALID_PROGRAM                          		] = cuT( "CL_INVALID_PROGRAM"							);
		MapErrors[CL_INVALID_PROGRAM_EXECUTABLE               		] = cuT( "CL_INVALID_PROGRAM_EXECUTABLE"				);
		MapErrors[CL_INVALID_KERNEL_NAME                      		] = cuT( "CL_INVALID_KERNEL_NAME"						);
		MapErrors[CL_INVALID_KERNEL_DEFINITION                		] = cuT( "CL_INVALID_KERNEL_DEFINITION"					);
		MapErrors[CL_INVALID_KERNEL                           		] = cuT( "CL_INVALID_KERNEL"							);
		MapErrors[CL_INVALID_ARG_INDEX                        		] = cuT( "CL_INVALID_ARG_INDEX"							);
		MapErrors[CL_INVALID_ARG_VALUE                        		] = cuT( "CL_INVALID_ARG_VALUE"							);
		MapErrors[CL_INVALID_ARG_SIZE                         		] = cuT( "CL_INVALID_ARG_SIZE"							);
		MapErrors[CL_INVALID_KERNEL_ARGS                      		] = cuT( "CL_INVALID_KERNEL_ARGS"						);
		MapErrors[CL_INVALID_WORK_DIMENSION                   		] = cuT( "CL_INVALID_WORK_DIMENSION"					);
		MapErrors[CL_INVALID_WORK_GROUP_SIZE                  		] = cuT( "CL_INVALID_WORK_GROUP_SIZE"					);
		MapErrors[CL_INVALID_WORK_ITEM_SIZE                   		] = cuT( "CL_INVALID_WORK_ITEM_SIZE"					);
		MapErrors[CL_INVALID_GLOBAL_OFFSET                    		] = cuT( "CL_INVALID_GLOBAL_OFFSET"						);
		MapErrors[CL_INVALID_EVENT_WAIT_LIST                  		] = cuT( "CL_INVALID_EVENT_WAIT_LIST"					);
		MapErrors[CL_INVALID_EVENT                            		] = cuT( "CL_INVALID_EVENT"								);
		MapErrors[CL_INVALID_OPERATION                        		] = cuT( "CL_INVALID_OPERATION"							);
		MapErrors[CL_INVALID_GL_OBJECT                        		] = cuT( "CL_INVALID_GL_OBJECT"							);
		MapErrors[CL_INVALID_BUFFER_SIZE                      		] = cuT( "CL_INVALID_BUFFER_SIZE"						);
		MapErrors[CL_INVALID_MIP_LEVEL                        		] = cuT( "CL_INVALID_MIP_LEVEL"							);
		MapErrors[CL_INVALID_GLOBAL_WORK_SIZE                 		] = cuT( "CL_INVALID_GLOBAL_WORK_SIZE"					);
		MapErrors[CL_INVALID_PROPERTY                         		] = cuT( "CL_INVALID_PROPERTY"							);
#if defined( CL_INVALID_IMAGE_DESCRIPTOR )
		MapErrors[CL_INVALID_IMAGE_DESCRIPTOR                 		] = cuT( "CL_INVALID_IMAGE_DESCRIPTOR"					);
#endif
#if defined( CL_INVALID_COMPILER_OPTIONS )
		MapErrors[CL_INVALID_COMPILER_OPTIONS                 		] = cuT( "CL_INVALID_COMPILER_OPTIONS"					);
#endif
#if defined( CL_INVALID_LINKER_OPTIONS )
		MapErrors[CL_INVALID_LINKER_OPTIONS                   		] = cuT( "CL_INVALID_LINKER_OPTIONS"					);
#endif
#if defined( CL_INVALID_DEVICE_PARTITION_COUNT )
		MapErrors[CL_INVALID_DEVICE_PARTITION_COUNT					] = cuT( "CL_INVALID_DEVICE_PARTITION_COUNT"			);
#endif
	};

	if( p_iErr != CL_SUCCESS )
	{
		std::map< cl_int, String >::const_iterator l_it = MapErrors.find( p_iErr );

		if( l_it != MapErrors.end() )
		{
			Castor::Logger::LogWarning( _T( "ERROR: %s - 0x%08X (%s)" ), Castor::str_utils::from_str( p_szName ).c_str(), p_iErr, l_it->second.c_str() );
		}
		else
		{
			Castor::Logger::LogWarning( _T( "ERROR: %s - 0x%08X" ), Castor::str_utils::from_str( p_szName ).c_str(), p_iErr );
		}
		l_bReturn = false;
	}

	return l_bReturn;
}

Matrix4x4r g_mtx1;
Matrix4x4r g_mtx2;
glm::mat4 g_mtx1glm;
glm::mat4 g_mtx2glm;
std::string g_strIn = "STR : Bonjoir éêèàÉÊÈÀ";
std::wstring g_wstrIn = L"STR : Bonjoir éêèàÉÊÈÀ";
std::wstring g_wstrOut;
std::string g_strOut;
Matrix4x4f g_mtx4x4CuA;
Matrix4x4f g_mtx4x4CuB;
Matrix4x4f g_mtx4x4CuC;

#if CL_VERSION_1_2
#	include <CL/cl.hpp>
#	undef max
	cl::Context g_context;
	std::vector< cl::Device > g_arrayDevices;
	std::vector< cl::Platform > g_arrayPlatforms;
	cl::Platform g_platform;
	cl::Buffer g_clBufferMtx4x4A;
	cl::Buffer g_clBufferMtx4x4B;
	cl::Buffer g_clBufferMtx4x4C;
	cl::Program g_program;
	cl::Kernel g_kernel;
	cl::CommandQueue g_queue;
	cl::Event g_event;
#else if CL_VERSION_1_1
	cl_context g_context;
	std::vector< cl_device_id > g_arrayDevices;
	std::vector< cl_platform_id > g_arrayPlatforms;
	cl_platform_id g_platform;
	cl_mem g_clBufferMtx4x4A;
	cl_mem g_clBufferMtx4x4B;
	cl_mem g_clBufferMtx4x4C;
	cl_program g_program;
	cl_kernel g_kernel;
	cl_command_queue g_queue;
	cl_event g_event;
#endif
float g_bufferMtx4x4A[16];
float g_bufferMtx4x4B[16];
float g_bufferMtx4x4C[16];
bool g_bClInitialised;

void randomInit( float * p_pData1, float * p_pData2, int size )
{
	for (int i = 0; i < size; ++i)
	{
		p_pData1[i] = rand() / float( RAND_MAX );
		p_pData2[i] = p_pData1[i];
	}
}

template< typename InChar, typename OutChar >
static inline void convert( std::basic_string< InChar > const & p_strIn, std::basic_string< OutChar > & p_strOut, std::locale const & p_locale = std::locale() )
{
	if( !p_strIn.empty() )
	{
		typedef typename std::codecvt< OutChar, InChar, std::mbstate_t > facet_type;
		typedef typename facet_type::result result_type;

		std::mbstate_t l_state = std::mbstate_t();
		result_type l_result;
		std::vector< OutChar > l_buffer( p_strIn.size() );
		InChar const * l_pEndIn = NULL;
		OutChar * l_pEndOut = NULL;

		l_result = std::use_facet< facet_type >( p_locale ).in( l_state,
						p_strIn.data(),		p_strIn.data() + p_strIn.length(),		l_pEndIn,
						&l_buffer.front(),	&l_buffer.front() + l_buffer.size(),	l_pEndOut
					);

		p_strOut = std::basic_string< OutChar >( &l_buffer.front(), l_pEndOut );
	}
	else
	{
		p_strOut.clear();
	}
}

static inline void narrow( std::wstring const & p_wstrIn, std::string & p_strOut )
{
	p_strOut.resize( p_wstrIn.size() + 1, '\0' );
	std::ostringstream l_stream;
	std::ctype< wchar_t > const & l_ctfacet = std::use_facet< std::ctype< wchar_t > >( l_stream.getloc() );

	if( p_wstrIn.size() > 0 )
	{
		l_ctfacet.narrow( p_wstrIn.data(), p_wstrIn.data() + p_wstrIn.size(), '?', &p_strOut[0] );
	}
}

static inline void widen( std::string const & p_strIn, std::wstring & p_wstrOut )
{
	p_wstrOut.resize( p_strIn.size() + 1, L'\0' );
	std::wostringstream l_wstream;
	std::ctype< wchar_t > const & l_ctfacet = std::use_facet< std::ctype< wchar_t > >( l_wstream.getloc() );

	if( p_strIn.size() > 0 )
	{
		l_ctfacet.widen( p_strIn.data(), p_strIn.data() + p_strIn.size(), &p_wstrOut[0] );
	}
}

CODE( StringConversions )
{
	String l_tstrOut;
	String l_tstrIn( cuT( "STR : Bonjoir éêèàÉÊÈÀ" ) );
		
	l_tstrOut = str_utils::from_str( g_strIn );
	Logger::LogMessage(			"*	Conversion from std::string to String" );
	Logger::LogMessage(			"*	Entry  : " + g_strIn );
	Logger::LogMessage( cuT(	"*	Result : " ) + l_tstrOut );
	Logger::LogMessage(			"*" );
		
	l_tstrOut = str_utils::from_wstr( g_wstrIn );
	Logger::LogMessage(			"*	Conversion from std::wstring to String" );
	Logger::LogMessage(		   L"*	Entry  : " + g_wstrIn );
	Logger::LogMessage( cuT(	"*	Result : " ) + l_tstrOut );
	Logger::LogMessage(			"*" );
		
	g_strOut = str_utils::to_str( l_tstrIn );
	Logger::LogMessage(			"*	Conversion from String to std::string" );
	Logger::LogMessage( cuT(	"*	Entry  : " ) + l_tstrIn );
	Logger::LogMessage(			"*	Result : " + g_strOut );
	Logger::LogMessage(			"*" );
		
	g_wstrOut = str_utils::to_wstr( l_tstrIn );
	Logger::LogMessage(			"*	Conversion from String to std::wstring" );
	Logger::LogMessage( cuT(	"*	Entry  : " ) + l_tstrIn );
	Logger::LogMessage(		   L"*	Result : " + g_wstrOut );
	Logger::LogMessage(			"*" );

	convert( g_strIn, g_wstrOut );
	Logger::LogMessage(			"*	Conversion from std::string to std::wstring" );
	Logger::LogMessage(			"*	Entry  : " + g_strIn );
	Logger::LogMessage(		   L"*	Result : " + g_wstrOut );
	Logger::LogMessage(			"*" );

	convert( g_wstrIn, g_strOut );
	Logger::LogMessage(			"*	Conversion from std::wstring to std::string" );
	Logger::LogMessage(		   L"*	Entry  : " + g_wstrIn );
	Logger::LogMessage(			"*	Result : " + g_strOut );
}

CODE( MatrixInversion )
{
	Matrix3x3d l_mtxRGBtoYUV;
	l_mtxRGBtoYUV[0][0] =  0.299;	l_mtxRGBtoYUV[1][0] =  0.587;	l_mtxRGBtoYUV[2][0] =  0.114;
	l_mtxRGBtoYUV[0][1] = -0.14713;	l_mtxRGBtoYUV[1][1] = -0.28886;	l_mtxRGBtoYUV[2][1] =  0.436;
	l_mtxRGBtoYUV[0][2] =  0.615;	l_mtxRGBtoYUV[1][2] = -0.51499;	l_mtxRGBtoYUV[2][2] = -0.10001;
	Logger::LogMessage( cuT( "RGB to YUV conversion matrix (BT 601) :" ) );
	Logger::LogMessage( cuT( "	%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][0], l_mtxRGBtoYUV[1][0], l_mtxRGBtoYUV[2][0] );
	Logger::LogMessage( cuT( "	%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][1], l_mtxRGBtoYUV[1][1], l_mtxRGBtoYUV[2][1] );
	Logger::LogMessage( cuT( "	%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][2], l_mtxRGBtoYUV[1][2], l_mtxRGBtoYUV[2][2] );
	Matrix3x3d l_mtxYUVtoRGB( l_mtxRGBtoYUV.get_inverse() );
	Logger::LogMessage( cuT( "YUV to RGB conversion matrix (BT 601) :" ) );
	Logger::LogMessage( cuT( "	%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][0], l_mtxYUVtoRGB[1][0], l_mtxYUVtoRGB[2][0] );
	Logger::LogMessage( cuT( "	%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][1], l_mtxYUVtoRGB[1][1], l_mtxYUVtoRGB[2][1] );
	Logger::LogMessage( cuT( "	%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][2], l_mtxYUVtoRGB[1][2], l_mtxYUVtoRGB[2][2] );

	l_mtxRGBtoYUV[0][0] =  0.2126;	l_mtxRGBtoYUV[1][0] =  0.7152;	l_mtxRGBtoYUV[2][0] =  0.0722;
	l_mtxRGBtoYUV[0][1] = -0.09991;	l_mtxRGBtoYUV[1][1] = -0.33609;	l_mtxRGBtoYUV[2][1] =  0.436;
	l_mtxRGBtoYUV[0][2] =  0.615;	l_mtxRGBtoYUV[1][2] = -0.55861;	l_mtxRGBtoYUV[2][2] = -0.05639;
	Logger::LogMessage( cuT( "RGB to YUV conversion matrix (BT 709) :" ) );
	Logger::LogMessage( cuT( "	%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][0], l_mtxRGBtoYUV[1][0], l_mtxRGBtoYUV[2][0] );
	Logger::LogMessage( cuT( "	%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][1], l_mtxRGBtoYUV[1][1], l_mtxRGBtoYUV[2][1] );
	Logger::LogMessage( cuT( "	%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][2], l_mtxRGBtoYUV[1][2], l_mtxRGBtoYUV[2][2] );
	l_mtxYUVtoRGB = l_mtxRGBtoYUV.get_inverse();
	Logger::LogMessage( cuT( "YUV to RGB conversion matrix (BT 709) :" ) );
	Logger::LogMessage( cuT( "	%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][0], l_mtxYUVtoRGB[1][0], l_mtxYUVtoRGB[2][0] );
	Logger::LogMessage( cuT( "	%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][1], l_mtxYUVtoRGB[1][1], l_mtxYUVtoRGB[2][1] );
	Logger::LogMessage( cuT( "	%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][2], l_mtxYUVtoRGB[1][2], l_mtxYUVtoRGB[2][2] );
}

BENCHMARK( MatrixMultiplicationsCastor, NB_TESTS )
{
	DoNotOptimizeAway( g_mtx1 * g_mtx2 );
}

BENCHMARK( MatrixMultiplicationsGlm, NB_TESTS )
{
	DoNotOptimizeAway( g_mtx1glm * g_mtx2glm );
}

BENCHMARK( MatrixInversionCastor, NB_TESTS )
{
	DoNotOptimizeAway( g_mtx1.get_inverse() );
}

BENCHMARK( MatrixInversionGlm, NB_TESTS )
{
	DoNotOptimizeAway( glm::inverse( g_mtx1glm ) );
}

CODE( MatrixInversionComparison )
{
	Matrix4x4r l_mtxRGBtoYUV;
	l_mtxRGBtoYUV[0][0] =  0.299f;		l_mtxRGBtoYUV[1][0] =  0.587f;		l_mtxRGBtoYUV[2][0] =  0.114f;		l_mtxRGBtoYUV[3][0] = 0.0f;
	l_mtxRGBtoYUV[0][1] = -0.14713f;	l_mtxRGBtoYUV[1][1] = -0.28886f;	l_mtxRGBtoYUV[2][1] =  0.436f;		l_mtxRGBtoYUV[3][1] = 0.0f;
	l_mtxRGBtoYUV[0][2] =  0.615f;		l_mtxRGBtoYUV[1][2] = -0.51499f;	l_mtxRGBtoYUV[2][2] = -0.10001f;	l_mtxRGBtoYUV[3][2] = 0.0f;
	l_mtxRGBtoYUV[0][3] =  0.0f;		l_mtxRGBtoYUV[1][3] = 0.0f;			l_mtxRGBtoYUV[2][3] = 0.0f;			l_mtxRGBtoYUV[3][3] = 1.0f;
	Logger::LogMessage( cuT( "Matrix Inversion with CastorUtils :" ) );
	Logger::LogMessage( cuT( "	Normal :" ) );
	Logger::LogMessage( cuT( "		%.5f %.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][0], l_mtxRGBtoYUV[1][0], l_mtxRGBtoYUV[2][0], l_mtxRGBtoYUV[3][0] );
	Logger::LogMessage( cuT( "		%.5f %.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][1], l_mtxRGBtoYUV[1][1], l_mtxRGBtoYUV[2][1], l_mtxRGBtoYUV[3][1] );
	Logger::LogMessage( cuT( "		%.5f %.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][2], l_mtxRGBtoYUV[1][2], l_mtxRGBtoYUV[2][2], l_mtxRGBtoYUV[3][2] );
	Logger::LogMessage( cuT( "		%.5f %.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][3], l_mtxRGBtoYUV[1][3], l_mtxRGBtoYUV[2][3], l_mtxRGBtoYUV[3][3] );
	Matrix4x4r l_mtxYUVtoRGB( l_mtxRGBtoYUV.get_inverse() );
	Logger::LogMessage( cuT( "	Inverted :" ) );
	Logger::LogMessage( cuT( "		%.5f %.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][0], l_mtxYUVtoRGB[1][0], l_mtxYUVtoRGB[2][0], l_mtxYUVtoRGB[3][0] );
	Logger::LogMessage( cuT( "		%.5f %.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][1], l_mtxYUVtoRGB[1][1], l_mtxYUVtoRGB[2][1], l_mtxYUVtoRGB[3][1] );
	Logger::LogMessage( cuT( "		%.5f %.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][2], l_mtxYUVtoRGB[1][2], l_mtxYUVtoRGB[2][2], l_mtxYUVtoRGB[3][2] );
	Logger::LogMessage( cuT( "		%.5f %.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][3], l_mtxYUVtoRGB[1][3], l_mtxYUVtoRGB[2][3], l_mtxYUVtoRGB[3][3] );

	glm::mat4x4 l_glmRGBtoYUV;
	l_glmRGBtoYUV[0][0] =  0.299f;		l_glmRGBtoYUV[1][0] =  0.587f;		l_glmRGBtoYUV[2][0] =  0.114f;		l_glmRGBtoYUV[3][0] = 0.0f;
	l_glmRGBtoYUV[0][1] = -0.14713f;	l_glmRGBtoYUV[1][1] = -0.28886f;	l_glmRGBtoYUV[2][1] =  0.436f;		l_glmRGBtoYUV[3][1] = 0.0f;
	l_glmRGBtoYUV[0][2] =  0.615f;		l_glmRGBtoYUV[1][2] = -0.51499f;	l_glmRGBtoYUV[2][2] = -0.10001f;	l_glmRGBtoYUV[3][2] = 0.0f;
	l_glmRGBtoYUV[0][3] =  0.0f;		l_glmRGBtoYUV[1][3] = 0.0f;			l_glmRGBtoYUV[2][3] = 0.0f;			l_glmRGBtoYUV[3][3] = 1.0f;
	Logger::LogMessage( cuT( "Matrix Inversion with GLM :" ) );
	Logger::LogMessage( cuT( "	Normal :" ) );
	Logger::LogMessage( cuT( "		%.5f %.5f %.5f %.5f" ), l_glmRGBtoYUV[0][0], l_glmRGBtoYUV[1][0], l_glmRGBtoYUV[2][0], l_glmRGBtoYUV[3][0] );
	Logger::LogMessage( cuT( "		%.5f %.5f %.5f %.5f" ), l_glmRGBtoYUV[0][1], l_glmRGBtoYUV[1][1], l_glmRGBtoYUV[2][1], l_glmRGBtoYUV[3][1] );
	Logger::LogMessage( cuT( "		%.5f %.5f %.5f %.5f" ), l_glmRGBtoYUV[0][2], l_glmRGBtoYUV[1][2], l_glmRGBtoYUV[2][2], l_glmRGBtoYUV[3][2] );
	Logger::LogMessage( cuT( "		%.5f %.5f %.5f %.5f" ), l_glmRGBtoYUV[0][3], l_glmRGBtoYUV[1][3], l_glmRGBtoYUV[2][3], l_glmRGBtoYUV[3][3] );
	glm::mat4x4 l_glmYUVtoRGB( glm::inverse( l_glmRGBtoYUV ) );
	Logger::LogMessage( cuT( "	Inverted :" ) );
	Logger::LogMessage( cuT( "		%.5f %.5f %.5f %.5f" ), l_glmYUVtoRGB[0][0], l_glmYUVtoRGB[1][0], l_glmYUVtoRGB[2][0], l_glmYUVtoRGB[3][0] );
	Logger::LogMessage( cuT( "		%.5f %.5f %.5f %.5f" ), l_glmYUVtoRGB[0][1], l_glmYUVtoRGB[1][1], l_glmYUVtoRGB[2][1], l_glmYUVtoRGB[3][1] );
	Logger::LogMessage( cuT( "		%.5f %.5f %.5f %.5f" ), l_glmYUVtoRGB[0][2], l_glmYUVtoRGB[1][2], l_glmYUVtoRGB[2][2], l_glmYUVtoRGB[3][2] );
	Logger::LogMessage( cuT( "		%.5f %.5f %.5f %.5f" ), l_glmYUVtoRGB[0][3], l_glmYUVtoRGB[1][3], l_glmYUVtoRGB[2][3], l_glmYUVtoRGB[3][3] );
}

BENCHMARK( MatrixCopyCastor, NB_TESTS )
{
	DoNotOptimizeAway( g_mtx2 = g_mtx1 );
}

BENCHMARK( MatrixCopyGlm, NB_TESTS )
{
	DoNotOptimizeAway( g_mtx2glm = g_mtx1glm );
}

BENCHMARK( StrToWStrUsingConvert, NB_TESTS )
{
	convert( g_strIn, g_wstrOut );
}

BENCHMARK( StrToWStrUsingWiden, NB_TESTS )
{
	widen( g_strIn, g_wstrOut );
}

BENCHMARK( WStrToStrUsingConvert, NB_TESTS )
{
	convert( g_wstrIn, g_strOut );
}

BENCHMARK( WStrToStrUsingNarrow, NB_TESTS )
{
	narrow( g_wstrIn, g_strOut );
}

BENCHMARK( MtxMultCastor, NB_TESTS )
{
	g_mtx4x4CuC = g_mtx4x4CuA * g_mtx4x4CuB;
}

BENCHMARK( MtxMultCL, NB_TESTS )
{
	if( g_bClInitialised )
	{
#if CL_VERSION_1_2
		g_queue.enqueueNDRangeKernel( g_kernel, cl::NullRange, cl::NDRange( 1, 1 ), cl::NDRange( 4, 4 ), NULL, &g_event );
		g_queue.enqueueReadBuffer( g_clBufferMtx4x4C, CL_TRUE, 0, 16 * sizeof( float ), g_bufferMtx4x4C );
#else
		size_t nullRange[3] = { 0 };
		size_t globalWorkSize[3] = { 1, 1, 1 };
		size_t localWorkSize[3] = { 4, 4, 1 };
		clEnqueueNDRangeKernel( g_queue, g_kernel, 1, nullRange, globalWorkSize, localWorkSize, 0, NULL, &g_event );
		clEnqueueReadBuffer( g_queue, g_clBufferMtx4x4C, CL_TRUE, 0, 16 * sizeof( float ), g_bufferMtx4x4C, 0, NULL, &g_event );
#endif
	}
}

void DoInitOpenCL()
{
/*
	std::string l_prog;
	cl_int l_iErr;
	cl::Platform::get( &g_arrayPlatforms );
	bool l_bContinue = CheckErr( g_arrayPlatforms.size() !=0 ? CL_SUCCESS : -1, "cl::Platform::get" );

	if( l_bContinue )
	{
		Logger::LogMessage( _T( "Platform count is: %d" ), g_arrayPlatforms.size() );
		std::string l_strInfo;
		g_platform = g_arrayPlatforms[0];
		g_platform.getInfo( (cl_platform_info)CL_PLATFORM_NAME, &l_strInfo );
		Logger::LogMessage( _T( "  Name    : %s" ), str_utils::from_str( l_strInfo ).c_str() );
		g_platform.getInfo( (cl_platform_info)CL_PLATFORM_VENDOR, &l_strInfo );
		Logger::LogMessage( _T( "  Vendor  : %s" ), str_utils::from_str( l_strInfo ).c_str() );
		g_platform.getInfo( (cl_platform_info)CL_PLATFORM_VERSION, &l_strInfo );
		Logger::LogMessage( _T( "  Version : %s" ), str_utils::from_str( l_strInfo ).c_str() );
		cl_context_properties l_props[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(g_platform)(), 0 };
		g_context = cl::Context( CL_DEVICE_TYPE_CPU, l_props, NULL, NULL, &l_iErr );
		l_bContinue = CheckErr( l_iErr, "Context::Context()" );
	}

	if( l_bContinue )
	{
		g_arrayDevices = g_context.getInfo< CL_CONTEXT_DEVICES >();
		l_bContinue = CheckErr( g_arrayDevices.size() > 0 ? CL_SUCCESS : -1, "l_arrayDevices.size() > 0" );
	}

	if( l_bContinue )
	{
		g_queue = cl::CommandQueue( g_context, g_arrayDevices[0], 0, &l_iErr );
		l_bContinue = CheckErr( l_iErr, "CommandQueue::CommandQueue()" );
	}

	if( l_bContinue )
	{
		g_clBufferMtx4x4A = cl::Buffer( g_context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, 16 * sizeof( float ), g_bufferMtx4x4A, &l_iErr );
		l_bContinue = CheckErr( l_iErr, "Buffer::Buffer() -Mtx4x4 A" );
	}

	if( l_bContinue )
	{
		g_clBufferMtx4x4B = cl::Buffer( g_context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, 16 * sizeof( float ), g_bufferMtx4x4B, &l_iErr );
		l_bContinue = CheckErr( l_iErr, "Buffer::Buffer() -Mtx4x4 B" );
	}

	if( l_bContinue )
	{
		g_clBufferMtx4x4C = cl::Buffer( g_context, CL_MEM_READ_WRITE, 16 * sizeof( float ), NULL, &l_iErr );
		l_bContinue = CheckErr( l_iErr, "Buffer::Buffer() -Mtx4x4 C" );
	}

	if( l_bContinue )
	{
		std::ifstream l_file( "mtx_ope.cl" );
		l_bContinue = CheckErr( l_file ? CL_SUCCESS : -1, "./mtx_ope.cl" );

		if( l_bContinue )
		{
			l_prog = std::string( std::istreambuf_iterator<char>( l_file ), std::istreambuf_iterator<char>() );
		}
	}

	if( l_bContinue )
	{
		cl::Program::Sources l_source( 1, std::make_pair( l_prog.c_str(), l_prog.length() + 1 ) );
		g_program = cl::Program( g_context, l_source );
		l_iErr = g_program.build( g_arrayDevices );
		l_bContinue = CheckErr( l_iErr, "Program::build()" );

		if( !l_bContinue )
		{
			cl::STRING_CLASS l_strInfo;
			l_iErr = g_program.getBuildInfo( g_arrayDevices[0], CL_PROGRAM_BUILD_LOG, &l_strInfo );

			if( CheckErr( l_iErr, "Program::getBuildInfo()" ) )
			{
				Logger::LogMessage( l_strInfo );
			}
		}
	}

	if( l_bContinue )
	{
		g_kernel = cl::Kernel( g_program, "mult_4x4", &l_iErr );
		l_bContinue = CheckErr( l_iErr, "Kernel::Kernel()" );
	}

	if( l_bContinue )
	{
		l_iErr = g_kernel.setArg( 0, g_clBufferMtx4x4C );
		l_bContinue = CheckErr( l_iErr, "Kernel::setArg() - g_clBufferMtx4x4C" );
	}

	if( l_bContinue )
	{
		l_iErr = g_kernel.setArg( 1, g_clBufferMtx4x4A );
		l_bContinue = CheckErr( l_iErr, "Kernel::setArg() - g_clBufferMtx4x4A" );
	}

	if( l_bContinue )
	{
		l_iErr = g_kernel.setArg( 2, g_clBufferMtx4x4B );
		l_bContinue = CheckErr( l_iErr, "Kernel::setArg() - g_clBufferMtx4x4B" );
	}

	if( l_bContinue )
	{
#if CL_VERSION_1_2
		g_queue.enqueueNDRangeKernel( g_kernel, cl::NullRange, cl::NDRange( 1, 1 ), cl::NDRange( 4, 4 ), NULL, &g_event );
		g_queue.enqueueReadBuffer( g_clBufferMtx4x4C, CL_TRUE, 0, 16 * sizeof( float ), g_bufferMtx4x4C );
#else
		size_t nullRange[3] = { 0 };
		size_t globalWorkSize[3] = { 1, 1, 1 };
		size_t localWorkSize[3] = { 4, 4, 1 };
		clEnqueueNDRangeKernel( g_queue, g_kernel, 1, nullRange, globalWorkSize, localWorkSize, 0, NULL, &g_event );
		clEnqueueReadBuffer( g_queue, g_clBufferMtx4x4C, CL_TRUE, 0, 16 * sizeof( float ), g_bufferMtx4x4C, 0, NULL, &g_event );
#endif
	}

	g_bClInitialised = l_bContinue;
*/
}

int main( int argc, char const * argv[] )
{
	int l_iReturn = EXIT_SUCCESS;

	g_mtx1[0][0] =  0.299f;		g_mtx1[1][0] =  0.587f;		g_mtx1[2][0] =  0.114f;		g_mtx1[3][0] = 0.0f;
	g_mtx1[0][1] = -0.14713f;	g_mtx1[1][1] = -0.28886f;	g_mtx1[2][1] =  0.436f;		g_mtx1[3][1] = 0.0f;
	g_mtx1[0][2] =  0.615f;		g_mtx1[1][2] = -0.51499f;	g_mtx1[2][2] = -0.10001f;	g_mtx1[3][2] = 0.0f;
	g_mtx1[0][3] =  0.0f;		g_mtx1[1][3] = 0.0f;		g_mtx1[2][3] = 0.0f;		g_mtx1[3][3] = 1.0f;

	g_mtx1glm[0][0] =  0.299f;		g_mtx1glm[1][0] =  0.587f;		g_mtx1glm[2][0] =  0.114f;		g_mtx1glm[3][0] = 0.0f;
	g_mtx1glm[0][1] = -0.14713f;	g_mtx1glm[1][1] = -0.28886f;	g_mtx1glm[2][1] =  0.436f;		g_mtx1glm[3][1] = 0.0f;
	g_mtx1glm[0][2] =  0.615f;		g_mtx1glm[1][2] = -0.51499f;	g_mtx1glm[2][2] = -0.10001f;	g_mtx1glm[3][2] = 0.0f;
	g_mtx1glm[0][3] =  0.0f;		g_mtx1glm[1][3] = 0.0f;			g_mtx1glm[2][3] = 0.0f;			g_mtx1glm[3][3] = 1.0f;

	if( argc < 2 )
	{
		std::cout << "Usage : CastorUtilsTest <log_file> <count (opt)>" << std::endl;
		l_iReturn = EXIT_FAILURE;
		std::cin.ignore( std::numeric_limits< std::streamsize >::max(), '\n' );
	}
	else
	{
		int l_iCount = 1;

		if( argc == 3 )
		{
			l_iCount = std::max< int >( 1, atoi( argv[2] ) );
		}

		Logger::Initialise( eLOG_TYPE_DEBUG );
		Logger::SetFileName( File::DirectoryGetCurrent() / str_utils::from_str( argv[1] ) );
		
		randomInit( g_bufferMtx4x4A, g_mtx4x4CuA.ptr(), 16 );
		randomInit( g_bufferMtx4x4B, g_mtx4x4CuB.ptr(), 16 );

		DoInitOpenCL();

		BENCHLOOP( l_iCount );

		Logger::LogMessage( cuT( "Hit return to continue..." ) );
		std::cin.ignore( std::numeric_limits< std::streamsize >::max(), '\n' );
		Logger::Cleanup();
	}

	return l_iReturn;
}