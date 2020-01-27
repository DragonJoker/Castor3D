set( FOLDER_NAME AssimpImporter )
project( castor3d${FOLDER_NAME} )

if ( VCPKG_TOOLCHAIN )
	find_package( assimp CONFIG REQUIRED assimp )
	set( ASSIMP_LIBRARIES assimp::assimp )
	set( ASSIMP_FOUND ON )
else ()
	find_package( assimp )
endif ()

if( ASSIMP_FOUND )
	set( Castor3DBinsDependencies 
		${Castor3DBinsDependencies}
		castor::${FOLDER_NAME}
		PARENT_SCOPE
	)

	set( ${PROJECT_NAME}_DESCRIPTION "Assimp powered importer" )
	set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
	set( ${PROJECT_NAME}_VERSION_MINOR	11 )
	set( ${PROJECT_NAME}_VERSION_BUILD	0 )

	add_target(
		${PROJECT_NAME}
		plugin
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}
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
			${ASSIMP_INCLUDE_DIR}
	)
	target_link_libraries( ${PROJECT_NAME}
		PUBLIC
			castor::Castor3D
		PRIVATE
			${ASSIMP_LIBRARIES}
	)

	set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Importers" )
	set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
	add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

	if ( WIN32 )
		if ( NOT VCPKG_TOOLCHAIN )
			# Preparing ASSIMP dll installation
			copy_dll( ${PROJECT_NAME} ${ASSIMP_LIBRARY_DEBUG} ${ASSIMP_LIBRARY_RELEASE} )
		endif ()
	elseif ( {${CASTOR_PACKAGE_DEB} )
		set( CPACK_DEBIAN_PACKAGE_DEPENDS "libassimp3-dev, ${CPACK_DEBIAN_PACKAGE_DEPENDS}" )
	endif ()
else()
	set( Build "no (ASSIMP library not found)" PARENT_SCOPE )
endif()
