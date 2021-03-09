add_library(lime)
target_sources(lime PRIVATE
	lime_writer.c
	lime_reader.c
	lime_header.c
	lime_utils.c
)
	
if( NOT HAVE_FSEEKO )
 target_sources(lime PRIVATE lime_fseeko.c)
endif()

target_include_directories(lime PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  $<INSTALL_INTERFACE:include>)

# generated includes
target_include_directories(lime PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
  $<INSTALL_INTERFACE:include>)

if( LIME_SANITIZER_OPTS )
  target_compile_options(lime PUBLIC ${LIME_SANITIZER_OPTS} )
  target_link_options(lime PUBLIC ${LIME_SANITIZER_OPTS} )
endif()

install(TARGETS lime 
	EXPORT CLimeTargets
  	ARCHIVE DESTINATION lib
  	LIBRARY  DESTINATION lib
  	RUNTIME DESTINATION bin
  	INCLUDES DESTINATION include
 )

#install the export description of the targets
install(EXPORT CLimeTargets
		FILE CLimeTargets.cmake
		NAMESPACE CLime::
		DESTINATION lib/cmake/CLime)  

add_library(CLime::lime ALIAS lime)
