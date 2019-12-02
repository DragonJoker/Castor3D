project( AssimpImporter )

find_package( ASSIMP )

if( ASSIMP_FOUND )
	message( STATUS "+ Found Assimp" )

	include_directories( ${ImporterIncludes} )
	include_directories( ${ASSIMP_INCLUDE_DIR} )

	set( ASSIMPLibraries "" )
	foreach( Lib ${ASSIMP_LIBRARIES} )
		if( ASSIMPLibraries )
			set( ASSIMPLibraries "${ASSIMPLibraries}|${Lib}" )
		else()
			set( ASSIMPLibraries "${Lib}" )
		endif()
	endforeach()

	set( Castor3DBinsDependencies 
		${Castor3DBinsDependencies}
		${PROJECT_NAME}
		PARENT_SCOPE
	)

	set( ${PROJECT_NAME}_DESCRIPTION "Assimp powered importer" )
	set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
	set( ${PROJECT_NAME}_VERSION_MINOR	11 )
	set( ${PROJECT_NAME}_VERSION_BUILD	0 )

	add_target(
		${PROJECT_NAME}
		plugin
		${CASTOR_SOURCE_DIR}/include/Plugins/Importers/${PROJECT_NAME}
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${PROJECT_NAME}
		"Castor3D"
		"${ImporterLibraries};${ASSIMPLibraries}"
	)
	add_library( castor::${PROJECT_NAME}
		ALIAS
		${PROJECT_NAME}
	)

	set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Importers" )
	set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
	add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

	if ( WIN32 )
		# Preparing ASSIMP dll installation
		copy_dll( ${PROJECT_NAME} ${ASSIMP_LIBRARY_DEBUG} ${ASSIMP_LIBRARY_RELEASE} )
	elseif ( {${CASTOR_PACKAGE_DEB} )
		set( CPACK_DEBIAN_PACKAGE_DEPENDS "libassimp3-dev, ${CPACK_DEBIAN_PACKAGE_DEPENDS}" )
	endif ()
else()
	set( Build "no (ASSIMP library not found)" PARENT_SCOPE )
endif()
