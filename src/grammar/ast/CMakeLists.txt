set(AST_SOURCES
        ast.h
        select.c select.h
        statement.c statement.h
        insert.c insert.h
        literal.c literal.h
        create_table.h create_table.c
        shared.c shared.h
        yystype.h
        )

list(TRANSFORM AST_SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/)
set(AST_SOURCES ${AST_SOURCES} PARENT_SCOPE)