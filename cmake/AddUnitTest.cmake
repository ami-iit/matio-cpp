# Copyright (C) 2020 Istituto Italiano di Tecnologia (IIT). All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-2-Clause license. See the accompanying LICENSE file for details.


find_package(Catch2 3.0.1 REQUIRED)

find_package(VALGRIND QUIET)

include (CMakeDependentOption)
cmake_dependent_option(FRAMEWORK_RUN_Valgrind_tests
  "Run Valgrind tests?" OFF
  "VALGRIND_FOUND" OFF)

if (FRAMEWORK_RUN_Valgrind_tests)
    set(CTEST_MEMORYCHECK_COMMAND ${VALGRIND_PROGRAM})
    set(MEMORYCHECK_COMMAND ${VALGRIND_PROGRAM})
    if (APPLE)
        set(MEMORYCHECK_SUPPRESSIONS "--suppressions=${PROJECT_SOURCE_DIR}/cmake/valgrind-macos.supp")
    else ()
        set(MEMORYCHECK_SUPPRESSIONS "")
    endif ()
    set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --error-exitcode=1 ${MEMORYCHECK_SUPPRESSIONS}"  CACHE STRING "Options to pass to the memory checker")
    mark_as_advanced(MEMORYCHECK_COMMAND_OPTIONS)
    set(MEMCHECK_COMMAND_COMPLETE "${MEMORYCHECK_COMMAND} ${MEMORYCHECK_COMMAND_OPTIONS}")
    separate_arguments(MEMCHECK_COMMAND_COMPLETE)
endif()

function(add_unit_test)

    set(options )
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES LINKS)

    set(prefix "matiocpp")

    cmake_parse_arguments(${prefix}
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN})

    set(name ${${prefix}_NAME})
    set(unit_test_files ${${prefix}_SOURCES})

    set(targetname ${name}UnitTests)
    add_executable(${targetname}
        "${unit_test_files}")

    target_link_libraries(${targetname} PRIVATE Catch2::Catch2WithMain ${${prefix}_LINKS})
    target_compile_definitions(${targetname} PRIVATE CATCH_CONFIG_FAST_COMPILE CATCH_CONFIG_DISABLE_MATCHERS)

    add_test(NAME ${targetname} COMMAND ${targetname})

    if(FRAMEWORK_RUN_Valgrind_tests)
        add_test(NAME memcheck_${targetname} COMMAND ${MEMCHECK_COMMAND_COMPLETE} $<TARGET_FILE:${targetname}>)
    endif()

endfunction()
