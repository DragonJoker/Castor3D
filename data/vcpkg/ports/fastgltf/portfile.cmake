vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO spnda/fastgltf
    REF 46f28f1355657c42c9f73bfcdde4ba37d05c2a2f
    SHA512 8e26d8d8061a4416d0c2d934a74e8569d3d60db9f01fa7eeae9b251b61e151941c569ea8da70050746e241ca2f0cb4ff14809701f7247f9f31a8538ae28f5c3c
    HEAD_REF main
    PATCHES find_package.patch
            fix_uri_escape_spaces.patch
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS -DFASTGLTF_DOWNLOAD_SIMDJSON=OFF
            -DFASTGLTF_ENABLE_DEPRECATED_EXT=ON
)
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/${PORT})
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE.md")
vcpkg_copy_pdbs()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
