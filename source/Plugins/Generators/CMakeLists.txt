option( CASTOR_BUILDGRP_GENERATOR "Build Castor3D Generator plugins" TRUE )
option( CASTOR_BUILD_GENERATOR_DIAMOND_SQUARE_TERRAIN "Build DiamondSquareTerrain generator plugin" TRUE )

function( GeneratorInit )
	set( Dst "no (Not wanted)" PARENT_SCOPE )
endfunction( GeneratorInit )

function( GeneratorBuild )
	if ( ${CASTOR_BUILDGRP_GENERATOR} )
		set( GeneratorIncludes
			${CASTOR_SOURCE_DIR}/include/Core
			${CASTOR_BINARY_DIR}/include/Core
			${CASTOR_SOURCE_DIR}/include/Plugins/Generators
			${CASTOR_BINARY_DIR}/include/Plugins/Generators
			${CASTOR_SOURCE_DIR}/source/Plugins/Generators
			${CASTOR_BINARY_DIR}/source/Plugins/Generators
		)

		if( ${CASTOR_BUILD_GENERATOR_DIAMOND_SQUARE_TERRAIN} )
			set( Build ${Dst} )
			add_subdirectory( Plugins/Generators/DiamondSquareTerrain )
			set( Dst ${Build} PARENT_SCOPE )
		endif()

		set( CastorMinLibraries
			${CastorMinLibraries}
			PARENT_SCOPE
		)

		set( Castor3DBinsDependencies
			${Castor3DBinsDependencies}
			PARENT_SCOPE
		)
	endif ()
endfunction( GeneratorBuild )

function( GeneratorSummary msg )
	if ( ${CASTOR_BUILDGRP_GENERATOR} )
		set( msg_tmp "\n\nGenerator :" )
		if( ${CASTOR_BUILD_GENERATOR_DIAMOND_SQUARE_TERRAIN} )
			set( msg_tmp "${msg_tmp}\n    DiamondSquareTerrain ${Dst}" )
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( GeneratorSummary )

function( GeneratorSetup )
	if ( ${CASTOR_BUILDGRP_GENERATOR} )
		cpack_add_component_group( Generators
			DISPLAY_NAME "Generators"
			DESCRIPTION "Generator plugins usable in Castor3D."
			PARENT_GROUP Plugins
		)
		if( ${CASTOR_BUILD_GENERATOR_DIAMOND_SQUARE_TERRAIN} )
			cpack_add_component( DiamondSquareTerrain
				DISPLAY_NAME "DiamondSquareTerrain"
				DESCRIPTION "Allows terrain generation using diamond-square algorithm in Castor3D."
				GROUP Generators
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
	endif ()
endfunction( GeneratorSetup )
