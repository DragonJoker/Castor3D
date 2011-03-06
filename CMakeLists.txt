cmake_minimum_required( VERSION 2.6)

set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

macro( declare_sources TargetName Sources_cpp Sources_c Sources_h)
	file(
		GLOB_RECURSE
		${Sources_cpp}
		source/${TargetName}/*.cpp
	)

	file(
		GLOB_RECURSE
		${Sources_c}
		source/${TargetName}/*.c
	)

	file(
		GLOB_RECURSE
		${Sources_h}
		include/${TargetName}/*.h
		include/${TargetName}/*.inl
	)
endmacro( declare_sources)

macro( add_target TargetName Sources StrType)
	declare_sources( ${TargetName} ${Sources}_cpp ${Sources}_c ${Sources}_h)
	string( COMPARE EQUAL ${StrType} "lib" IsLib)
	string( COMPARE EQUAL ${StrType} "dll" IsDll)
	string( COMPARE EQUAL ${StrType} "gldll" IsGlDll)
	string( COMPARE EQUAL ${StrType} "dx9dll" IsDx9Dll)
	string( COMPARE EQUAL ${StrType} "exe" IsExe)
	string( COMPARE EQUAL ${StrType} "plugin" IsPlugin)
	string( COMPARE EQUAL ${TargetName} "CastorUtils" IsUtils)
	string( COMPARE EQUAL ${TargetName} "GuiCommon" IsGuiCommon)
	string( COMPARE EQUAL ${TargetName} "OpenGlCommon" IsGlCommon)

#	message( STATUS "${TargetName} cpp : ${${Sources}_cpp}")
#	message( STATUS "${TargetName} c : ${${Sources}_c}")
#	message( STATUS "${TargetName} h : ${${Sources}_h}")

	if (WIN32)
		if (NOT IsPlugin)
			set( ${Sources} ${${Sources}} resource/${TargetName}/${TargetName}.rc resource/${TargetName}/resource.h)
		endif (NOT IsPlugin)
	endif (WIN32)

	if (IsGlDll)
		message( STATUS "GL Dll ${TargetName}")
		add_library( ${TargetName} SHARED ${${Sources}_cpp} ${${Sources}_c} ${${Sources}_h})
		set_target_properties( ${TargetName} PROPERTIES COMPILE_FLAGS "-D${TargetName}_EXPORTS")
		target_link_libraries( ${TargetName} Castor3D)
		target_link_libraries( ${TargetName} OpenGlCommon)
		target_link_libraries( ${TargetName} ${Cg_GL_LIBRARY})
		target_link_libraries( ${TargetName} ${OPENGL_gl_LIBRARY})
		add_dependencies( ${TargetName} OpenGlCommon)
	elseif (IsDx9Dll)
		message( STATUS "D3D9 Dll ${TargetName}")
		add_library( ${TargetName} SHARED ${${Sources}_cpp} ${${Sources}_c} ${${Sources}_h})
		set_target_properties( ${TargetName} PROPERTIES COMPILE_FLAGS "-D${TargetName}_EXPORTS")
		target_link_libraries( ${TargetName} Castor3D)
		target_link_libraries( ${TargetName} ${Cg_D3D9_LIBRARY})
		target_link_libraries( ${TargetName} ${D3D9_LIBRARY})
		target_link_libraries( ${TargetName} ${D3D9_X_LIBRARY})
	elseif (IsPlugin)
		message( STATUS "Plugin ${TargetName}")
		add_library( ${TargetName} SHARED ${${Sources}_cpp} ${${Sources}_c} ${${Sources}_h})
		set_target_properties( ${TargetName} PROPERTIES COMPILE_FLAGS "-D${TargetName}_EXPORTS")
		target_link_libraries( ${TargetName} Castor3D)
	elseif (IsDll)
		message( STATUS "Dll    ${TargetName}")
		add_library( ${TargetName} SHARED ${${Sources}_cpp} ${${Sources}_c} ${${Sources}_h})
		set_target_properties( ${TargetName} PROPERTIES COMPILE_FLAGS "-D${TargetName}_EXPORTS")
		target_link_libraries( ${TargetName} CastorUtils)
		target_link_libraries( ${TargetName} ${Cg_LIBRARY})
		target_link_libraries( ${TargetName} ${Boost_LIBRARIES})
		target_link_libraries( ${TargetName} ${FreeImage_LIBRARY})
		target_link_libraries( ${TargetName} ${FREETYPE_LIBRARIES})
		add_dependencies( ${TargetName} CastorUtils)
	elseif (IsExe)
		message( STATUS "Exe    ${TargetName}")
		add_executable( ${TargetName} ${${Sources}_cpp} ${${Sources}_c} ${${Sources}_h})
		set_target_properties( ${TargetName} PROPERTIES COMPILE_FLAGS "${wxWidgets_CXXFLAGS}")
		target_link_libraries( ${TargetName} Castor3D)
		target_link_libraries( ${TargetName} GuiCommon)
		target_link_libraries( ${TargetName} ${wxWidgets_LIBRARIES})
		target_link_libraries( ${TargetName} ${Boost_LIBRARIES})
		add_dependencies( ${TargetName} GuiCommon)
		if (WIN32)
			add_dependencies( ${TargetName} OpenGlCommon)
			add_dependencies( ${TargetName} Gl2RenderSystem)
			add_dependencies( ${TargetName} Gl3RenderSystem)
		else (WIN32)
			target_link_libraries( ${TargetName} ${GTK2_LIBRARIES})
		endif (WIN32)
	elseif (IsLib)
		message( STATUS "Lib    ${TargetName}")
		add_library( ${TargetName} STATIC ${${Sources}_cpp} ${${Sources}_c} ${${Sources}_h})
	
		if (IsGuiCommon OR IsGlCommon)
			if (IsGuiCommon)
#				message( STATUS "wxWidgets_DEFINITIONS : ${wxWidgets_DEFINITIONS}")
#				message( STATUS "wxWidgets_CXX_FLAGS : ${wxWidgets_CXX_FLAGS}")
				set_target_properties( ${TargetName} PROPERTIES COMPILE_FLAGS "${wxWidgets_CXXFLAGS}")
				target_link_libraries( ${TargetName} ${wxWidgets_LIBRARIES})
#				message( STATUS "wxWidgets_LIBRARIES : ${wxWidgets_LIBRARIES}")
			elseif (IsGlCommon)
				target_link_libraries( ${TargetName} ${Cg_GL_LIBRARY})
				target_link_libraries( ${TargetName} ${OPENGL_gl_LIBRARY})
				add_dependencies( ${TargetName} GuiCommon)
				if (NOT WIN32)
					add_dependencies( ${TargetName} CastorViewer)
					add_dependencies( ${TargetName} CastorShape)
					add_dependencies( ${TargetName} CastorArchitect)
				endif (NOT WIN32)
			endif (IsGuiCommon)

			target_link_libraries( ${TargetName} Castor3D)
			add_dependencies( ${TargetName} Castor3D)
			
			if (NOT MSVC)
				add_dependencies( ${TargetName} LoopDivider)
				add_dependencies( ${TargetName} AseImporter)
				add_dependencies( ${TargetName} BspImporter)
				add_dependencies( ${TargetName} GsdImporter)
				add_dependencies( ${TargetName} Md2Importer)
				add_dependencies( ${TargetName} Md3Importer)
				add_dependencies( ${TargetName} ObjImporter)
				add_dependencies( ${TargetName} PlyImporter)
				add_dependencies( ${TargetName} SMaxImporter)
			endif (NOT MSVC)
		endif (IsGuiCommon OR IsGlCommon)
	endif (IsGlDll)

	if (WIN32)
		if (MSVC)
			if (NOT IsPlugin)
#				message( STATUS "TargetName : ${TargetName} - ${${Sources}_cpp}")
				set_source_files_properties( ${${Sources}_cpp} PROPERTIES COMPILE_FLAGS "/Yu${TargetName}/PrecompiledHeader.h -D_CRT_SECURE_NO_WARNINGS")
				set_source_files_properties( ${${Sources}_c} PROPERTIES COMPILE_FLAGS "-D_CRT_SECURE_NO_WARNINGS")
				set_source_files_properties( source/${TargetName}/PrecompiledHeader.cpp PROPERTIES COMPILE_FLAGS "/Yc${TargetName}/PrecompiledHeader.h -D_CRT_SECURE_NO_WARNINGS")
			else (NOT IsPlugin)
				set_target_properties( ${TargetName} PROPERTIES COMPILE_FLAGS "-D_CRT_SECURE_NO_WARNINGS")
			endif (NOT IsPlugin)

		endif (MSVC)
	else (WIN32)
		set_source_files_properties( ${${Sources}_cpp} PROPERTIES COMPILE_FLAGS "-D_HAS_0X -std=c++0x")
		set_source_files_properties( ${${Sources}_h} PROPERTIES COMPILE_FLAGS "-D_HAS_0X -std=c++0x")
	endif (WIN32)
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

if (WIN32)
	if (MSVC)
		set( EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/msvc")
		set( LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/msvc")
	elseif (BORLAND)
		set( EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/borland/${CMAKE_BUILD_TYPE}")
		set( LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/borland/${CMAKE_BUILD_TYPE}")
	else (MINGW)
		set( EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/mingw/${CMAKE_BUILD_TYPE}")
		set( LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/mingw/${CMAKE_BUILD_TYPE}")
	endif (MSVC)
else (WIN32)
	set( LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
	set( EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
endif (WIN32)

#message( STATUS "EXECUTABLE_OUTPUT_PATH : ${EXECUTABLE_OUTPUT_PATH}")
#message( STATUS "CMAKE_BUILD_TYPE : ${CMAKE_BUILD_TYPE}")

set( Boost_DEBUG ON)

set( Boost_USE_STATIC_LIBS ON)
find_package( Boost REQUIRED date_time filesystem system)
find_package( OpenGL)
find_package( wxWidgets)
find_package( Freetype)
find_package( FreeImage)
find_package( Cg)

if (WIN32)
	find_package( Direct3D9)
else (WIN32)
	find_package( GTK2)
endif (WIN32)

include_directories( include)
include_directories( ${Boost_INCLUDE_DIRS})
include_directories( ${wxWidgets_INCLUDE_DIRS})
include_directories( ${FREETYPE_INCLUDE_DIRS})
include_directories( ${Cg_INCLUDE_DIR})
include_directories( ${FreeImage_INCLUDE_DIR})

if (WIN32)
	include_directories( ${D3D9_INCLUDE_DIR})
else (WIN32)
	include_directories( ${GTK2_INCLUDE_DIRS})
endif (WIN32)

link_directories( "${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
link_directories( ${Boost_LIBRARY_DIRS})
link_directories( ${wxWidgets_LIBRARY_DIRS})
link_directories( ${Cg_LIBRARY_DIR})
link_directories( ${FreeImage_LIBRARY_DIR})

if (WIN32)
	link_directories( ${D3D9_LIBRARY_DIR})
endif (WIN32)

string( COMPARE EQUAL "${wxWidgets_USE_DEBUG}" "ON" IsWxDebug)

if (WIN32)
	if (IsWxDebug)
		set( wxWidgets_CXXFLAGS "-D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXDEBUG__")
	else (IsWxDebug)
		set( wxWidgets_CXXFLAGS "-D_FILE_OFFSET_BITS=64 -D_LARGE_FILES")
	endif (IsWxDebug)
else (WIN32)
	if (IsWxDebug)
		set( wxWidgets_CXXFLAGS "-D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXDEBUG__ -D__WXGTK__")
	else (IsWxDebug)
		set( wxWidgets_CXXFLAGS "-D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__")
	endif (IsWxDebug)
endif (WIN32)

add_target( CastorUtils 		castor_utils			lib)
add_target( Castor3D 			castor_3d			dll)
add_target( LoopDivider			loop_divider			plugin)
add_target( AseImporter			ase_importer			plugin)
add_target( BspImporter			bsp_importer			plugin)
add_target( GsdImporter			gsd_importer			plugin)
add_target( Md2Importer			md2_importer			plugin)
add_target( Md3Importer			md3_importer			plugin)
add_target( ObjImporter			obj_importer			plugin)
add_target( PlyImporter			ply_importer			plugin)
add_target( SMaxImporter		smax_importer			plugin)
add_target( GuiCommon 			gui_common			lib)
add_target( CastorViewer 		castor_viewer			exe)
add_target( CastorArchitect		castor_architect		exe)
add_target( CastorShape 		castor_shape			exe)
add_target( ImgToXpm			img_to_xpm			exe)
add_target( OpenGlCommon 		opengl_common			lib)
add_target( Gl2RenderSystem		gl2_render_system		gldll)
add_target( Gl3RenderSystem		gl3_render_system		gldll)
if (WIN32)
	add_target( Dx9RenderSystem		dx9_render_system		dx9dll)
endif (WIN32)

if (WIN32)
	set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}  /subsystem:windows")
endif (WIN32)

install(
	TARGETS
		CastorUtils Castor3D OpenGlCommon Gl2RenderSystem Gl3RenderSystem LoopDivider AseImporter BspImporter GsdImporter Md2Importer Md3Importer ObjImporter PlyImporter SMaxImporter GuiCommon CastorViewer CastorArchitect CastorShape
	DESTINATION
		"${EXECUTABLE_OUTPUT_PATH}")
