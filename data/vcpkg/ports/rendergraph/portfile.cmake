vcpkg_from_github(OUT_SOURCE_PATH SOURCE_PATH
    REPO DragonJoker/RenderGraph
    REF b60f202edd2e7397ffbb15dd8c5a163013e5840d
    HEAD_REF master
    SHA512 c3f15e458953a93f90c389013c1af4e4bbabf9473b33dfe5abdf730c6c7b7e5a48fb278e081a05d188bed9d03da5ef595bd6b3c24a33527dbb6957a13c8f063f
)

vcpkg_from_github(OUT_SOURCE_PATH CMAKE_SOURCE_PATH
    REPO DragonJoker/CMakeUtils
    REF 81726a69658dbe26cbf8da77b00b43d87006e9d6
    HEAD_REF master
    SHA512 1cb3404a327b145e8239e242a991ba906a0146ce1622623d629b6820677b0efee75773dc03b8bf8de2d232bcb1df03ade04835845ed043280a777d26a5d271d3
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

vcpkg_copy_pdbs()
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/RenderGraph)

file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)
