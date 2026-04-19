cmake_minimum_required(VERSION 3.20)

project(LBR26GroundSoftwareRoot LANGUAGES CXX)

include(CTest)

find_package(Doxygen)
find_program(PDFLATEX_EXECUTABLE pdflatex)

set(LBR_DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs/doxygen)
set(LBR_DOXYGEN_INPUT_DIR ${CMAKE_SOURCE_DIR})

if(DOXYGEN_FOUND)
	configure_file(
		${CMAKE_SOURCE_DIR}/docs/Doxyfile.in
		${CMAKE_BINARY_DIR}/Doxyfile
		@ONLY
	)

	add_custom_target(docs
		COMMAND ${CMAKE_COMMAND} -E make_directory ${LBR_DOXYGEN_OUTPUT_DIR}
		COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen."
		VERBATIM
	)
else()
	add_custom_target(docs
		COMMAND ${CMAKE_COMMAND} -E echo "Doxygen not found in PATH."
		COMMAND ${CMAKE_COMMAND} -E false
		COMMENT "Doxygen is required for docs target."
	)
endif()

if(DOXYGEN_FOUND AND PDFLATEX_EXECUTABLE)
	add_custom_target(docs-pdf
		COMMAND ${CMAKE_COMMAND} -E chdir ${LBR_DOXYGEN_OUTPUT_DIR}/latex cmd /c make.bat
		DEPENDS docs
		COMMENT "Generating PDF documentation with Doxygen LaTeX output."
		VERBATIM
	)
else()
	add_custom_target(docs-pdf
		COMMAND ${CMAKE_COMMAND} -E echo "Doxygen and pdflatex are required for docs-pdf target."
		COMMAND ${CMAKE_COMMAND} -E false
		COMMENT "Missing tools for docs-pdf target."
	)
endif()

add_subdirectory(LoRa)
