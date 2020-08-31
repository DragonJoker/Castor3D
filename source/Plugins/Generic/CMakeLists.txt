option( CASTOR_BUILDGRP_GENERIC "Build Castor3D Generic plugins" TRUE )
option( CASTOR_BUILD_GENERIC_CASTORGUI "Build CastorGui generic plugin" TRUE )

function( GenericInit )
	set( Cgui "no (Not wanted)" PARENT_SCOPE )
endfunction( GenericInit )

function( GenericBuild )
	if ( ${CASTOR_BUILDGRP_GENERIC} )
		set( GenericIncludes
			${CASTOR_SOURCE_DIR}/include/Core
			${CASTOR_BINARY_DIR}/include/Core
			${CASTOR_SOURCE_DIR}/source/Plugins/Generic
			${CASTOR_BINARY_DIR}/source/Plugins/Generic
		)

		if( ${CASTOR_BUILD_GENERIC_CASTORGUI} )
			set( Build ${Cgui} )
			add_subdirectory( Plugins/Generic/CastorGui )
			set( Cgui ${Build} PARENT_SCOPE )
		endif()

		set( CastorMinLibraries
			${CastorMinLibraries}
			PARENT_SCOPE
		)
		set( Castor3DBinsDependencies
			${Castor3DBinsDependencies}
			PARENT_SCOPE
		)
		set( CASTOR_INSTEXP_LIST
			${CASTOR_INSTEXP_LIST}
			PARENT_SCOPE
		)
	endif ()
endfunction( GenericBuild )

function( GenericSummary msg )
	if ( ${CASTOR_BUILDGRP_GENERIC} )
		set( msg_tmp "\n\n  Generic plugins:" )
		if( ${CASTOR_BUILD_GENERIC_CASTORGUI} )
			set( msg_tmp "${msg_tmp}\n    CastorGui            ${Cgui}" )
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( GenericSummary )

function( GenericSetup )
	if ( ${CASTOR_BUILDGRP_GENERIC} )
		cpack_add_component_group( Generic
			DISPLAY_NAME "Generic"
			DESCRIPTION "Generic plugins usable in Castor3D."
			PARENT_GROUP Plugins
		)
		if( ${CASTOR_BUILD_GENERIC_CASTORGUI} )
			cpack_add_component( CastorGui
				DISPLAY_NAME "CastorGui"
				DESCRIPTION "Allows GUI creation using Castor3D."
				GROUP Generic
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
			cpack_add_component( CastorGui_dev
				DISPLAY_NAME "CastorGui SDK"
				DESCRIPTION "Development files for CastorGui."
				GROUP Development
				DEPENDS Castor3D_dev
				INSTALL_TYPES Developer Full
			)
		endif()
	endif ()
endfunction( GenericSetup )
