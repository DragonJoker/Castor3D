option( CASTOR_BUILD_CASTOR3D "Build Castor3D" TRUE )

function( CoreInit )
	set( Cutils "no (Not wanted)" PARENT_SCOPE )
	set( GlslW "no (Not wanted)" PARENT_SCOPE )
	set( C3D "no (Not wanted)" PARENT_SCOPE )
endfunction( CoreInit )

function( CoreBuild )
	set( Error FALSE )
	set( Build ${Cutils} )
	add_subdirectory( Core/CastorUtils )
	set( Cutils ${Build} PARENT_SCOPE )
	
	if ( Error )
		message( SEND_ERROR "CastorUtils : ${Build}" )
	endif ()

	set( CastorMinLibraries
		${CastorMinLibraries}
		PARENT_SCOPE
	)

	if ( ${CASTOR_BUILD_CASTOR3D} )
		set( Build ${GlslW} )
		add_subdirectory( Core/GlslWriter )
		set( GlslW ${Build} PARENT_SCOPE )

		set( Build ${C3D} )
		add_subdirectory( Core/Castor3D )
		set( C3D ${Build} PARENT_SCOPE )
	endif ()

	if( ${CASTOR_BUILDGRP_TEST} )
		add_subdirectory( Core/CastorTest )
	endif()

	set( CastorMinLibraries
		${CastorMinLibraries}
		PARENT_SCOPE
	)

	set( CastorBinsDependencies
		${CastorBinsDependencies}
		PARENT_SCOPE
	)
endfunction( CoreBuild )

function( CoreSummary msg )
	set( msg_tmp "\nCore :" )
	set( msg_tmp "${msg_tmp}\n    CastorUtils         ${Cutils}" )
	set( msg_tmp "${msg_tmp}\n    GlslWriter          ${GlslW}" )
	set( msg_tmp "${msg_tmp}\n    Castor3D            ${C3D}" )
	SET( msg "${msg}${msg_tmp}" PARENT_SCOPE )
endfunction( CoreSummary )

function( CoreSetup )
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
	cpack_add_component( CastorUtils_doc
		DISPLAY_NAME "CastorUtils help"
		DESCRIPTION "CastorUtils HTML documentation."
		GROUP Documentation
		INSTALL_TYPES Developer Full
	)

	if ( ${CASTOR_BUILDGRP_TEST} )
		cpack_add_component( CastorTest
			DISPLAY_NAME "Castor test library"
			DESCRIPTION "Test library for tests."
			GROUP Test
			INSTALL_TYPES Minimal Full
		)
		cpack_add_component( CastorTest_dev
			DISPLAY_NAME "Castor test SDK"
			DESCRIPTION "Development files for CastorTest."
			GROUP Test
			DEPENDS CastorTest
			INSTALL_TYPES Minimal Full
		)
		cpack_add_component( CastorUtilsTest
			DISPLAY_NAME "CastorUtils test"
			DESCRIPTION "Test application for CastorUtils."
			GROUP Test
			DEPENDS CastorTest
			INSTALL_TYPES Minimal Full
		)
		cpack_add_component( Castor3DTest
			DISPLAY_NAME "Castor3D test"
			DESCRIPTION "Test application for Castor3D."
			GROUP Test
			DEPENDS CastorTest
			INSTALL_TYPES Minimal Full
		)
	endif ()

	if ( ${CASTOR_BUILD_CASTOR3D} )
		cpack_add_component( GlslWriter
			DISPLAY_NAME "GlslWriter"
			DESCRIPTION "Enables GLSL code writing from C++."
			REQUIRED
			GROUP Core
			DEPENDS Castor3D
			INSTALL_TYPES Minimal Developer Full
		)
		cpack_add_component( GlslWriter_dev
			DISPLAY_NAME "GlslWriter SDK"
			DESCRIPTION "Development files for GlslWriter."
			GROUP Development
			DEPENDS Castor3D CastorUtils_dev Castor3D Castor3D_dev
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
			DEPENDS CastorUtils CastorUtils_dev Castor3D Castor3D_dev
			INSTALL_TYPES Developer Full
		)
		cpack_add_component( Castor3D_doc
			DISPLAY_NAME "Castor3D help"
			DESCRIPTION "Castor3D HTML documentation."
			GROUP Documentation
			INSTALL_TYPES Developer Full
		)
		cpack_add_component( Castor3D_Samples
			DISPLAY_NAME "Castor3D scenes"
			DESCRIPTION "Sample scenes and data used to demonstrate the usage of Castor3D."
			GROUP Samples
			DEPENDS Castor3D
			INSTALL_TYPES Full
		)
	endif ()
endfunction( CoreSetup )
