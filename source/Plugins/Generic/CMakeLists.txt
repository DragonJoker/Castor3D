option( CASTOR_BUILDGRP_GENERIC "Build Castor3D Generic plugins" TRUE )
option( CASTOR_BUILD_GENERIC_CASTORGUI "Build CastorGui generic plugin" TRUE )
option( CASTOR_BUILD_GENERIC_TOONMATERIAL "Build Toon Material generic plugin" TRUE )
option( CASTOR_BUILD_GENERIC_WATERRENDERING "Build water rendering plugin" TRUE )

function( GenericInit )
	set( Cgui "no (Not wanted)" PARENT_SCOPE )
	set( Toon "no (Not wanted)" PARENT_SCOPE )
	set( Watr "no (Not wanted)" PARENT_SCOPE )

	if( ${CASTOR_BUILD_GENERIC_TOONMATERIAL} )
		set( Castor3D_MATERIAL_DIRS
			${Castor3D_MATERIAL_DIRS}
			${CASTOR_SOURCE_DIR}/source/Plugins/Generic/ToonMaterial
		)
		set( Castor3D_MATERIAL_DEPS
			${Castor3D_MATERIAL_DEPS}
			castor::ToonMaterial
		)
	endif()

	set( Castor3D_MATERIAL_DIRS
		${Castor3D_MATERIAL_DIRS}
		PARENT_SCOPE
	)
	set( Castor3D_MATERIAL_DEPS
		${Castor3D_MATERIAL_DEPS}
		PARENT_SCOPE
	)
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

		if( ${CASTOR_BUILD_GENERIC_TOONMATERIAL} )
			set( Build ${Toon} )
			add_subdirectory( Plugins/Generic/ToonMaterial )
			set( Toon ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_GENERIC_WATERRENDERING} )
			set( Build ${Watr} )
			add_subdirectory( Plugins/Generic/WaterRendering )
			set( Watr ${Build} PARENT_SCOPE )
		endif()

		set( CastorMinLibraries
			${CastorMinLibraries}
			PARENT_SCOPE
		)
		set( Castor3DBinsDependencies
			${Castor3DBinsDependencies}
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
	endif ()
endfunction( GenericBuild )

function( GenericSummary msg )
	if ( ${CASTOR_BUILDGRP_GENERIC} )
		set( msg_tmp "\n\n  Generic plugins:" )
		if( ${CASTOR_BUILD_GENERIC_CASTORGUI} )
			set( msg_tmp "${msg_tmp}\n    CastorGui            ${Cgui}" )
		endif ()
		if( ${CASTOR_BUILD_GENERIC_TOONMATERIAL} )
			set( msg_tmp "${msg_tmp}\n    ToonMaterial         ${Toon}" )
		endif ()
		if( ${CASTOR_BUILD_GENERIC_WATERRENDERING} )
			set( msg_tmp "${msg_tmp}\n    WaterRendering       ${Watr}" )
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( GenericSummary )

function( GenericSetup )
	if ( ${CASTOR_BUILDGRP_GENERIC} )
		if( ${CASTOR_BUILD_GENERIC_CASTORGUI} )
			cpack_add_component( CastorGui
				DISPLAY_NAME "CastorGui"
				DESCRIPTION "Allows GUI creation using Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
			cpack_add_component( CastorGui_dev
				DISPLAY_NAME "CastorGui SDK"
				DESCRIPTION "Development files for CastorGui."
				GROUP Development
				DEPENDS CastorGui Castor3D_dev
				INSTALL_TYPES Developer Full
			)
		endif()
		if( ${CASTOR_BUILD_GENERIC_TOONMATERIAL} )
			cpack_add_component( ToonMaterial
				DISPLAY_NAME "ToonMaterial"
				DESCRIPTION "Defines a toon material in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
		if( ${CASTOR_BUILD_GENERIC_WATERRENDERING} )
			cpack_add_component( WaterRendering
				DISPLAY_NAME "WaterRendering"
				DESCRIPTION "Allows water rendering in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
	endif ()
endfunction( GenericSetup )
