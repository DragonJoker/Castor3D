cmake_minimum_required( VERSION 2.8)
#--------------------------------------------------------------------------------------------------
#	Initial configurations
#--------------------------------------------------------------------------------------------------
# Set project name, used in folders and in workspace creation
set(ProjectName "Castor3D")

# Set project version numbers
set( VERSION_MAJOR	0		)
set( VERSION_MINOR	7		)
set( VERSION_BUILD	0		)
set( VERSION_YEAR	2013	)

# Used to look for external modules
set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")
# Experimental Precompiled headers support for GCC
include( ${CMAKE_SOURCE_DIR}/cmake_modules/PCHSupport.cmake)
# Force Unicode projects (MBCS is past :P)
add_definitions(-DUNICODE -D_UNICODE)
# Declare the project
project(${ProjectName})

include( ${CMAKE_SOURCE_DIR}/cmake_modules/Project.cmake)

#--------------------------------------------------------------------------------------------------
#	Looking for needed libraries
#--------------------------------------------------------------------------------------------------
if (NOT MINGW)
	set( Boost_USE_STATIC_LIBS ON)
	set( BOOST_THREAD_USE_LIB ON)
endif()
if("${CASTOR_BUILD_TOOLS}" STREQUAL "")
    set( CASTOR_BUILD_TOOLS 0 CACHE BOOL "Build binary tools such as CastorViewer" )
endif()
if("${CASTOR_BUILD_SAMPLES}" STREQUAL "")
    set( CASTOR_BUILD_SAMPLES 0 CACHE BOOL "Build Castor3D samples" )
endif()
if("${CASTOR_BUILD_TESTING}" STREQUAL "")
    set( CASTOR_BUILD_TESTING 0 CACHE BOOL "Build CastorUtils testing project (needs GLM)" )
endif()
set( Boost_NEEDED TRUE )

if (CMAKE_COMPILER_IS_GNUCXX)
	COMPILER_DUMPVERSION(Castor_COMPILER_VERSION)
	msg_debug( "GCC version : ${Castor_COMPILER_VERSION}" )
	if (${Castor_COMPILER_VERSION} LESS "46" OR MINGW)
		find_package(	Boost REQUIRED thread system chrono			)
	else()
		set( Boost_NEEDED FALSE )
		set( Boost_FOUND TRUE )
	endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	COMPILER_DUMPVERSION(Castor_COMPILER_VERSION)
	msg_debug( "clang version : ${Castor_COMPILER_VERSION}" )
	set( Boost_NEEDED FALSE )
	set( Boost_FOUND TRUE )
elseif (BORLAND)
	find_package( 		Boost REQUIRED thread system				)
elseif (NOT MSVC11 AND NOT MSVC12)
	find_package( 		Boost REQUIRED thread system chrono			)
else()
	set( Boost_NEEDED FALSE )
	set( Boost_FOUND TRUE )
endif ()
find_package( 			OpenGL										)
find_package( 			Freetype									)
find_package( 			FreeImage									)
if( WIN32 )
	find_package( 		Direct3D9									)
	find_package( 		Direct3D10									)
	find_package( 		Direct3D11									)
	if( DIRECT3D9_FOUND )
		if( DIRECT3D10_FOUND )
			if( DIRECT3D11_FOUND )
				find_package( CgToolkit COMPONENTS GL D3D9 D3D10 D3D11 )
			else()
				find_package( CgToolkit COMPONENTS GL D3D9 D3D10 )
			endif()
		else()
			if( DIRECT3D11_FOUND )
				find_package( CgToolkit COMPONENTS GL D3D9 D3D11 )
			else()
				find_package( CgToolkit COMPONENTS GL D3D9 )
			endif()
		endif()
	else()
		if( DIRECT3D10_FOUND )
			if( DIRECT3D11_FOUND )
				find_package( CgToolkit COMPONENTS GL D3D10 D3D11 )
			else()
				find_package( CgToolkit COMPONENTS GL D3D10 )
			endif()
		else()
			if( DIRECT3D11_FOUND )
				find_package( CgToolkit COMPONENTS GL D3D11 )
			else()
				find_package( CgToolkit COMPONENTS GL )
			endif()
		endif()
	endif()
