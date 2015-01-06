include( Languages )
include( Logging )
include( CompilerVersion )
include( ComputeAbi )

if( MSVC )
	find_package( VLD )
endif()
if( VLD_FOUND )
	message( STATUS "+ Found Visual Leak Detector" )
	option( PROJECTS_USE_VLD "Use Visual Leaks Detector" TRUE )
	if ( PROJECTS_USE_VLD )
		include_directories( ${VLD_INCLUDE_DIR} )
		link_directories( ${VLD_LIBRARY_DIR} )
		set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVLD_AVAILABLE")
		set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DVLD_AVAILABLE")
		msg_debug( "Using Visual Leak Detector to check for Memory leaks" )
	endif()
else ()
	set( PROJECTS_USE_VLD FALSE )
endif ()

set( PROJECTS_VERSION "" )
set( PROJECTS_SOVERSION "" )
if( (NOT "${VERSION_MAJOR}" STREQUAL "") AND (NOT "${VERSION_MINOR}" STREQUAL "") AND (NOT "${VERSION_BUILD}" STREQUAL "") )
	set( PROJECTS_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}" )
	set( PROJECTS_SOVERSION "${VERSION_BUILD}" )
endif()
#--------------------------------------------------------------------------------------------------
#	Defining output paths for each project configuration
#--------------------------------------------------------------------------------------------------
if ( MSVC )
	option( PROJECTS_PROFILING "Activate code profiling or not" FALSE )
endif()

set( PROJECTS_PLATFORM "x86" )
if ( MSVC )
	if( (CMAKE_CL_64 OR CMAKE_GENERATOR MATCHES Win64) )
		set( PROJECTS_PLATFORM_FLAGS "/MACHINE:X64" )
		set( PROJECTS_PLATFORM "x64" )
	else()
		set( PROJECTS_PLATFORM_FLAGS "/MACHINE:X86" )
	endif()
else()
	if( (${CMAKE_SIZEOF_VOID_P} EQUAL 8) AND NOT MINGW )
		set( PROJECTS_PLATFORM_FLAGS "-m64" )
		if ( WIN32 )
			set( PROJECTS_PLATFORM "x64" )
		else ()
			set( PROJECTS_PLATFORM "amd64" )
		endif ()
	else()
		set( PROJECTS_PLATFORM_FLAGS "-m32" )
	endif()
endif()

get_filename_component( CMAKE_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} PATH )

set( PROJECTS_TEMPLATES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Templates )

if("${PROJECTS_BINARIES_OUTPUT_DIR}" STREQUAL "")
    set( PROJECTS_BINARIES_OUTPUT_DIR "${CMAKE_PARENT_DIR}/binaries/${PROJECTS_PLATFORM}" CACHE PATH "The path to the built binaries" )
endif()
if("${PROJECTS_SETUP_OUTPUT_DIR}" STREQUAL "")
    set( PROJECTS_SETUP_OUTPUT_DIR "${CMAKE_PARENT_DIR}/setup/${PROJECTS_PLATFORM}" CACHE PATH "The path to the built setup packages" )
endif()
if("${PROJECTS_DOCUMENTATION_OUTPUT_DIR}" STREQUAL "")
    set( PROJECTS_DOCUMENTATION_OUTPUT_DIR "${CMAKE_PARENT_DIR}/doc/${PROJECTS_PLATFORM}" CACHE PATH "The path to the built documentation" )
endif()

set( PROJECTS_BINARIES_OUTPUT_DIR_RELEASE ${PROJECTS_BINARIES_OUTPUT_DIR}/Release )
set( PROJECTS_BINARIES_OUTPUT_DIR_DEBUG ${PROJECTS_BINARIES_OUTPUT_DIR}/Debug )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/lib/" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/lib/" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/bin/" )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/lib/" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/lib/" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/bin/" )

msg_debug(	"PROJECTS_BINARIES_OUTPUT_DIR               ${PROJECTS_BINARIES_OUTPUT_DIR}" )
msg_debug(	"CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG       ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG}" )
msg_debug(	"CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG       ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG}" )
msg_debug(	"CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}" )
msg_debug(	"CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE     ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE}" )
msg_debug(	"CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE     ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE}" )
msg_debug(	"CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}" )

