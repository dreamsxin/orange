#include "parser.h"

/*
	start 			::=		statements 
	statements	::= 	statement statements
	statement 	::= 	function | expression | declaration | $ 

	function		::= 	'def' opt_id statements 'end'
	opt_id			::= 	ID opt_parens | '(' opt_args ')' 
	opt_parens 	::=		'(' opt_args ')' | $ 
	opt_args		::=		'(' args ')' | $ 
	args 				::=		arg 
	arg 				::=		opt_type ID opt_comma 
	opt_type		::=		TYPE | $ 
	opt_comma		::=		',' args	

	declaration	::=		TYPE decl_assign opt_assigns 
	opt_assigns	::=		',' decl_assign opt_assigns | $
	decl_assign	::=		ID opt_assign
	opt_assign  ::=		assign_ops expression | $
	assign_ops	::=		'=' | '<-'
	
	expression	::= 	assign_expr	
	assign_expr ::=		add_expr ( ('=' | '+=' | '-=' | '*=' | '/=' | '%=') add_expr) * 
	add_expr	 	::=		mult_expr ( ('-' | '+') mult_expr) *
	mult_expr		::= 	primary ( ('*' | '/') primary ) *
	primary			::= 	'(' expression ')' | NUMBER | ID 		 
*/