vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO spnda/fastgltf
    REF 90b4327f67a444f795dfc638e8066a9007b93a44
    SHA512 20c747c5565d78cd4e7b7cd24638b9f23946cd538cec1e5d33c026583969e41180a439cfea71915f014b8fa30d763e9f59ae7c72db68bc056cffa747e3d2ac2d
    HEAD_REF main
    PATCHES find_package.patch
            fix_uri_escape_spaces.patch
            add_pbr_specular_glossiness_support.patch
            add_gpu_instantiation_support.patch
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS -DFASTGLTF_DOWNLOAD_SIMDJSON=OFF
)
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/${PORT})
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE.md")
vcpkg_copy_pdbs()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
