option( CASTOR_BUILD_TEST_CASTORUTILS "Build CastorUtils test application" TRUE )
option( CASTOR_BUILD_TEST_CASTOR3D "Build Castor3D test application" TRUE )

function( CoreInit )
	set( Cutils "no (Not wanted)" PARENT_SCOPE )
	set( CUtlT "no (Not wanted)" PARENT_SCOPE )
	set( C3D "no (Not wanted)" PARENT_SCOPE )
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
	endif ()

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
		cpack_add_component( ShaderWriter_dev
			DISPLAY_NAME "ShaderWriter SDK"
			DESCRIPTION "Development files for ShaderWriter."
			GROUP Development
			DEPENDS ShaderWriter
			INSTALL_TYPES Developer Full
		)

		cpack_add_component( Ashes
			DISPLAY_NAME "Ashes"
			DESCRIPTION "Base renderer API."
			REQUIRED
			GROUP Core
			INSTALL_TYPES Minimal Developer Full
		)
		cpack_add_component( Ashes_dev
			DISPLAY_NAME "Ashes SDK"
			DESCRIPTION "Development files for Ashes."
			GROUP Development
			DEPENDS Ashes
			INSTALL_TYPES Developer Full
		)
		cpack_add_component( Ashes_French_Doc
			DISPLAY_NAME "Ashes French help"
			DESCRIPTION "Ashes French HTML documentation."
			GROUP Documentation
			INSTALL_TYPES Developer Full
		)
		cpack_add_component( Ashes_English_Doc
			DISPLAY_NAME "Ashes English help"
			DESCRIPTION "Ashes English HTML documentation."
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
			DEPENDS CastorUtils CastorUtils_dev Castor3D
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
	endif ()
endfunction( CoreSetup )
