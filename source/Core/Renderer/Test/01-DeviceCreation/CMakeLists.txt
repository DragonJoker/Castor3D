project( 01-DeviceCreation )

set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 1 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

file( GLOB SOURCE_FILES
	Src/*.cpp
)

file( GLOB HEADER_FILES
	Src/*.hpp
	Src/*.inl
)

include_directories( ${CMAKE_SOURCE_DIR}/Test/00-Common/Src )

add_executable( ${PROJECT_NAME} WIN32
	${SOURCE_FILES}
	${HEADER_FILES}
)

target_link_libraries( ${PROJECT_NAME}
	${VkLib_LIBRARIES}
	Utils
	Renderer
	00-Common
	${BinaryLibraries}
	${wxWidgets_LIBRARIES}
	${GTK2_LIBRARIES}
)

add_dependencies( ${PROJECT_NAME}
	00-Common
)

set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14 )
set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Test" )