else()
	find_package( 		CgToolkit COMPONENTS	GL					)
endif()
find_package( 			ASSIMP										)
if (NOT WIN32)
	find_package( 		GTK2										)
endif ()
if( MSVC )
	find_package(		VLD											)
endif()
if( ${CASTOR_BUILD_TESTING} )
	find_package(		GLM											)
	find_package(		OpenCL										)
endif()

#--------------------------------------------------------------------------------------------------
#	Adding include dirs to include search path
#--------------------------------------------------------------------------------------------------
include_directories( 		include						)
if( Boost_FOUND AND Boost_NEEDED )
	include_directories(	${Boost_INCLUDE_DIRS}		)
endif()
if( FREETYPE_FOUND )
	include_directories(	${FREETYPE_INCLUDE_DIRS}	)
endif()
if (NOT WIN32)
	include_directories(	${GTK2_INCLUDE_DIRS}		)
endif()
if( FREEIMAGE_FOUND )
	include_directories(	${FreeImage_INCLUDE_DIR}	)
endif()
if( CgToolkit_FOUND )
	include_directories(	${CgToolkit_INCLUDE_DIR}	)
endif()
if( ASSIMP_FOUND )
	include_directories(	${ASSIMP_INCLUDE_DIR}		)
endif()
if( DIRECT3D9_FOUND )
	include_directories(	${D3D9_INCLUDE_DIR}			)
endif ()
if( DIRECT3D10_FOUND )
	include_directories(	${D3D10_INCLUDE_DIR}		)
endif ()
if( DIRECT3D11_FOUND )
	include_directories(	${D3D11_INCLUDE_DIR}		)
endif ()
if( VLD_FOUND )
	include_directories(	${VLD_INCLUDE_DIR}			)
endif ()
if( GLM_FOUND )
	include_directories(	${GLM_INCLUDE_DIR}			)
endif ()
if( OPENCL_FOUND )
	include_directories(	${OPENCL_INCLUDE_DIRS}		)
endif ()

#--------------------------------------------------------------------------------------------------
#	Adding few library dirs to link search path
#--------------------------------------------------------------------------------------------------
if( VLD_FOUND )
	link_directories(		${VLD_LIBRARY_DIR}			)
endif()

#--------------------------------------------------------------------------------------------------
#	Normalising libraries names
#--------------------------------------------------------------------------------------------------
#FreeImage Libs
set( FreeImageLibraries "")
FOREACH(Lib ${FreeImage_LIBRARIES})
	if (FreeImageLibraries)
		set(FreeImageLibraries "${FreeImageLibraries}¤${Lib}")
	else()
		set(FreeImageLibraries "${Lib}")
	endif()
ENDFOREACH()

#FreeType Libs
set( FreeTypeLibraries "")
FOREACH(Lib ${FREETYPE_LIBRARIES})
	if (FreeTypeLibraries)
		set(FreeTypeLibraries "${FreeTypeLibraries}¤${Lib}")
	else()
		set(FreeTypeLibraries "${Lib}")
	endif()
ENDFOREACH()

#OpenGL Libs
set( OpenGlLibraries "")
FOREACH(Lib ${OPENGL_gl_LIBRARY})
	if (OpenGlLibraries)
		set(OpenGlLibraries "${OpenGlLibraries}¤${Lib}")
	else()
		set(OpenGlLibraries "${Lib}")
	endif()
ENDFOREACH()

#CgToolkit Libs
set( CgToolkitLibraries "")
FOREACH(Lib ${CgToolkit_LIBRARIES})
	if (CgToolkitLibraries)
		set(CgToolkitLibraries "${CgToolkitLibraries}¤${Lib}")
	else()
		set(CgToolkitLibraries "${Lib}")
	endif()
ENDFOREACH()

#ASSIMP Libs
set( ASSIMPLibraries "")
FOREACH(Lib ${ASSIMP_LIBRARIES})
	if (ASSIMPLibraries)
		set(ASSIMPLibraries "${ASSIMPLibraries}¤${Lib}")
	else()
		set(ASSIMPLibraries "${Lib}")
	endif()
ENDFOREACH()

