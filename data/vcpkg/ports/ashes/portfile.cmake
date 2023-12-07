vcpkg_check_linkage(ONLY_DYNAMIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO DragonJoker/Ashes
    REF 2da486ddb45bfcc743a4bd4320fdc121328d629e
    HEAD_REF master
    SHA512 751ab479a275db95f8bd6cb823b6ec5e4e8ebcb2fe5458ee305e3ce18c25497cf266ef9af6e54281bd00c5fac2a5eba02abc88f8ef5b93385be64d6e8ac552a1
)

vcpkg_from_github(
    OUT_SOURCE_PATH CMAKE_SOURCE_PATH
    REPO DragonJoker/CMakeUtils
    REF 4e0292ed50d76dab5fc8c81840ae0e021dc60c2a
    HEAD_REF master
    SHA512 c79c6a5ef2e059b56d4de20cc73e74386bf8b6acea2f6b76fd9949a6a2760f82302c90419e4a753f50c30d01cc4f3a039e04b585f5c0d4461cce3464d9fb9c95
)

file(REMOVE_RECURSE "${SOURCE_PATH}/CMake")
file(COPY "${CMAKE_SOURCE_PATH}/" DESTINATION "${SOURCE_PATH}/CMake")

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        direct3d11 ASHES_BUILD_RENDERER_D3D11
        opengl     ASHES_BUILD_RENDERER_OGL
        vulkan     ASHES_BUILD_RENDERER_VKN
)

vcpkg_cmake_configure(
    SOURCE_PATH ${SOURCE_PATH}
    OPTIONS
        -DVCPKG_PACKAGE_BUILD=ON
        -DASHES_BUILD_TEMPLATES=OFF
        -DASHES_BUILD_TESTS=OFF
        -DASHES_BUILD_INFO=OFF
        -DASHES_BUILD_SAMPLES=OFF
        ${FEATURE_OPTIONS}
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/ashes)

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
