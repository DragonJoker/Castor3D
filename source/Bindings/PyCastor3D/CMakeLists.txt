project( PyCastor3D )

find_package( PythonLibs )

set( PYC_VERSION_MAJOR 0 )
set( PYC_VERSION_MINOR 7 )
set( PYC_VERSION_BUILD 0 )

if ( PYTHONLIBS_FOUND )
	find_package( Boost OPTIONAL_COMPONENTS python )

	if ( Boost_PYTHON_FOUND )
		include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D )
		include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils )
        include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )
		include_directories( ${Boost_INCLUDE_DIR} )
		include_directories( ${PYTHON_INCLUDE_DIR} )
		
		link_directories( ${Boost_LIBRARY_DIR} )
		
		#Python Libs
		set( PythonLibraries "" )
		FOREACH( Lib ${Boost_PYTHON_LIBRARY} )
			if ( PythonLibraries )
				set( PythonLibraries "${PythonLibraries}|${Lib}" )
			else ()
				set( PythonLibraries "${Lib}" )
			endif ()
		ENDFOREACH()
		FOREACH( Lib ${PYTHON_LIBRARY} )
			if ( PythonLibraries )
				set( PythonLibraries "${PythonLibraries}|${Lib}" )
			else ()
				set( PythonLibraries "${Lib}" )
			endif ()
		ENDFOREACH()

		add_target( 
			PyCastor3D
			dll
			"CastorUtils;Castor3D"
			"CastorUtils;Castor3D;${PythonLibraries}"
			"PyCastor3DPch.hpp"
			"PyCastor3DPch.cpp"
		)

		set_property( TARGET PyCastor3D PROPERTY FOLDER "Bindings" )
		add_target_astyle( PyCastor3D ".h;.hpp;.inl;.cpp" )
		set( Build "yes (version ${PYC_VERSION_MAJOR}.${PYC_VERSION_MINOR}.${PYC_VERSION_BUILD})" PARENT_SCOPE )
	else ()
		set( Build "no (missing Boost.Python library" PARENT_SCOPE )
	endif ()
else ()
	set( Build "no (missing Python library)" PARENT_SCOPE )
endif ()