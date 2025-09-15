find_path(e2fsprogs_INCLUDE_DIR NAMES ext2fs et)
find_library(e2fsprogs_com_err_LIBRARY NAMES com_err)
find_library(e2fsprogs_ext2fs_LIBRARY NAMES ext2fs)

message(INFO ${e2fsprogs_com_err_INCLUDE_DIR} ${e2fsprogs_com_err_LIBRARY} ${e2fsprogs_ext2fs_INCLUDE_DIR} ${e2fsprogs_ext2fs_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(e2fsprogs
    REQUIRED_VARS
        e2fsprogs_INCLUDE_DIR
        e2fsprogs_com_err_LIBRARY
        e2fsprogs_ext2fs_LIBRARY
)

if(e2fsprogs_FOUND)
  set(e2fsprogs_LIBRARIES ${e2fsprogs_ext2fs_LIBRARY} ${e2fsprogs_com_err_LIBRARY})
  set(e2fsprogs_INCLUDE_DIRS ${e2fsprogs_INCLUDE_DIR})

  if(NOT TARGET e2fsprogs::com_err)
    add_library(e2fsprogs::com_err UNKNOWN IMPORTED)
    set_target_properties(e2fsprogs::com_err PROPERTIES
      IMPORTED_LOCATION "${e2fsprogs_com_err_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${e2fsprogs_INCLUDE_DIR}"
    )
  endif()

  if(NOT TARGET e2fsprogs::ext2fs)
    add_library(e2fsprogs::ext2fs UNKNOWN IMPORTED)
    set_target_properties(e2fsprogs::ext2fs PROPERTIES
      IMPORTED_LOCATION "${e2fsprogs_ext2fs_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${e2fsprogs_INCLUDE_DIR}"
    )
  endif()
endif()
