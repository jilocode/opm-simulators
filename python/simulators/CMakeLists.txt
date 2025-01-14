# NOTE: we assume that add_subdirectory( pybind11 ) is called from the
#    parent folder's CMakeLists.txt before this CMakeLists.txt is loaded.
#    Therefore, pybind11's CMakeLists.txt has already run
#    find_package(PYTHON) to define variables like ${PYTHON_EXECUTABLE}
#
# NOTE: The variable ${PYBIND11_SYSTEM} is set in python/CMakeLists.txt
#   to the value "SYSTEM" or unset, depending on the current version of Pybind11.
#   The value is then forwarded to target_include_directories(), see
#
#  https://cmake.org/cmake/help/latest/command/target_include_directories.html
#  https://pybind11.readthedocs.io/en/stable/compiling.html
#
pybind11_add_module(simulators ${PYBIND11_SYSTEM}
  PyBlackOilSimulator.cpp
  Pybind11Exporter.cpp)

set_target_properties( simulators PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/python/opm2 )

target_sources(simulators
  PRIVATE
  $<TARGET_OBJECTS:moduleVersion>
  $<TARGET_OBJECTS:flow_libblackoil>)

target_link_libraries( simulators PRIVATE opmsimulators )

set(PYTHON_PACKAGE_PATH "site-packages")
set(PYTHON_INSTALL_PREFIX "lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/${PYTHON_PACKAGE_PATH}" CACHE STRING "Subdirectory to install Python modules in")

install(TARGETS simulators DESTINATION ${DEST_PREFIX}${CMAKE_INSTALL_PREFIX}/${PYTHON_INSTALL_PREFIX}/opm)

file( COPY ${PROJECT_SOURCE_DIR}/python/test
      DESTINATION ${PROJECT_BINARY_DIR}/python)
file( COPY ${PROJECT_SOURCE_DIR}/python/test_data
      DESTINATION ${PROJECT_BINARY_DIR}/python)

if(OPM_ENABLE_PYTHON_TESTS)
  if(Python3_EXECUTABLE AND NOT PYTHON_EXECUTABLE)
    set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
  endif()
  set(PYTHON_PATH ${PROJECT_BINARY_DIR}/python:${opm-common_DIR}/python:$ENV{PYTHONPATH})
  # NOTE: See comment in test_basic.py for the reason why we are
  #   splitting the python tests into multiple add_test() tests instead
  #   of having a single "python -m unittest" test call that will run all
  #   the tests in the "test" sub directory.
  add_test(NAME python_basic
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/python
      COMMAND ${CMAKE_COMMAND}
      -E env PYTHONPATH=${PYTHON_PATH} ${PYTHON_EXECUTABLE}
      -m unittest test/test_basic.py)
  add_test(NAME python_schedule
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/python
      COMMAND ${CMAKE_COMMAND}
      -E env PYTHONPATH=${PYTHON_PATH} ${PYTHON_EXECUTABLE}
      -m unittest test/test_schedule.py)
endif()
