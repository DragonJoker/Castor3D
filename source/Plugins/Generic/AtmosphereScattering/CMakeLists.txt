set( FOLDER_NAME AtmosphereScattering )
project( ${FOLDER_NAME} )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	castor::${FOLDER_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Atmosphere scattering" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 4 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )
set( PROJECT_SOVERSION "${${PROJECT_NAME}_VERSION_BUILD}" )

set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereBackground.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereBackgroundModel.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereBackgroundPass.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereCameraUbo.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereLightingModel.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereModel.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereMultiScatteringPass.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereScattering_Parsers.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereScatteringConfig.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereScatteringPrerequisites.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereScatteringUbo.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereSkyViewPass.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereTransmittancePass.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereVolumePass.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CloudsConfig.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CloudsCurlPass.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CloudsModel.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CloudsPerlinPass.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CloudsResolvePass.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CloudsUbo.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CloudsVolumePass.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CloudsWeatherPass.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CloudsWorleyPass.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/ScatteringModel.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/WeatherConfig.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/WeatherUbo.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereBackground.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereBackgroundModel.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereBackgroundPass.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereCameraUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereLightingModel.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereModel.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereMultiScatteringPass.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereScattering_Parsers.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereScatteringPlugin.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereScatteringUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereSkyViewPass.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereTransmittancePass.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/AtmosphereVolumePass.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CloudsCurlPass.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CloudsModel.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CloudsPerlinPass.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CloudsResolvePass.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CloudsUbo.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CloudsVolumePass.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CloudsWeatherPass.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/CloudsWorleyPass.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/ScatteringModel.cpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}/WeatherUbo.cpp
)
source_group( "Header Files" FILES ${${PROJECT_NAME}_HDR_FILES} )
source_group( "Source Files" FILES ${${PROJECT_NAME}_SRC_FILES} )

add_target_min(
	${PROJECT_NAME}
	api_plugin
	""
	""
)
target_sources( ${PROJECT_NAME} 
	PRIVATE
		${CASTOR_EDITORCONFIG_FILE}
)
if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	target_compile_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/Zi>" )
	target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/DEBUG>" )
	target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:REF>" )
	target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>" )
endif ()
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
target_compile_options( ${PROJECT_NAME}
	PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/bigobj>
)
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_SOVERSION}
		PREFIX ${C3D_LIB_PREFIX}
		FOLDER "Plugins/Generic"
)
c3d_release_pdbs( ${PROJECT_NAME} )
install_target_ex( ${PROJECT_NAME}
	Castor3D
	Plugins
	api_plugin
	${CASTOR_SOURCE_DIR}/source/Plugins/Generic/${FOLDER_NAME}
)
set( CASTOR_INSTEXP_LIST
	${CASTOR_INSTEXP_LIST}
	${PROJECT_NAME}
	PARENT_SCOPE
)
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
