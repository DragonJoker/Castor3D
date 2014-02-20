#--------------------------------------------------------------------------------------------------
#	Macro :	msg_debug
#	Used to print debug messages
#--------------------------------------------------------------------------------------------------
macro(msg_debug	msg)
#	message( STATUS "${msg}")
endmacro(msg_debug)
#--------------------------------------------------------------------------------------------------
#	Macro :	copy_languages
#	Used to copy language files for TargetName from SrcFolder to DstFolder
#--------------------------------------------------------------------------------------------------
macro( copy_languages TargetName SrcFolder DstFolder )
	# Copying internationalisation files (french, for now) for ${TargetName}
	if( MSVC )
		file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/Debug/share/${DstFolder}")
		file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/Debug/share/${DstFolder}/fr")
		file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/Release/share/${DstFolder}")
		file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/Release/share/${DstFolder}/fr")
		file( COPY ${SrcFolder}/po/fr/${TargetName}.mo DESTINATION ${PLUGINS_OUTPUT_PATH}/Debug/${DstFolder}/fr)
		file( COPY ${SrcFolder}/po/fr/${TargetName}.mo DESTINATION ${PLUGINS_OUTPUT_PATH}/Release/${DstFolder}/fr)
		file( COPY ${SrcFolder}/po/fr/${TargetName}.mo DESTINATION ${PROJECTS_BINARIES_OUTPUT_PATH}/Debug/share/${DstFolder}/fr)
		file( COPY ${SrcFolder}/po/fr/${TargetName}.mo DESTINATION ${PROJECTS_BINARIES_OUTPUT_PATH}/Release/share/${DstFolder}/fr)
	else()
		file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/share/${DstFolder}")
		file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/share/${DstFolder}/fr")
		file( COPY ${SrcFolder}/po/fr/${TargetName}.mo DESTINATION ${PROJECTS_BINARIES_OUTPUT_PATH}/share/${DstFolder}/fr)
		install(
			FILES ${SrcFolder}/po/fr/${TargetName}.mo
			DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${DstFolder}/fr/
		)
	endif()
endmacro()

set( PROJECTS_VERSION "" )
set( PROJECTS_SOVERSION "" )
if( (NOT "${VERSION_MAJOR}" STREQUAL "") AND (NOT "${VERSION_MINOR}" STREQUAL "") AND (NOT "${VERSION_BUILD}" STREQUAL "") )
	set( PROJECTS_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}" )
	set( PROJECTS_SOVERSION "${VERSION_BUILD}" )
endif()
#--------------------------------------------------------------------------------------------------
#	Defining output paths for each project configuration
#--------------------------------------------------------------------------------------------------
if("${PROJECTS_BINARIES_OUTPUT_PATH}" STREQUAL "")
    set( PROJECTS_BINARIES_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/binaries" CACHE PATH "The path to the built binaries" )
endif()
if("${PROJECTS_AMD64}" STREQUAL "")
	set( PROJECTS_AMD64 	0 CACHE BOOL "Compile in 64 bits (for AMD64 or X64)")
endif()
if(MSVC)
	if( (CMAKE_CL_64 OR CMAKE_GENERATOR MATCHES Win64) AND ${PROJECTS_AMD64} )
		set(PROJECTS_PLATFORM_FLAGS "/MACHINE:X64")
	else()
		set(PROJECTS_PLATFORM_FLAGS "/MACHINE:X86")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Wp64")
	endif()
elseif(CMAKE_COMPILER_IS_GNUCXX)
	if( (${CMAKE_SIZEOF_VOID_P} EQUAL 8) AND ${PROJECTS_AMD64} )
		set(PROJECTS_PLATFORM_FLAGS "-m64")
	else()
		set(PROJECTS_PLATFORM_FLAGS "-m32")
	endif()
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	if( (${CMAKE_SIZEOF_VOID_P} EQUAL 8) AND ${PROJECTS_AMD64} )
		set(PROJECTS_PLATFORM_FLAGS "-m64")
	else()
		set(PROJECTS_PLATFORM_FLAGS "-m32")
	endif()
endif()
if(MSVC)
	if("${PROJECTS_PROFILING}" STREQUAL "")
		set( PROJECTS_PROFILING 	0 CACHE BOOL "Activate code profiling or not")
	endif()
endif()
if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "")
	set( PROJECTS_BINARIES_OUTPUT_PATH	"${PROJECTS_BINARIES_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}")
	set( EXECUTABLE_OUTPUT_PATH 		"${PROJECTS_BINARIES_OUTPUT_PATH}/bin")
	set( LIBRARY_OUTPUT_PATH 			"${PROJECTS_BINARIES_OUTPUT_PATH}/lib")
	set( RUNTIME_OUTPUT_DIRECTORY		"${PROJECTS_BINARIES_OUTPUT_PATH}/bin")
	set( PLUGINS_OUTPUT_PATH 			"${PROJECTS_BINARIES_OUTPUT_PATH}/share/${ProjectName}")
	#Creating folders
	file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/bin")
	file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/lib")
	file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/share")
	file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/share/${ProjectName}")
