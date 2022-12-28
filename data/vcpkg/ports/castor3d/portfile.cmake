vcpkg_from_github(OUT_SOURCE_PATH SOURCE_PATH
    REPO DragonJoker/Castor3D
    HEAD_REF development
)

vcpkg_from_github(OUT_SOURCE_PATH CMAKE_SOURCE_PATH
    REPO DragonJoker/CMakeUtils
    REF e2a9d422a02dab0e04f54b3e1bc515eba652a9d1
    HEAD_REF master
    SHA512 4ebd6141b9e5aa5283f31892da7108aa09fbd59292f0e98f2c9fe67577856f0af253184d41fdc16bb11094c4635401f181ea2e3abfa560adcf5c029f0d663b24
)
get_filename_component(SRC_PATH ${CMAKE_SOURCE_PATH} DIRECTORY)
if (EXISTS ${SRC_PATH}/CMake)
    file(REMOVE_RECURSE ${SRC_PATH}/CMake)
endif()
file(RENAME ${CMAKE_SOURCE_PATH} ${SRC_PATH}/CMake)
set(CMAKE_SOURCE_PATH ${SRC_PATH}/CMake)
file(COPY ${CMAKE_SOURCE_PATH} DESTINATION ${SOURCE_PATH})

vcpkg_from_github(OUT_SOURCE_PATH CVTT_SOURCE_PATH
    REPO elasota/ConvectionKernels
    REF dc2dbbe0ae2cf2be06ef56d1021e2222a56c7fe2
    HEAD_REF master
    SHA512 2bf3aff1acb7b2365b882b4c1274ea8bcb9aea3015b5009e0ec50279122ecc623074d0f4fa04ddf8cd457e1f6868075a773bf8a2fa5b4fa9e2fd51d0a76d2560
)
get_filename_component(SRC_PATH ${CVTT_SOURCE_PATH} DIRECTORY)
if (EXISTS ${SRC_PATH}/external/cvtt)
    file(REMOVE_RECURSE ${SRC_PATH}/external/cvtt)
endif()
file(MAKE_DIRECTORY ${SRC_PATH}/external)
file(RENAME ${CVTT_SOURCE_PATH} ${SRC_PATH}/external/cvtt)
set(CVTT_SOURCE_PATH ${SRC_PATH}/external/cvtt)
file(COPY ${CVTT_SOURCE_PATH} DESTINATION ${SOURCE_PATH}/external)

vcpkg_from_github(OUT_SOURCE_PATH VKFFT_SOURCE_PATH
    REPO DTolm/VkFFT
    REF 82419410ffce0d07739d6100e2e0dd7588153f46
    HEAD_REF master
    SHA512 f5b34caa6ae2df5c4e2cd76a1c31dd849f697eedbf5c1f0ca3f975082e379e06b151de455ecd55a88da33e37aecb7261c245b0f71e46b3a8c98c6fc57ec2ce8a
)
get_filename_component(SRC_PATH ${VKFFT_SOURCE_PATH} DIRECTORY)
if (EXISTS ${SRC_PATH}/external/VkFFT)
    file(REMOVE_RECURSE ${SRC_PATH}/external/VkFFT)
endif()
file(RENAME ${VKFFT_SOURCE_PATH} ${SRC_PATH}/external/VkFFT)
set(VKFFT_SOURCE_PATH ${SRC_PATH}/external/VkFFT)
file(COPY ${VKFFT_SOURCE_PATH} DESTINATION ${SOURCE_PATH}/external)

string(COMPARE EQUAL "${VCPKG_LIBRARY_LINKAGE}" "static" BUILD_STATIC)

vcpkg_cmake_configure(
    SOURCE_PATH ${SOURCE_PATH}
    OPTIONS
        -DCASTOR_UNITY_BUILD=ON
        -DCASTOR_BUILD_PLUGINS=ON
        -DCASTOR_BUILDGRP_INTEROP=OFF
        -DCASTOR_BUILDGRP_TEST=OFF
        -DCASTOR_USE_GLSLANG=ON
        -DCASTOR_DISABLE_DELAYED_INITIALISATION=ON
        -DCASTOR_BUILDGRP_DEMO=OFF
        -DCASTOR_BUILDGRP_TOOL=OFF
        -DVCPKG_BUILD=ON
        -DVCPKG_INSTALLED_DIR=${CURRENT_INSTALLED_DIR}
        -DAshes_DIR=${CURRENT_INSTALLED_DIR}/share/ashes
        -DRenderGraph_DIR=${CURRENT_INSTALLED_DIR}/share/rendergraph
        -DShaderWriter_DIR=${CURRENT_INSTALLED_DIR}/share/shaderwriter
        -DSdwCompilers_DIR=${CURRENT_INSTALLED_DIR}/share/shaderwriter
        -Dglslang_DIR=${CURRENT_INSTALLED_DIR}/share/glslang
        -Dfreetype_DIR=${CURRENT_INSTALLED_DIR}/share/freetype
        -Dassimp_DIR=${CURRENT_INSTALLED_DIR}/share/assimp
        -Dirrlicht_DIR=${CURRENT_INSTALLED_DIR}/share/irrlicht
        -Dpolyclipping_DIR=${CURRENT_INSTALLED_DIR}/share/polyclipping
        -Dminizip_DIR=${CURRENT_INSTALLED_DIR}/share/minizip
        -Dkubazip_DIR=${CURRENT_INSTALLED_DIR}/share/kubazip
        -Dpoly2tri_DIR=${CURRENT_INSTALLED_DIR}/share/poly2tri
        -Dutf8cpp_DIR=${CURRENT_INSTALLED_DIR}/share/utf8cpp
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/Castor3D)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)