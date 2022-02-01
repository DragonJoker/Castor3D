set( FOLDER_NAME CastorGui )
project( ${FOLDER_NAME} )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	castor::${FOLDER_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Castor3D GUI creation" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 4 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )
set( PROJECT_SOVERSION "${${PROJECT_NAME}_VERSION_BUILD}" )

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
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Controls/CtrlButton.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Controls/CtrlComboBox.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Controls/CtrlControl.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Controls/CtrlEdit.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Controls/CtrlListBox.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Controls/CtrlSlider.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Controls/CtrlStatic.hpp
)
set( ${PROJECT_NAME}_CTRLS_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Controls/CtrlButton.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Controls/CtrlComboBox.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Controls/CtrlControl.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Controls/CtrlEdit.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Controls/CtrlListBox.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Controls/CtrlSlider.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Controls/CtrlStatic.cpp
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
set( ${PROJECT_NAME}_STYLS_HDR_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Theme/StyleButton.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Theme/StyleComboBox.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Theme/StyleControl.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Theme/StyleEdit.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Theme/StyleListBox.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Theme/StyleSlider.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Theme/StyleStatic.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/Theme/Theme.hpp
)
set( ${PROJECT_NAME}_STYLS_SRC_FILES
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_STYLS_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_STYLS_HDR_FILES}
)
source_group( "Header Files\\Style"
	FILES
		${${PROJECT_NAME}_STYLS_HDR_FILES}
)
source_group( "Source Files\\Style"
	FILES
		${${PROJECT_NAME}_STYLS_SRC_FILES}
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
install_target_ex( ${PROJECT_NAME}
	Plugins
	plugin
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}
)
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_SOVERSION}
		CXX_STANDARD 20
		CXX_EXTENSIONS OFF
		PREFIX ${C3D_LIB_PREFIX}
		FOLDER "Plugins/Generic"
)
set( CASTOR_INSTEXP_LIST
	${CASTOR_INSTEXP_LIST}
	${PROJECT_NAME}
	PARENT_SCOPE
)
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
