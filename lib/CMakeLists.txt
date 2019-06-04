set(LIME_SRC
	lime_writer.c
	lime_reader.c
	lime_header.c
	lime_utils.c)
	
if( NOT HAVE_FSEEKO )
 list(APPEND LIME_SRC lime_fseeko.c)
endif()

add_library(lime ${LIME_SRC})
	
target_include_directories(lime PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  $<INSTALL_INTERFACE:include>)

# generated includes
target_include_directories(lime PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
  $<INSTALL_INTERFACE:include>)
  
install(TARGETS lime EXPORT LIMEConfig
  ARCHIVE DESTINATION lib
  LIBRARY  DESTINATION lib
  RUNTIME DESTINATION lib)
  
install(EXPORT LIMEConfig DESTINATION cmake)
export(TARGETS lime FILE LIMEConfig.cmake )