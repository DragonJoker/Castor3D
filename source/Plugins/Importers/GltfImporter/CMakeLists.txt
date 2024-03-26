set( FOLDER_NAME GltfImporter )
project( ${FOLDER_NAME} )

if ( VCPKG_TOOLCHAIN )
	set( simdjson_DIR ${VCPKG_SHARE_DIR}/simdjson )
	set( fastgltf_DIR ${VCPKG_SHARE_DIR}/fastgltf )
	find_package( simdjson CONFIG QUIET )
	find_package( fastgltf CONFIG QUIET )
else ()
	find_package( fastgltf )
endif ()

if( fastgltf_FOUND )
	set( Castor3DBinsDependencies 
		${Castor3DBinsDependencies}
		castor::${FOLDER_NAME}
		PARENT_SCOPE
	)

	set( ${PROJECT_NAME}_DESCRIPTION "fastgltf powered importer" )
	set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
	set( ${PROJECT_NAME}_VERSION_MINOR 1 )
	set( ${PROJECT_NAME}_VERSION_BUILD 0 )

	set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )
	set( PROJECT_SOVERSION "${${PROJECT_NAME}_VERSION_BUILD}" )

	set( ${PROJECT_NAME}_NVS_FILES
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfImporter.natvis
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfAnimationImporter.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfCameraImporter.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfImporterFile.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfLightImporter.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfMaterialImporter.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfMeshImporter.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfSceneNodeImporter.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfSkeletonImporter.hpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfAnimationImporter.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfCameraImporter.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfImporterFile.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfImporterPlugin.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfLightImporter.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfMaterialImporter.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfMeshImporter.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfSceneNodeImporter.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/GltfSkeletonImporter.cpp
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
		plugin
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
		PRIVATE
			${ImporterDependencies}
			simdjson::simdjson
			fastgltf::fastgltf
			meshoptimizer::meshoptimizer
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
		plugin
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
