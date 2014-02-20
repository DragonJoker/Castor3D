# - Try to find precompiled headers support for GCC 3.4 and 4.x
# Once done this will define:
#
# Variable:
#   PCHSupport_FOUND
#
# Macro:
#   ADD_PRECOMPILED_HEADER  _targetName _input  _dowarn

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

IF (CMAKE_COMPILER_IS_GNUCXX)
	# Verifies if GCC supports precompiled header
	# Its version should be >= 3.4.0
	EXEC_PROGRAM( ${CMAKE_CXX_COMPILER} ARGS ${CMAKE_CXX_COMPILER_ARG1} -dumpversion OUTPUT_VARIABLE gcc_compiler_version)

	IF(gcc_compiler_version MATCHES "4\\.[0-9]\\.[0-9]")
		SET(PCHSupport_FOUND TRUE)
	ELSE()
		IF(gcc_compiler_version MATCHES "3\\.4\\.[0-9]")
			SET(PCHSupport_FOUND TRUE)
		ENDIF()
	ENDIF()

	SET(_PCH_include_prefix -I)
ELSE()
	IF(MSVC)
		SET(PCHSupport_FOUND TRUE) # for experimental msvc support
		SET(_PCH_include_prefix /I)
	ELSE()
		SET(PCHSupport_FOUND FALSE)
	ENDIF()     
ENDIF()


MACRO(_PCH_GET_COMPILE_FLAGS _out_compile_flags)
	SET(${_out_compile_flags} ${CMAKE_CXX_FLAGS} )

	STRING(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _flags_var_name)
	if(${_flags_var_name})
		list(APPEND ${_out_compile_flags} ${${_flags_var_name}} )
	endif(${_flags_var_name})

	# If the compiler is g++ and the target type is shared library, we have
	# to add -fPIC to its compile flags.
	IF(CMAKE_COMPILER_IS_GNUCXX)
		GET_TARGET_PROPERTY(_targetType ${_PCH_current_target} TYPE)
		IF(${_targetType} STREQUAL SHARED_LIBRARY)
			LIST(APPEND ${_out_compile_flags} -fPIC)
		ENDIF()
	ELSE(MSVC)
		LIST(APPEND ${_out_compile_flags} /Fd${CMAKE_CURRENT_BINARY_DIR}/${_PCH_current_target}.pdb)
	ENDIF()

	# Add all include directives...
	GET_DIRECTORY_PROPERTY(DIRINC INCLUDE_DIRECTORIES )
	FOREACH(item ${DIRINC})
	LIST(APPEND ${_out_compile_flags} ${_PCH_include_prefix}${item})
	ENDFOREACH(item)

	# Add all definitions...
	GET_TARGET_PROPERTY(_compiler_flags ${_PCH_current_target} COMPILE_FLAGS)
	if(_compiler_flags)
		LIST(APPEND ${_out_compile_flags} ${_compiler_flags})
	endif()

	GET_DIRECTORY_PROPERTY(_directory_flags COMPILE_DEFINITIONS)
	if(_directory_flags)
		foreach(flag ${_directory_flags})
			LIST(APPEND ${_out_compile_flags} -D${flag})
		endforeach(flag)
	endif(_directory_flags)

	STRING(TOUPPER "COMPILE_DEFINITIONS_${CMAKE_BUILD_TYPE}" _defs_prop_name)
	GET_DIRECTORY_PROPERTY(_directory_flags ${_defs_prop_name})
	if(_directory_flags)
		foreach(flag ${_directory_flags})
			LIST(APPEND ${_out_compile_flags} -D${flag})
		endforeach(flag)
	endif(_directory_flags)

	get_target_property(_target_flags ${_PCH_current_target} 
	COMPILE_DEFINITIONS)
	if(_target_flags)
		foreach(flag ${_target_flags})
			LIST(APPEND ${_out_compile_flags} -D${flag})
		endforeach(flag)
	endif(_target_flags)

	get_target_property(_target_flags ${_PCH_current_target} ${_defs_prop_name})
	if(_target_flags)
		foreach(flag ${_target_flags})
			LIST(APPEND ${_out_compile_flags} -D${flag})
		endforeach(flag)
	endif(_target_flags)

	SEPARATE_ARGUMENTS(${_out_compile_flags})
ENDMACRO(_PCH_GET_COMPILE_FLAGS)

