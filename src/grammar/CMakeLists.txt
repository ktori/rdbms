add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lexer.c ${CMAKE_CURRENT_BINARY_DIR}/lexer.h
        COMMAND ${FLEX_EXECUTABLE} --header-file=${CMAKE_CURRENT_BINARY_DIR}/lexer.h -o ${CMAKE_CURRENT_BINARY_DIR}/lexer.c ${CMAKE_CURRENT_LIST_DIR}/lexer.l
        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/parser.h
        VERBATIM
)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/parser.c ${CMAKE_CURRENT_BINARY_DIR}/parser.h
        COMMAND ${BISON_EXECUTABLE} --defines=${CMAKE_CURRENT_BINARY_DIR}/parser.h --output=${CMAKE_CURRENT_BINARY_DIR}/parser.c ${CMAKE_CURRENT_LIST_DIR}/parser.y
        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/parser.y
        VERBATIM
)

set(GRAMMAR_GENERATED_SOURCES lexer.c lexer.h parser.c parser.h)
list(TRANSFORM GRAMMAR_GENERATED_SOURCES PREPEND ${CMAKE_CURRENT_BINARY_DIR}/)
set(GRAMMAR_SOURCES ${CMAKE_CURRENT_LIST_DIR}/ast.h ${CMAKE_CURRENT_LIST_DIR}/ast.c ${GRAMMAR_GENERATED_SOURCES})

add_custom_target(grammar DEPENDS ${GRAMMAR_SOURCES})

add_executable(grammar_debug)
target_sources(grammar_debug PRIVATE ${GRAMMAR_SOURCES} main.c)
target_link_libraries(grammar_debug PRIVATE fl)
target_include_directories(grammar_debug PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_BINARY_DIR})

foreach(S IN LISTS ${GRAMMAR_GENERATED_SOURCES})
    set_property(SOURCE ${S} PROPERTY GENERATED 1)
endforeach()

add_dependencies(${PROJECT_NAME} grammar)
target_sources(${PROJECT_NAME} PRIVATE ${GRAMMAR_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE fl)