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

#include "argo.h"

//given
//reads the next char without removing it from the input.
int	peek(FILE *stream)
{
	int	c = getc(stream); //reads the next char and returns it
	ungetc(c, stream); //pushes c back to stream
	return c;
}

//given
// prints the error message
void	unexpected(FILE *stream)
{
	if (peek(stream) != EOF) //if a bad char appears before the EOF
		printf("unexpected token '%c'\n", peek(stream));
	else //if file ends too early
		printf("unexpected end of input\n");
}

//given
// peeks next char and consumes (moves forward)
int	accept(FILE *stream, char c)
{
	if (peek(stream) == c) //read next char
	{
		(void)getc(stream); //advance
		return 1;
	}
	return 0;
}

//given
// accepts and, if error, prints error message
int	expect(FILE *stream, char c)
{
	if (accept(stream, c)) //if next char is c -> good!
		return 1;
	unexpected(stream); //print error
	return 0;
}

//added
//parses an interger and stores it in the AST
int parse_int(json *dst, FILE *stream)
{
	int n = 0;

	fscanf(stream, "%d", &n); //scans input (stream) according to format ("%d") and changes n to strem
	dst->type = INTEGER; //attributes type
	dst->integer = n; //atributes value
	return (1);
}

//added
// parses a string that starts with '"' (str[0])
char *get_str(FILE *stream)
{
	char *res = calloc(4096, sizeof(char));
	int i = 0;
	char c = getc(stream); //gets 1st char (str[0])

	while (1) //loop until "
	{
		c = getc(stream);

		if (c == '"') //if it sees the last " it stops
			break;
		if (c == EOF) //if it finds EOF it prints error and stops
		{
			unexpected(stream); //prints error
			return (NULL); //returns null
		}
		if (c == '\\')
			c = getc(stream); //if it finds a '\' it ignores it and gets the next char
		res[i++] = c; //copies the str char by char into res
	}
	return (res);
}

//added
int	parse_map(json *dst, FILE *stream)
{ 
	//define type and init size and data
	dst->type = MAP;
	dst->map.size = 0;
	dst->map.data = NULL;
	char c = getc(stream); //get {

	if (peek(stream) == '}') //empty map
		return (1);
	
	while (1) //loop until }
	{
		c = peek(stream); //move passed {
		if (c != '"') //the key always starts with a "
		{
			unexpected(stream); //prints error
			return (-1); //returns -1 for error
		}
		//reallocate memory for data each loop round (map.size+1*sizeof(pair))
		dst->map.data = realloc(dst->map.data, (dst->map.size + 1) * sizeof(pair));
		pair *curr = &dst->map.data[dst->map.size]; //initiate a new pair at map.size
		curr->key = get_str(stream); //key is 1st str - get_str
		if (!curr->key)
			return (-1);
		if (expect(stream, ':') == 0) //if not : - error
			return (-1);
		if (argo(&curr->value, stream) == -1) //call argo to parse the value
			return (-1); //if -1 return error
		dst->map.size++; //increment size
		
		//after each pair...
		c = peek(stream);
		if (c == '}') //if } ->break
		{
			accept(stream, c);
			break;
		}
		if (c == ',') //if , we go again
			accept(stream, c);
		else //error
		{
			unexpected(stream);
			return (-1);
		}
	}
	return (1);	
}

//added
int parser(json *dst, FILE *stream)
{
	char c = peek(stream); //look at next char and decide

	if (c == EOF) //if EOF - print and error
	{
		unexpected(stream);
		return (-1);
	}
	if (isdigit(c)) //if number - parse int
		return (parse_int(dst, stream));
	else if (c == '"') //if string - define and get str
	{
		dst->type = STRING;
		dst->string = get_str(stream);
		if (!dst->string)
			return (-1);
		return (1);
	}
	else if (c == '{') //if map - parse map
		return (parse_map(dst, stream));
	else //error
	{
		unexpected(stream); //print error
		return (-1);
	}
	return (1);
}

int		argo(json *dst, FILE *stream)
{
	if (!dst || !stream)
		return (-1);
	if (parser(dst, stream) == -1)
		return (-1);
	return (1);
}