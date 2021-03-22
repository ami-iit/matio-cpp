
# - Try to find MatIO
#
# The following variables are optionally searched for defaults
#  MATIO_ROOT_DIR:            Base directory where all MatIO components are found
#
# The following are set after configuration is done:
#  MATIO_FOUND
#
# The following imported targets are created:
#
# MATIO::MATIO
#

include(FindPackageHandleStandardArgs)

set(MATIO_ROOT_DIR "$ENV{MATIO_DIR}" CACHE PATH "Folder contains MatIO")

find_path(MATIO_INCLUDE_DIR matio.h PATHS ${MATIO_ROOT_DIR} PATH_SUFFIXES include)
mark_as_advanced(MATIO_INCLUDE_DIR)
find_library(MATIO_LIBRARY matio libmatio PATHS ${MATIO_ROOT_DIR} PATH_SUFFIXES lib)
mark_as_advanced(MATIO_LIBRARY)

find_package_handle_standard_args(MATIO DEFAULT_MSG MATIO_INCLUDE_DIR MATIO_LIBRARY)

if(MATIO_FOUND)
    set(MATIO_INCLUDE_DIRS ${MATIO_INCLUDE_DIR})
    set(MATIO_LIBRARIES ${MATIO_LIBRARY})

    if(NOT TARGET MATIO::MATIO)
      add_library(MATIO::MATIO UNKNOWN IMPORTED)
      set_target_properties(MATIO::MATIO PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${MATIO_INCLUDE_DIR}")
      set_property(TARGET MATIO::MATIO APPEND PROPERTY
        IMPORTED_LOCATION "${MATIO_LIBRARY}")
    endif()
endif()
