# Distributed under the OSI-approved BSD 3-Clause License.

#.rst:
# Findbcdec
# ------------
#
# Find the bcdec include headers.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ``bcdec_FOUND``
#   True if bcdec library found
#
# ``bcdec_INCLUDE_DIR``
#   Location of bcdec headers
#

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
include(${CMAKE_ROOT}/Modules/SelectLibraryConfigurations.cmake)

if(NOT bcdec_INCLUDE_DIR)
  find_path(bcdec_INCLUDE_DIR NAMES bcdec.h PATHS ${bcdec_DIR} PATH_SUFFIXES include)
endif()

find_package_handle_standard_args(bcdec DEFAULT_MSG bcdec_INCLUDE_DIR)
mark_as_advanced(bcdec_INCLUDE_DIR)
