include (LibAddMacros)

find_package (GObjectIntrospection 1.38)

add_headers (HDR_FILES)

if (INTROSPECTION_FOUND)
	set (GIR_NS      "GElektra")
	set (GIR_PREFIX  "${GIR_NS}-${SO_VERSION}.0")
	set (GIR_FILE    "${GIR_PREFIX}.gir")
	set (GIR_TYPELIB "${GIR_PREFIX}.typelib")

	get_filename_component (GELEKTRA_HEADER_DIR ${CMAKE_CURRENT_BINARY_DIR}/../glib REALPATH)
	get_filename_component (GELEKTRA_LIBRARY_DIR ${CMAKE_BINARY_DIR}/lib REALPATH)
	set (GI_TYPELIB_DIR ${CMAKE_CURRENT_BINARY_DIR})

	string (TOLOWER ${GIR_NS} GIR_SYMBOL_PREFIX)
	file (GLOB GIR_INPUT "../glib/*.[hc]")

	get_directory_property (incdirs INCLUDE_DIRECTORIES)
	string (REPLACE ";" ";-I" GIR_INCDIRS "${incdirs}")

	add_custom_command(
		OUTPUT ${GIR_FILE}
		COMMAND ${INTROSPECTION_SCANNER}
			--warn-all --warn-error
			--namespace=${GIR_NS}
			--symbol-prefix=${GIR_SYMBOL_PREFIX}
			--nsversion=${SO_VERSION}.0
			--no-libtool
			--library-path=${GELEKTRA_LIBRARY_DIR}
			--library=${GELEKTRA_LIBRARY}
			--include=GObject-2.0
			-I${GIR_INCDIRS}
			--output=${GIR_FILE}
			${GELEKTRA_HEADER_DIR}/*.h
			${GIR_INPUT}
		DEPENDS ${GIR_INPUT} ${GELEKTRA_LIBRARY}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)

	add_custom_command(
		OUTPUT ${GIR_TYPELIB}
		COMMAND ${INTROSPECTION_COMPILER}
			--output=${GIR_TYPELIB}
			${GIR_FILE}
		DEPENDS ${GIR_FILE} ${GELEKTRA_LIBRARY}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)

	add_custom_target(gir ALL DEPENDS ${GIR_FILE} ${GIR_TYPELIB})
	add_dependencies(gir ${GELEKTRA_LIBRARY})

	install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${GIR_FILE} DESTINATION ${INTROSPECTION_GIRDIR})
	install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${GIR_TYPELIB} DESTINATION ${INTROSPECTION_TYPELIBDIR})

	add_subdirectory (python3)
	add_subdirectory (lua)
else (INTROSPECTION_FOUND)
	message (FATAL_ERROR "g-ir-scanner/g-ir-compiler not found. Please install GObject Introspection")
endif (INTROSPECTION_FOUND)