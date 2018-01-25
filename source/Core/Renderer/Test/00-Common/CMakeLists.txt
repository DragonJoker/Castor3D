project( 00-Common )

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

add_library( ${PROJECT_NAME} STATIC
	${SOURCE_FILES}
	${HEADER_FILES}
)

target_link_libraries( ${PROJECT_NAME}
	Renderer
)

add_dependencies( ${PROJECT_NAME}
	GlRenderer
	VkRenderer
)

set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14 )
set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Test" )

