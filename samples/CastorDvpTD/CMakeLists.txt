project( CastorDvpTD )

set( ${PROJECT_NAME}_WXWIDGET 1 )
set( ${PROJECT_NAME}_DESCRIPTION "Castor Tower Defense" )
set( ${PROJECT_NAME}_VERSION_MAJOR	1 )
set( ${PROJECT_NAME}_VERSION_MINOR	1 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/Ability.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/Bullet.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/CastorDvpTD.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/CastorDvpTDPrerequisites.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/Enemy.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/EnemyCategory.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/EnemySpawner.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/Game.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/Grid.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/Hud.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/MainFrame.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/RenderPanel.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/Tower.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/TowerCategory.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/Bullet.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/CastorDvpTD.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/CastorDvpTDPrerequisites.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/Enemy.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/EnemyCategory.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/EnemySpawner.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/Game.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/Grid.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/Hud.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/MainFrame.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/RenderPanel.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/Tower.cpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/TowerCategory.cpp
)
source_group( "Header Files"
	FILES
		${${PROJECT_NAME}_HDR_FILES}
)
source_group( "Source Files"
	FILES
		${${PROJECT_NAME}_SRC_FILES}
)
add_target_min(
	${PROJECT_NAME}
	bin
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/${PROJECT_NAME}Pch.hpp
	${CASTOR_SOURCE_DIR}/samples/${PROJECT_NAME}/${PROJECT_NAME}Pch.cpp
)
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${Castor3DIncludeDirs}
)
target_link_libraries( ${PROJECT_NAME}
	PRIVATE
		${Castor3DBinsDependencies}
		castor::GuiCommon
)
set_property( TARGET ${PROJECT_NAME}
	PROPERTY
		FOLDER "Samples"
)
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

file(
	GLOB
		DataFiles
		${CASTOR_SOURCE_DIR}/data/${PROJECT_NAME}/*.*
)

copy_target_files( ${PROJECT_NAME} "" ${DataFiles} )
