project( 08-TextureBuffer )

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

file( GLOB SHADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.*
)

source_group( "Shader Files" FILES ${SHADER_FILES} )
include_directories( ${CMAKE_SOURCE_DIR}/Test/00-Common/Src )

add_executable( ${PROJECT_NAME} WIN32
	${SOURCE_FILES}
	${HEADER_FILES}
	${SHADER_FILES}
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

foreach( SHADER ${SHADER_FILES} )
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory
			$<$<CONFIG:Debug>:${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/share/${PROJECT_NAME}/Shaders>
			$<$<CONFIG:Release>:${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/share/${PROJECT_NAME}/Shaders>
			$<$<CONFIG:RelWithDebInfo>:${PROJECTS_BINARIES_OUTPUT_DIR_RELWITHDEBINFO}/share/${PROJECT_NAME}/Shaders>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SHADER}
			$<$<CONFIG:Debug>:${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/share/${PROJECT_NAME}/Shaders>
			$<$<CONFIG:Release>:${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/share/${PROJECT_NAME}/Shaders>
			$<$<CONFIG:RelWithDebInfo>:${PROJECTS_BINARIES_OUTPUT_DIR_RELWITHDEBINFO}/share/${PROJECT_NAME}/Shaders>
	)
endforeach()
