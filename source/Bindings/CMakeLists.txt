option( CASTOR_BUILDGRP_BINDING "Build Castor3D binding plugins" TRUE )
option( CASTOR_BUILD_BINDING_PYCASTOR3D "Build Python Castor3D binding" TRUE )
option( CASTOR_BUILD_BINDING_PYGUICOMMON "Build Python GuiCommon binding" TRUE )
option( CASTOR_BUILD_BINDING_COM "Build COM Castor3D binding" TRUE )

function( BindingsInit )
	set( PyC3D "no (Not wanted)" PARENT_SCOPE )
	set( PyGui "no (Not wanted)" PARENT_SCOPE )
	if ( MSVC )
		set( ComC3D "no (Not wanted)" PARENT_SCOPE )
	else ()
		set( ComC3D "no (Not available)" PARENT_SCOPE )
	endif ()
endfunction( BindingsInit )

function( BindingsBuild )
	if( ${CASTOR_BUILDGRP_BINDING} )
		if ( ${CASTOR_BUILD_BINDING_PYCASTOR3D} OR ${CASTOR_BUILD_BINDING_PYGUICOMMON} )
			find_package( PythonLibs )

			if ( PYTHONLIBS_FOUND )
				set( Boost_USE_STATIC_LIBS ON )
				find_package( Boost OPTIONAL_COMPONENTS python )

				if ( Boost_PYTHON_FOUND )
					#Python Libs
					set( PythonLibraries "" )
					foreach( Lib ${Boost_PYTHON_LIBRARY} )
						if ( PythonLibraries )
							set( PythonLibraries "${PythonLibraries}|${Lib}" )
						else ()
							set( PythonLibraries "${Lib}" )
						endif ()
					endforeach()
					foreach( Lib ${PYTHON_LIBRARY} )
						if ( PythonLibraries )
							set( PythonLibraries "${PythonLibraries}|${Lib}" )
						else ()
							set( PythonLibraries "${Lib}" )
						endif ()
					endforeach()

					if( ${CASTOR_BUILD_BINDING_PYCASTOR3D} )
						set( Build ${PyC3D} )
						add_subdirectory( Bindings/PyCastor3D )
						set( PyC3D ${Build} PARENT_SCOPE )
					endif()

					if( ${CASTOR_BUILD_BINDING_PYGUICOMMON} )
						set( Build ${PyGui} )
						add_subdirectory( Bindings/PyGuiCommon )
						set( PyGui ${Build} PARENT_SCOPE )
					endif()
				else ()
					set( PyC3D "no (missing Boost.Python library)" PARENT_SCOPE )
					set( PyGui "no (missing Boost.Python library)" PARENT_SCOPE )
				endif ()
			else ()
				set( PyC3D "no (missing Python library)" PARENT_SCOPE )
				set( PyGui "no (missing Python library)" PARENT_SCOPE )
			endif ()
		endif ()

		if( ${CASTOR_BUILD_BINDING_COM} )
			set( Build ${ComC3D} )
			add_subdirectory( Bindings/ComCastor3D )
			set( ComC3D ${Build} PARENT_SCOPE )
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
endfunction( BindingsBuild )

function( BindingsSummary msg )
	set( msg_tmp "\n\nBindings :" )
	set( msg_tmp "${msg_tmp}\n    PyCastor3D          ${PyC3D}" )
	set( msg_tmp "${msg_tmp}\n    PyGuiCommon         ${PyGui}" )
	set( msg_tmp "${msg_tmp}\n    COM                 ${ComC3D}" )
	SET( msg "${msg}${msg_tmp}" PARENT_SCOPE )
endfunction( BindingsSummary )

function( BindingsSetup )
	if ( ${CASTOR_BUILDGRP_BINDING} )
		cpack_add_component_group( Bindings
			DESCRIPTION "Bindings to use Castor3D in other languages (C# and Python yet)"
		)

		if( ${CASTOR_BUILD_BINDING_PYCASTOR3D} )
			cpack_add_component( PyCastor3D
				DISPLAY_NAME "PyCastor3D library"
				DESCRIPTION "Python binding for Castor3D"
				GROUP Bindings
				INSTALL_TYPES Full
			)
		endif()

		if( ${CASTOR_BUILD_BINDING_PYGUICOMMON} )
			cpack_add_component( PyGuiCommon
				DISPLAY_NAME "PyGuiCommon library"
				DESCRIPTION "Python binding for GuiCommon"
				GROUP Bindings
				INSTALL_TYPES Full
			)
		endif()

		if( ${CASTOR_BUILD_BINDING_COM} )
			cpack_add_component( ComCastor3D
				DISPLAY_NAME "ComCastor3D library"
				DESCRIPTION "COM binding for Castor3D"
				GROUP Bindings
				INSTALL_TYPES Full
			)
		endif()
	endif ()
endfunction( BindingsSetup )
