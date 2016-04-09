option( CASTOR_BUILDGRP_INTEROP "Build Castor3D binding plugins" FALSE )
option( CASTOR_BUILD_INTEROP_PYCASTOR3D "Build Python Castor3D binding" TRUE )
option( CASTOR_BUILD_INTEROP_PYGUICOMMON "Build Python GuiCommon binding" TRUE )
option( CASTOR_BUILD_INTEROP_COM "Build COM Castor3D binding" TRUE )

function( InteropInit )
	set( PyC3D "no (Not wanted)" PARENT_SCOPE )
	set( PyGui "no (Not wanted)" PARENT_SCOPE )
	if ( MSVC )
		set( ComC3D "no (Not wanted)" PARENT_SCOPE )
	else ()
		set( ComC3D "no (Not available)" PARENT_SCOPE )
	endif ()
endfunction( InteropInit )

function( InteropBuild )
	if( ${CASTOR_BUILDGRP_INTEROP} )
		if ( ${CASTOR_BUILD_INTEROP_PYCASTOR3D} OR ${CASTOR_BUILD_INTEROP_PYGUICOMMON} )
			find_package( PythonLibs )

			if ( PYTHONLIBS_FOUND )
			#set( Boost_USE_STATIC_LIBS ON )
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

					if( ${CASTOR_BUILD_INTEROP_PYCASTOR3D} )
						set( Build ${PyC3D} )
					add_subdirectory( Interop/PyCastor3D )
						set( PyC3D ${Build} PARENT_SCOPE )
					endif()

					if( ${CASTOR_BUILD_INTEROP_PYGUICOMMON} )
						set( Build ${PyGui} )
					add_subdirectory( Interop/PyGuiCommon )
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

		if( ${CASTOR_BUILD_INTEROP_COM} )
			set( Build ${ComC3D} )
		add_subdirectory( Interop/ComCastor3D )
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
endfunction( InteropBuild )

function( InteropSummary msg )
	if ( ${CASTOR_BUILDGRP_INTEROP} )
		set( msg_tmp "\n\nInteroperability :" )
		if ( ${CASTOR_BUILD_INTEROP_PYCASTOR3D} )
			set( msg_tmp "${msg_tmp}\n    PyCastor3D          ${PyC3D}" )
		endif ()
		if ( ${CASTOR_BUILD_INTEROP_PYGUICOMMON} )
			set( msg_tmp "${msg_tmp}\n    PyGuiCommon         ${PyGui}" )
		endif ()
		if ( ${CASTOR_BUILD_INTEROP_COM} )
			set( msg_tmp "${msg_tmp}\n    COM                 ${ComC3D}" )
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( InteropSummary )

function( InteropSetup )
if ( ${CASTOR_BUILDGRP_INTEROP} )
	cpack_add_component_group( Interoperability
			DESCRIPTION "Bindings to use Castor3D in other languages (C# and Python yet)"
		)

		if( ${CASTOR_BUILD_INTEROP_PYCASTOR3D} )
			cpack_add_component( PyCastor3D
				DISPLAY_NAME "PyCastor3D library"
				DESCRIPTION "Python binding for Castor3D"
			GROUP Interoperability
				INSTALL_TYPES Full
			)
		endif()

		if( ${CASTOR_BUILD_INTEROP_PYGUICOMMON} )
			cpack_add_component( PyGuiCommon
				DISPLAY_NAME "PyGuiCommon library"
				DESCRIPTION "Python binding for GuiCommon"
			GROUP Interoperability
				INSTALL_TYPES Full
			)
		endif()

		if( ${CASTOR_BUILD_INTEROP_COM} )
			cpack_add_component( ComCastor3D
				DISPLAY_NAME "ComCastor3D library"
				DESCRIPTION "COM binding for Castor3D"
			GROUP Interoperability
				INSTALL_TYPES Full
			)
		endif()
	endif ()
endfunction( InteropSetup )
