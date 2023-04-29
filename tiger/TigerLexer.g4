lexer grammar TigerLexer;

options { caseInsensitive = true; }

ARRAY : 'array' ;
BEGIN : 'begin' ;
BREAK : 'break' ;
DO : 'do' ;
ELSE : 'else' ;
END : 'end' ;
ENDDO : 'enddo' ;
ENDIF : 'endif' ;
FLOAT : 'float' ;
FOR : 'for' ;
FUNCTION : 'function' ;
IF : 'if' ;
IN : 'in' ;
INT : 'int' ;
LET : 'let' ;
MAIN : 'main' ;
OF : 'of' ;
RETURN : 'return' ;
THEN : 'then' ;
TO : 'to' ;
TYPE : 'type' ;
VAR : 'var' ;
WHILE : 'while' ;
COMMA : ',' ;
DOT : '.' ;
COLON : ':' ;
SEMICOLON : ';' ;
OPENPAREN : '(' ;
CLOSEPAREN : ')' ;
OPENBRACK : '[' ;
CLOSEBRACK : ']' ;
OPENCURLY: '{' ;
CLOSECURLY: '}' ;
PLUS: '+';
MINUS: '-';
MULT: '*';
DIV: '/';
LESS: '<';
GREAT: '>';
TASSIGN: '=';
AND: '&';
OR: '|';
EQUAL: '==';
NEQUAL: '!=';
LESSEQ: '<=';
GREATEQ: '>=';
ASSIGN: ':=';
POW: '**';

ID : [a-zA-Z][a-zA-Z0-9_]* ;
INTLIT : [1-9][0-9]* | [0];
FLOATLIT : [0-9]+[.][0-9]*;
COMMENT: '/*' .*? '*/' -> skip;
WS  : [ \t\r\n]+ -> skip;