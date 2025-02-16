option( CASTOR_BUILDGRP_INTEROP "Build Castor3D binding plugins" ${C3D_BUILDGRP_INTEROP} )

if ( CASTOR_BUILDGRP_INTEROP )
	option( CASTOR_BUILD_INTEROP_COM "Build COM Castor3D binding" TRUE )
	option( CASTOR_BUILD_INTEROP_C "Build Castor3D C interface" TRUE )
	option( CASTOR_BUILD_TEST_INTEROP_COM "Build COM Castor3D binding test application" TRUE )
endif ()

function( InteropInit )
	set( CC3D "no (Not wanted)" PARENT_SCOPE )
	if ( MSVC )
		set( ComC3D "no (Not wanted)" PARENT_SCOPE )
		set( ComC3DT "no (Not wanted)" PARENT_SCOPE )
	else ()
		set( ComC3D "no (Not available)" PARENT_SCOPE )
		set( ComC3DT "no (Not available)" PARENT_SCOPE )
	endif ()
endfunction( InteropInit )

function( InteropBuild )
	if( ${CASTOR_BUILDGRP_INTEROP} )
		set( Castor3DIncludeDirs
			${Castor3DIncludeDirs}
			${CASTOR_BINARY_DIR}/include/Core
			${CASTOR_SOURCE_DIR}/include/Core
		)

		if( ${CASTOR_BUILD_INTEROP_C} )
			set( Build ${CC3D} )
			add_subdirectory( Interop/CCastor3D )
			set( CC3D ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_INTEROP_COM} )
			set( Build ${ComC3D} )
			add_subdirectory( Interop/ComCastor3D )
			set( ComC3D ${Build} PARENT_SCOPE )
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
endfunction( InteropBuild )

function( InteropSummary msg )
	if ( ${CASTOR_BUILDGRP_INTEROP} )
		set( msg_tmp "\n\n  Interoperability:" )
		if ( ${CASTOR_BUILD_INTEROP_C} )
			set( msg_tmp "${msg_tmp}\n    C                    ${CC3D}" )
		endif ()
		if ( ${CASTOR_BUILD_INTEROP_COM} )
			set( msg_tmp "${msg_tmp}\n    COM                  ${ComC3D}" )
			if( ${CASTOR_BUILDGRP_TEST} )
				set( msgtest_tmp "${msgtest_tmp}\n    COM Test             ${ComC3DT}" )
				set( msgtest "${msgtest}${msgtest_tmp}" PARENT_SCOPE )
			endif()
		endif ()

		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( InteropSummary )

function( InteropSetup )
	if ( ${CASTOR_BUILDGRP_INTEROP} )
		cpack_add_component_group( Interoperability
			DESCRIPTION "Bindings to use Castor3D in other languages (C# and Python yet)"
		)

		if( ${CASTOR_BUILD_INTEROP_C} )
			cpack_add_component( CCastor3D
				DISPLAY_NAME "Castor3DC library"
				DESCRIPTION "C interface for Castor3D"
				GROUP Interoperability
				INSTALL_TYPES Minimal Full
			)
		endif()

		if( ${CASTOR_BUILD_INTEROP_COM} )
			cpack_add_component( ComCastor3D
				DISPLAY_NAME "ComCastor3D library"
				DESCRIPTION "COM binding for Castor3D"
				GROUP Interoperability
				INSTALL_TYPES Minimal Full
			)
		endif()
	endif ()
endfunction( InteropSetup )
