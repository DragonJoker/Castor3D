#--------------------------------------------------------------------------------------------------
#	Macro :	copy_languages
#	Used to copy language files for TARGET_NAME from SRC_FOLDER to DST_FOLDER
#--------------------------------------------------------------------------------------------------
macro( copy_languages TARGET_NAME SRC_FOLDER DST_FOLDER LANGUAGES )
	# First compute the destination folders
	if ( MSVC )
		set( FOLDERS
			"${PROJECTS_BINARIES_OUTPUT_DIR}/Debug/share/${DST_FOLDER}"
			"${PROJECTS_BINARIES_OUTPUT_DIR}/Release/share/${DST_FOLDER}"
		)
	else ()
		set( FOLDERS
			"${PROJECTS_BINARIES_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}/share/${DST_FOLDER}"
		)
	endif ()
	# Then copy each language file into each destination folder
	FOREACH( FOLDER ${FOLDERS} )
		file( MAKE_DIRECTORY "${FOLDER}" )
		FOREACH( LANGUAGE ${LANGUAGES} )
			file( MAKE_DIRECTORY "${FOLDER}/${LANGUAGE}" )
			file( COPY ${SRC_FOLDER}/po/${LANGUAGE}/${TARGET_NAME}.mo DESTINATION ${FOLDER}/${LANGUAGE} )
		ENDFOREACH()
	ENDFOREACH()
	# For non Win32 platforms, prepare installation of language files
	if ( NOT WIN32 )
		FOREACH( LANGUAGE ${LANGUAGES} )
			install(
				FILES ${SRC_FOLDER}/po/${LANGUAGE}/${TARGET_NAME}.mo
				DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${DST_FOLDER}/${LANGUAGE}/
				COMPONENT ${TARGET_NAME}
			)
		ENDFOREACH()
	endif()
endmacro()
