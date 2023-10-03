cmake_minimum_required(VERSION 3.0)

target_compile_options(cat PUBLIC -Wall -Wextra -Wconversion)

add_executable(test_parse test_parse.c)
target_link_libraries(test_parse cat)

add_test(test_parse ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_parse)

add_executable(test_run test_run.c)
target_link_libraries(test_run cat)
add_test(test_run ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_run)

add_executable(test_read test_read.c)
target_link_libraries(test_read cat)
add_test(test_read ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_read)

add_executable(test_write test_write.c)
target_link_libraries(test_write cat)
add_test(test_write ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_write)

add_executable(test_write_parse test_write_parse.c)
target_link_libraries(test_write_parse cat)
add_test(test_write_parse ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_write_parse)

add_executable(test_write_int_range test_write_int_range.c)
target_link_libraries(test_write_int_range cat)
add_test(test_write_int_range
         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_write_int_range)

add_executable(test_write_uint_range test_write_uint_range.c)
target_link_libraries(test_write_uint_range cat)
add_test(test_write_uint_range
         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_write_uint_range)

add_executable(test_write_hex_range test_write_hex_range.c)
target_link_libraries(test_write_hex_range cat)
add_test(test_write_hex_range
         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_write_hex_range)

add_executable(test_write_hex_buffer test_write_hex_buffer.c)
target_link_libraries(test_write_hex_buffer cat)
add_test(test_write_hex_buffer
         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_write_hex_buffer)

add_executable(test_write_string_buffer test_write_string_buffer.c)
target_link_libraries(test_write_string_buffer cat)
add_test(test_write_string_buffer
         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_write_string_buffer)

add_executable(test_read_args test_read_args.c)
target_link_libraries(test_read_args cat)
add_test(test_read_args ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_read_args)

add_executable(test_test test_test.c)
target_link_libraries(test_test cat)
add_test(test_test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_test)

add_executable(test_test_args test_test_args.c)
target_link_libraries(test_test_args cat)
add_test(test_test_args ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_test_args)

add_executable(test_mutex test_mutex.c)
target_link_libraries(test_mutex cat)
add_test(test_mutex ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_mutex)

add_executable(test_unsolicited_read test_unsolicited_read.c)
target_link_libraries(test_unsolicited_read cat)
add_test(test_unsolicited_read
         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_unsolicited_read)

add_executable(test_unsolicited_read_stress test_unsolicited_read_stress.c)
target_link_libraries(test_unsolicited_read_stress cat)
add_test(test_unsolicited_read_stress
         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_unsolicited_read_stress)

add_executable(test_unsolicited_read_buffer test_unsolicited_read_buffer.c)
target_compile_definitions(test_unsolicited_read_buffer
                           PRIVATE "CAT_UNSOLICITED_CMD_BUFFER_SIZE=2")
target_link_libraries(test_unsolicited_read_buffer cat)
add_test(test_unsolicited_read_buffer
         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_unsolicited_read_buffer)

add_executable(test_hold_state test_hold_state.c)
target_link_libraries(test_hold_state cat)
add_test(test_hold_state ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_hold_state)

add_executable(test_return_read test_return_read.c)
target_link_libraries(test_return_read cat)
add_test(test_return_read ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_return_read)

add_executable(test_return_write test_return_write.c)
target_link_libraries(test_return_write cat)
add_test(test_return_write ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_return_write)

add_executable(test_unsolicited_test test_unsolicited_test.c)
target_link_libraries(test_unsolicited_test cat)
add_test(test_unsolicited_test
         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_unsolicited_test)

add_executable(test_return_test test_return_test.c)
target_link_libraries(test_return_test cat)
add_test(test_return_test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_return_test)

add_executable(test_return_run test_return_run.c)
target_link_libraries(test_return_run cat)
add_test(test_return_run ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_return_run)

add_executable(test_test_only test_test_only.c)
target_link_libraries(test_test_only cat)
add_test(test_test_only ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_test_only)

add_executable(test_search_cmd test_search_cmd.c)
target_link_libraries(test_search_cmd cat)
add_test(test_search_cmd ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_search_cmd)

add_executable(test_order test_order.c)
target_link_libraries(test_order cat)
add_test(test_order ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_order)

add_executable(test_cmd_list test_cmd_list.c)
target_link_libraries(test_cmd_list cat)
add_test(test_cmd_list ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_cmd_list)

add_executable(test_var_access test_var_access.c)
target_link_libraries(test_var_access cat)
add_test(test_var_access ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_var_access)

add_executable(test_shortcuts test_shortcuts.c)
target_link_libraries(test_shortcuts cat)
add_test(test_shortcuts ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_shortcuts)

add_executable(test_implicit_write test_implicit_write.c)
target_link_libraries(test_implicit_write cat)
add_test(test_implicit_write
         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_implicit_write)
