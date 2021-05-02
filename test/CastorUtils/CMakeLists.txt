project( CastorUtilsTest )

if ( VCPKG_TOOLCHAIN )
	find_package( glm CONFIG QUIET )
	find_package( OpenCL CONFIG QUIET )
	if ( glm_FOUND )
		set( GLM_FOUND ${glm_FOUND} )
	endif ()
else ()
	find_package( GLM QUIET )
	find_package( OpenCL QUIET )
endif ()

set( ${PROJECT_NAME}_DESCRIPTION "${PROJECT_NAME} application" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 4 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )
set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )

set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsArrayViewTest.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsBuddyAllocatorTest.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsChangeTrackedTest.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsDynamicBitsetTest.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsMatrixTest.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsObjectsPoolTest.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsPixelBufferExtractTest.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsPixelFormatTest.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsQuaternionTest.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsSignalTest.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsSpeedTest.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsStringTest.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsTestPrerequisites.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsTextWriterTest.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsThreadPoolTest.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsUniqueTest.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsWorkerThreadTest.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsZipTest.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/cl.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/OpenClBench.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/Point.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/Point.inl
	${CASTOR_SOURCE_DIR}/test/CastorUtils/PoolTestPrerequisites.hpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/TestObjectPool.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsArrayViewTest.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsBuddyAllocatorTest.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsChangeTrackedTest.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsDynamicBitsetTest.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsMatrixTest.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsObjectsPoolTest.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsPixelBufferExtractTest.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsPixelFormatTest.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsQuaternionTest.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsSignalTest.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsSpeedTest.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsStringTest.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsTextWriterTest.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsThreadPoolTest.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsUniqueTest.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsWorkerThreadTest.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/CastorUtilsZipTest.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/main.cpp
	${CASTOR_SOURCE_DIR}/test/CastorUtils/OpenClBench.cpp
)
set( ${PROJECT_NAME}_OCL_FILES
	${CASTOR_SOURCE_DIR}/test/${FOLDER_NAME}/mtx_ope.cl
)
source_group( "Header Files"
	FILES
		${${PROJECT_NAME}_HDR_FILES}
)
source_group( "Source Files"
	FILES
		${${PROJECT_NAME}_SRC_FILES}
)
source_group( "OpenCL Files"
	FILES
		${${PROJECT_NAME}_OCL_FILES}
)
add_target_min(
	${PROJECT_NAME}
	bin_dos
)
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${CMAKE_SOURCE_DIR}/include/Core
		${CMAKE_BINARY_DIR}/include/Core
		${CMAKE_SOURCE_DIR}/test
		${CMAKE_BINARY_DIR}/test
		${CMAKE_SOURCE_DIR}/test/CastorUtilsTest
		${CMAKE_BINARY_DIR}/test/CastorUtilsTest
)
target_link_libraries( ${PROJECT_NAME}
	PRIVATE
		castor::CastorTest
)
if( GLM_FOUND )
	target_compile_definitions( ${PROJECT_NAME}
		PRIVATE 
			CASTOR_USE_GLM
	)
	target_link_libraries( ${PROJECT_NAME}
		PRIVATE
			glm
	)
endif()
if( OPENCL_FOUND )
	target_compile_definitions( ${PROJECT_NAME}
		PRIVATE 
			CASTOR_USE_OCL
	)
	target_link_libraries( ${PROJECT_NAME}
		PRIVATE
			OpenCL::OpenCL
	)
endif()
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		VERSION ${PROJECT_VERSION}
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
		FOLDER "Tests/Castor"
)
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
copy_target_files( ${PROJECT_NAME} "" ${CMAKE_CURRENT_SOURCE_DIR}/mtx_ope.cl )

add_test( NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME} )

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