#Boost Libs
set( BoostLibraries "")
FOREACH(Lib ${Boost_LIBRARIES})
	if (BoostLibraries)
		set(BoostLibraries "${BoostLibraries}¤${Lib}")
	else()
		set(BoostLibraries "${Lib}")
	endif()
ENDFOREACH()

#Direct3D9 Libs
set( D3d9Libraries "")
FOREACH(Lib ${D3D9_LIBRARIES})
	if (D3d9Libraries)
		set(D3d9Libraries "${D3d9Libraries}¤${Lib}")
	else()
		set(D3d9Libraries "${Lib}")
	endif()
ENDFOREACH()

#Direct3D10 Libs
set( D3d10Libraries "")
FOREACH(Lib ${D3D10_LIBRARIES})
	if (D3d10Libraries)
		set(D3d10Libraries "${D3d10Libraries}¤${Lib}")
	else()
		set(D3d10Libraries "${Lib}")
	endif()
ENDFOREACH()

#Direct3D11 Libs
set( D3d11Libraries "")
FOREACH(Lib ${D3D11_LIBRARIES})
	if (D3d11Libraries)
		set(D3d11Libraries "${D3d11Libraries}¤${Lib}")
	else()
		set(D3d11Libraries "${Lib}")
	endif()
ENDFOREACH()

#VLD Libs
set( VldLibraries "")
FOREACH(Lib ${VLD_LIBRARIES})
	if (VldLibraries)
		set(VldLibraries "${VldLibraries}¤${Lib}")
	else()
		set(VldLibraries "${Lib}")
	endif()
ENDFOREACH()

#OpenCL Libs
set( OpenCLLibraries "")
FOREACH(Lib ${OPENCL_LIBRARIES})
	if (OpenCLLibraries)
		set(OpenCLLibraries "${OpenCLLibraries}¤${Lib}")
	else()
		set(OpenCLLibraries "${Lib}")
	endif()
ENDFOREACH()

if (${CastorDebug})
	show_libs()
endif ()

#In order to be sure we have the right flags, we set it manually
if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "")
	if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}")
		set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
	else()
		set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE}")
		set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
	endif()
endif()

#if VLD found, we tell it to the compiler
if( VLD_FOUND )
	set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVLD_AVAILABLE")
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DVLD_AVAILABLE")
	MESSAGE( STATUS "Using Visual Leak Detector to check for Memory leaks" )
endif()

msg_debug( "CMAKE_BUILD_TYPE : ${CMAKE_BUILD_TYPE}")
msg_debug( "CMAKE_C_FLAGS    : ${CMAKE_C_FLAGS}" )
msg_debug( "CMAKE_CXX_FLAGS  : ${CMAKE_CXX_FLAGS}" )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/include/Castor3D/RequiredVersion.hpp.in	${CMAKE_CURRENT_SOURCE_DIR}/include/Castor3D/RequiredVersion.hpp)

SET( CastorMinLibraries "${FreeImageLibraries};${FreeTypeLibraries};${BoostLibraries}" )
SET( Castor3DLibraries "CastorUtils;Castor3D" )

