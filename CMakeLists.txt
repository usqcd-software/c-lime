cmake_minimum_required(VERSION 3.11)
project(C-Lime VERSION 2.53)

#Boiler Plate for installation
include(GNUInstallDirs)
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckTypeSize)
#Boiler Plate for Testing
include(CTest)

#Options
option(LIME_LARGEGFILE "Enable Large File Support" OFF)

### FIXME: Do we still need this?
option(LIME_DCAP "Ebable DCache DCap Support" OFF)

if( LIME_DCAP )

  check_include_file("dcap.h" HAVE_DCAP_H)
  if( NOT HAVE_DCAP_H )
    message(FATAL_ERROR "DCAP Headers not found")
  endif()
endif()

check_symbol_exists("fseeko" "stdio.h" HAVE_FSEEKO)

check_include_file("stdint.h" HAVE_STDINT_H)
check_include_file("memory.h" HAVE_MEMORY_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)
check_include_file("stdlib.h" HAVE_STDLIB_H)
check_include_file("strings.h" HAVE_STRINGS_H)
check_include_file("string.h" HAVE_STRING_H)
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file("unistd.h" HAVE_UNISTD_H)

# Have symbols
check_type_size("uint16_t" UINT16_T)
check_type_size("uint32_t" UINT32_T)
check_type_size("uint64_t" UINT64_T)

# Size symbols 
check_type_size("unsigned char" SIZEOF_UNSIGNED_CHAR)
check_type_size("unsigned int" SIZEOF_UNSIGNED_INT)
check_type_size("unsigned long" SIZEOF_UNSIGNED_LONG)
check_type_size("unsigned long long" SIZEOF_UNSIGNED_LONG_LONG)
check_type_size("unsigned short" SIZEOF_UNSIGNED_SHORT)

configure_file(include/lime_config_internal.cmake.h.in include/lime_config_internal.h)


add_subdirectory(lib)
add_subdirectory(examples)

# FIXME do PM generation
# add_subdirecotry(doc)

#install the targets and add into the Export LimeTargets 
install(TARGETS lime EXPORT C-LimeTargets
  ARCHIVE DESTINATION lib
  LIBRARY  DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include)
 
#instal the include directory 
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#install the export description of the targets
install(EXPORT C-LimeTargets DESTINATION lib/cmake/C-Lime)

#write a Version file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    C-LimeVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
    )


# Using this macro we can pass some path vars down...
# But we are not doing this for now.
configure_package_config_file(C-LimeConfig.cmake.in C-LimeConfig.cmake INSTALL_DESTINATION lib/cmake/C-Lime)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/C-LimeVersion.cmake ${CMAKE_CURRENT_BINARY_DIR}/C-LimeConfig.cmake 
        DESTINATION lib/cmake/C-Lime)

export(TARGETS lime FILE C-LimeTargets.cmake)
