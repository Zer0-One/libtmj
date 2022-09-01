find_package(PkgConfig QUIET)
pkg_check_modules(Zstd QUIET IMPORTED_TARGET zstd)

find_path(Zstd_INCLUDE_DIR zstd.h
	PATHS
	  "/usr/include"
	  "/usr/local/include"
      ${Zstd_INCLUDEDIR}
	  PATH_SUFFIXES "zstd"
	)

find_library(Zstd_LIBRARY
	NAMES zstd
	PATHS
	  /usr/lib
	  /usr/local/lib
	  ${Zstd_LIBDIR}
	)

if (Zstd_INCLUDE_DIR AND Zstd_LIBRARY)
	set(Zstd_FOUND TRUE)
	set(Zstd_LIBRARIES ${Zstd_LIBRARY})
	set(Zstd_INCLUDE_DIRS ${Zstd_INCLUDE_DIR})
else (Zstd_INCLUDE_DIR AND Zstd_LIBRARY)
	set(Zstd_FOUND FALSE)
endif (Zstd_INCLUDE_DIR AND Zstd_LIBRARY)


find_package_handle_standard_args(Zstd DEFAULT_MSG
	Zstd_LIBRARIES
	Zstd_INCLUDE_DIRS)

if(Zstd_FOUND AND NOT TARGET Zstd::Zstd)
	if (SYSTEM_Zstd)
		add_library(Zstd::Zstd IMPORTED INTERFACE)
	else()
		add_library(Zstd::Zstd UNKNOWN IMPORTED)
		set_property(TARGET Zstd::Zstd APPEND PROPERTY
			IMPORTED_LOCATION "${Zstd_LIBRARY}")
	endif()

	set_target_properties(Zstd::Zstd PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${Zstd_INCLUDE_DIRS}")
endif()
