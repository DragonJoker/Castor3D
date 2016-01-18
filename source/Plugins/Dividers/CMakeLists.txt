option( CASTOR_BUILDGRP_DIVIDER "Build Castor3D Divider plugins" TRUE )
option( CASTOR_BUILD_DIVIDER_LOOP "Build Loop Divider plugin" TRUE )
option( CASTOR_BUILD_DIVIDER_PNTRIANGLES "Build PN-Triangles Divider plugin" TRUE )
option( CASTOR_BUILD_DIVIDER_PHONG "Build Phong Divider plugin" TRUE )

function( DividersInit )
	set( PNTri "no (Not wanted)" PARENT_SCOPE )
	set( Loop "no (Not wanted)" PARENT_SCOPE )
	set( Phong "no (Not wanted)" PARENT_SCOPE )
endfunction( DividersInit )

function( DividersBuild )
	if ( ${CASTOR_BUILDGRP_DIVIDER} )
		if( ${CASTOR_BUILD_DIVIDER_PNTRIANGLES} )
			set( Build ${PNTri} )
			add_subdirectory( Plugins/Dividers/PnTrianglesDivider )
			set( PNTri ${Build} PARENT_SCOPE )
		endif()
		
		if( ${CASTOR_BUILD_DIVIDER_LOOP} )
			set( Build ${Loop} )
			add_subdirectory( Plugins/Dividers/LoopDivider )
			set( Loop ${Build} PARENT_SCOPE )
		endif()
		
		if( ${CASTOR_BUILD_DIVIDER_PHONG} )
			set( Build ${Phong} )
			add_subdirectory( Plugins/Dividers/PhongDivider )
			set( Phong ${Build} PARENT_SCOPE )
		endif()

		set( CastorMinLibraries
			${CastorMinLibraries}
			PARENT_SCOPE
		)

		set( CastorBinsDependencies
			${CastorBinsDependencies}
			PARENT_SCOPE
		)
	endif ()
endfunction( DividersBuild )

function( DividersSummary msg )
	set( msg_tmp "\n\nSubdividers :" )
	set( msg_tmp "${msg_tmp}\n    PN-Triangles        ${PNTri}" )
	set( msg_tmp "${msg_tmp}\n    Loop                ${Loop}" )
	set( msg_tmp "${msg_tmp}\n    Phong               ${Phong}" )
	SET( msg "${msg}${msg_tmp}" PARENT_SCOPE )
endfunction( DividersSummary )

function( DividersSetup )
	if ( ${CASTOR_BUILDGRP_DIVIDER} )
		if( ${CASTOR_BUILD_DIVIDER_LOOP} )
			cpack_add_component( LoopDivider
				DISPLAY_NAME "LoopDivider"
				DESCRIPTION "Allows the mesh subdivision using Loop subdivision surfaces."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif()
		if( ${CASTOR_BUILD_DIVIDER_PNTRIANGLES} )
			cpack_add_component( PnTrianglesDivider
				DISPLAY_NAME "PnTrianglesDivider"
				DESCRIPTION "Allows the mesh subdivision using PN triangles algorithm."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif()
		if( ${CASTOR_BUILD_DIVIDER_PNTRIANGLES} )
			cpack_add_component( PhongDivider
				DISPLAY_NAME "PhongDivider"
				DESCRIPTION "Allows the mesh subdivision using Phong tessellation."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif()
	endif ()
endfunction( DividersSetup )
