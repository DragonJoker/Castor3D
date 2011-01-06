cmake_minimum_required( VERSION 2.6)

macro( add_target TargetName Sources StrType)
	string( COMPARE EQUAL ${StrType} "lib" IsLib)
	string( COMPARE EQUAL ${StrType} "dll" IsDll)
	string( COMPARE EQUAL ${StrType} "gldll" IsGlDll)
	string( COMPARE EQUAL ${StrType} "exe" IsExe)
	string( COMPARE EQUAL ${StrType} "plugin" IsPlugin)

	if (WIN32)
		if (NOT IsPlugin)
			set( ${Sources} ${${Sources}} resource/${TargetName}/${TargetName}.rc resource/${TargetName}/resource.h)
		endif (NOT IsPlugin)
	endif (WIN32)

	if (IsGlDll)
		message( STATUS "GL Dll ${TargetName} Sources : ${Sources}")
		add_library( ${TargetName} SHARED ${${Sources}})
		target_link_libraries( ${TargetName} Castor3D)
		target_link_libraries( ${TargetName} OpenGLCommon)
		target_link_libraries( ${TargetName} ${CgGL_LIBRARY_NAME})
		target_link_libraries( ${TargetName} ${OPENGL_gl_LIBRARY})
		target_link_libraries( ${TargetName} ${OPENGL_glu_LIBRARY})
		target_link_libraries( ${TargetName} ${Glew_LIBRARY_NAME})
		add_dependencies( ${TargetName} OpenGLCommon)
		set_target_properties( ${TargetName} PROPERTIES COMPILE_FLAGS "-D${TargetName}_EXPORTS")
	elseif (IsPlugin)
		message( STATUS "Plugin ${TargetName} Sources : ${Sources}")
		add_library( ${TargetName} SHARED ${${Sources}})
		target_link_libraries( ${TargetName} Castor3D)
		add_dependencies( ${TargetName} OpenGLCommon)
		set_target_properties( ${TargetName} PROPERTIES COMPILE_FLAGS "-D${TargetName}_EXPORTS")
	elseif (IsDll)
		message( STATUS "Dll    ${TargetName} Sources : ${Sources}")
		add_library( ${TargetName} SHARED ${${Sources}})
		target_link_libraries( ${TargetName} CastorUtils)
		target_link_libraries( ${TargetName} ${Cg_LIBRARY_NAME})
		target_link_libraries( ${TargetName} ${FreeImage_LIBRARY_NAME})
		target_link_libraries( ${TargetName} ${FreeType_LIBRARY_NAME})
		add_dependencies( ${TargetName} CastorUtils)
		set_target_properties( ${TargetName} PROPERTIES COMPILE_FLAGS "-D${TargetName}_EXPORTS")
	elseif (IsExe)
		message( STATUS "Exe    ${TargetName} Sources : ${Sources}")
		add_executable( ${TargetName} ${${Sources}})
		target_link_libraries( ${TargetName} Castor3D)
		target_link_libraries( ${TargetName} GUICommon)
		target_link_libraries( ${TargetName} ${wxWidgets_LIBRARIES})
		add_dependencies( ${TargetName} GUICommon)
		add_dependencies( ${TargetName} OpenGLCommon)
		add_dependencies( ${TargetName} GL2RenderSystem)
		add_dependencies( ${TargetName} GL3RenderSystem)
		add_dependencies( ${TargetName} LoopDivider)
		add_dependencies( ${TargetName} AseImporter)
		add_dependencies( ${TargetName} BspImporter)
		add_dependencies( ${TargetName} GsdImporter)
		add_dependencies( ${TargetName} Md2Importer)
		add_dependencies( ${TargetName} Md3Importer)
		add_dependencies( ${TargetName} ObjImporter)
		add_dependencies( ${TargetName} PlyImporter)
		add_dependencies( ${TargetName} SMaxImporter)
	elseif (IsLib)
		message( STATUS "Lib    ${TargetName}  Sources : ${Sources}")
		add_library( ${TargetName} STATIC ${${Sources}})
		string( COMPARE NOTEQUAL ${TargetName} "CastorUtils" IsNotUtils)
		
		if (IsNotUtils)
			add_dependencies( ${TargetName} Castor3D)
		endif (IsNotUtils)
	endif (IsGlDll)

	if (MSVC)
		if (NOT IsPlugin)
			set_target_properties( ${TargetName} PROPERTIES COMPILE_FLAGS "/Yu${TargetName}/PrecompiledHeader.h")
			set_source_files_properties( source/${TargetName}/PrecompiledHeader.cpp PROPERTIES COMPILE_FLAGS "/Yc${TargetName}/PrecompiledHeader.h")
		endif (NOT IsPlugin)
	endif (MSVC)
