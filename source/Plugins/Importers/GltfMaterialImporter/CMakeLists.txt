set( FOLDER_NAME GltfMaterialImporter )
project( ${FOLDER_NAME} )

if ( VCPKG_TOOLCHAIN )
	set( simdjson_DIR ${VCPKG_SHARE_DIR}/simdjson )
	set( fastgltf_DIR ${VCPKG_SHARE_DIR}/fastgltf )
	find_package( simdjson CONFIG QUIET )
	find_package( fastgltf CONFIG QUIET )
else ()
	FetchContent_Declare( simdjson
		GIT_REPOSITORY https://github.com/simdjson/simdjson
		GIT_TAG b4a0fd3ff4f00c4a5a9ab86046f06daa471a3b57
		SYSTEM )
	set( SIMDJSON_JUST_LIBRARY ON )
	set( SIMDJSON_SANITIZE_UNDEFINED OFF )
	set( SIMDJSON_SANITIZE OFF )
	set( SIMDJSON_SANITIZE_THREADS OFF )
	set( SIMDJSON_BUILD_STATIC OFF )
	set( SIMDJSON_DEVELOPMENT_CHECKS OFF )
	set( SIMDJSON_VERBOSE_LOGGING OFF )
	FetchContent_MakeAvailable( simdjson )
	FetchContent_Declare( fastgltf
		GIT_REPOSITORY https://github.com/spnda/fastgltf
		GIT_TAG 0d1b67a28c4950ea2deb796702006dcbe31e02b3
		SYSTEM )
	set( FASTGLTF_ENABLE_DEPRECATED_EXT ON )
	FetchContent_MakeAvailable( fastgltf )
endif ()

if( fastgltf_FOUND )
	set( Castor3DBinsDependencies 
		${Castor3DBinsDependencies}
		castor::${FOLDER_NAME}
		PARENT_SCOPE
	)

	set( ${PROJECT_NAME}_DESCRIPTION "fastgltf powered material importer" )
	set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
	set( ${PROJECT_NAME}_VERSION_MINOR 1 )
	set( ${PROJECT_NAME}_VERSION_BUILD 0 )

	set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )
	set( PROJECT_SOVERSION "${${PROJECT_NAME}_VERSION_BUILD}" )

	set( ${PROJECT_NAME}_HDR_FILES
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfMaterialImporter.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfMaterialsFile.hpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfMaterialImporter.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfMaterialsFile.cpp
	)
	source_group( "Header Files"
		FILES
			${${PROJECT_NAME}_HDR_FILES}
	)
	source_group( "Source Files"
		FILES
			${${PROJECT_NAME}_SRC_FILES}
	)
	source_group( "Natvis Files"
		FILES
			${${PROJECT_NAME}_NVS_FILES}
	)
	add_target_min(
		${PROJECT_NAME}
		api_dll
		""
		""
	)
	if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
		target_compile_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/Zi>" )
		target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/DEBUG>" )
		target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:REF>" )
		target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>" )
	endif ()
	add_library( castor::${FOLDER_NAME}
		ALIAS
		${PROJECT_NAME}
	)
	target_include_directories( ${PROJECT_NAME}
		PRIVATE
			${ImporterIncludes}
	)
	target_link_libraries( ${PROJECT_NAME}
		PUBLIC
			castor::Castor3D
			fastgltf::fastgltf
		PRIVATE
			${ImporterDependencies}
			simdjson::simdjson
	)
	set_target_properties( ${PROJECT_NAME}
		PROPERTIES
			VERSION ${PROJECT_VERSION}
			SOVERSION ${PROJECT_SOVERSION}
			PREFIX ${C3D_LIB_PREFIX}
			FOLDER "Plugins/Importers"
	)
	c3d_release_pdbs( ${PROJECT_NAME} )
	install_target_ex( ${PROJECT_NAME}
		Castor3D
		Plugins
		api_dll
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}
	)
	set( CASTOR_INSTEXP_PLUGINS
		${CASTOR_INSTEXP_PLUGINS}
		${PROJECT_NAME}
		PARENT_SCOPE
	)
	set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
	add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

	if ( WIN32 )
		get_property( LibPathRel TARGET fastgltf::fastgltf PROPERTY IMPORTED_LOCATION_RELEASE )
		set( ADDITIONAL_DEPS_REL
			${ADDITIONAL_DEPS_REL}
			${LibPathRel}
			PARENT_SCOPE
		)
		get_property( LibPathDbg TARGET fastgltf::fastgltf PROPERTY IMPORTED_LOCATION_DEBUG )
		set( ADDITIONAL_DEPS_DBG
			${ADDITIONAL_DEPS_DBG}
			${LibPathDbg}
			PARENT_SCOPE
		)
	endif ()
else()
	set( Build "no (fastgltf library not found)" PARENT_SCOPE )
endif()
