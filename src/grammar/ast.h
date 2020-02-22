struct name_ast_node_s
{
  char *name;
};

struct name_ast_node_s *ast_name(const char *name);

struct name_list_ast_node_s
{
  struct name_ast_node_s **array;
  unsigned count;
  unsigned size;
};

struct name_list_ast_node_s *ast_name_list_add(struct name_list_ast_node_s *list, struct name_ast_node_s *name);

struct from_ast_node_s
{
  struct name_ast_node_s *name;
};

struct from_ast_node_s *ast_from(struct name_ast_node_s *name_node);

struct select_ast_node_s
{
  struct name_list_ast_node_s *columns;
  struct from_ast_node_s *from;
};

struct select_ast_node_s *ast_select(struct name_list_ast_node_s *columns, struct from_ast_node_s *from);

void select_print(struct select_ast_node_s *select);