option( CASTOR_BUILD_TEST_CASTORUTILS "Build CastorUtils test application" TRUE )
option( CASTOR_BUILD_TEST_CASTOR3D "Build Castor3D test application" TRUE )

function( CoreInit )
	set( Cutils "no (Not wanted)" PARENT_SCOPE )
	set( CUtlT "no (Not wanted)" PARENT_SCOPE )
	set( C3D "no (Not wanted)" PARENT_SCOPE )
	set( SEx "no (Not wanted)" PARENT_SCOPE )
	set( Rnd "no (Not wanted)" PARENT_SCOPE )
	set( C3DT "no (Not wanted)" PARENT_SCOPE )
	set( CT "no (Not wanted)" PARENT_SCOPE )
endfunction( CoreInit )

function( CoreBuild )
	set( Error FALSE )

	set( Build ${Cutils} )
	add_subdirectory( Core/CastorUtils )
	set( Cutils ${Build} PARENT_SCOPE )
	
	if ( Error )
		message( SEND_ERROR "CastorUtils : ${Build}" )
	endif ()

	if ( CASTOR_BUILD_CASTOR3D )
		set( Build ${C3D} )
		add_subdirectory( Core/Castor3D )
		set( C3D ${Build} PARENT_SCOPE )
		set( Build ${SEx} )
		add_subdirectory( Core/SceneExporter )
		set( SEx ${Build} PARENT_SCOPE )
	endif ()

	set( ADDITIONAL_DEPS_REL
		${ADDITIONAL_DEPS_REL}
		PARENT_SCOPE
	)
	set( ADDITIONAL_DEPS_DBG
		${ADDITIONAL_DEPS_DBG}
		PARENT_SCOPE
	)
	set( CastorBinsDependencies
		${CastorBinsDependencies}
		PARENT_SCOPE
	)
	set( Castor3D_MATERIAL_DIRS
		${Castor3D_MATERIAL_DIRS}
		PARENT_SCOPE
	)
	set( CASTOR_INSTEXP_LIST
		${CASTOR_INSTEXP_LIST}
		PARENT_SCOPE
	)
endfunction( CoreBuild )

function( CoreSummary msg )
	set( msg_tmp "\n  Core libraries:" )
	set( msg_tmp "${msg_tmp}\n    CastorUtils          ${Cutils}" )
	set( msg_tmp "${msg_tmp}\n    Castor3D             ${C3D}" )
	set( msg_tmp "${msg_tmp}\n    SceneExporter        ${SEx}" )
	set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
endfunction( CoreSummary )