endmacro( add_target)

macro( add_doc TargetName)
	if (DOXYGEN_FOUND)
		if (WIN32)
			set( DOXYGEN_INPUT doc/${TargetName}_Win32.Doxyfile)
		endif (WIN32)
		
		if (NOT WIN32)
			set( DOXYGEN_INPUT doc/${TargetName}.Doxyfile)
		endif (NOT WIN32)

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
	endif (DOXYGEN_FOUND)
endmacro( add_doc)

project( Castor3D)

include( FindBoost)
include( FindOpenGL)
include( FindwxWidgets)
include( FindDoxygen)
include( FindFreetype)

include (CheckIncludeFileCXX)

set( NeededHeaders FreeImage.h cg.h cgGL.h glew.h)

if (WIN32)
	set( ${NeededHeaders} ${${NeededHeaders}} wglew.h)
else (WIN32)
	set( ${NeededHeaders} ${${NeededHeaders}} glxew.h)
endif (WIN32)

foreach (RQ_HDR ${NeededHeaders})
	check_include_file_cxx( ${RQ_HDR} RQ_HDR_RET)

	if (NOT RQ_HDR_RET)
		message( FATAL_ERROR "missing ${RQ_HDR} !")
	endif (NOT RQ_HDR_RET)

	message ( STATUS "${RQ_HDR} ${RQ_HDR_RET}")
endforeach (RQ_HDR ${NeededHeaders})

set( FreeType_INCLUDE_DIRS C:/Dev/Libs/freetype-2.4.3/include)

set( FreeType_LIBRARY_DIR C:/Dev/Libs/freetype-2.4.3/objs/win32/vc2008)
set( FreeType_LIBRARY_NAME freetype243)

set( FreeImage_LIBRARY_DIR C:/Dev/Libs/FreeImage/Dist)
set( FreeImage_LIBRARY_NAME FreeImage)

set( Glew_LIBRARY_DIR C:/Dev/Libs/OpenGL/glew-1.5.7/lib)
if (WIN32)
	set( Glew_LIBRARY_NAME glew32)
elseif (NOT WIN32)
endif (WIN32)

set( Cg_LIBRARY_DIR C:/Dev/Libs/Cg/lib)
set( Cg_LIBRARY_NAME cg)
set( CgGL_LIBRARY_NAME CgGL)

include_directories( include)
include_directories( ${Boost_INCLUDE_DIRS})
include_directories( ${wxWidgets_INCLUDE_DIRS})
include_directories( ${Cg_INCLUDE_DIR})
include_directories( ${FreeImage_INCLUDE_DIR})
include_directories( ${FreeType_INCLUDE_DIRS})

