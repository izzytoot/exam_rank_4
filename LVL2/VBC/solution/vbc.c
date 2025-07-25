#include "vbc.h"

node    *new_node(node n)
{
    node *ret = calloc(1, sizeof(n));
    if (!ret)
        return (NULL);
    *ret = n;
    return (ret);
}

void    destroy_tree(node *n)
{
    if (!n)
        return ;
    if (n->type != VAL)
    {
        destroy_tree(n->l);
        destroy_tree(n->r);
    }
    free(n);
}

void    unexpected(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of input\n");
}

int ch_balance(char *s)
{
	int n = 0;
	int i = -1;

	while (s[++i])
	{
		if (s[i] == '(')
			n++;
		else if (s[i] == ')')
		{
			n--;
			if (n < 0)
				return (printf("Unexpected token ')'\n"), -1);
		}
	}
	if (n != 0)
		return (printf("Unexpected token '('\n"), -1);
	else
		return (0);
}

node *parse_nb_or_group(char **s)
{
	node *res;
	node tmp;

	if (**s == '(')
	{
		(*s)++;
		res = parse_add(s);
		if (!res)
			return (NULL);
		if (**s != ')')
		{
			destroy_tree(res);
			unexpected(**s);
			return (NULL);
		}
		(*s)++;
		return (res);
	}
	if (isdigit(**s))
	{
		tmp.type = VAL;
		tmp.val = **s - '0';
		res = new_node(tmp);
		if (!res)
			return (NULL);
		(*s)++;
		return(res);
	}
	unexpected(**s);
	return (NULL);
}

node *parse_mult(char **s)
{
	node *l;
	node *r;
	node tmp;

	l = parse_nb_or_group(s);
	if (!l)
		return (NULL);
	while (**s == '*')
	{
		(*s)++;
		r = parse_nb_or_group(s);
		if (!r)
		{
			destroy_tree(l);
			return (NULL);
		}
		tmp.type = MULTI;
		tmp.l = l;
		tmp.r = r;
		l = new_node(tmp);
		if (!l)
			return (NULL);
	}
	return (l);
}

node *parse_add(char **s)
{
	node *l;
	node *r;
	node tmp;

	l = parse_mult(s);
	if (!l)
		return (NULL);
	while (**s == '+')
	{
		(*s)++;
		r = parse_mult(s);
		if (!r)
		{
			destroy_tree(l);
			return (NULL);
		}
		tmp.type = ADD;
		tmp.l = l;
		tmp.r = r;
		l = new_node(tmp);
		if (!l)
			return (NULL);
	}
	return (l);
}

int eval_tree(node *tree)
{
    switch (tree->type)
    {
        case ADD:
            return (eval_tree(tree->l) + eval_tree(tree->r));
        case MULTI:
            return (eval_tree(tree->l) * eval_tree(tree->r));
        case VAL:
            return (tree->val);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
	{
        return (1);
	}
	char *input = argv[1];
	if (ch_balance(input) == -1)
	{
		return (1);
	}	
    node *tree = parse_add(&input);
    if (!tree)
	{
        return (1);
	}
	if (*input)
	{
		unexpected(*input);
		destroy_tree (tree);
		return (1);
	}
	printf("%d\n", eval_tree(tree));
    destroy_tree(tree);
	return (0);
}