elseif(MSVC)
	# With VisualStudio, we build in a temp dir then we copy in binaries dir
    if("${PROJECTS_BINTEMP_OUTPUT_PATH}" STREQUAL "")
        set( PROJECTS_BINTEMP_OUTPUT_PATH 	"${CMAKE_CURRENT_SOURCE_DIR}/bin_temp" CACHE PATH "Path to temporary binaries")
    endif()
	set( EXECUTABLE_OUTPUT_PATH 	"${PROJECTS_BINTEMP_OUTPUT_PATH}/bin")
	set( LIBRARY_OUTPUT_PATH 		"${PROJECTS_BINTEMP_OUTPUT_PATH}/lib")
	set( RUNTIME_OUTPUT_DIRECTORY	"${PROJECTS_BINTEMP_OUTPUT_PATH}/bin")
	set( PLUGINS_OUTPUT_PATH 		"${PROJECTS_BINTEMP_OUTPUT_PATH}/share")
	#Creating folders
	file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/Debug/bin")
	file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/Debug/lib")
	file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/Debug/share")
	file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/Debug/share/${ProjectName}")
	file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/Release/bin")
	file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/Release/lib")
	file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/Release/share")
	file( MAKE_DIRECTORY "${PROJECTS_BINARIES_OUTPUT_PATH}/Release/share/${ProjectName}")
else ()
	MESSAGE( ERROR " Choose CMAKE_BUILD_TYPE please" )
endif()
msg_debug(	"BINARIES_OUTPUT_PATH :     ${PROJECTS_BINARIES_OUTPUT_PATH}"	)
msg_debug(	"EXECUTABLE_OUTPUT_PATH :   ${EXECUTABLE_OUTPUT_PATH}"			)
msg_debug(	"LIBRARY_OUTPUT_PATH :      ${LIBRARY_OUTPUT_PATH}"				)
msg_debug(	"RUNTIME_OUTPUT_DIRECTORY : ${RUNTIME_OUTPUT_DIRECTORY}"		)
msg_debug(	"PLUGINS_OUTPUT_PATH :      ${PLUGINS_OUTPUT_PATH}"				)

#--------------------------------------------------------------------------------------------------
#	Function :	COMPILER_DUMPVERSION
# 	Function which gives the GNU Compiler version, used to build name of project's libs
#--------------------------------------------------------------------------------------------------
function( COMPILER_DUMPVERSION OUTPUT_VERSION)
	exec_program(${CMAKE_CXX_COMPILER} ARGS ${CMAKE_CXX_COMPILER_ARG1} -dumpversion OUTPUT_VARIABLE COMPILER_VERSION)
	string(REGEX REPLACE "([0-9])\\.([0-9])(\\.[0-9])?" "\\1\\2" COMPILER_VERSION ${COMPILER_VERSION})
	set(${OUTPUT_VERSION} ${COMPILER_VERSION} PARENT_SCOPE)
endfunction()

