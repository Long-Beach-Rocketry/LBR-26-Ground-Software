if(NOT DEFINED ROOT)
    message(FATAL_ERROR "ROOT must be set to the repository root.")
endif()

file(TO_CMAKE_PATH "${ROOT}" ROOT_CMAKE)
file(GLOB_RECURSE _files
    "${ROOT_CMAKE}/LoRa/include/*.h"
    "${ROOT_CMAKE}/LoRa/src/*.cc"
    "${ROOT_CMAKE}/LoRa/tests/*.cc"
)

if(NOT _files)
    message(FATAL_ERROR "No C++ files found to validate headers.")
endif()

foreach(_file IN LISTS _files)
    file(RELATIVE_PATH _relative_path "${ROOT_CMAKE}" "${_file}")
    file(READ "${_file}" _content)
    string(REPLACE "\r\n" "\n" _content "${_content}")
    string(REPLACE "\n" ";" _lines "${_content}")

    list(LENGTH _lines _line_count)
    if(_line_count LESS 6)
        message(FATAL_ERROR "Invalid file header in ${_relative_path}. Expected a 6-line Doxygen header block.")
    endif()

    list(SUBLIST _lines 0 6 _header_lines)
    list(LENGTH _header_lines _header_line_count)
    if(NOT _header_line_count EQUAL 6)
        message(FATAL_ERROR "Invalid file header in ${_relative_path}. Expected a 6-line Doxygen header block.")
    endif()

    list(GET _header_lines 0 _line1)
    list(GET _header_lines 1 _line2)
    list(GET _header_lines 2 _line3)
    list(GET _header_lines 3 _line4)
    list(GET _header_lines 4 _line5)
    list(GET _header_lines 5 _line6)

    if(NOT _line1 STREQUAL "/**")
        message(FATAL_ERROR "Invalid file header in ${_relative_path}: line 1 must be /**.")
    endif()
    if(NOT _line2 STREQUAL " * @file ${_relative_path}")
        message(FATAL_ERROR "Invalid file header in ${_relative_path}: line 2 must be @file ${_relative_path}.")
    endif()
    string(FIND "${_line3}" " * @brief " _brief_prefix)
    if(NOT _brief_prefix EQUAL 0)
        message(FATAL_ERROR "Invalid file header in ${_relative_path}: line 3 must start with @brief.")
    endif()
    if(NOT _line4 STREQUAL " * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)")
        message(FATAL_ERROR "Invalid file header in ${_relative_path}: line 4 must contain the expected author and email.")
    endif()
    if(NOT _line5 STREQUAL " * @note Origin: Long Beach Rocketry")
        message(FATAL_ERROR "Invalid file header in ${_relative_path}: line 5 must contain the origin note.")
    endif()
    if(NOT _line6 STREQUAL " */")
        message(FATAL_ERROR "Invalid file header in ${_relative_path}: line 6 must close the Doxygen block.")
    endif()
endforeach()

message(STATUS "Validated ${_files} file headers.")