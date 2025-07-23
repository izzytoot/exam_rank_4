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
int	accept(FILE *stream, char c)
{
	if (peek(stream) == c)
	{
		(void)getc(stream);
		return 1;
	}
	return 0;
}

//given
int	expect(FILE *stream, char c)
{
	if (accept(stream, c))
		return 1;
	unexpected(stream);
	return 0;
}

//added
int parse_int(json *dst, FILE *stream)
{
	int n = 0;

	fscanf(stream, "%d", &n);
	dst->type = INTEGER;
	dst->integer = n;
	return (1);
}

//added
char *get_str(FILE *stream)
{
	char *res = calloc(4096, sizeof(char));
	int i = 0;
	char c = getc(stream);

	while (1)
	{
		c = getc(stream);

		if (c == '"')
			break;
		if (c == EOF)
		{
			unexpected(stream);
			return (NULL);
		}
		if (c == '\\')
			c = getc(stream);
		res[i++] = c;
	}
	return (res);
}

//added
int	parse_map(json *dst, FILE *stream)
{
	dst->type = MAP;
	dst->map.size = 0;
	dst->map.data = NULL;
	char c = getc(stream);

	if (peek(stream) == '}')
		return (1);
	
	while (1)
	{
		c = peek(stream);
		if (c != '"')
		{
			unexpected(stream);
			return (-1);
		}
		dst->map.data = realloc(dst->map.data, (dst->map.size + 1) * sizeof(pair));
		pair *curr = &dst->map.data[dst->map.size];
		curr->key = get_str(stream);
		if (!curr->key)
			return (-1);
		dst->map.size++;
		if (expect(stream, ':') == 0)
			return (-1);
		if (argo(&curr->value, stream) == -1)
			return (-1);
		c = peek(stream);
		if (c == '}')
		{
			accept(stream, c);
			break;
		}
		if (c == ',')
			accept(stream, ',');
		else
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
	int c = peek(stream);

	if (c == EOF)
	{
		unexpected(stream);
		return (-1);
	}
	if (isdigit(c)) //if number - parse int
		return (parse_int(dst, stream));
	else if (c == '"') //if string - get str
	{
		dst->type = STRING;
		dst->string = get_str(stream);
		if (dst->string == NULL)
			return (-1);
		return (-1);
	}
	else if (c == '{') //if map - parse map
		return (parse_map(dst, stream));
	else
	{
		unexpected(stream);
		return (-1);
	}
	return (1);
}

int		argo(json *dst, FILE *stream)
{
	if (parser(dst, stream) == -1)
		return (-1);
	return (1);
}