#--------------------------------------------------------------------------------------------------
#	Function :	add_target
#	Main function, used to create a target of given type, with it's dependencies and libraries
#--------------------------------------------------------------------------------------------------
function( add_target TargetName StrType Dependencies Links PchHeader PchCpp OptCFlags OptCxxFlags SrcPrefix)
	if((NOT "${CMAKE_BUILD_TYPE}" STREQUAL "") OR MSVC)
		msg_debug("----------------------------------------------------------------------------------------------------")
		message(STATUS "Type : 	${StrType} 	- Target : 	${TargetName}")
		#First we retrieve the kind of target we will build
		string( COMPARE EQUAL ${StrType} 	"dll" 				IsDll)		#A dll will be installed in <install_dir>/bin
		string( COMPARE EQUAL ${StrType} 	"lib" 				IsLib)		#A lib will be installed in <install_dir>/lib
		string( COMPARE EQUAL ${StrType} 	"bin" 				IsBin)		#A binary will be installed in <install_dir>/bin
		string( COMPARE EQUAL ${StrType}	"bin_dos"			IsBinDos)	#A dos binary will be installed in <install_dir>/bin and will have a console
		string( COMPARE EQUAL ${StrType} 	"plugin"			IsPlugin)	#A plugin will be installed in <install_dir>/share/${ProjectName}
		string( COMPARE EQUAL ${TargetName} "GlRenderSystem"	IsGlDll)
		string( COMPARE EQUAL ${TargetName} "Dx9RenderSystem"	IsDx9Dll)
		string( COMPARE EQUAL ${TargetName}	"CastorUtils" 		IsUtils)
		string( COMPARE EQUAL ${TargetName}	"GuiCommon" 		IsGuiCommon)
		SET( ${TargetName}_SrcPrefix "" )
		if( NOT "${SrcPrefix}" STREQUAL "" )
			SET( ${TargetName}_SrcPrefix "${SrcPrefix}/" )
			include_directories( ${${TargetName}_SrcPrefix}include )
		endif()
		#Optional prefix (for lib, dll and plugin)
		set( ${TargetName}_Prefix 	"")
		#Optional suffix
		set( ${TargetName}_Suffix 	"")
		#We now compute the ABI name of the target (dlls and plugins only)
		if (IsDll)
			set( OptCFlags "${OptCFlags} -D${TargetName}_EXPORTS" )
			set( OptCxxFlags "${OptCxxFlags} -D${TargetName}_EXPORTS" )
			set( OptCFlags "${OptCFlags} -D${TargetName}_SHARED" )
			set( OptCxxFlags "${OptCxxFlags} -D${TargetName}_SHARED" )
			if (WIN32)
				set( ${TargetName}_Suffix 	".dll")
			else()
				set( ${TargetName}_Suffix 	".so")
			endif()
		elseif (IsPlugin)
			set( OptCFlags "${OptCFlags} -D${TargetName}_EXPORTS" )
			set( OptCxxFlags "${OptCxxFlags} -D${TargetName}_EXPORTS" )
			if (WIN32)
				set( ${TargetName}_Suffix 	".dll")
			else()
				set( ${TargetName}_Suffix 	".so")
			endif()
			if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" OR "${CMAKE_CXX_COMPILER}" MATCHES "icl" OR "${CMAKE_CXX_COMPILER}" MATCHES "icpc")
				set( ${TargetName}_Prefix "")
			elseif (MSVC)
				set( ${TargetName}_Prefix "")
			elseif (BORLAND)
				set( ${TargetName}_Prefix "")
			elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "SunPro")
				set( ${TargetName}_Prefix "")
			elseif (MINGW)
				set( ${TargetName}_Prefix "lib")
			elseif (UNIX)
				set( ${TargetName}_Prefix "lib")
			else ()
				set( ${TargetName}_Prefix "lib")
			endif()
		elseif (IsLib)
			if (WIN32)
				set( ${TargetName}_Suffix 	".lib")
			else()
				set( ${TargetName}_Suffix 	".a")
			endif()
			if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" OR "${CMAKE_CXX_COMPILER}" MATCHES "icl" OR "${CMAKE_CXX_COMPILER}" MATCHES "icpc")
				#Intel compiler
				if(WIN32)
					set( ${TargetName}_ABI_Name "-iw")
				else()
					set( ${TargetName}_ABI_Name "-il")
				endif()
				set( ${TargetName}_ABI_Name_Debug "-d")
			elseif (MSVC12)
				#Visual Studio 2013
				set( ${TargetName}_ABI_Name "-vc120")
				set( ${TargetName}_ABI_Name_Debug "-d")
			elseif (MSVC11)
				#Visual Studio 2012
				set( ${TargetName}_ABI_Name "-vc110")
				set( ${TargetName}_ABI_Name_Debug "-d")
			elseif (MSVC10)
				#Visual Studio 2010
				set( ${TargetName}_ABI_Name "-vc100")
				set( ${TargetName}_ABI_Name_Debug "-d")
			elseif (MSVC90)
				#Visual Studio 2008
				set( ${TargetName}_ABI_Name "-vc90")
				set( ${TargetName}_ABI_Name_Debug "-d")
			elseif (MSVC80)
				#Visual Studio 2005
				set( ${TargetName}_ABI_Name "-vc80")
				set( ${TargetName}_ABI_Name_Debug "-d")
			elseif (MSVC71)
				#Visual Studio 2003
				set( ${TargetName}_ABI_Name "-vc71")
				set( ${TargetName}_ABI_Name_Debug "-d")
			elseif (MSVC70)
				#Visual Studio 7
				set( ${TargetName}_ABI_Name "-vc7")
				set( ${TargetName}_ABI_Name_Debug "-d")
			elseif (MSVC60)
				#Visual Studio 6
				set( ${TargetName}_ABI_Name "-vc6")
				set( ${TargetName}_ABI_Name_Debug "-d")
			elseif (BORLAND)
				#Borland compiler
				set( ${TargetName}_ABI_Name "-bcb")
				set( ${TargetName}_ABI_Name_Debug "-d")
			elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "SunPro")
				#Sun compiler
				set( ${TargetName}_ABI_Name "-sw")
				set( ${TargetName}_ABI_Name_Debug "-d")
			elseif (MINGW)
				#MinGW compiler
				COMPILER_DUMPVERSION(COMPILER_VERSION)
				set( ${TargetName}_ABI_Name "-mgw${COMPILER_VERSION}")
				set( ${TargetName}_ABI_Name_Debug "-d")
			elseif (UNIX)
				#Unix system
				if (CMAKE_COMPILER_IS_GNUCXX)
					#GNU C/C++ compiler (can be gcc, g++, clang, ...)
					COMPILER_DUMPVERSION(COMPILER_VERSION)
					if(APPLE)
						#Apple system
						set( ${TargetName}_ABI_Name "-xgcc${COMPILER_VERSION}")
						set( ${TargetName}_ABI_Name_Debug "-d")
					else()
						#GNU/Linux system
						set( ${TargetName}_ABI_Name "-gcc${COMPILER_VERSION}")
						set( ${TargetName}_ABI_Name_Debug "-d")
					endif()
				endif (CMAKE_COMPILER_IS_GNUCXX)
			endif()
		else ()
			if (WIN32)
				set( ${TargetName}_Suffix 	".exe")
			else()
				set( ${TargetName}_Suffix 	"")
			endif()
		endif()
		#We then retrieve target files (located in include/${TargetName}, source/${TargetName} and resource/${TargetName}
		file( GLOB_RECURSE ${TargetName}_cpp 	${${TargetName}_SrcPrefix}source/${TargetName}/*.cpp)
		file( GLOB_RECURSE ${TargetName}_c		${${TargetName}_SrcPrefix}source/${TargetName}/*.c)
		file( GLOB_RECURSE ${TargetName}_h 		${${TargetName}_SrcPrefix}include/${TargetName}/*.h	${${TargetName}_SrcPrefix}include/${TargetName}/*.hpp	${${TargetName}_SrcPrefix}include/${TargetName}/*.inl)
		if (WIN32)
			#We include resource files in Visual Studio or MINGW with Windows
			enable_language( RC )
			configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/${${TargetName}_SrcPrefix}resource/${TargetName}/${TargetName}.rc.in	${CMAKE_CURRENT_SOURCE_DIR}/${${TargetName}_SrcPrefix}resource/${TargetName}/${TargetName}.rc)
			set( ${TargetName}_h ${${TargetName}_h} ${${TargetName}_SrcPrefix}resource/${TargetName}/${TargetName}.rc ${${TargetName}_SrcPrefix}resource/${TargetName}/resource.h)
			if (IsBin)
				#We include wxWidgets resource files for binaries
				set( ${TargetName}_h ${${TargetName}_h} ${${TargetName}_SrcPrefix}resource/wx/wx.rc)
			endif()
		endif ()
		#We complete C/C++ compilation flags with configuration dependant ones and optional ones
		set( ${TargetName}_C_FLAGS 			"${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE}} ${OptCFlags}")
		set( ${TargetName}_CXX_FLAGS		"${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}} ${OptCxxFlags}")
		set( ${TargetName}_LINK_FLAGS		"${PROJECTS_PLATFORM_FLAGS}")
		#On GNU compiler, we use c++0x and c++1x support, and also PIC
		if (CMAKE_COMPILER_IS_GNUCXX)
			COMPILER_DUMPVERSION(COMPILER_VERSION)
			if (MINGW)
				if (${COMPILER_VERSION} LESS "46")
					set( ${TargetName}_CXX_FLAGS "${${TargetName}_CXX_FLAGS} -fPIC")
					set( ${TargetName}_C_FLAGS "${${TargetName}_C_FLAGS} -fPIC")
				elseif (${COMPILER_VERSION} LESS "47")
					set( ${TargetName}_CXX_FLAGS "${${TargetName}_CXX_FLAGS} -fPIC -std=gnu++0x")
					set( ${TargetName}_C_FLAGS "${${TargetName}_C_FLAGS} -fPIC")
				else ()
					set( ${TargetName}_CXX_FLAGS "${${TargetName}_CXX_FLAGS} -fPIC -std=gnu++11")
					set( ${TargetName}_C_FLAGS "${${TargetName}_C_FLAGS} -fPIC")
				endif()
			else()
				if (${COMPILER_VERSION} LESS "46")
					set( ${TargetName}_CXX_FLAGS "${${TargetName}_CXX_FLAGS} -fPIC")
					set( ${TargetName}_C_FLAGS "${${TargetName}_C_FLAGS} -fPIC")
				elseif (${COMPILER_VERSION} LESS "47")
					set( ${TargetName}_CXX_FLAGS "${${TargetName}_CXX_FLAGS} -fPIC -std=gnu++0x")
					set( ${TargetName}_C_FLAGS "${${TargetName}_C_FLAGS} -fPIC")
				else ()
					set( ${TargetName}_CXX_FLAGS "${${TargetName}_CXX_FLAGS} -fPIC -std=c++11")
					set( ${TargetName}_C_FLAGS "${${TargetName}_C_FLAGS} -fPIC")
				endif()
			endif()
		elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
			COMPILER_DUMPVERSION(COMPILER_VERSION)
			set( ${TargetName}_CXX_FLAGS "${${TargetName}_CXX_FLAGS} -fPIC -std=c++11")
			set( ${TargetName}_C_FLAGS "${${TargetName}_C_FLAGS} -fPIC")
		endif()
		#We now effectively create the target
		if (IsPlugin OR IsDll)
			add_library( ${TargetName} SHARED ${${TargetName}_cpp} ${${TargetName}_c} ${${TargetName}_h})
			if (IsPlugin)
				set_target_properties( ${TargetName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PLUGINS_OUTPUT_PATH})
				set_target_properties( ${TargetName} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PLUGINS_OUTPUT_PATH})
				set_target_properties( ${TargetName} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PLUGINS_OUTPUT_PATH})
			else ()
				set_target_properties( ${TargetName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
				set_target_properties( ${TargetName} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
				set_target_properties( ${TargetName} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
			endif ()
			set_target_properties( ${TargetName} PROPERTIES VERSION ${PROJECTS_VERSION} SOVERSION ${PROJECTS_SOVERSION})
			if (IsDx9Dll AND CMAKE_COMPILER_IS_GNUCXX)
				set( ${TargetName}_CXX_FLAGS	"${${TargetName}_CXX_FLAGS} -fcheck-new")
				set( ${TargetName}_C_FLAGS 	"${${TargetName}_C_FLAGS} -fcheck-new")
			endif ()
		elseif (IsBin)
			if (WIN32)
				add_executable( ${TargetName} WIN32 ${${TargetName}_cpp} ${${TargetName}_c} ${${TargetName}_h} )
			else()
				add_executable( ${TargetName} ${${TargetName}_cpp} ${${TargetName}_c} ${${TargetName}_h})
			endif()
			set_target_properties( ${TargetName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
			set_target_properties( ${TargetName} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
			set_target_properties( ${TargetName} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
		elseif (IsBinDos)
			add_executable( ${TargetName} ${${TargetName}_cpp} ${${TargetName}_c} ${${TargetName}_h})
			set_target_properties( ${TargetName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
			set_target_properties( ${TargetName} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
			set_target_properties( ${TargetName} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
		elseif (IsLib)
			add_library( ${TargetName} STATIC ${${TargetName}_cpp} ${${TargetName}_c} ${${TargetName}_h})
			set_target_properties( ${TargetName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
			set_target_properties( ${TargetName} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
			set_target_properties( ${TargetName} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
			#For libs, we install headers to <install_dir>/include/${TargetName}
			if( NOT MSVC )
				install(
					FILES ${${TargetName}_h}
					DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${TargetName}
				)
			endif ()
		endif()
		#We add computed ABI name to target outputs
		set_target_properties( ${TargetName} PROPERTIES LIBRARY_OUTPUT_NAME "${TargetName}${${TargetName}_ABI_Name}")
		set_target_properties( ${TargetName} PROPERTIES RUNTIME_OUTPUT_NAME "${TargetName}${${TargetName}_ABI_Name}")
		set_target_properties( ${TargetName} PROPERTIES ARCHIVE_OUTPUT_NAME "${TargetName}${${TargetName}_ABI_Name}")
		#Idem for debug
		set_target_properties( ${TargetName} PROPERTIES LIBRARY_OUTPUT_NAME_DEBUG "${TargetName}${${TargetName}_ABI_Name}${${TargetName}_ABI_Name_Debug}")
		set_target_properties( ${TargetName} PROPERTIES RUNTIME_OUTPUT_NAME_DEBUG "${TargetName}${${TargetName}_ABI_Name}${${TargetName}_ABI_Name_Debug}")
		set_target_properties( ${TargetName} PROPERTIES ARCHIVE_OUTPUT_NAME_DEBUG "${TargetName}${${TargetName}_ABI_Name}${${TargetName}_ABI_Name_Debug}")
		#We scan dependencies to add it to the target
		FOREACH(Dependency ${Dependencies})
			msg_debug( "${TargetName}_Dependency ${Dependency}")
			add_dependencies( ${TargetName} ${Dependency})
		ENDFOREACH()
		#We scan libraries to add it to the linker
		FOREACH(Link ${Links})
			string( REPLACE "¤" ";" Link2 ${Link})
			msg_debug( 	"Link			${Link2}")
			target_link_libraries( ${TargetName} ${Link2})
		ENDFOREACH()
		#We set the compilation and link flags for target's files
		set_target_properties( ${TargetName} PROPERTIES LINK_FLAGS "${${TargetName}_LINK_FLAGS}")
		
		if (MSVC)
			set( ${TargetName}_CXX_FLAGS "${${TargetName}_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS")
			set( ${TargetName}_C_FLAGS "${${TargetName}_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS")
			set( ${TargetName}_PCH_FLAGS "${${TargetName}_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS")
			set_source_files_properties( ${${TargetName}_c} PROPERTIES COMPILE_FLAGS "${${TargetName}_C_FLAGS}")
			if (PchHeader STREQUAL "")
				msg_debug(	"Precompiled headers:		NO")
				set_source_files_properties( ${${TargetName}_cpp} PROPERTIES COMPILE_FLAGS "${${TargetName}_CXX_FLAGS}")
			else ()
				msg_debug( 	"Link			${Link2}")
				msg_debug(	"Precompiled headers:		YES")
				set( ${TargetName}_PCH_FLAGS "${${TargetName}_PCH_FLAGS} /Yc${TargetName}/${PchHeader}")
				set( ${TargetName}_CXX_FLAGS "${${TargetName}_CXX_FLAGS} /Yu${TargetName}/${PchHeader}")
				set_source_files_properties( ${${TargetName}_cpp} PROPERTIES COMPILE_FLAGS "${${TargetName}_CXX_FLAGS}")
				set_source_files_properties( ${${TargetName}_SrcPrefix}source/${TargetName}/${PchCpp} PROPERTIES COMPILE_FLAGS "${${TargetName}_PCH_FLAGS}")
			endif ()
			if (${PROJECTS_PROFILING})
				set_target_properties( ${TargetName} PROPERTIES LINK_FLAGS_DEBUG "${${TargetName}_LINK_FLAGS} /OPT:NOREF /PROFILE")
			endif ()
		else()
			set_source_files_properties( ${${TargetName}_cpp} PROPERTIES COMPILE_FLAGS "${${TargetName}_CXX_FLAGS}")
			set_source_files_properties( ${${TargetName}_c} PROPERTIES COMPILE_FLAGS "${${TargetName}_C_FLAGS}")
			if(NOT PchHeader STREQUAL "")
				msg_debug(	"Precompiled headers:		YES")
				add_precompiled_header( ${TargetName}, ${PchHeader}, "${${TargetName}_SrcPrefix}source/${TargetName}/${PchCpp}")
			else()
				msg_debug(	"Precompiled headers:		NO")
			endif()
		endif()
		if (MSVC)
			#With Visual Studio, we copy outputs from bin_temp to binaries folder
			if( IsPlugin )
				add_custom_command(	TARGET ${TargetName} POST_BUILD COMMAND "copy" ARGS "/Y" "\"$(OutDir)$(TargetName)$(TargetExt)\"" "\"${PROJECTS_BINARIES_OUTPUT_PATH}\\$(Configuration)\\share\\${ProjectName}\\$(TargetName)$(TargetExt)\"")
			elseif( IsLib )
				add_custom_command(	TARGET ${TargetName} POST_BUILD COMMAND "copy" ARGS "/Y" "\"$(OutDir)$(TargetName)$(TargetExt)\"" "\"${PROJECTS_BINARIES_OUTPUT_PATH}\\$(Configuration)\\lib\\$(TargetName)$(TargetExt)\"")
			else()
				add_custom_command(	TARGET ${TargetName} POST_BUILD COMMAND "copy" ARGS "/Y" "\"$(OutDir)$(TargetName)$(TargetExt)\"" "\"${PROJECTS_BINARIES_OUTPUT_PATH}\\$(Configuration)\\bin\\$(TargetName)$(TargetExt)\"")
			endif()
		endif ()
		msg_debug( "${TargetName}_CXX_FLAGS  :	${${TargetName}_CXX_FLAGS}")
		msg_debug( "${TargetName}_PCH_FLAGS  :	${${TargetName}_PCH_FLAGS}")
		msg_debug( "${TargetName}_C_FLAGS    :	${${TargetName}_C_FLAGS}")
		msg_debug( "${TargetName}_LINK_FLAGS :	${${TargetName}_LINK_FLAGS}")
		#We now build the install script
		if (NOT MSVC)
			#We copy each lib in <install_dir>/lib folder
			if( IsPlugin )
				install(
					TARGETS	${TargetName}
					ARCHIVE DESTINATION lib
					LIBRARY DESTINATION share/${ProjectName}/
					RUNTIME DESTINATION bin
				)
			else()
				install(
					TARGETS	${TargetName}
					ARCHIVE DESTINATION lib
					LIBRARY DESTINATION lib
					RUNTIME DESTINATION bin
				)
			endif()
		endif()
	endif()
endfunction( add_target)

#--------------------------------------------------------------------------------------------------
#	Function :	add_doc
#	Generates doc for given target
#--------------------------------------------------------------------------------------------------
function( add_doc TargetName)
	if (DOXYGEN_FOUND)
		if (WIN32)
			set( DOXYGEN_INPUT doc/${TargetName}_Win32.Doxyfile)
		endif ()
		
		if (NOT WIN32)
			set( DOXYGEN_INPUT doc/${TargetName}.Doxyfile)
		endif ()

		set( DOXYGEN_OUTPUT doc/${TargetName})

		add_custom_command(
			OUTPUT ${DOXYGEN_OUTPUT}
			COMMAND ${CMAKE_COMMAND} -E echo_append "Building ${TargetName} API Documentation..."
			COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
			COMMAND ${CMAKE_COMMAND} -E echo "Done."
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			DEPENDS ${DOXYGEN_INPUT}
		)

		add_custom_target( Doc_${TargetName} ALL DEPENDS ${DOXYGEN_OUTPUT})
	endif ()
endfunction( add_doc)
