target_sources(${PROJECT_NAME} PRIVATE main.c)

add_subdirectory(grammar)
add_subdirectory(table)
add_subdirectory(server)