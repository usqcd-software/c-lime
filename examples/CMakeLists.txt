foreach(prog lime_contents
             lime_extract_record
             lime_extract_type
             lime_pack
             lime_unpack
             lime_writer_test1)

add_executable(${prog} "${prog}.c"  )
target_link_libraries(${prog} lime m)
set_target_properties(${prog} PROPERTIES C_STANDARD 99)
set_target_properties(${prog} PROPERTIES C_EXTENSIONS OFF)

install(TARGETS ${prog} DESTINATION bin )
endforeach()