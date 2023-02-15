vcpkg_from_github(OUT_SOURCE_PATH SOURCE_PATH
    REPO DragonJoker/RenderGraph
    REF 7713960350810dadd6f091b4079f9f6a8cbd583d
    HEAD_REF master
    SHA512 98f41a9a318b160759954001b03c15bba678195e74428688d43c883b16dbfcfaebc942fa07c72393a1ecb2e1a65da865ea57a07a72158af13fe25b2758c470fc
)

vcpkg_from_github(OUT_SOURCE_PATH CMAKE_SOURCE_PATH
    REPO DragonJoker/CMakeUtils
    REF 7d355194fa795c437ce970cecf00e23ae10fc686
    HEAD_REF master
    SHA512 ca25b19bdeb3e8fda7abc32f8548731f0ba1cd09889a70f6f287ad76d2fdfa0fedbb7f6f65b26d356ea51543bed8926c6bb463f8e8461b7d51d3b7b33134374c
)

get_filename_component(SRC_PATH "${CMAKE_SOURCE_PATH}" DIRECTORY)
if (EXISTS "${SRC_PATH}/CMake")
    file(REMOVE_RECURSE "${SRC_PATH}/CMake")
endif()
file(RENAME "${CMAKE_SOURCE_PATH}" "${SRC_PATH}/CMake")
set(CMAKE_SOURCE_PATH "${SRC_PATH}/CMake")
file(COPY "${CMAKE_SOURCE_PATH}" DESTINATION "${SOURCE_PATH}")

string(COMPARE EQUAL "${VCPKG_LIBRARY_LINKAGE}" "static" BUILD_STATIC)

vcpkg_cmake_configure(
    SOURCE_PATH ${SOURCE_PATH}
    OPTIONS
        -DPROJECTS_USE_PRECOMPILED_HEADERS=ON
        -DCRG_UNITY_BUILD=ON 
        -DCRG_BUILD_STATIC=${BUILD_STATIC}
        -DVULKAN_HEADERS_INCLUDE_DIRS=${CURRENT_INSTALLED_DIR}/include
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/RenderGraph)

file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)