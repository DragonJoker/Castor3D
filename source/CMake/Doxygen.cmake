option( PROJECTS_GENERATE_DOC "Generate Doxygen documentation" FALSE )

#--------------------------------------------------------------------------------------------------
#	Function :	add_target_doc
#	Generates doc for given target
#--------------------------------------------------------------------------------------------------
function( add_target_doc TARGET_NAME LANGUAGE EXT_LIST )
	find_package( HTMLHelp )
	set( CHM_NAME ${TARGET_NAME}-${LANGUAGE}.chm )
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/Doc/${TARGET_NAME}.Doxyfile.in
		${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}_${LANGUAGE}.Doxyfile
		@ONLY
		NEWLINE_STYLE LF
	)
	if ( DOXYGEN_FOUND AND PROJECTS_GENERATE_DOC )
		set( DOXYGEN_INPUT ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}_${LANGUAGE}.Doxyfile )
		set( DOXYGEN_OUTPUT ${PROJECTS_DOCUMENTATION_OUTPUT_DIR}/${TARGET_NAME}/${LANGUAGE} )
		file( MAKE_DIRECTORY ${DOXYGEN_OUTPUT} )
		set( DOXYGEN_TARGET_NAME ${TARGET_NAME}_${LANGUAGE}_Doc )
		add_custom_target(
			${DOXYGEN_TARGET_NAME}
			COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
			COMMENT "Building Doxygen ${LANGUAGE} documentation for ${TARGET_NAME}" VERBATIM
		)
		set_property( TARGET ${DOXYGEN_TARGET_NAME} PROPERTY FOLDER "Documentation/${TARGET_NAME}" )
		set( _DOC_FILE ${PROJECTS_DOCUMENTATION_OUTPUT_DIR}/${TARGET_NAME}/${LANGUAGE}/${CHM_NAME} )
		if ( EXISTS ${_DOC_FILE} )
			install(
				FILES ${_DOC_FILE}
				DESTINATION share/doc/${TARGET_NAME}
				COMPONENT ${TARGET_NAME}_doc
			)
		endif ()
	endif ()
endfunction( add_target_doc )