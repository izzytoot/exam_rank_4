#ifndef ARGO_H
# define ARGO_H

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>


typedef struct	json {
	enum {
		MAP,
		INTEGER,
		STRING
	} type;
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
int 	parse_int(json *dst, FILE *stream);
char 	*get_str(FILE *stream);
int 	parse_map(json *dst, FILE *stream);
int 	parser(json *dst, FILE *stream);
int		argo(json *dst, FILE *stream);

#endif