MACRO(_PCH_GET_COMPILE_COMMAND out_command _input _output)
	FILE(TO_NATIVE_PATH ${_input} _native_input)
	FILE(TO_NATIVE_PATH ${_output} _native_output)

	IF(CMAKE_COMPILER_IS_GNUCXX)
		IF(CMAKE_CXX_COMPILER_ARG1)
			# remove leading space in compiler argument
			STRING(REGEX REPLACE "^ +" "" pchsupport_compiler_cxx_arg1 
			${CMAKE_CXX_COMPILER_ARG1})
		ELSE()
			SET(pchsupport_compiler_cxx_arg1 "")
		ENDIF()
		SET(${out_command} ${CMAKE_CXX_COMPILER} ${pchsupport_compiler_cxx_arg1} ${_compile_FLAGS}     -x c++-header -o ${_output} ${_input})
	ELSE()
		set(_tempdir ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${_PCH_current_target}.dir)
		GET_FILENAME_COMPONENT(_namewe ${_input} NAME_WE)
		SET(_pchsource ${_tempdir}/${_namewe}.cpp)
		FILE(TO_NATIVE_PATH ${_pchsource} _native_pchsource)

		SET(_dummy_str "#include \"${_input}\"")
		FILE(WRITE ${_pchsource} ${_dummy_str})

		SET(${out_command} ${CMAKE_CXX_COMPILER} ${_compile_FLAGS} /c /Fp${_native_output} /Yc${_native_input} /Fo${_tempdir}/ ${_native_pchsource} /nologo)       
	ENDIF()
ENDMACRO(_PCH_GET_COMPILE_COMMAND )

MACRO(_PCH_GET_TARGET_COMPILE_FLAGS _cflags  _header_name _pch_path _dowarn )
	FILE(TO_NATIVE_PATH ${_pch_path} _native_pch_path)
	#message(${_native_pch_path})

	IF(CMAKE_COMPILER_IS_GNUCXX)
		# for use with distcc and gcc>4.0.1 if preprocessed files are accessible
		# on all remote machines set
		# PCH_ADDITIONAL_COMPILER_FLAGS to -fpch-preprocess
		# if you want warnings for invalid header files (which is very 
		# inconvenient if you have different versions of the headers for
		# different build types you may set _pch_dowarn
		LIST(APPEND ${_cflags} ${PCH_ADDITIONAL_COMPILER_FLAGS})
		IF (_dowarn)
			LIST(APPEND ${_cflags} -Winvalid-pch)
		ENDIF()
	ELSE()
		set(${_cflags} "/Fp${_native_pch_path}" "/Yu${_header_name}" )  
	ENDIF()     

	get_target_property(_old_target_cflags ${_PCH_current_target} COMPILE_FLAGS)
	if(_old_target_cflags)
		list(APPEND ${_cflags} ${_old_target_cflags})
	endif()

	STRING(REPLACE ";" " " ${_cflags} "${${_cflags}}")
ENDMACRO(_PCH_GET_TARGET_COMPILE_FLAGS )

MACRO(GET_PRECOMPILED_HEADER_OUTPUT _targetName _input _output)
	GET_FILENAME_COMPONENT(_name ${_input} NAME)
	if(CMAKE_COMPILER_IS_GNUCXX)
		set(_build "")
		IF(CMAKE_BUILD_TYPE)
			set(_build _${CMAKE_BUILD_TYPE})
		endif()
		file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_name}.gch)
		SET(_output "${CMAKE_CURRENT_BINARY_DIR}/${_name}.gch/${_targetName}${_build}.h++")
	else()
		SET(_output "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${_targetName}${_name}.pch")
	endif()
ENDMACRO(GET_PRECOMPILED_HEADER_OUTPUT _targetName _input)

MACRO(ADD_PRECOMPILED_HEADER _targetName _pchHeader _input)
	SET(_PCH_current_target ${_targetName})

	if(${ARGN})
		set(_dowarn 1)
	else()
		set(_dowarn 0)
	endif()

	GET_FILENAME_COMPONENT(_name ${_input} NAME)
	GET_FILENAME_COMPONENT(_path ${_input} PATH)
	GET_PRECOMPILED_HEADER_OUTPUT( ${_targetName} ${_input} _output)
	GET_FILENAME_COMPONENT(_outdir ${_output} PATH )

	FILE(MAKE_DIRECTORY ${_outdir})

	_PCH_GET_COMPILE_FLAGS(_compile_FLAGS)

	# Ensure same directory! Required by gcc
	IF (NOT ${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
		SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/${_name} PROPERTIES GENERATED 1)
		ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_targetName}${_name} COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${_input} ${CMAKE_CURRENT_BINARY_DIR}/${_targetName}${_name} DEPENDS ${_input})
	ENDIF()

	#message("_command  ${_input} ${_output}")
	_PCH_GET_COMPILE_COMMAND(_command  ${CMAKE_CURRENT_BINARY_DIR}/${_targetName}${_name} ${_output} )

	ADD_CUSTOM_COMMAND(OUTPUT ${_output} COMMAND ${_command} IMPLICIT_DEPENDS CXX  ${CMAKE_CURRENT_BINARY_DIR}/${_targetName}${_name})

	GET_TARGET_PROPERTY(_sources ${_targetName} SOURCES)
	FOREACH(_src ${_sources})
		SET_SOURCE_FILES_PROPERTIES(${_src} PROPERTIES OBJECT_DEPENDS ${_output})
	ENDFOREACH()

	_PCH_GET_TARGET_COMPILE_FLAGS(_target_cflags ${_name} ${_output} ${_dowarn})

	if(_target_c_flags)
		SET_TARGET_PROPERTIES(${_targetName} PROPERTIES COMPILE_FLAGS ${_target_cflags} )
	endif(_target_c_flags)
ENDMACRO(ADD_PRECOMPILED_HEADER)