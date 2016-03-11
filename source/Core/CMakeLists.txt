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
	cpack_add_component_group( Core
		EXPANDED
		DISPLAY_NAME "Core libraries"
		DESCRIPTION "Base libraries needed to use Castor3D."
	)
	cpack_add_component_group( Development
		DISPLAY_NAME "Development"
		DESCRIPTION "All the tools needed to develop a project using Castor3D."
	)
	cpack_add_component_group( Documentation
		DISPLAY_NAME "Documentation"
		DESCRIPTION "Projects documentations"
	)
	cpack_add_component_group( Samples
		DISPLAY_NAME "Samples"
		DESCRIPTION "Sample applications and scenes using Castor3D."
	)

	cpack_add_component( CastorUtils
		DISPLAY_NAME "CastorUtils library"
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
	cpack_add_component( CastorUtilsTest
		DISPLAY_NAME "CastorUtilsTest"
		DESCRIPTION "Test application for CastorUtils."
		REQUIRED
		GROUP Test
		INSTALL_TYPES Minimal Full
	)
	cpack_add_component( CastorUtils_doc
		DISPLAY_NAME "CastorUtils help"
		DESCRIPTION "CastorUtils HTML documentation."
		GROUP Documentation
		INSTALL_TYPES Developer Full
	)

	if ( ${CASTOR_BUILD_CASTOR3D} )
		cpack_add_component( GlslWriter
			DISPLAY_NAME "GlslWriter"
			DESCRIPTION "Enables GLSL code writing from C++."
			REQUIRED
			GROUP Plugins
			DEPENDS Castor3D
			INSTALL_TYPES Developer Full
		)
		cpack_add_component( GlslWriter_dev
			DISPLAY_NAME "GlslWriter SDK"
			DESCRIPTION "Development files for GlslWriter."
			GROUP Development
			DEPENDS Castor3D CastorUtils_dev Castor3D Castor3D_dev
			INSTALL_TYPES Developer Full
		)

		cpack_add_component( Castor3D
			DISPLAY_NAME "Castor3D Engine"
			DESCRIPTION "The main Castor3D shared library, including the necessary files to be able to build projects using Castor3D."
			REQUIRED
			GROUP Core
			DEPENDS CastorUtils
			INSTALL_TYPES Minimal Full
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
			DISPLAY_NAME "Sample scenes"
			DESCRIPTION "Sample scenes and data used to demonstrate the usage of Castor3D."
			GROUP Samples
			DEPENDS Castor3D
			INSTALL_TYPES Full
		)
	endif ()
endfunction( CoreSetup )
