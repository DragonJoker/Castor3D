FIND_PATH(GLM_ROOT_DIR glm/glm.hpp
  HINTS
  PATH_SUFFIXES glm
  PATHS
  /usr/local
  /usr
)

FIND_PATH(GLM_INCLUDE_DIR glm/glm.hpp
  HINTS
  PATH_SUFFIXES glm
  PATHS
  ${GLM_ROOT_DIR}
)

find_package_handle_standard_args( GLM DEFAULT_MSG GLM_INCLUDE_DIR )
