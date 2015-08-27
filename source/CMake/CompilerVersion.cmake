#--------------------------------------------------------------------------------------------------
#	Function :	DumpCompilerVersion
# 	Function which gives the GNU Compiler version, used to build name of project's libs
#--------------------------------------------------------------------------------------------------
function( DumpCompilerVersion OUTPUT_VERSION)
	exec_program( ${CMAKE_CXX_COMPILER}
        ARGS ${CMAKE_CXX_COMPILER_ARG1} -dumpversion 
        OUTPUT_VARIABLE COMPILER_VERSION
    )
	string( REGEX 
        REPLACE
        "([0-9])\\.([0-9])(\\.[0-9])?" "\\1\\2"
        COMPILER_VERSION
        ${COMPILER_VERSION}
    )
	set( ${OUTPUT_VERSION} ${COMPILER_VERSION} PARENT_SCOPE )
endfunction()