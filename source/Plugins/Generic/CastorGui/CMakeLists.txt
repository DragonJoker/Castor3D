set( FOLDER_NAME CastorGui )
project( castor3d${FOLDER_NAME} )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	castor::${FOLDER_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Castor3D GUI creation" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 4 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CastorGui_Parsers.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CastorGuiPrerequisites.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/ControlsManager.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CastorGui_Parsers.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CastorGuiPlugin.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CastorGuiPrerequisites.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/ControlsManager.cpp
)
source_group( "Header Files"
	FILES
		${${PROJECT_NAME}_HDR_FILES}
)
source_group( "Source Files"
	FILES
		${${PROJECT_NAME}_SRC_FILES}
)
set( ${PROJECT_NAME}_CTRLS_HDR_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CtrlButton.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CtrlComboBox.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CtrlControl.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CtrlEdit.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CtrlListBox.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CtrlSlider.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CtrlStatic.hpp
)
set( ${PROJECT_NAME}_CTRLS_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CtrlButton.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CtrlComboBox.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CtrlControl.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CtrlEdit.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CtrlListBox.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CtrlSlider.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CtrlStatic.cpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_CTRLS_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_CTRLS_HDR_FILES}
)
source_group( "Header Files\\Controls"
	FILES
		${${PROJECT_NAME}_CTRLS_HDR_FILES}
)
source_group( "Source Files\\Controls"
	FILES
		${${PROJECT_NAME}_CTRLS_SRC_FILES}
)
add_target_min(
	${PROJECT_NAME}
	plugin
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/${FOLDER_NAME}Pch.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/${FOLDER_NAME}Pch.cpp
)
add_library( castor::${FOLDER_NAME}
	ALIAS
	${PROJECT_NAME}
)
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${GenericIncludes}
)
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		castor::Castor3D
)
install_target( ${PROJECT_NAME}
	plugin
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}
)
set_property( TARGET ${PROJECT_NAME}
	PROPERTY
		FOLDER "Plugins/Generic"
)
set( CASTOR_INSTEXP_LIST
	${CASTOR_INSTEXP_LIST}
	${PROJECT_NAME}
	PARENT_SCOPE
)
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
