#******************************************************************************
# add_com_interfaces
#
# Syntax: add_com_interfaces( <output list> <idl1> [<idl2> [...]] )
# Notes: 
#   <idl1> should be absolute paths so the MIDL compiler can find them.
#   For every idl file xyz.idl, two files xyz_h.h and xyz.c are generated
#   which are added to the <output list>
#
#******************************************************************************
macro( add_com_interfaces OUTPUT_LIST )    
    foreach( IN_FILE ${ARGN} )
        get_filename_component( OUT_FILE ${IN_FILE} NAME_WE )
        get_filename_component( IN_PATH ${IN_FILE} PATH )

        set( OUT_HEADER_NAME ${OUT_FILE}_h.h )
        set( OUT_HEADER ${CMAKE_CURRENT_BINARY_DIR}/${OUT_HEADER_NAME} )
        set( OUT_IID_NAME ${OUT_FILE}.c )
        set( OUT_IID ${CMAKE_CURRENT_BINARY_DIR}/${OUT_IID_NAME} )
    
        add_custom_command(
            OUTPUT ${OUT_HEADER} ${OUT_IID}
            DEPENDS ${IN_FILE}
            COMMAND midl /header ${OUT_HEADER_NAME} /iid ${OUT_IID_NAME} ${IN_FILE} /I ${CMAKE_CURRENT_SOURCE_DIR}/Src
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )

        set_source_files_properties(
            ${OUT_HEADER}
            ${OUT_IID}
            PROPERTIES
            GENERATED TRUE
        )
    
        set_source_files_properties( ${IN_FILE} PROPERTIES HEADER_FILE_ONLY TRUE )

        set( ${OUTPUT_LIST} ${${OUTPUT_LIST}} ${OUT_HEADER} ${OUT_IID} )
    endforeach()
endmacro( add_com_interfaces )

#******************************************************************************
# add_interface
#
# Notes:
#   Creates the RGS file to be able to register the object as a COM component
#   Adds the RGS file to the RGS files list, for the resources .rc file
#   Adds the RGS file id to the resources .h file
#
#******************************************************************************
macro( add_interface OBJECT_IID RESRC_LIST RESH_LIST RESOURCE_ID OBJECT_NAME )
    SET( _OBJECT_IID ${OBJECT_IID} )
    SET( _OBJECT_NAME ${OBJECT_NAME} )
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/Win32/ComCastor.rgs.in
        ${CMAKE_CURRENT_BINARY_DIR}/Src/Win32/Com${_OBJECT_NAME}.rgs
        @ONLY
    )
    set( ${RESRC_LIST} "${${RESRC_LIST}}\nIDR_${_OBJECT_NAME}	REGISTRY	\"${CMAKE_CURRENT_BINARY_DIR}/Src/Win32/Com${_OBJECT_NAME}.rgs\"" )
    set( ${RESH_LIST} "${${RESH_LIST}}\n#define IDR_${_OBJECT_NAME}		${${RESOURCE_ID}}" )
    math( EXPR ${RESOURCE_ID} "${${RESOURCE_ID}}+1")
endmacro( add_interface )
