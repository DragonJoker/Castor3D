vcpkg_minimum_required(VERSION 2022-10-12) # for ${VERSION}

vcpkg_from_github(OUT_SOURCE_PATH SOURCE_PATH
    REPO DragonJoker/ShaderWriter
    REF 3309b2e6d60a3028c6ae2773e7de540e25176dab
    HEAD_REF development
    SHA512 5d01174269c53ac489837194c9dc72643e3cf5b361313ab686ed6c618920afabf4cd216a76c0be5f308497942821819a6dc851b8d19ee6157c478aeb5be7dc81
)

vcpkg_from_github(OUT_SOURCE_PATH CMAKE_SOURCE_PATH
    REPO DragonJoker/CMakeUtils
    REF 7d355194fa795c437ce970cecf00e23ae10fc686
    HEAD_REF master
    SHA512 ca25b19bdeb3e8fda7abc32f8548731f0ba1cd09889a70f6f287ad76d2fdfa0fedbb7f6f65b26d356ea51543bed8926c6bb463f8e8461b7d51d3b7b33134374c
)

file(REMOVE_RECURSE "${SOURCE_PATH}/CMake")
file(COPY "${CMAKE_SOURCE_PATH}/" DESTINATION "${SOURCE_PATH}/CMake")

string(COMPARE EQUAL "${VCPKG_LIBRARY_LINKAGE}" "static" BUILD_STATIC)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        spirv SDW_BUILD_EXPORTER_SPIRV
        glsl  SDW_BUILD_EXPORTER_HLSL
        hlsl  SDW_BUILD_EXPORTER_GLSL
)

vcpkg_cmake_configure(
    SOURCE_PATH ${SOURCE_PATH}
    OPTIONS
        -DPROJECTS_USE_PRECOMPILED_HEADERS=OFF
        -DSDW_GENERATE_SOURCE=OFF
        -DSDW_BUILD_VULKAN_LAYER=OFF
        -DSDW_BUILD_TESTS=OFF
        -DSDW_BUILD_STATIC_SDW=${BUILD_STATIC}
        -DSDW_BUILD_STATIC_SDAST=${BUILD_STATIC}
        -DSDW_UNITY_BUILD=ON
        ${FEATURE_OPTIONS}
)

vcpkg_copy_pdbs()
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME shaderwriter CONFIG_PATH lib/cmake/shaderwriter)

file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)
