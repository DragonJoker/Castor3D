set( FOLDER_NAME AssimpImporter )
project( ${FOLDER_NAME} )

if ( VCPKG_TOOLCHAIN )
	option( CASTOR_USE_SYSTEM_ASSIMP "Use system Assimp instead of vcpkg's" OFF )

	if ( CASTOR_USE_SYSTEM_ASSIMP )
		set( CMAKE_MODULE_PATH
			${CMAKE_CURRENT_SOURCE_DIR}/CMake
			${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules
			${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules/Traditional
			${CMAKE_CURRENT_SOURCE_DIR}/CMake/Toolchains
		)
	endif ()
endif ()

if ( VCPKG_BUILD OR ( VCPKG_TOOLCHAIN AND NOT CASTOR_USE_SYSTEM_ASSIMP ) )
	find_package( irrlicht CONFIG QUIET )
	find_package( polyclipping CONFIG QUIET )
	find_package( minizip CONFIG QUIET )
	find_package( kubazip CONFIG QUIET )
	find_package( poly2tri CONFIG QUIET )
	find_package( utf8cpp CONFIG QUIET )
	if ( VCPKG_BUILD )
		find_library( ZLIB_LIBRARY_DEBUG zlib PATHS ${VCPKG_INSTALLED_DIR}/debug/lib )
		find_library( ZLIB_LIBRARY_RELEASE zlib PATHS ${VCPKG_INSTALLED_DIR}/lib )
		find_path( ZLIB_INCLUDE_DIR zlib.h PATHS ${VCPKG_INSTALLED_DIR}/include )
	endif ()
	find_package( assimp CONFIG QUIET )
else ()
	find_package( assimp 5.0 )
endif ()
if ( TARGET assimp::assimp )
	set( assimp_FOUND ON )
	get_target_property( assimp_INCLUDE_DIR assimp::assimp INTERFACE_INCLUDE_DIRECTORIES )
	set( assimp_LIBRARIES assimp::assimp )
	get_target_property( assimp_LOCATION assimp::assimp IMPORTED_LOCATION_DEBUG )
	if ( NOT assimp_LOCATION )
		get_target_property( assimp_LOCATION assimp::assimp IMPORTED_LOCATION_RELEASE )
		get_target_property( assimp_IMPLIB assimp::assimp IMPORTED_IMPLIB_RELEASE )
		set_target_properties( assimp::assimp PROPERTIES
			IMPORTED_LOCATION_DEBUG "${assimp_LOCATION}"
			IMPORTED_IMPLIB_DEBUG "${assimp_IMPLIB}"
		)
	endif ()
endif ()

if( assimp_FOUND )
	set( Castor3DBinsDependencies 
		${Castor3DBinsDependencies}
		castor::${FOLDER_NAME}
		PARENT_SCOPE
	)

	set( ${PROJECT_NAME}_DESCRIPTION "Assimp powered importer" )
	set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
	set( ${PROJECT_NAME}_VERSION_MINOR 6 )
	set( ${PROJECT_NAME}_VERSION_BUILD 0 )

	set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )
	set( PROJECT_SOVERSION "${${PROJECT_NAME}_VERSION_BUILD}" )

	set( ${PROJECT_NAME}_NVS_FILES
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpImporter.natvis
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpHelpers.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpImporter.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/MaterialImporter.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/MeshImporter.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/SceneImporter.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/SkeletonImporter.hpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpImporter.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/AssimpImporterPlugin.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/MaterialImporter.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/MeshImporter.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/SceneImporter.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/SkeletonImporter.cpp
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
	install_target_ex( ${PROJECT_NAME}
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
	elseif ( {${CASTOR_PACKAGE_DEB} )
		set( CPACK_DEBIAN_PACKAGE_DEPENDS "libassimp3-dev, ${CPACK_DEBIAN_PACKAGE_DEPENDS}" )
	endif ()
else()
	set( Build "no (ASSIMP library not found)" PARENT_SCOPE )
endif()
