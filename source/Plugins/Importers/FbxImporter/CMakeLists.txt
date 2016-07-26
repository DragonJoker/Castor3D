project( FbxImporter )

find_package( FbxSDK )

if( FbxSDK_FOUND OR FBXSDK_FOUND )
	message( STATUS "+ Found FBX SDK" )

	include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
	include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
	include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )
	include_directories( ${FbxSDK_INCLUDE_DIR} )

	set( FbxSDKLibraries "" )
	foreach( Lib ${FbxSDK_LIBRARIES} )
		if( FbxSDKLibraries )
			set( FbxSDKLibraries "${FbxSDKLibraries}|${Lib}" )
		else()
			set( FbxSDKLibraries "${Lib}" )
		endif()
	endforeach()

	set( CastorBinsDependencies 
		${CastorBinsDependencies}
		${PROJECT_NAME}
		PARENT_SCOPE
	)

	set( ${PROJECT_NAME}_DESCRIPTION "Fbx SDK powered importer" )
	set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
	set( ${PROJECT_NAME}_VERSION_MINOR	9 )
	set( ${PROJECT_NAME}_VERSION_BUILD	0 )

	add_target(
		${PROJECT_NAME}
		plugin
		"Castor3D"
		"${CastorMinLibraries};${FbxSDKLibraries}"
	)

	set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Importers" )
	set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
	add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

	if ( ( NOT WIN32 ) AND {${CASTOR_PACKAGE_DEB} )
		set( CPACK_DEBIAN_PACKAGE_DEPENDS "libfbxsdk, ${CPACK_DEBIAN_PACKAGE_DEPENDS}" )
	endif ()
else()
	set( Build "no (FBX SDK library not found)" PARENT_SCOPE )
endif()
