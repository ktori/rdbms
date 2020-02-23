target_sources(${PROJECT_NAME} PRIVATE main.c)

add_subdirectory(grammar)
add_subdirectory(table)
add_subdirectory(server)
add_subdirectory(query)
add_subdirectory(lib)
add_subdirectory(storage)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})