if( FREEIMAGE_FOUND AND FREETYPE_FOUND AND Boost_FOUND )
	SET( Testing "no" )
	SET( Assimp "no" )
	SET( Ase "no" )
	SET( Md2 "no" )
	SET( Md3 "no" )
	SET( 3ds "no" )
	SET( Lwo "no" )
	SET( Obj "no" )
	SET( Ply "yes" )
	SET( Tools "no" )
	SET( Samples "no" )
	SET( PNTri "yes" )
	SET( Loop "yes" )
	SET( D3d9 "no" )
	SET( D3d10 "no" )
	SET( D3d11 "no" )
	SET( Cg "no" )
	#--------------------------------------------------------------------------------------------------
	#	Creation of each project in the workspace
	#--------------------------------------------------------------------------------------------------
	SET( CastorBinsDependencies "CastorUtils;Castor3D;PnTrianglesDivider;LoopDivider")
	add_target(
		CastorUtils
 		lib
		""
		"${CastorMinLibraries}"
		"PrecompiledHeader.hpp"
		"PrecompiledHeader.cpp"	""	""	""
		)
	if( ${CASTOR_BUILD_TESTING} AND GLM_FOUND AND OPENCL_FOUND )
		add_target(
			CastorUtilsTest
			bin_dos
			"CastorUtils"
			"CastorUtils;${CastorMinLibraries};${OpenCLLibraries}"	""	""	""	""	""
			)
		SET( Testing "yes" )
	endif()
	add_target(
		Castor3D
		dll
		"CastorUtils"
		"CastorUtils;${CastorMinLibraries}"
		"PrecompiledHeader.hpp"
		"PrecompiledHeader.cpp"	""	""	""
		)
	add_target(
		PnTrianglesDivider
		plugin
		"Castor3D"
		"${Castor3DLibraries}"	""	""	""	""	""
		)
	add_target(
		LoopDivider
		plugin
		"Castor3D"
		"${Castor3DLibraries}"	""	""	""	""	""
		)
	SET( CastorBinsDependencies "${CastorBinsDependencies};GlRenderSystem")
	add_target(
		GlRenderSystem
		plugin
		"Castor3D"
		"${Castor3DLibraries};${OpenGlLibraries}"
		"PrecompiledHeader.hpp"
		"PrecompiledHeader.cpp"	""	""	""
		)
	if( ASSIMP_FOUND )
		SET( CastorBinsDependencies "${CastorBinsDependencies};ASSIMPImporter")
		add_target(
			ASSIMPImporter
			plugin
			"Castor3D"
			"${Castor3DLibraries};${ASSIMPLibraries}"	""	""	""	""	""
			)
		SET( Assimp "yes" )
	else()
		SET( CastorBinsDependencies "${CastorBinsDependencies};AseImporter;Md2Importer;Md3Importer;ObjImporter;PlyImporter;SMaxImporter;LwoImporter")
		add_target(
			AseImporter
			plugin
			"Castor3D"
			"${Castor3DLibraries}"	""	""	""	""	""
			)
		SET( Ase "yes" )
		add_target(
			Md2Importer
			plugin
			"Castor3D"
			"${Castor3DLibraries}"	""	""	""	""	""
			)
		SET( Md2 "yes" )
		add_target(
			Md3Importer
			plugin
			"Castor3D"
			"${Castor3DLibraries}"	""	""	""	""	""
			)
		SET( Md3 "yes" )
		add_target(
			SMaxImporter
			plugin
			"Castor3D"
			"${Castor3DLibraries}"	""	""	""	""	""
			)
		SET( 3ds "yes" )
		add_target(
			LwoImporter
			plugin
			"Castor3D"
			"${Castor3DLibraries}"	""	""	""	""	""
			)
		SET( Lwo "yes" )
		add_target(
			ObjImporter
			plugin
			"Castor3D"
			"${Castor3DLibraries}"	""	""	""	""	""
			)
		SET( Obj "yes" )
		message( STATUS "Missing Assimp, ASSIMPImporter won't be built, own importers will be used")
	ENDIF()
	add_target(
		PlyImporter
		plugin
		"Castor3D"
		"${Castor3DLibraries}"	""	""	""	""	""
		)
	if (DIRECT3D9_FOUND)
		SET( CastorBinsDependencies "${CastorBinsDependencies};Dx9RenderSystem")
		add_target(
			Dx9RenderSystem
			plugin
			"Castor3D"
			"${Castor3DLibraries};${D3d9Libraries}"
			"PrecompiledHeader.hpp"
			"PrecompiledHeader.cpp"	""	""	""
			)
		SET( D3d9 "yes" )
	endif()
	if (DIRECT3D10_FOUND)
		SET( CastorBinsDependencies "${CastorBinsDependencies};Dx10RenderSystem")
		add_target(
			Dx10RenderSystem
			plugin
			"Castor3D"
			"${Castor3DLibraries};${D3d10Libraries}"
			"PrecompiledHeader.hpp"
			"PrecompiledHeader.cpp"	""	""	""
			)
		SET( D3d10 "yes" )
	endif()
	if (DIRECT3D11_FOUND)
		SET( CastorBinsDependencies "${CastorBinsDependencies};Dx11RenderSystem")
		add_target(
			Dx11RenderSystem
			plugin
			"Castor3D"
			"${Castor3DLibraries};${D3d9Libraries};${D3d11Libraries}"
			"PrecompiledHeader.hpp"
			"PrecompiledHeader.cpp"	""	""	""
			)
		SET( D3d11 "yes" )
	endif()
	if( CgToolkit_FOUND AND MSVC )
		SET( CgFlags	"")
		SET( CgLibraries "${Castor3DLibraries};${CgToolkitLibraries}")
		SET( CgDependencies "Castor3D")
		SET( CastorBinsDependencies "${CastorBinsDependencies};ShaderProgramCg")
		SET( CgFlags	"${CgFlags} -DCg_OpenGL")
		SET( CgDependencies "${CgDependencies};GlRenderSystem")
		SET( CgLibraries "${CgLibraries};${OpenGlLibraries};GlRenderSystem")
		if( DIRECT3D9_FOUND )
			SET( CgFlags	"${CgFlags} -DCg_Direct3D9")
			SET( CgDependencies "${CgDependencies};Dx9RenderSystem")
			SET( CgLibraries "${CgLibraries};${D3d9Libraries};Dx9RenderSystem")
		endif()
		if( DIRECT3D10_FOUND )
			SET( CgFlags	"${CgFlags} -DCg_Direct3D10")
			SET( CgDependencies "${CgDependencies};Dx10RenderSystem")
			if( DIRECT3D9_FOUND )
				SET( CgLibraries "${CgLibraries};${D3d10Libraries};Dx10RenderSystem")
			else()
				SET( CgLibraries "${CgLibraries};${D3d9Libraries};${D3d10Libraries};Dx10RenderSystem")
			endif()
		endif()
		if( DIRECT3D11_FOUND )
			SET( CgFlags	"${CgFlags} -DCg_Direct3D11")
			SET( CgDependencies "${CgDependencies};Dx11RenderSystem")
			if( DIRECT3D9_FOUND )
				SET( CgLibraries "${CgLibraries};${D3d11Libraries};Dx11RenderSystem")
			elseif( DIRECT3D10_FOUND )
				SET( CgLibraries "${CgLibraries};${D3d11Libraries};Dx11RenderSystem")
			else()
				SET( CgLibraries "${CgLibraries};${D3d9Libraries};${D3d11Libraries};Dx11RenderSystem")
			endif()
		endif()
		add_target(
			ShaderProgramCg
			plugin
			"${CgDependencies}"
			"${CgLibraries}"
			"PrecompiledHeader.hpp"
			"PrecompiledHeader.cpp"
			"${CgFlags}"
			"${CgFlags}"
			""
			)
		SET( Cg "yes" )
	endif ()
	if( ${CASTOR_BUILD_TOOLS} OR ${CASTOR_BUILD_SAMPLES} )
		#if asked for tools or samples we look for wxWidgets
		if (WIN32)
			find_package( 		wxWidgets REQUIRED stc scintilla core base aui	)
		else ()
			find_package( 		wxWidgets REQUIRED core base stc aui			)
		endif()
		if( wxWidgets_FOUND )
			include_directories(	${wxWidgets_INCLUDE_DIRS}	)
		endif()
		#wxWidgets Libs
		set( wxWidgetsLibraries "")
		FOREACH(Lib ${wxWidgets_LIBRARIES})
			if (wxWidgetsLibraries)
				set(wxWidgetsLibraries "${wxWidgetsLibraries}¤${Lib}")
			else()
				set(wxWidgetsLibraries "${Lib}")
			endif()
		ENDFOREACH()
		FOREACH(Lib ${GTK2_LIBRARIES})
			if (wxWidgetsLibraries)
				set(wxWidgetsLibraries "${wxWidgetsLibraries}¤${Lib}")
			else()
				set(wxWidgetsLibraries "${Lib}")
			endif()
		ENDFOREACH()
		if( wxWidgets_FOUND )
			#Few more configuration for wxWidgets
			string( COMPARE EQUAL "${wxWidgets_USE_DEBUG}" "ON" IsWxDebug)
			set( wxWidgets_CXXFLAGS "${wxWidgets_CXX_FLAGS} -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES")
			if (NOT WIN32)
				set( wxWidgets_CXXFLAGS "${wxWidgets_CXXFLAGS} -D__WXGTK__")
			endif ()
			if (IsWxDebug)
				set( wxWidgets_CXXFLAGS "${wxWidgets_CXXFLAGS} -D__WXDEBUG__")
			endif ()
			SET( CastorBinsDependencies "${CastorBinsDependencies};GuiCommon")
			SET( CastorBinLibraries "${Castor3DLibraries};${CastorMinLibraries};${wxWidgetsLibraries};GuiCommon" )
			# GuiCommon is built so it can be used in tools and samples
			add_target(
				GuiCommon
				lib
				"Castor3D"
				"${Castor3DLibraries};${CastorMinLibraries};${wxWidgetsLibraries}"
				"PrecompiledHeader.hpp"
				"PrecompiledHeader.cpp"
				"${wxWidgets_CFLAGS}"
				"${wxWidgets_CXXFLAGS}"
				"tools"
				)
			# Copying language files for GLSL and HLSL coloration in wxShaderDialog
			file( GLOB_RECURSE Parser_languages 	tools/resource/GuiCommon/languages/*.lang)
			if( MSVC )
				file( COPY ${Parser_languages} DESTINATION ${PLUGINS_OUTPUT_PATH}/Debug/languages)
				file( COPY ${Parser_languages} DESTINATION ${PLUGINS_OUTPUT_PATH}/Release/languages)
				file( COPY ${Parser_languages} DESTINATION ${PROJECTS_BINARIES_OUTPUT_PATH}/Debug/share/Castor3D/languages)
				file( COPY ${Parser_languages} DESTINATION ${PROJECTS_BINARIES_OUTPUT_PATH}/Release/share/Castor3D/languages)
			else()
				file( COPY ${Parser_languages} DESTINATION ${PROJECTS_BINARIES_OUTPUT_PATH}/share/Castor3D/languages)
			endif()
			if( NOT MSVC )
				install(
					FILES ${Parser_languages}
					DESTINATION ${CMAKE_INSTALL_PREFIX}/share/Castor3D/languages/
				)
			endif()
		else()
			MESSAGE( STATUS "Missing wxWidgets, tools and samples can't be built" )
		endif()
	endif()
	if( ${CASTOR_BUILD_TOOLS} AND wxWidgets_FOUND )
		add_target(
			ImgConverter
			bin
			""
			"${wxWidgetsLibraries}"
			"PrecompiledHeader.hpp"
			"PrecompiledHeader.cpp"
			"${wxWidgets_CFLAGS}"
			"${wxWidgets_CXXFLAGS}"
			"tools"
			)
		# Copying internationalisation files (french, for now) for ImgConverter
		copy_languages( ImgConverter tools ImgConverter )
		add_target(
			CastorShape
			bin
			"${CastorBinsDependencies}"
			"${CastorBinLibraries}"
			"PrecompiledHeader.hpp"
			"PrecompiledHeader.cpp"
			"${wxWidgets_CFLAGS}"
			"${wxWidgets_CXXFLAGS}"
			"tools"
			)
		# Copying internationalisation files (french, for now) for CastorShape
		copy_languages( CastorShape tools CastorShape )
		add_target(
			CastorViewer
			bin
			"${CastorBinsDependencies}"
			"${CastorBinLibraries}"
			"PrecompiledHeader.hpp"
			"PrecompiledHeader.cpp"
			"${wxWidgets_CFLAGS}"
			"${wxWidgets_CXXFLAGS}"
			"tools"
			)
		# Copying internationalisation files (french, for now) for CastorViewer
		copy_languages( CastorViewer tools CastorViewer )
		SET( Tools "yes" )
	endif()
	if( ${CASTOR_BUILD_SAMPLES} )
		if( wxWidgets_FOUND )
			add_target(
				OceanLightingFFT
				bin
				"${CastorBinsDependencies}"
				"${CastorBinLibraries}"
				""
				""
				"${wxWidgets_CFLAGS}"
				"${wxWidgets_CXXFLAGS}"
				"samples"
				)
			SET( Samples "yes" )
			# Copying data files
			file( GLOB OceanLightingFFT_datas1 	samples/resource/OceanLightingFFT/rsc/*.*)
			if( MSVC )
				file( COPY ${OceanLightingFFT_datas1} DESTINATION ${PLUGINS_OUTPUT_PATH}/OceanLightingFFT)
				file( COPY ${OceanLightingFFT_datas1} DESTINATION ${PROJECTS_BINARIES_OUTPUT_PATH}/Debug/share/OceanLightingFFT)
				file( COPY ${OceanLightingFFT_datas1} DESTINATION ${PROJECTS_BINARIES_OUTPUT_PATH}/Release/share/OceanLightingFFT)
			else()
				file( COPY ${OceanLightingFFT_datas1} DESTINATION ${PROJECTS_BINARIES_OUTPUT_PATH}/share/OceanLightingFFT)
			endif()
			if( NOT MSVC )
				install(
					FILES ${OceanLightingFFT_datas1}
					DESTINATION ${CMAKE_INSTALL_PREFIX}/share/OceanLightingFFT/
				)
			endif()
			file( GLOB OceanLightingFFT_datas2 	samples/resource/OceanLightingFFT/rsc/data/*.*)
			if( MSVC )
				file( COPY ${OceanLightingFFT_datas2} DESTINATION ${PLUGINS_OUTPUT_PATH}/OceanLightingFFT/data)
				file( COPY ${OceanLightingFFT_datas2} DESTINATION ${PROJECTS_BINARIES_OUTPUT_PATH}/Debug/share/OceanLightingFFT/data)
				file( COPY ${OceanLightingFFT_datas2} DESTINATION ${PROJECTS_BINARIES_OUTPUT_PATH}/Release/share/OceanLightingFFT/data)
			else()
				file( COPY ${OceanLightingFFT_datas2} DESTINATION ${PROJECTS_BINARIES_OUTPUT_PATH}/share/OceanLightingFFT/data)
			endif()
			if( NOT MSVC )
				install(
					FILES ${OceanLightingFFT_datas2}
					DESTINATION ${CMAKE_INSTALL_PREFIX}/share/OceanLightingFFT/data/
				)
			endif()
		else()
			SET( msg "Castor tools can't be built because of following missing libraries :" )
			if( NOT wxWidgets_FOUND )
				SET( msg "${msg}\n\twxWidgets" )
			endif()
		endif()
	endif()
	SET( msg "Castor following projects will be built" )
	SET( msg "${msg}\nImporters :" )
	SET( msg "${msg}\n    ASSIMP       ${Assimp}" )
	SET( msg "${msg}\n    Built-in ASE ${Ase}" )
	SET( msg "${msg}\n    Built-in MD2 ${Md2}" )
	SET( msg "${msg}\n    Built-in MD3 ${Md3}" )
	SET( msg "${msg}\n    Built-in 3DS ${3ds}" )
	SET( msg "${msg}\n    Built-in LWO ${Lwo}" )
	SET( msg "${msg}\n    Built-in OBJ ${Obj}" )
	SET( msg "${msg}\n    Built-in PLY ${Ply}" )
	SET( msg "${msg}\n\nSubdividers :" )
	SET( msg "${msg}\n    PN-Triangles ${PNTri}" )
	SET( msg "${msg}\n    Loop         ${Loop}" )
	SET( msg "${msg}\n\nRenderers :" )
	SET( msg "${msg}\n    Direct3D 9  ${D3d9}" )
	SET( msg "${msg}\n    Direct3D 10 ${D3d10}" )
	SET( msg "${msg}\n    Direct3D 11 ${D3d11}" )
	SET( msg "${msg}\n\nOther :" )
	SET( msg "${msg}\n    Tools               ${Tools}" )
	SET( msg "${msg}\n    Samples             ${Samples}" )
	SET( msg "${msg}\n    Cg Language support ${Cg}" )
	SET( msg "${msg}\n    CastorUtils test    ${Testing}" )
	MESSAGE( STATUS ${msg} )
else()
	SET( msg "Castor projects can't be built because of following missing libraries :" )
	if( NOT FREEIMAGE_FOUND )
		SET( msg "${msg}\n\tFreeImage" )
	endif()
	if( NOT FREETYPE_FOUND )
		SET( msg "${msg}\n\tFreetype" )
	endif()
	if( Boost_NEEDED AND NOT Boost_FOUND )
		SET( msg "${msg}\n\tBoost thread" )
	endif()
	MESSAGE( SEND_ERROR ${msg} )
endif()
