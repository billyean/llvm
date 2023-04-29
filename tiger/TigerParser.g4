parser grammar TigerParser;

tiger_program
  :  MAIN LET declaration_segment IN BEGIN stat_seq END
  ;

declaration_segment
  :  type_declaration_list var_declaration_list funct_declaration_list
  ;

type_declaration_list
  :type_declaration type_declaration_list
  |
  ;

var_declaration_list
  :  var_declaration var_declaration_list
  |
  ;

funct_declaration_list
  :  funct_declaration  funct_declaration_list
  |
  ;

type_declaration
  :  TYPE ID TASSIGN type SEMICOLON
  ;

type
  :  type_id                                              # internalType
  |  ARRAY OPENBRACK size=INTLIT CLOSEBRACK OF type_id    # arrayType
  |  ID                                                   # redefineType
  ;

type_id
  :  dt=INT
  |  dt=FLOAT
  ;

var_declaration
  :  VAR id_list COLON type optional_init SEMICOLON
  ;

id_list
  :  ID id_list_lr
  ;

id_list_lr
  :  COMMA ID id_list_lr
  |
  ;

optional_init
  :  ASSIGN const_value
  |
  ;

funct_declaration
  :  funct_declaration_header BEGIN funct_declaration_body END SEMICOLON
  ;

funct_declaration_header
  :  FUNCTION ID OPENPAREN param_list CLOSEPAREN ret_type
  ;

funct_declaration_body
  : stat_seq
  ;

param_list
  :  param param_list_tail
  |
  ;

param_list_tail
  :  COMMA param param_list_tail
  |
  ;

ret_type
  :  COLON type
  |
  ;

param
  :  ID COLON type
  ;

stat_seq
  :  stat stat_seq_lr
  ;

stat_seq_lr
  : stat_seq
  |
  ;

stat
  :  lvalue ASSIGN lrvalue SEMICOLON                           # LAssign
  |  stat_if_condition THEN stat_seq if_stat_tail              # If
  |  stat_while_condition DO stat_seq ENDDO SEMICOLON          # While
  |  stat_for_condition DO stat_seq ENDDO SEMICOLON            # For
  |  ID OPENPAREN expr_list CLOSEPAREN SEMICOLON               # Procedure
  |  BREAK SEMICOLON                                           # Break
  |  RETURN expr SEMICOLON                                     # Return
  |  LET declaration_segment IN stat_seq END                   # Let
  ;

lrvalue
  : lvalue ASSIGN lrvalue                                      # LRAssign
  | expr                                                       # EAssign
  | ID OPENPAREN expr_list CLOSEPAREN                          # Function
  ;

stat_if_condition
  : IF condition_expr
  ;

stat_while_condition
  : WHILE condition_expr
  ;

stat_for_condition
  : FOR for_assign TO expr
  ;

for_assign
  :
  for_lvalue ASSIGN expr
  ;

if_stat_tail
  : ELSE stat_seq ENDIF SEMICOLON
  | ENDIF SEMICOLON
  ;

condition_expr
  : expr
  ;

expr
  :  <assoc=right> expr POW expr                               # Power
  |  expr mult_operator expr                                   # Multiplicative
  |  expr plus_operator expr                                   # Plus
  |  expr compare_operator expr                                # Compare
  |  expr AND expr                                             # And
  |  expr OR expr                                              # Or
  |  single_value                                              # Single
  ;

const_value
  :  INTLIT                                                    # Intlit
  |  FLOATLIT                                                  # Floatlit
  ;

compare_operator
  :  EQUAL
  |  NEQUAL
  |  LESS
  |  GREAT
  |  LESSEQ
  |  GREATEQ
  ;

plus_operator
  :  PLUS
  |  MINUS
  ;

mult_operator
  :  MULT
  |  DIV
  ;

expr_list
  :  expr expr_list_tail
  |
  ;

expr_list_tail
  :  COMMA expr expr_list_tail
  |
  ;

lvalue
  :  ID lvalue_tail
  ;

lvalue_tail
  :  OPENBRACK expr CLOSEBRACK
  |
  ;

for_lvalue
  : ID
  ;

single_value
  :  rvariable
  |  const_value
  |  OPENPAREN expr CLOSEPAREN
  ;

rvariable
  :  ID rvalue_tail
  ;

rvalue_tail
  :  OPENBRACK expr CLOSEBRACK
  |
  ;