#--------------------------------------------------------------------------------------------------
#	Function :	add_target
#	Main function, used to create a target of given type, with it's dependencies and libraries
#--------------------------------------------------------------------------------------------------
function( add_target TARGET_NAME TARGET_TYPE TARGET_DEPENDENCIES TARGET_LINKS )# ARGV4=PCH_HEADER ARGV5=PCH_CPP ARGV6=OPT_C_FLAGS ARGV7=OPT_CXX_FLAGS ARGV8=OPT_LINK_FLAGS ARGV9=OPT_FILES
	set( PCH_HEADER "${ARGV4}" )
	set( PCH_CPP "${ARGV5}" )
	set( OPT_C_FLAGS "${ARGV6}" )
	set( OPT_CXX_FLAGS "${ARGV7}" )
	set( OPT_LINK_FLAGS "${ARGV8}" )
	set( OPT_FILES "${ARGV9}" )
	if((NOT "${CMAKE_BUILD_TYPE}" STREQUAL "") OR MSVC)
		msg_debug( "----------------------------------------------------------------------------------------------------" )
		msg_debug( "Type : 	${TARGET_TYPE} 	- Target : 	${TARGET_NAME}" )
		msg_debug( "PCH_HEADER            [${PCH_HEADER}]" )
		msg_debug( "PCH_CPP               [${PCH_CPP}]" )
		msg_debug( "OPT_C_FLAGS           [${OPT_C_FLAGS}]" )
		msg_debug( "OPT_CXX_FLAGS         [${OPT_CXX_FLAGS}]" )
		#First we retrieve the kind of target we will build
		string( COMPARE EQUAL ${TARGET_TYPE} 	"api_dll" 	IS_API_DLL )	#A dll will be installed in <install_dir>/bin, an API dll will also have its includes installed in <install_dir>/include/<TARGET_NAME>
		string( COMPARE EQUAL ${TARGET_TYPE} 	"dll" 		IS_DLL )		#A dll will be installed in <install_dir>/bin
		string( COMPARE EQUAL ${TARGET_TYPE} 	"lib" 		IS_LIB )		#A lib will be installed in <install_dir>/lib
		string( COMPARE EQUAL ${TARGET_TYPE} 	"bin" 		IS_BIN )		#A binary will be installed in <install_dir>/bin
		string( COMPARE EQUAL ${TARGET_TYPE}	"bin_dos"	IS_BIN_DOS )	#A dos binary will be installed in <install_dir>/bin and will have a console
		string( COMPARE EQUAL ${TARGET_TYPE}	"plugin"	IS_PLUGIN )		#A plugin will be installed in <install_dir>/lib/<project_name>
		msg_debug( "IS_API_DLL            [${IS_API_DLL}]" )
		msg_debug( "IS_DLL                [${IS_DLL}]" )
		msg_debug( "IS_LIB                [${IS_LIB}]" )
		msg_debug( "IS_BIN                [${IS_BIN}]" )
		msg_debug( "IS_BIN_DOS            [${IS_BIN_DOS}]" )
		msg_debug( "IS_PLUGIN             [${IS_PLUGIN}]" )
		#We now compute the extended name of the target (libs only)
		if ( IS_LIB )
			compute_abi_name( TARGET_ABI_NAME TARGET_ABI_NAME_DEBUG )
			msg_debug( "TARGET_ABI_NAME       ${TARGET_ABI_NAME}" )
			msg_debug( "TARGET_ABI_NAME_DEBUG ${TARGET_ABI_NAME_DEBUG}" )
		endif ()
		#We then retrieve target files (located in include/${TARGET_NAME}, source/${TARGET_NAME} and resource/${TARGET_NAME}
		file(
			GLOB_RECURSE
				TARGET_SOURCE_CPP
				Src/*.cpp
		)
		file(
			GLOB_RECURSE
				TARGET_SOURCE_C
				Src/*.c
		)
		file(
			GLOB_RECURSE
				TARGET_SOURCE_H_ONLY
				Src/*.h
				Src/*.hpp
				Src/*.inl
		)
        if ( WIN32 )
            #We include resource files in Visual Studio or MINGW with Windows
            enable_language( RC )
            configure_file(
                Src/Win32/${TARGET_NAME}.rc.in
                Src/Win32/${TARGET_NAME}.rc
                NEWLINE_STYLE LF
            )
            set( TARGET_RSC
                ${CMAKE_CURRENT_BINARY_DIR}/Src/Win32/${TARGET_NAME}.rc
            )
            if ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Src/Win32/resource.h" )
                set( TARGET_RSC
                    ${TARGET_RSC}
                    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Win32/resource.h
                )
            elseif ( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/Src/Win32/resource.h" )
                set( TARGET_RSC
                    ${TARGET_RSC}
                    ${CMAKE_CURRENT_BINARY_DIR}/Src/Win32/resource.h
                )
            endif ()
            include_directories( Src/Win32 )
            set( TARGET_SOURCE_H
                ${TARGET_SOURCE_H_ONLY}
                ${TARGET_RSC}
            )
            source_group( "Resource Files" FILES ${TARGET_RSC} )
        else ()
            set( TARGET_SOURCE_H
                ${TARGET_SOURCE_H_ONLY}
            )
        endif ()
        set( TARGET_C_FLAGS "" )
        set( TARGET_CXX_FLAGS "" )
        set( TARGET_LINK_FLAGS "" )
        compute_compilation_flags( ${TARGET_NAME} ${TARGET_TYPE} "${OPT_C_FLAGS}" "${OPT_CXX_FLAGS}" "${OPT_LINK_FLAGS}" TARGET_C_FLAGS TARGET_CXX_FLAGS TARGET_LINK_FLAGS )
		# set( CPACK_SOURCE_OUTPUT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/CPackSourceConfig.cmake" )
		# set( CPACK_OUTPUT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/CPackConfig.cmake" )
		#We now effectively create the target
		if ( IS_API_DLL )
			set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/lib/" )
			set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/lib/" )
			set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/bin/" )
			set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/lib/" )
			set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/lib/" )
			set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/bin/" )
			add_library( ${TARGET_NAME} SHARED ${TARGET_SOURCE_CPP} ${TARGET_SOURCE_C} ${TARGET_SOURCE_H} ${OPT_FILES} )
			set_target_properties( ${TARGET_NAME} PROPERTIES VERSION ${PROJECTS_VERSION} SOVERSION ${PROJECTS_SOVERSION} )
            set_target_properties( ${TARGET_NAME} PROPERTIES LINK_FLAGS "${TARGET_LINK_FLAGS}" )
			#We now build the install script
			if ( WIN32 )
				#We install each .dll in <install_dir>/bin folder
				install(
					TARGETS	${TARGET_NAME}
					RUNTIME DESTINATION bin
					COMPONENT ${TARGET_NAME}
				)
				install(
					TARGETS	${TARGET_NAME}
					ARCHIVE DESTINATION lib
					LIBRARY DESTINATION lib
					COMPONENT ${TARGET_NAME}_dev
				)
			else ()
				#We install each .so in <install_dir>/lib folder
				install(
					TARGETS	${TARGET_NAME}
					LIBRARY DESTINATION lib
					COMPONENT ${TARGET_NAME}
				)
			endif()
			#For API DLLs, we install headers to <install_dir>/include/${TARGET_NAME}
			install(
				FILES ${TARGET_SOURCE_H_ONLY}
				DESTINATION include/${TARGET_NAME}
				COMPONENT ${TARGET_NAME}_dev
			)
		elseif ( IS_DLL )
			set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/lib/" )
			set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/lib/" )
			set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/bin/" )
			set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/lib/" )
			set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/lib/" )
			set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/bin/" )
			add_library( ${TARGET_NAME} SHARED ${TARGET_SOURCE_CPP} ${TARGET_SOURCE_C} ${TARGET_SOURCE_H} ${OPT_FILES} )
			set_target_properties( ${TARGET_NAME} PROPERTIES VERSION ${PROJECTS_VERSION} SOVERSION ${PROJECTS_SOVERSION} )
            set_target_properties( ${TARGET_NAME} PROPERTIES LINK_FLAGS "${TARGET_LINK_FLAGS}" )
			#We now build the install script
			if ( WIN32 )
				#We copy each .dll in <install_dir>/bin folder
				install(
					TARGETS	${TARGET_NAME}
					RUNTIME DESTINATION bin
					COMPONENT ${TARGET_NAME}
				)
			else ()
				#We copy each .so in <install_dir>/lib folder
				install(
					TARGETS	${TARGET_NAME}
					LIBRARY DESTINATION lib
					COMPONENT ${TARGET_NAME}
				)
			endif()
		elseif ( IS_PLUGIN )
			set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/lib/${MAIN_PROJECT_NAME}/" )
			set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/lib/${MAIN_PROJECT_NAME}/" )
			set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/lib/${MAIN_PROJECT_NAME}/" )
			set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/lib/${MAIN_PROJECT_NAME}/" )
			set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/lib/${MAIN_PROJECT_NAME}/" )
			set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/lib/${MAIN_PROJECT_NAME}/" )
			add_library( ${TARGET_NAME} MODULE ${TARGET_SOURCE_CPP} ${TARGET_SOURCE_C} ${TARGET_SOURCE_H} ${OPT_FILES} )
			set_target_properties( ${TARGET_NAME} PROPERTIES VERSION ${PROJECTS_VERSION} SOVERSION ${PROJECTS_SOVERSION} )
            set_target_properties( ${TARGET_NAME} PROPERTIES LINK_FLAGS "${TARGET_LINK_FLAGS}" )
			if ( WIN32 )
				#We install each plugin .dll in <install_dir>/lib/<project_name> folder
				install(
					FILES ${PROJECTS_BINARIES_OUTPUT_DIR}/$<CONFIG>/lib/${MAIN_PROJECT_NAME}/${TARGET_NAME}.dll
					DESTINATION lib/${MAIN_PROJECT_NAME}
					COMPONENT ${TARGET_NAME}
				)
			else ()
				#We install each plugin .dll in <install_dir>/lib/<project_name> folder
				install(
					TARGETS	${TARGET_NAME}
					LIBRARY DESTINATION lib/${MAIN_PROJECT_NAME}
					COMPONENT ${TARGET_NAME}
				)
			endif ()
		elseif ( IS_BIN )
			set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/lib/" )
			set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/lib/" )
			set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/bin/" )
			set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/lib/" )
			set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/lib/" )
			set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/bin/" )
			if (WIN32)
				add_executable( ${TARGET_NAME} WIN32 ${TARGET_SOURCE_CPP} ${TARGET_SOURCE_C} ${TARGET_SOURCE_H} ${OPT_FILES} )
			else()
				add_executable( ${TARGET_NAME} ${TARGET_SOURCE_CPP} ${TARGET_SOURCE_C} ${TARGET_SOURCE_H} ${OPT_FILES} )
			endif()
            set_target_properties( ${TARGET_NAME} PROPERTIES LINK_FLAGS "${TARGET_LINK_FLAGS}" )
			#We now build the install script
			#We copy each exe in <install_dir>/bin folder
			install(
				TARGETS	${TARGET_NAME}
				RUNTIME DESTINATION bin
				COMPONENT ${TARGET_NAME}
			)
		elseif ( IS_BIN_DOS )
			set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/lib/" )
			set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/lib/" )
			set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/bin/" )
			set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/lib/" )
			set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/lib/" )
			set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/bin/" )
			add_executable( ${TARGET_NAME} ${TARGET_SOURCE_CPP} ${TARGET_SOURCE_C} ${TARGET_SOURCE_H} ${OPT_FILES} )
            set_target_properties( ${TARGET_NAME} PROPERTIES LINK_FLAGS "${TARGET_LINK_FLAGS}" )
			#We copy each lib in <install_dir>/lib folder
			install(
				TARGETS	${TARGET_NAME}
				RUNTIME DESTINATION bin
				COMPONENT ${TARGET_NAME}
        	)
		elseif ( IS_LIB )
			set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/lib/" )
			set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/lib/" )
			set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/bin/" )
			set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/lib/" )
			set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/lib/" )
			set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/bin/" )
			add_library( ${TARGET_NAME} STATIC ${TARGET_SOURCE_CPP} ${TARGET_SOURCE_C} ${TARGET_SOURCE_H} ${OPT_FILES} )
			add_target_compilation_flags( ${TARGET_NAME} "-D${TARGET_NAME}_STATIC" )
			if ( MSVC )
	            set_target_properties( ${TARGET_NAME} PROPERTIES STATIC_LIBRARY_FLAGS "${TARGET_LINK_FLAGS}" )
	        endif()
			#We now build the install script
			#We copy each lib in <install_dir>/lib folder
			install(
				TARGETS	${TARGET_NAME}
				ARCHIVE DESTINATION lib
				COMPONENT ${TARGET_NAME}_dev
        	)
			#For libs, we install headers to <install_dir>/include/${TARGET_NAME}
			install(
				FILES ${TARGET_SOURCE_H_ONLY}
				DESTINATION include/${TARGET_NAME}
				COMPONENT ${TARGET_NAME}_dev
			)
        else()
            message( FATAL_ERROR " Unknown target type : [${TARGET_TYPE}]" )
		endif()
		#We add computed ABI name to target outputs
		set_target_properties( ${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_NAME "${TARGET_NAME}${TARGET_ABI_NAME}")
		set_target_properties( ${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_NAME "${TARGET_NAME}${TARGET_ABI_NAME}")
		set_target_properties( ${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_NAME "${TARGET_NAME}${TARGET_ABI_NAME}")
		#Idem for debug
		set_target_properties( ${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_NAME_DEBUG "${TARGET_NAME}${TARGET_ABI_NAME}${TARGET_ABI_NAME_DEBUG}")
		set_target_properties( ${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_NAME_DEBUG "${TARGET_NAME}${TARGET_ABI_NAME}${TARGET_ABI_NAME_DEBUG}")
		set_target_properties( ${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_NAME_DEBUG "${TARGET_NAME}${TARGET_ABI_NAME}${TARGET_ABI_NAME_DEBUG}")
		#We scan dependencies to add it to the target
		foreach( TARGET_DEPENDENCY ${TARGET_DEPENDENCIES} )
			msg_debug( "TARGET_DEPENDENCY     ${TARGET_DEPENDENCY}")
			add_dependencies( ${TARGET_NAME} ${TARGET_DEPENDENCY} )
		endforeach()
		#We scan libraries to add it to the linker
		foreach( TARGET_LINK ${TARGET_LINKS} )
			string( REPLACE "|" ";" TARGET_LINK ${TARGET_LINK})
			msg_debug( "TARGET_LINK           ${TARGET_LINK}" )
			target_link_libraries( ${TARGET_NAME} ${TARGET_LINK} )
		endforeach()
		
        set_source_files_properties( ${TARGET_SOURCE_C} PROPERTIES COMPILE_FLAGS "${TARGET_C_FLAGS}")
        set_source_files_properties( ${TARGET_SOURCE_CPP} PROPERTIES COMPILE_FLAGS "${TARGET_CXX_FLAGS}")
        
		if ( PCH_HEADER STREQUAL "" OR NOT ${PROJECTS_USE_PRECOMPILED_HEADERS} )
			msg_debug( "PRECOMPILED HEADERS   No" )
		else ()
			msg_debug( "PRECOMPILED HEADERS   Yes" )
			add_target_precompiled_header( ${TARGET_NAME} ${PCH_HEADER} ${PCH_CPP} ${TARGET_CXX_FLAGS} ${TARGET_SOURCE_CPP} )
		endif ()
		if ( MSVC )
			if ( ${PROJECTS_PROFILING} )
				set_target_properties( ${TARGET_NAME} PROPERTIES LINK_FLAGS_DEBUG "${TARGET_LINK_FLAGS} /OPT:NOREF /PROFILE")
			endif ()
		endif ()
		msg_debug( "TARGET_CXX_FLAGS:     ${TARGET_CXX_FLAGS}")
		msg_debug( "TARGET_PCH_FLAGS:     ${TARGET_PCH_FLAGS}")
		msg_debug( "TARGET_C_FLAGS:       ${TARGET_C_FLAGS}")
		msg_debug( "TARGET_LINK_FLAGS:    ${TARGET_LINK_FLAGS}")
	endif()
endfunction( add_target )
