// Assignment name: argo
// Expected files: argo.c
// Allowed functions: getc, ungetc, printf, malloc, calloc, realloc, free, isdigit, fscanf, write
// -----------------
// Write a function argo that will parse a json file in the structure declared in argo.h:

// int	argo(json *dst, FILE *stream);

// 	dst	- is the pointer to the AST that you will create
// 	stream	- is the file to parse (man FILE)

// Your function will return 1 for success and -1 for failure.
// If an unexpected token is found you will print the following message in stdout: "Unexpected token '%c'\n"
// or if the token is EOF: "Unexpected end of input\n"
// Handle escaping in string only for backslashes and quotation marks.
// You don't have to handle spaces in the input (so they should be considered as invalid tokens).

// In case of doubt how to parse json, read rfc8259. But you won't need it as the format is simple. Tested with the main, the output should be exactly the same as the input (except for errors).
// There are some functions in argo.c that might help you.

// Examples that should work:

// echo -n input | ./argo /dev/stdin | cat -e		output
// ------------------------------------------		--------------------------------------------
// '1'							1$
// '"bonjour"'						"bonjour"$
// '"escape!\""'						"escape!\""$
// '{"tomatoes":42,"potatoes":234}'		    	{"tomatoes":42,"potatoes":234}$
// '{"recursion":{"recursion":{"recursion":"recursion"}}}'	{"recursion":{"recursion":{"recursion":"recursion"}}}$
// '"unfinished string'					Unexpected end of input$
// '"unfinished string 2\"'				Unexpected end of input$
// '{"no value?":}'					Unexpected token '}'$
// '{:12}'							Unexpected token ':'$
