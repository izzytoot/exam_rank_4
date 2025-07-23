#ifndef ARGO_H
# define ARGO_H

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>

//json is a node of the AST (tree)
//json can be INTERGER, STRING or MAP
//a map will store an array of *keys and values
//if json j is a map { "x": "hello" }, it would store:
	// j.type = MAP;
	// j.map.size = 1;
	// j.map.data[0].key = "x";
	// j.map.data[0].value.type = STRING;
	// j.map.data[0].value.string = "hello"

typedef struct	json {
	enum {
		MAP, //{"pos", 1}
		INTEGER, //42
		STRING //"hello"
	} type; //type of the node
	union {
		struct {
			struct pair	*data;
			size_t		size;
		} map;
		int	integer;
		char	*string;
	};
}	json;

typedef struct	pair {
	char	*key;
	json	value;
}	pair;

void	free_json(json j);
int		argo(json *dst, FILE *stream);
int		peek(FILE *stream);
void	unexpected(FILE *stream);
int		accept(FILE *stream, char c);
int		expect(FILE *stream, char c);

#endif