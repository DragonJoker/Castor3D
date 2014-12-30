include( CompilerVersion )
include( Logging )

#--------------------------------------------------------------------------------------------------
#	Function :	compute_abi_name
# 	Function which computes the extended library name, with compiler version and debug flag
#--------------------------------------------------------------------------------------------------
function( compute_abi_name ABI_Name ABI_Name_Debug )
    if ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel" OR ${CMAKE_CXX_COMPILER} MATCHES "icl" OR ${CMAKE_CXX_COMPILER} MATCHES "icpc" )
        #Intel compiler
        if(WIN32)
            set( _ABI_Name "-iw")
        else()
            set( _ABI_Name "-il")
        endif()
        set( _ABI_Name_Debug "-d")
    elseif (MSVC12)
        #Visual Studio 2013
        set( _ABI_Name "-vc120")
        set( _ABI_Name_Debug "-d")
    elseif (MSVC11)
        #Visual Studio 2012
        set( _ABI_Name "-vc110")
        set( _ABI_Name_Debug "-d")
    elseif (MSVC10)
        #Visual Studio 2010
        set( _ABI_Name "-vc100")
        set( _ABI_Name_Debug "-d")
    elseif (MSVC90)
        #Visual Studio 2008
        set( _ABI_Name "-vc90")
        set( _ABI_Name_Debug "-d")
    elseif (MSVC80)
        #Visual Studio 2005
        set( _ABI_Name "-vc80")
        set( _ABI_Name_Debug "-d")
    elseif (MSVC71)
        #Visual Studio 2003
        set( _ABI_Name "-vc71")
        set( _ABI_Name_Debug "-d")
    elseif (MSVC70)
        #Visual Studio 7
        set( _ABI_Name "-vc7")
        set( _ABI_Name_Debug "-d")
    elseif (MSVC60)
        #Visual Studio 6
        set( _ABI_Name "-vc6")
        set( _ABI_Name_Debug "-d")
    elseif (BORLAND)
        #Borland compiler
        set( _ABI_Name "-bcb")
        set( _ABI_Name_Debug "-d")
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "SunPro")
        #Sun compiler
        set( _ABI_Name "-sw")
        set( _ABI_Name_Debug "-d")
    elseif (MINGW)
        #MinGW compiler
        DumpCompilerVersion( COMPILER_VERSION )
        set( _ABI_Name "-mgw${COMPILER_VERSION}")
        set( _ABI_Name_Debug "-d")
    elseif (UNIX)
        #Unix system
        if (CMAKE_COMPILER_IS_GNUCXX)
            #GNU C/C++ compiler (can be gcc, g++, clang, ...)
            DumpCompilerVersion( COMPILER_VERSION )
            if(APPLE)
                #Apple system
                set( _ABI_Name "-xgcc${COMPILER_VERSION}")
                set( _ABI_Name_Debug "-d")
            else()
                #GNU/Linux system
                set( _ABI_Name "-gcc${COMPILER_VERSION}")
                set( _ABI_Name_Debug "-d")
            endif()
        endif (CMAKE_COMPILER_IS_GNUCXX)
    endif()
    set( ${ABI_Name} ${_ABI_Name} PARENT_SCOPE )
    set( ${ABI_Name_Debug} ${_ABI_Name_Debug} PARENT_SCOPE )
endfunction( compute_abi_name )