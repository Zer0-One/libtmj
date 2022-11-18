find_package(PkgConfig QUIET)
pkg_check_modules(Jansson QUIET IMPORTED_TARGET jansson)

find_path(Jansson_INCLUDE_DIR jansson.h
	PATHS
	  "/usr/include"
	  "/usr/local/include"
      ${Jansson_INCLUDEDIR}
	  PATH_SUFFIXES "jansson"
	)

find_library(Jansson_LIBRARY
	NAMES jansson
	PATHS
	  /usr/lib
	  /usr/local/lib
	  ${Jansson_LIBDIR}
	)

if (Jansson_INCLUDE_DIR AND Jansson_LIBRARY)
	set(Jansson_FOUND TRUE)
	set(Jansson_LIBRARIES ${Jansson_LIBRARY})
	set(Jansson_INCLUDE_DIRS ${Jansson_INCLUDE_DIR})
else (Jansson_INCLUDE_DIR AND Jansson_LIBRARY)
	set(Jansson_FOUND FALSE)
endif (Jansson_INCLUDE_DIR AND Jansson_LIBRARY)


find_package_handle_standard_args(Jansson DEFAULT_MSG
	Jansson_LIBRARIES
	Jansson_INCLUDE_DIRS)

if(Jansson_FOUND AND NOT TARGET Jansson::Jansson)
	if (SYSTEM_Jansson)
		add_library(jansson::jansson IMPORTED INTERFACE)
	else()
		add_library(jansson::jansson UNKNOWN IMPORTED)
		set_property(TARGET jansson::jansson APPEND PROPERTY
			IMPORTED_LOCATION "${Jansson_LIBRARY}")
	endif()

	set_target_properties(jansson::jansson PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${Jansson_INCLUDE_DIRS}")
endif()
