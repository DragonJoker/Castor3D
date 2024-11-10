vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO spnda/fastgltf
    REF 06cf7326aeb5763d87f400cb75c576cb9d5463af
    SHA512 3830cfe44c9e9738d306c353b914d476b74cb8cf0a4d00cc2f43d737f4a03f95cf7b672b02fe50ba2011eb31db00914439e0d8e0c7be0521d5c7e85db6ec92c7
    HEAD_REF main
    PATCHES
        fix_uri_escape_spaces.patch
        KHR_material_diffuse_transmission.patch
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS -DFASTGLTF_ENABLE_DEPRECATED_EXT=ON
)
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/${PORT})
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE.md")
vcpkg_copy_pdbs()

file(READ "${CURRENT_PACKAGES_DIR}/share/fastgltf/fastgltfConfig.cmake" contents)
file(WRITE "${CURRENT_PACKAGES_DIR}/share/fastgltf/fastgltfConfig.cmake" "
include(CMakeFindDependencyMacro)
find_dependency(simdjson)
${contents}")

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
