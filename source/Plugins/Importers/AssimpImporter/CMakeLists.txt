set( FOLDER_NAME AssimpImporter )
project( ${FOLDER_NAME} )

if ( VCPKG_TOOLCHAIN )
	option( CASTOR_USE_SYSTEM_ASSIMP "Use system Assimp instead of vcpkg's" OFF )
endif ()

if ( VCPKG_BUILD OR ( VCPKG_TOOLCHAIN AND NOT CASTOR_USE_SYSTEM_ASSIMP ) )
	set( irrlicht_DIR ${VCPKG_SHARE_DIR}/irrlicht )
	set( polyclipping_DIR ${VCPKG_SHARE_DIR}/polyclipping )
	set( minizip_DIR ${VCPKG_SHARE_DIR}/minizip )
	set( kubazip_DIR ${VCPKG_SHARE_DIR}/kubazip )
	set( poly2tri_DIR ${VCPKG_SHARE_DIR}/poly2tri )
	set( pugixml_DIR ${VCPKG_SHARE_DIR}/pugixml )
	set( RapidJSON_DIR ${VCPKG_SHARE_DIR}/rapidjson )
	set( utf8cpp_DIR ${VCPKG_SHARE_DIR}/utf8cpp )
	if ( VCPKG_BUILD )
		find_library( ZLIB_LIBRARY_DEBUG zlib PATHS ${_VCPKG_INSTALLED_DIR}/debug/lib )
		find_library( ZLIB_LIBRARY_RELEASE zlib PATHS ${_VCPKG_INSTALLED_DIR}/lib )
		find_path( ZLIB_INCLUDE_DIR zlib.h PATHS ${_VCPKG_INSTALLED_DIR}/include )
	endif ()
	set( assimp_DIR ${VCPKG_SHARE_DIR}/assimp )
	find_package( assimp CONFIG QUIET )
	set( assimp_LIBRARIES assimp::assimp )
else ()
	set( CMAKE_MODULE_PATH
		${CMAKE_CURRENT_SOURCE_DIR}/CMake
		${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules
		${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules/Traditional
		${CMAKE_CURRENT_SOURCE_DIR}/CMake/Toolchains
	)
	find_package( assimp 5.0 )
endif ()

if( assimp_FOUND )
	set( Castor3DBinsDependencies 
		${Castor3DBinsDependencies}
		castor::${FOLDER_NAME}
		PARENT_SCOPE
	)

	set( ${PROJECT_NAME}_DESCRIPTION "Assimp powered importer" )
	set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
	set( ${PROJECT_NAME}_VERSION_MINOR 9 )
	set( ${PROJECT_NAME}_VERSION_BUILD 0 )

	set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )
	set( PROJECT_SOVERSION "${${PROJECT_NAME}_VERSION_BUILD}" )

	set( ${PROJECT_NAME}_NVS_FILES
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/Assimp.natvis
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpImporter.natvis
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpAnimationImporter.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpCameraImporter.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpHelpers.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpImporterFile.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpLightImporter.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpMaterialImporter.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpMeshImporter.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpSceneNodeImporter.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpSkeletonImporter.hpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpAnimationImporter.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpCameraImporter.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpImporterFile.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpImporterPlugin.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpLightImporter.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpMaterialImporter.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpMeshImporter.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpSceneNodeImporter.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpSkeletonImporter.cpp
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
	add_library( castor::${FOLDER_NAME}
		ALIAS
		${PROJECT_NAME}
	)
	target_include_directories( ${PROJECT_NAME}
		PRIVATE
			${ImporterIncludes}
	)
	target_include_directories( ${PROJECT_NAME} SYSTEM
		PRIVATE
			${assimp_INCLUDE_DIR}
	)
	target_link_libraries( ${PROJECT_NAME}
		PUBLIC
			castor::Castor3D
		PRIVATE
			${ImporterDependencies}
			${assimp_LIBRARIES}
	)
	c3d_release_pdbs( ${PROJECT_NAME} )
	install_target_ex( ${PROJECT_NAME}
		Castor3D
		Plugins
		plugin
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}
	)
	set_target_properties( ${PROJECT_NAME}
		PROPERTIES
			VERSION ${PROJECT_VERSION}
			SOVERSION ${PROJECT_SOVERSION}
			PREFIX ${C3D_LIB_PREFIX}
			FOLDER "Plugins/Importers"
	)
	set( CASTOR_INSTEXP_PLUGINS
		${CASTOR_INSTEXP_PLUGINS}
		${PROJECT_NAME}
		PARENT_SCOPE
	)
	set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
	add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

	if ( WIN32 )
		get_property( LibPathRel TARGET assimp::assimp PROPERTY IMPORTED_LOCATION_RELEASE )
		set( ADDITIONAL_DEPS_REL
			${ADDITIONAL_DEPS_REL}
			${LibPathRel}
			PARENT_SCOPE
		)
		get_property( LibPathDbg TARGET assimp::assimp PROPERTY IMPORTED_LOCATION_DEBUG )
		set( ADDITIONAL_DEPS_DBG
			${ADDITIONAL_DEPS_DBG}
			${LibPathDbg}
			PARENT_SCOPE
		)
	elseif ( {${CASTOR_PACKAGE_DEB} )
		set( CPACK_DEBIAN_PACKAGE_DEPENDS "libassimp3-dev, ${CPACK_DEBIAN_PACKAGE_DEPENDS}" )
	endif ()
else()
	set( Build "no (ASSIMP library not found)" PARENT_SCOPE )
endif()
