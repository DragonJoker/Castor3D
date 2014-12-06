project( ASSIMPImporter )

find_package( ASSIMP )

if( ASSIMP_FOUND )
	message( STATUS "+ Found ASSIMP" )

    include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
    include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
    include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )
	include_directories( ${ASSIMP_INCLUDE_DIR} )

	set( ASSIMPLibraries "" )
	foreach( Lib ${ASSIMP_LIBRARIES} )
		if( ASSIMPLibraries )
			set( ASSIMPLibraries "${ASSIMPLibraries}|${Lib}" )
		else()
			set( ASSIMPLibraries "${Lib}" )
		endif()
	endforeach()

	set( CastorBinsDependencies 
		${CastorBinsDependencies}
		ASSIMPImporter
		PARENT_SCOPE
	)

    set( ASSIMP_VERSION_MAJOR	0 )
    set( ASSIMP_VERSION_MINOR	7 )
    set( ASSIMP_VERSION_BUILD	0 )

	add_target(
		ASSIMPImporter
		plugin
		"Castor3D"
		"${CastorMinLibraries};${ASSIMPLibraries}"
	)

	set_property( TARGET ASSIMPImporter PROPERTY FOLDER "Importers" )
	set( Build "yes (version ${ASSIMP_VERSION_MAJOR}.${ASSIMP_VERSION_MINOR}.${ASSIMP_VERSION_BUILD})" PARENT_SCOPE )
	add_target_astyle( ASSIMPImporter ".h;.hpp;.inl;.cpp" )

	if ( WIN32 )
		# Preparing ASSIMP dll installation
		if ( EXISTS ${ASSIMP_LIBRARY_RELEASE_DIR}/assimp.dll )
			install(
				FILES ${ASSIMP_LIBRARY_RELEASE_DIR}/assimp.dll
				DESTINATION bin
				CONFIGURATIONS Release
				COMPONENT ASSIMPImporter
			)
		elseif ( EXISTS ${ASSIMP_LIBRARY_RELEASE_DIR}/../../bin/${PROJECTS_PLATFORM}/assimp.dll )
			install(
				FILES ${ASSIMP_LIBRARY_RELEASE_DIR}/../../bin/${PROJECTS_PLATFORM}/assimp.dll
				DESTINATION bin
				CONFIGURATIONS Release
				COMPONENT ASSIMPImporter
			)
		endif ()
		if ( EXISTS ${ASSIMP_LIBRARY_DEBUG_DIR}/assimpD.dll )
			install(
				FILES ${ASSIMP_LIBRARY_DEBUG_DIR}/assimpD.dll
				DESTINATION bin
				CONFIGURATIONS Debug
				COMPONENT ASSIMPImporter
			)
		elseif ( EXISTS ${ASSIMP_LIBRARY_DEBUG_DIR}/../../bin/${PROJECTS_PLATFORM}/assimpD.dll )
			install( FILES ${ASSIMP_LIBRARY_DEBUG_DIR}/../../bin/${PROJECTS_PLATFORM}/assimpD.dll
				DESTINATION bin
				CONFIGURATIONS Debug
				COMPONENT ASSIMPImporter
			)
		endif ()
	elseif ( {${CASTOR_PACKAGE_DEB} )
		set( CPACK_DEBIAN_PACKAGE_DEPENDS "libassimp3-dev, ${CPACK_DEBIAN_PACKAGE_DEPENDS}" )
	endif ()
else()
	set( Build "no (ASSIMP library not found)" PARENT_SCOPE )
endif()