function( CoreSetup )
	if ( WIN32 )
		set( CMAKE_INSTALL_DEBUG_LIBRARIES TRUE )
		set( CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION bin )
		unset( MSVC_REDIST_DIR CACHE )
		include( InstallRequiredSystemLibraries )
		get_filename_component( VSTOOLS_FOLDER ${CMAKE_LINKER} DIRECTORY )
		set( CMAKE_GET_RUNTIME_DEPENDENCIES_COMMAND "${VSTOOLS_FOLDER}/dumpbin.exe" )
		install(
			FILES ${ADDITIONAL_DEPS_REL}
			DESTINATION bin
			COMPONENT Castor3D
			CONFIGURATIONS Release RelWithDebInfo
		)
		install(
			FILES ${ADDITIONAL_DEPS_DBG}
			DESTINATION bin
			COMPONENT Castor3D
			CONFIGURATIONS Debug
		)
		set(GEN_SCRIPT [[
			file( GET_RUNTIME_DEPENDENCIES
				LIBRARIES @ADDITIONAL_DEPS@
				RESOLVED_DEPENDENCIES_VAR r_deps
				UNRESOLVED_DEPENDENCIES_VAR u_deps
			)
			foreach ( _file ${r_deps} )
				string( TOLOWER ${_file} _file_lower )
				if ( NOT ${_file_lower} MATCHES "c:[\\/]windows[\\/]system32.*" )
					file( INSTALL
						DESTINATION @CMAKE_INSTALL_PREFIX@/bin
						TYPE SHARED_LIBRARY
						FOLLOW_SYMLINK_CHAIN
						FILES "${_file}"
					)
				endif ()
			endforeach ()
		]] )
		set( ADDITIONAL_DEPS ${ADDITIONAL_DEPS_REL} )
		file( CONFIGURE OUTPUT ${CMAKE_BINARY_DIR}/GetLibs-Release.cmake CONTENT ${GEN_SCRIPT} @ONLY )
		set( ADDITIONAL_DEPS ${ADDITIONAL_DEPS_DBG} )
		file( CONFIGURE OUTPUT ${CMAKE_BINARY_DIR}/GetLibs-Debug.cmake CONTENT ${GEN_SCRIPT} @ONLY )
		install( SCRIPT ${CMAKE_BINARY_DIR}/GetLibs-Release.cmake
			COMPONENT Castor3D
			CONFIGURATIONS Release RelWithDebInfo
		)
		install( SCRIPT ${CMAKE_BINARY_DIR}/GetLibs-Debug.cmake
			COMPONENT Castor3D
			CONFIGURATIONS Debug
		)
	endif ()

	cpack_add_component( CastorUtils
		DISPLAY_NAME "CastorUtils"
		DESCRIPTION "Base utilities library used in Castor3D."
		REQUIRED
		GROUP Core
		INSTALL_TYPES Minimal Full
	)
	cpack_add_component( CastorUtils_French_Doc
		DISPLAY_NAME "CastorUtils French help"
		DESCRIPTION "CastorUtils French HTML documentation."
		GROUP Documentation
		INSTALL_TYPES Minimal Full
	)
	cpack_add_component( CastorUtils_English_Doc
		DISPLAY_NAME "CastorUtils English help"
		DESCRIPTION "CastorUtils English HTML documentation."
		GROUP Documentation
		INSTALL_TYPES Minimal Full
	)

	if ( ${CASTOR_BUILDGRP_TEST} )
		cpack_add_component( CastorTest
			DISPLAY_NAME "Castor test library"
			DESCRIPTION "Test library for tests."
			GROUP Test
			INSTALL_TYPES Minimal Full
		)

		if ( ${CASTOR_BUILD_TEST_CASTORUTILS} )
			cpack_add_component( CastorUtilsTest
				DISPLAY_NAME "CastorUtils test"
				DESCRIPTION "Test application for CastorUtils."
				GROUP Test
				DEPENDS CastorTest
				INSTALL_TYPES Minimal Full
			)
		endif ()

		if ( ${CASTOR_BUILD_TEST_CASTOR3D} )
			cpack_add_component( Castor3DTest
				DISPLAY_NAME "Castor3D test"
				DESCRIPTION "Test application for Castor3D."
				GROUP Test
				DEPENDS CastorTest
				INSTALL_TYPES Minimal Full
			)
		endif ()
	endif ()

	if ( ${CASTOR_BUILD_CASTOR3D} )
		cpack_add_component( ShaderWriter
			DISPLAY_NAME "ShaderWriter"
			DESCRIPTION "Enables shaders code writing from C++."
			GROUP Core
			INSTALL_TYPES Minimal Full
		)

		cpack_add_component( RenderGraph
			DISPLAY_NAME "RenderGraph"
			DESCRIPTION "RenderGraph libraries."
			GROUP Core
			INSTALL_TYPES Minimal Full
		)

		cpack_add_component( Ashes
			DISPLAY_NAME "Ashes"
			DESCRIPTION "Ashes libraries."
			GROUP Core
			INSTALL_TYPES Minimal Full
		)
		cpack_add_component( AshesRenderers
			DISPLAY_NAME "AshesRenderers"
			DESCRIPTION "Ashes renderer plugins."
			GROUP Core
			INSTALL_TYPES Minimal Full
		)
		cpack_add_component( Ashes_Doc
			DISPLAY_NAME "Ashes help"
			DESCRIPTION "Ashes HTML documentation."
			GROUP Documentation
			INSTALL_TYPES Minimal Full
		)

		cpack_add_component( meshoptimizer
			DISPLAY_NAME "meshoptimizer"
			GROUP Core
			INSTALL_TYPES Minimal Full
		)

		cpack_add_component( Castor3D
			DISPLAY_NAME "Castor3D"
			DESCRIPTION "The main Castor3D shared library, including the necessary files to be able to build projects using Castor3D."
			GROUP Core
			DEPENDS CastorUtils
			INSTALL_TYPES Minimal Full
		)
		cpack_add_component( Castor3D_French_Doc
			DISPLAY_NAME "Castor3D French help"
			DESCRIPTION "Castor3D French HTML documentation."
			GROUP Documentation
			INSTALL_TYPES Minimal Full
		)
		cpack_add_component( Castor3D_English_Doc
			DISPLAY_NAME "Castor3D English help"
			DESCRIPTION "Castor3D English HTML documentation."
			GROUP Documentation
			INSTALL_TYPES Minimal Full
		)

		cpack_add_component( SceneExporter
			DISPLAY_NAME "SceneExporter"
			DESCRIPTION "Library used to export Castor3D scenes."
			REQUIRED
			GROUP Core
			DEPENDS Castor3D
			INSTALL_TYPES Minimal Full
		)
	endif ()
endfunction( CoreSetup )
