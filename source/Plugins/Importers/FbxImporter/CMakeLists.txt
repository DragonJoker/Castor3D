set( FOLDER_NAME FbxImporter )
project( ${FOLDER_NAME} )

find_package( FbxSDK )

if( FbxSDK_FOUND OR FBXSDK_FOUND )
	set( Castor3DBinsDependencies 
		${Castor3DBinsDependencies}
		castor::${FOLDER_NAME}
		PARENT_SCOPE
	)
	include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
	include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
	include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )
	include_directories( ${FbxSDK_INCLUDE_DIR} )

	set( ${PROJECT_NAME}_DESCRIPTION "FBX SDK powered importer" )
	set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
	set( ${PROJECT_NAME}_VERSION_MINOR 1 )
	set( ${PROJECT_NAME}_VERSION_BUILD 0 )

	set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )
	set( PROJECT_SOVERSION "${${PROJECT_NAME}_VERSION_BUILD}" )

	set( ${PROJECT_NAME}_HDR_FILES
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/FbxImporterFile.hpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/FbxMaterialImporter.hpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/FbxImporterFile.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/FbxImporterPlugin.cpp
		${CASTOR_SOURCE_DIR}/source/Plugins/Importers/${FOLDER_NAME}/FbxMaterialImporter.cpp
	)
	source_group( "Header Files"
		FILES
			${${PROJECT_NAME}_HDR_FILES}
	)
	source_group( "Source Files"
		FILES
			${${PROJECT_NAME}_SRC_FILES}
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
			${FbxSDK_INCLUDE_DIR}
	)
	target_link_libraries( ${PROJECT_NAME}
		PUBLIC
			castor::Castor3D
		PRIVATE
			${ImporterDependencies}
			FbxSDK::FbxSDK
			castor::AssimpImporter
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
		set( CMAKE_INSTALL_DEBUG_LIBRARIES TRUE )
		set( CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION bin/$<$<CONFIG:Debug>:Debug/> )
		get_property( LibPathRel TARGET FbxSDK::FbxSDK PROPERTY IMPORTED_LOCATION_RELEASE )
		set( ADDITIONAL_DEPS_REL
			${ADDITIONAL_DEPS_REL}
			${LibPathRel}
			PARENT_SCOPE
		)
		get_property( LibPathDbg TARGET FbxSDK::FbxSDK PROPERTY IMPORTED_LOCATION_DEBUG )
		set( ADDITIONAL_DEPS_DBG
			${ADDITIONAL_DEPS_DBG}
			${LibPathDbg}
			PARENT_SCOPE
		)
		if ( FbxSDK_SDK_LIBRARY_RELEASE OR FbxSDK_SDK_LIBRARY_DEBUG )
			if ( FbxSDK_SDK_LIBRARY_RELEASE AND FbxSDK_SDK_LIBRARY_DEBUG )
				copy_dll( ${PROJECT_NAME} ${FbxSDK_SDK_LIBRARY_DEBUG} ${FbxSDK_SDK_LIBRARY_RELEASE} )
			elseif ( FbxSDK_SDK_LIBRARY_RELEASE )
				copy_dll( ${PROJECT_NAME} ${FbxSDK_SDK_LIBRARY_RELEASE} ${FbxSDK_SDK_LIBRARY_RELEASE} )
			else ()
				copy_dll( ${PROJECT_NAME} ${FbxSDK_SDK_LIBRARY_DEBUG} ${FbxSDK_SDK_LIBRARY_DEBUG} )
			endif ()
		endif ()
	endif ()
else()
	set( Build "no (FBX SDK library not found)" PARENT_SCOPE )
endif()
