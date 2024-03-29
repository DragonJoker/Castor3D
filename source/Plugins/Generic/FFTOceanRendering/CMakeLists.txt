set( FOLDER_NAME FFTOceanRendering )
project( ${FOLDER_NAME} )

if ( VCPKG_TOOLCHAIN )
	find_path( VKFFT_INCLUDE_DIRS "VkFFT/vkFFT.h" PATHS ${VCPKG_TRIPLET_DIR}/include )
else ()
	find_path( VKFFT_INCLUDE_DIRS "VkFFT/vkFFT.h" )
endif ()

if ( CASTOR_USE_GLSLANG AND VKFFT_INCLUDE_DIRS )
	set( VKFFT_BACKEND "0" )
	set( Castor3DBinsDependencies 
		${Castor3DBinsDependencies}
		castor::${FOLDER_NAME}
		PARENT_SCOPE
	)

	set( ${PROJECT_NAME}_DESCRIPTION "FFT Ocean rendering for Castor3D" )
	set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
	set( ${PROJECT_NAME}_VERSION_MINOR 4 )
	set( ${PROJECT_NAME}_VERSION_BUILD 0 )

	set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )
	set( PROJECT_SOVERSION "${${PROJECT_NAME}_VERSION_BUILD}" )

	set( ${PROJECT_NAME}_HDR_FILES
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/BakeHeightGradientPass.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/DownsampleDistributionPass.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/FFTWavesComponent.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/FFTWaterComponent.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/GenerateDisplacementPass.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/GenerateDistributionPass.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/GenerateHeightmapPass.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/GenerateMipmapsPass.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/GenerateNormalPass.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/OceanFFT.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/OceanFFTRenderingPrerequisites.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/OceanFFTUbo.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/ProcessFFTPass.hpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/BakeHeightGradientPass.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/DownsampleDistributionPass.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/FFTWavesComponent.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/FFTWaterComponent.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/GenerateDisplacementPass.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/GenerateDistributionPass.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/GenerateHeightmapPass.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/GenerateMipmapsPass.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/GenerateNormalPass.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/OceanFFT.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/OceanFFTRenderingPlugin.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/OceanFFTUbo.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/ProcessFFTPass.cpp
	)
	source_group( "Header Files" FILES ${${PROJECT_NAME}_HDR_FILES} )
	source_group( "Source Files" FILES ${${PROJECT_NAME}_SRC_FILES} )

	add_target_min(
		${PROJECT_NAME}
		plugin
		""
		""
	)
	target_sources( ${PROJECT_NAME} 
		PRIVATE
			${CASTOR_EDITORCONFIG_FILE}
	)
	if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
		target_compile_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/Zi>" )
		target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/DEBUG>" )
		target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:REF>" )
		target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>" )
	endif ()
	find_path( glslang_CAPI_DIR glslang_c_interface.h
		PATHS
			glslang/Include
			${glslang_INCLUDE_DIR}
			${glslang_INCLUDE_DIR}/glslang/Include
			${glslang_INCLUDE_DIR}/glslang/glslang/Include
	)
	add_library( castor::${FOLDER_NAME}
		ALIAS
		${PROJECT_NAME}
	)
	target_include_directories( ${PROJECT_NAME}
		PRIVATE
			${GenericIncludes}
			${VKFFT_INCLUDE_DIRS}
			${glslang_INCLUDE_DIR}
			${glslang_CAPI_DIR}
	)
	target_link_libraries( ${PROJECT_NAME}
		PRIVATE
			${glslang_LIBRARIES}
		PUBLIC
			castor::Castor3D
	)
	target_compile_definitions( ${PROJECT_NAME}
		PRIVATE
			VK_API_VERSION=12
	)
	set_target_properties( ${PROJECT_NAME}
		PROPERTIES
			VERSION ${PROJECT_VERSION}
			SOVERSION ${PROJECT_SOVERSION}
			PREFIX ${C3D_LIB_PREFIX}
			FOLDER "Plugins/Generic"
	)
	c3d_release_pdbs( ${PROJECT_NAME} )
	install_target_ex( ${PROJECT_NAME}
		Castor3D
		Plugins
		plugin
		${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}
	)
	set( CASTOR_INSTEXP_PLUGINS
		${CASTOR_INSTEXP_PLUGINS}
		${PROJECT_NAME}
		PARENT_SCOPE
	)
	set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
	add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
else ()
	if ( NOT CASTOR_USE_GLSLANG )
		set( Build "no (needs CASTOR_USE_GLSLANG to be ON)" PARENT_SCOPE )
	else ()
		set( Build "no (missing VkFFT headers)" PARENT_SCOPE )
	endif ()
endif ()
