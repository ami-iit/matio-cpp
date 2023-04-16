
# - Try to find visit_struct
#
# The following are set after configuration is done:
#  visit_struct_FOUND
#
# The following imported targets are created:
#
# visit_struct::visit_struct
#

include(FindPackageHandleStandardArgs)

find_package(visit_struct CONFIG QUIET)

if(NOT visit_struct_FOUND)
  find_path(VISIT_STRUCT_INCLUDE_DIR visit_struct/visit_struct_intrusive.hpp)
  mark_as_advanced(VISIT_STRUCT_INCLUDE_DIR)
  
  if(VISIT_STRUCT_INCLUDE_DIR)
    set(visit_struct_FOUND)
  endif()
endif()

find_package_handle_standard_args(visit_struct DEFAULT_MSG visit_struct_FOUND)

if(visit_struct_FOUND)
    if(NOT TARGET visit_struct::visit_struct)
      add_library(visit_struct::visit_struct INTERFACE IMPORTED)
      set_target_properties(visit_struct::visit_struct PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${VISIT_STRUCT_INCLUDE_DIR}")
    endif()
endif()
