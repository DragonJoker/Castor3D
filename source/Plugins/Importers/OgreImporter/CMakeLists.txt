project( OgreImporter )

find_package( Ogre )
find_package( Boost COMPONENTS thread )

if( OGRE_FOUND AND ( BOOST_FOUND OR Boost_FOUND ))
	message( STATUS "+ Found Ogre (version ${Ogre_VERSION})" )

	include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
	include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
	include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )
	include_directories( ${Ogre_INCLUDE_DIR} )
	include_directories( ${Ogre_INCLUDE_DIRS} )
	include_directories( ${Boost_INCLUDE_DIR} )

	link_directories( ${Boost_LIBRARY_DIR_DEBUG} )

	set( OgreLibraries "" )
	foreach( Lib optimized;${Ogre_Main_LIBRARY_RELEASE};debug;${Ogre_Main_LIBRARY_DEBUG} )
		if( OgreLibraries )
			set( OgreLibraries "${OgreLibraries}|${Lib}" )
		else()
			set( OgreLibraries "${Lib}" )
		endif()
	endforeach()

	set( CastorBinsDependencies 
		${CastorBinsDependencies}
		${PROJECT_NAME}
		PARENT_SCOPE
	)

	set( ${PROJECT_NAME}_DESCRIPTION "Ogre3D powered importer" )
	set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
	set( ${PROJECT_NAME}_VERSION_MINOR	9 )
	set( ${PROJECT_NAME}_VERSION_BUILD	0 )

	add_target(
		${PROJECT_NAME}
		plugin
		"Castor3D"
		"${CastorMinLibraries};${OgreLibraries}"
	)

	set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Importers" )
	set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
	add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

	if ( WIN32 )
		# Preparing Ogre3D dll installation
		copy_dll( ${PROJECT_NAME} ${Ogre_Main_LIBRARY_RELEASE} Release )
		copy_dll( ${PROJECT_NAME} ${Ogre_Main_LIBRARY_DEBUG} Debug )
	elseif ( {${CASTOR_PACKAGE_DEB} )
		set( CPACK_DEBIAN_PACKAGE_DEPENDS "libogre3d-dev, ${CPACK_DEBIAN_PACKAGE_DEPENDS}" )
	endif ()
else()
	set( Build "no (Ogre3D library not found)" PARENT_SCOPE )
endif()
