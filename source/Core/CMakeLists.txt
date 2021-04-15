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
	set( CastorBinsDependencies
		${CastorBinsDependencies}
		PARENT_SCOPE
	)
endfunction( CoreBuild )

function( CoreSummary msg )
	set( msg_tmp "\n  Core libraries:" )
	set( msg_tmp "${msg_tmp}\n    CastorUtils          ${Cutils}" )
	set( msg_tmp "${msg_tmp}\n    Castor3D             ${C3D}" )
	set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
endfunction( CoreSummary )

function( CoreSetup )
	if ( WIN32 )
		set( CMAKE_INSTALL_DEBUG_LIBRARIES TRUE )
		set( CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION bin/$<$<CONFIG:Debug>:Debug/> )
		include( InstallRequiredSystemLibraries )
		file( GET_RUNTIME_DEPENDENCIES
			LIBRARIES ${ADDITIONAL_DEPS_REL}
			RESOLVED_DEPENDENCIES_VAR r_deps
			UNRESOLVED_DEPENDENCIES_VAR u_deps
		)
		install(
			FILES ${r_deps} ${ADDITIONAL_DEPS_REL}
			DESTINATION bin
			COMPONENT Castor3D
			CONFIGURATIONS Release
		)
	endif ()

	cpack_add_component( CastorUtils
		DISPLAY_NAME "CastorUtils"
		DESCRIPTION "Base utilities library used in Castor3D."
		REQUIRED
		GROUP Core
		INSTALL_TYPES Minimal Full
	)
	cpack_add_component( CastorUtils_dev
		DISPLAY_NAME "CastorUtils SDK"
		DESCRIPTION "Development files for CastorUtils."
		GROUP Development
		DEPENDS CastorUtils
		INSTALL_TYPES Developer Full
	)
	cpack_add_component( CastorUtils_French_Doc
		DISPLAY_NAME "CastorUtils French help"
		DESCRIPTION "CastorUtils French HTML documentation."
		GROUP Documentation
		INSTALL_TYPES Developer Full
	)
	cpack_add_component( CastorUtils_English_Doc
		DISPLAY_NAME "CastorUtils English help"
		DESCRIPTION "CastorUtils English HTML documentation."
		GROUP Documentation
		INSTALL_TYPES Developer Full
	)

	if ( ${CASTOR_BUILDGRP_TEST} )
		cpack_add_component( CastorTest
			DISPLAY_NAME "Castor test library"
			DESCRIPTION "Test library for tests."
			GROUP Test
			INSTALL_TYPES Developer Full
		)
		cpack_add_component( CastorTest_dev
			DISPLAY_NAME "Castor test SDK"
			DESCRIPTION "Development files for CastorTest."
			GROUP Test
			DEPENDS CastorTest
			INSTALL_TYPES Developer Full
		)

		if ( ${CASTOR_BUILD_TEST_CASTORUTILS} )
			cpack_add_component( CastorUtilsTest
				DISPLAY_NAME "CastorUtils test"
				DESCRIPTION "Test application for CastorUtils."
				GROUP Test
				DEPENDS CastorTest
				INSTALL_TYPES Developer Full
			)
		endif ()

		if ( ${CASTOR_BUILD_TEST_CASTOR3D} )
			cpack_add_component( Castor3DTest
				DISPLAY_NAME "Castor3D test"
				DESCRIPTION "Test application for Castor3D."
				GROUP Test
				DEPENDS CastorTest
				INSTALL_TYPES Developer Full
			)
		endif ()
	endif ()

	if ( ${CASTOR_BUILD_CASTOR3D} )
		cpack_add_component( ShaderWriter
			DISPLAY_NAME "ShaderWriter"
			DESCRIPTION "Enables shaders code writing from C++."
			REQUIRED
			GROUP Core
			INSTALL_TYPES Minimal Developer Full
		)
		cpack_add_component( SdwCompilers
			DISPLAY_NAME "ShaderWriter Compilers"
			DESCRIPTION "Compilers for ShaderWriter."
			REQUIRED
			GROUP Core
			INSTALL_TYPES Minimal Developer Full
		)
		cpack_add_component( ShaderWriter_dev
			DISPLAY_NAME "ShaderWriter SDK"
			DESCRIPTION "Development files for ShaderWriter."
			GROUP Development
			DEPENDS ShaderWriter
			INSTALL_TYPES Developer Full
		)
		cpack_add_component( SdwCompilers_dev
			DISPLAY_NAME "ShaderWriter Compilers SDK"
			DESCRIPTION "Development files for ShaderWriter Compilers."
			GROUP Development
			DEPENDS ShaderWriter
			INSTALL_TYPES Developer Full
		)

		cpack_add_component( Ashes
			DISPLAY_NAME "Ashes"
			DESCRIPTION "Ashes libraries."
			REQUIRED
			GROUP Core
			INSTALL_TYPES Minimal Developer Full
		)
		cpack_add_component( AshesRenderers
			DISPLAY_NAME "AshesRenderers"
			DESCRIPTION "Ashes renderer plugins."
			REQUIRED
			GROUP Core
			INSTALL_TYPES Minimal Developer Full
		)
		cpack_add_component( ashes_common_dev
			DISPLAY_NAME "AshesCommon SDK"
			DESCRIPTION "Development files for AshesCommon."
			GROUP Development
			DEPENDS Ashes
			INSTALL_TYPES Developer Full
		)
		cpack_add_component( ashes_dev
			DISPLAY_NAME "Ashes SDK"
			DESCRIPTION "Development files for Ashes."
			GROUP Development
			DEPENDS ashes_common_dev
			INSTALL_TYPES Developer Full
		)
		cpack_add_component( ashespp_dev
			DISPLAY_NAME "AshesPP SDK"
			DESCRIPTION "Development files for AshesPP."
			GROUP Development
			DEPENDS ashes_dev
			INSTALL_TYPES Developer Full
		)
		cpack_add_component( Ashes_Doc
			DISPLAY_NAME "Ashes help"
			DESCRIPTION "Ashes HTML documentation."
			GROUP Documentation
			INSTALL_TYPES Developer Full
		)

		cpack_add_component( Castor3D
			DISPLAY_NAME "Castor3D"
			DESCRIPTION "The main Castor3D shared library, including the necessary files to be able to build projects using Castor3D."
			REQUIRED
			GROUP Core
			DEPENDS CastorUtils
			INSTALL_TYPES Minimal Developer Full
		)
		cpack_add_component( Castor3D_dev
			DISPLAY_NAME "Castor3D SDK"
			DESCRIPTION "Development files for Castor3D."
			GROUP Development
			DEPENDS CastorUtils_dev Castor3D
			INSTALL_TYPES Developer Full
		)
		cpack_add_component( Castor3D_French_Doc
			DISPLAY_NAME "Castor3D French help"
			DESCRIPTION "Castor3D French HTML documentation."
			GROUP Documentation
			INSTALL_TYPES Developer Full
		)
		cpack_add_component( Castor3D_English_Doc
			DISPLAY_NAME "Castor3D English help"
			DESCRIPTION "Castor3D English HTML documentation."
			GROUP Documentation
			INSTALL_TYPES Developer Full
		)

		cpack_add_component( SceneExporter
			DISPLAY_NAME "SceneExporter"
			DESCRIPTION "Library used to export Castor3D scenes."
			REQUIRED
			GROUP Core
			DEPENDS Castor3D
			INSTALL_TYPES Minimal Developer Full
		)
		cpack_add_component( SceneExporter_dev
			DISPLAY_NAME "SceneExporter SDK"
			DESCRIPTION "Development files for SceneExporter."
			GROUP Development
			DEPENDS Castor3D_dev SceneExporter
			INSTALL_TYPES Developer Full
		)
	endif ()
endfunction( CoreSetup )