link_directories( "${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
link_directories( ${Boost_LIBRARY_DIRS})
link_directories( ${wxWidgets_LIBRARY_DIRS})
link_directories( ${Cg_LIBRARY_DIR})
link_directories( ${FreeImage_LIBRARY_DIR})
link_directories( ${FreeType_LIBRARY_DIR})

set( EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
set( LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")

find_package(
	Boost
	1.36.0
	REQUIRED date_time
	REQUIRED filesystem
	REQUIRED system
)

find_package(
	OpenGL
	REQUIRED OpenGL
)

find_package(
	wxWidgets
	2.8.7
)

find_package(
	freetype
)

file(
	GLOB_RECURSE
	castor_utils
	source/CastorUtils/*
	include/CastorUtils/*
)

file(
	GLOB_RECURSE
	castor_3d
	source/Castor3D/*
	include/Castor3D/*
)

file(
	GLOB_RECURSE
	opengl_common
	source/OpenGLCommon/*
	include/OpenGLCommon/*
)

file(
	GLOB_RECURSE
	gl2_render_system
	source/GL2RenderSystem/*
	include/GL2RenderSystem/*
)

file(
	GLOB_RECURSE
	gl3_render_system
	source/GL3RenderSystem/*
	include/GL3RenderSystem/*
)

file(
	GLOB_RECURSE
	gui_common
	source/GUICommon/*
	include/GUICommon/*
)

file(
	GLOB_RECURSE
	castor_viewer
	source/CastorViewer/*
	include/CastorViewer/*
)

file(
	GLOB_RECURSE
	castor_architect
	source/CastorArchitect/*
	include/CastorArchitect/*
)

file(
	GLOB_RECURSE
	castor_shape
	source/CastorShape/*
	include/CastorShape/*
)

file(
	GLOB_RECURSE
	loop_divider
	source/LoopDivider/*
	include/LoopDivider/*
)

file(
	GLOB_RECURSE
	ase_importer
	source/AseImporter/*
	include/AseImporter/*
)

file(
	GLOB_RECURSE
	bsp_importer
	source/BspImporter/*
	include/BspImporter/*
)

file(
	GLOB_RECURSE
	gsd_importer
	source/GsdImporter/*
	include/GsdImporter/*
)

file(
	GLOB_RECURSE
	md2_importer
	source/Md2Importer/*
	include/Md2Importer/*
)

file(
	GLOB_RECURSE
	md3_importer
	source/Md3Importer/*
	include/Md3Importer/*
)

file(
	GLOB_RECURSE
	obj_importer
	source/ObjImporter/*
	include/ObjImporter/*
)

file(
	GLOB_RECURSE
	ply_importer
	source/PlyImporter/*
	include/PlyImporter/*
)

file(
	GLOB_RECURSE
	smax_importer
	source/SMaxImporter/*
	include/SMaxImporter/*
)

add_target( CastorUtils 		castor_utils			lib)
add_target( Castor3D 			castor_3d				dll)
add_target( OpenGLCommon 		opengl_common			lib)
add_target( GL2RenderSystem		gl2_render_system		gldll)
add_target( GL3RenderSystem		gl3_render_system		gldll)
add_target( LoopDivider			loop_divider			plugin)
add_target( AseImporter			ase_importer			plugin)
add_target( BspImporter			bsp_importer			plugin)
add_target( GsdImporter			gsd_importer			plugin)
add_target( Md2Importer			md2_importer			plugin)
add_target( Md3Importer			md3_importer			plugin)
add_target( ObjImporter			obj_importer			plugin)
add_target( PlyImporter			ply_importer			plugin)
add_target( SMaxImporter		smax_importer			plugin)
add_target( GUICommon 			gui_common				lib)
add_target( CastorViewer 		castor_viewer			exe)
add_target( CastorArchitect		castor_architect		exe)
add_target( CastorShape 		castor_shape			exe)

if (WIN32)
	set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}  /subsystem:windows")
endif (WIN32)

install(
	TARGETS
		CastorUtils Castor3D OpenGLCommon GL2RenderSystem GL3RenderSystem LoopDivider AseImporter BspImporter GsdImporter Md2Importer Md3Importer ObjImporter PlyImporter SMaxImporter GUICommon CastorViewer CastorArchitect CastorShape
	DESTINATION
		bin)