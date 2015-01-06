project( AssimpImporter )

find_package( ASSIMP )

if( ASSIMP_FOUND )
	message( STATUS "+ Found Assimp" )

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
		AssimpImporter
		PARENT_SCOPE
	)

    set( ASSIMP_VERSION_MAJOR	0 )
    set( ASSIMP_VERSION_MINOR	7 )
    set( ASSIMP_VERSION_BUILD	0 )

	add_target(
		AssimpImporter
		plugin
		"Castor3D"
		"${CastorMinLibraries};${ASSIMPLibraries}"
	)

	set_property( TARGET AssimpImporter PROPERTY FOLDER "Importers" )
	set( Build "yes (version ${ASSIMP_VERSION_MAJOR}.${ASSIMP_VERSION_MINOR}.${ASSIMP_VERSION_BUILD})" PARENT_SCOPE )
	add_target_astyle( AssimpImporter ".h;.hpp;.inl;.cpp" )

	if ( WIN32 )
		# Preparing ASSIMP dll installation
		set( ASSIMP_RELEASE_DIR )
		if ( EXISTS ${ASSIMP_LIBRARY_RELEASE_DIR}/assimp.dll )
			set( ASSIMP_RELEASE_DIR ${ASSIMP_LIBRARY_RELEASE_DIR} )
		elseif ( EXISTS ${ASSIMP_LIBRARY_RELEASE_DIR}/../../bin/${PROJECTS_PLATFORM}/assimp.dll )
			get_filename_component( ASSIMP_RELEASE_DIR ${ASSIMP_LIBRARY_RELEASE_DIR} PATH )
			get_filename_component( ASSIMP_RELEASE_DIR ${ASSIMP_RELEASE_DIR} PATH )
			set( ASSIMP_RELEASE_DIR ${ASSIMP_RELEASE_DIR}/bin/${PROJECTS_PLATFORM} )
		elseif ( EXISTS ${ASSIMP_LIBRARY_RELEASE_DIR}/../bin/assimp.dll )
			get_filename_component( ASSIMP_RELEASE_DIR ${ASSIMP_LIBRARY_RELEASE_DIR} PATH )
			set( ASSIMP_RELEASE_DIR ${ASSIMP_RELEASE_DIR}/bin )
		endif ()
		install(
			FILES ${ASSIMP_RELEASE_DIR}/assimp.dll
			DESTINATION bin
			CONFIGURATIONS Release
			COMPONENT AssimpImporter
		)
		add_custom_command(
			TARGET AssimpImporter
			POST_BUILD
			COMMAND if 1==$<CONFIG:Release>
				${CMAKE_COMMAND} -E
					copy_if_different
					${ASSIMP_RELEASE_DIR}/assimp.dll
					${PROJECTS_BINARIES_OUTPUT_DIR}/$<CONFIGURATION>/bin
		)
		set( ASSIMP_DEBUG_DIR )
		if ( EXISTS ${ASSIMP_LIBRARY_DEBUG_DIR}/assimpD.dll )
			set( ASSIMP_DEBUG_DIR ${ASSIMP_LIBRARY_DEBUG_DIR} )
		elseif ( EXISTS ${ASSIMP_LIBRARY_DEBUG_DIR}/../../bin/${PROJECTS_PLATFORM}/assimpD.dll )
			get_filename_component( ASSIMP_DEBUG_DIR ${ASSIMP_LIBRARY_DEBUG_DIR} PATH )
			get_filename_component( ASSIMP_DEBUG_DIR ${ASSIMP_DEBUG_DIR} PATH )
			set( ASSIMP_DEBUG_DIR ${ASSIMP_DEBUG_DIR}/bin/${PROJECTS_PLATFORM} )
		elseif ( EXISTS ${ASSIMP_LIBRARY_DEBUG_DIR}/../bin/assimpD.dll )
			get_filename_component( ASSIMP_DEBUG_DIR ${ASSIMP_LIBRARY_DEBUG_DIR} PATH )
			set( ASSIMP_DEBUG_DIR ${ASSIMP_DEBUG_DIR}/bin )
		endif ()
		install(
			FILES ${ASSIMP_DEBUG_DIR}/assimpD.dll
			DESTINATION bin
			CONFIGURATIONS Debug
			COMPONENT AssimpImporter
		)
		add_custom_command(
			TARGET AssimpImporter
			POST_BUILD
			COMMAND if 1==$<CONFIG:Debug>
				${CMAKE_COMMAND} -E
					copy_if_different
					${ASSIMP_DEBUG_DIR}/assimpD.dll
					${PROJECTS_BINARIES_OUTPUT_DIR}/$<CONFIGURATION>/bin
		)
	elseif ( {${CASTOR_PACKAGE_DEB} )
		set( CPACK_DEBIAN_PACKAGE_DEPENDS "libassimp3-dev, ${CPACK_DEBIAN_PACKAGE_DEPENDS}" )
	endif ()
else()
	set( Build "no (ASSIMP library not found)" PARENT_SCOPE )
endif()
