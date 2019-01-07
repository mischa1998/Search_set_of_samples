#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

struct Node;

struct nod {
	char *key;
	struct Node *a;
	struct nod *left;
	struct nod *right;
};

struct tr {
	struct nod *root;
	int count;
};

typedef struct nod nod;
typedef struct tr tr;

struct Node {
	bool terminal;
	int number;
	int len;
	tr *links;
	struct Node* parent;
	char* to_parent;
	struct Node* sufflink;
	struct Node* exitlink;
};

struct bor {
	struct Node *root;
	int count;
};

tr* tr_create()
{
	tr *t = malloc(sizeof(tr));
	t->root = NULL;
	t->count = 0;
	return t;
}

void insert(tr *t, char *key, struct Node *a)
{
	//printf("%d\n", *a);
	if (!t->root) {
		t->root = malloc(sizeof(nod));
		t->root->key = malloc(sizeof(char) * (strlen(key) + 1));
		strcpy(t->root->key, key);
		t->root->a = a;
		t->root->left = NULL;
		t->root->right = NULL;
		//printf("%d\n", *(t->root->a));
		return;
	}
	nod *n = t->root;
	nod *parent;
	while (n) {
		if (strcmp(key, n->key) < 0) {
			parent = n;
			n = n->left;
		} else {
			parent = n;
			n = n->right;
		}
	}
	n = malloc(sizeof(nod));
	n->left = NULL;
	n->right = NULL;
	n->key = malloc(sizeof(char) * (strlen(key) + 1));
	n->a = a;
	strcpy(n->key, key);
	if (strcmp(n->key, parent->key) < 0) {
		parent->left = n;
	} else {
		parent->right = n;
	}
}


nod* search(tr *t, char *key)
{
	if (!t) {
		printf("Error, node not exist\n");
	}
	if (!t->root) {
		return NULL;
	}
	nod *n = t->root;
	while (n) {
		if (strcmp(key, n->key) < 0) {
			n = n->left;
			continue;
		}
		if (strcmp(key, n->key) > 0) {
			n = n->right;
			continue;
		}
		if (!strcmp(key, n->key)) {
			return n;
		}
	}
	return NULL;
}

void delete_node(nod *a)
{
	if (a->left) {
		delete_node(a->left);
	}
	if (a->right) {
		delete_node(a->right);
	}
	free(a->key);
	free(a);
}


void delet(tr *t)
{
	if (!t->root) {
		free(t);
		return;
	}
	delete_node(t->root);
	free(t);
}


typedef struct bor tree;
typedef struct Node node;

typedef node* TYPE;

struct Q_Node {
	TYPE t;
	struct Q_Node *next;
};



struct queue{
	struct Q_Node *begin;
	struct Q_Node *end;
	int size;
};
//реализация очереди для обхода в ширину вершин построенного "бора"
typedef struct Q_Node q_node;
typedef struct queue Qu;

Qu* q_create()
{
	Qu *m = (Qu*)malloc(sizeof(Qu));
	m->size = 0;
	m->begin = NULL;
	m->end = NULL;
	return m;
}


void push_qu(Qu *t, TYPE value)
{
	if (!t->begin) {
		q_node *n = (q_node*)malloc(sizeof(q_node));
		n->t = value;
		t->begin = n;
		t->end = n;
		return;
	}
	q_node *n = (q_node*)malloc(sizeof(q_node));
	n->t = value;
	t->end->next = n;
	t->end = n;
}

void pop_qu(Qu *t)
{
	if (t->begin == t->end) {
		free(t->begin);
		t->begin = NULL;
		t->end = NULL;
		return;
	}
	q_node *n = t->begin->next;
	free(t->begin);
	t->begin = n;
}

TYPE top_qu(Qu *t)
{
	return t->begin->t;
}

bool is_empty(Qu *t)
{
	return (t->begin ==  NULL && t->end == NULL);
}

void qu_delete(Qu *t)
{
	free(t);
}



tree* create()
{
	tree *m = (tree*)malloc(sizeof(tree));
	m->root = NULL;
	m->count = 0;
	return m;
}

int count = 0;

void add_to_bor(tree *t, char **str, int size)
{
	if (!(t->root)) {
		//printf("Create root\n");
		t->root = malloc(sizeof(node));
		t->root->links = tr_create();
		t->root->sufflink = t->root;
		t->root->exitlink = NULL;
		t->root->terminal = false;
		t->root->parent = t->root;
		t->root->to_parent = NULL;
	}
	int i = 0;
	node *n = t->root;
	node *parent;
	node *b;
	int len = 0;
	for (i = 0; i < size; i++) {
		len += strlen(str[i]);
		nod *r = search(n->links, str[i]);
		if (!r) {
			b = malloc(sizeof(node));
			b->links = tr_create();
			b->number = 0;
			b->terminal = false;
			insert(n->links, str[i], b);
			b->to_parent = malloc(sizeof(char) * (strlen(str[i]) + 1));
			strcpy(b->to_parent, str[i]);
			parent = n;
			n = b;
			n->parent = parent;
		} else {
			n = r->a;
		}
	}
	n->terminal = true;
	n->len = size;
	n->number = count++;
	t->count++;
}
//функция добавляет в очередь всех сыновей переданного узла "бора"
void toq(Qu *q, nod *n)
{
	push_qu(q, n->a);
	if (n->left) {
		toq(q, n->left);
	}
	if (n->right) {
		toq(q, n->right);
	}
}

//функция проставляет ссылки перехода, обохдя "бор" в ширину
void passlink(tree *t)
{
	Qu *q = q_create();
	push_qu(q, t->root);
	char c = '\0';
	while(!is_empty(q)) {		
		node *n = top_qu(q);
		pop_qu(q);
		char *a = n->to_parent;
		if (!a) {
			a = &c;
		}
		node *parent = n->parent;
		node *v = parent->sufflink;		
		nod *e = search(v->links, a);
		node *g;
		if (!e) {
			g = NULL;
		} else {
			g = e->a;
		}
		while (!g && v != t->root) {
			v = v->sufflink;
			e = search(v->links, a);
			if (!e) {
				g = NULL;
			} else {
				g = e->a;
			}
		}
		if (parent == t->root) {
			n->sufflink = t->root;
		} else if (g) {
			n->sufflink = g;
		} else {
			n->sufflink = t->root;
		}
		if (n->links->root) {
			toq(q, n->links->root);
		}
		
	}
	qu_delete(q);
}
//функция проставляет ссылки выхода, они срабатывают если один из образцов является подстрокой другого
void exitlink(tree *t)
{
	Qu *q = q_create();
	push_qu(q, t->root);		
	while (!is_empty(q)) {
		node *n = top_qu(q);
		pop_qu(q);
		node *v = n->sufflink;
		if (v->terminal) {
			n->exitlink = v;
		} else if (v->exitlink) {
			n->exitlink = v->exitlink;
		} else {
			n->exitlink = NULL;
		}
		if (n->links->root) {
			toq(q, n->links->root);
		}
	}
	qu_delete(q);
}
//функция приведения строки к нижнему регистру(преподаватель требовал)
void to_str(char *str)
{
	int i = 0;
	while (str[i] != '\0') {
		str[i] = (char)tolower(str[i]);
		i++;
	}
}



typedef struct {
	int *mas;
} stroka;

void compound(stroka *s, int str, int number, int len)
{
	int res = s->mas[str] - len;
	if (res >= 0) {
		printf("%d, %d, %d\n", str, res + 1, number + 1);
		return;
	}
	int ost = len;
	while (res < 0) {
		ost -= s->mas[str];
		str--;
		res = s->mas[str] - ost;
	}
	printf("%d, %d, %d\n", str, res + 1, number + 1);
}


void searching (tree *p)
{
	char **in = (char**)malloc(sizeof(char*) * 5);
	char c;
	node *n = p->root;
	nod *e;
	char str[1024];
	int size = 0;
	stroka *s = calloc(1, sizeof(stroka));
	s->mas = calloc(5, sizeof(int));
	int i = 0;
	int string = 1;
	int flag = 0;
	while (1) {
		while (1) {
			if (string % 5 == 0) {
				s->mas = realloc(s->mas, (string + 5) * sizeof(int));
			}
			c = getchar();
			while (c == ' ') {
				c = getchar();
			}
			if ((c == '\n') || (c == EOF)) {
				break;
			}
			ungetc(c, stdin);
			if (size % 5 == 0) {
				in = (char**)realloc(in, sizeof(char *) * (size + 5));
			}
			scanf("%s", str);
			to_str(str);
			s->mas[string]++;
			in[size] = (char *)malloc(sizeof(char) * (strlen(str) + 1));
			memcpy(in[size], str, strlen(str) + 1);
			size++;
		}
		if (c == EOF) {
			flag = 1;
		}
		s->mas[string] = 0;
		for (i = 0; i < size; i++) {
			s->mas[string] = i + 1; 
			e = search(n->links, in[i]);
			if (e) {
				n = e->a;
				if (n->terminal) {
					compound(s, string, n->number, n->len);
				}
				if (n->exitlink) {
					node *e = n->exitlink;
					while (e) {
						compound(s, string, e->number, e->len);
						e = e->exitlink;
					}
				}
			} else {
				if (n != p->root) {
					i--;					
				}
				n = n->sufflink;
			}
		}
		for (i = 0; i < size; i++) {
			free(in[i]);
		}
		size = 0;
		string++;
		if (flag == 1) {
			break;
		}
	}
	free(s->mas);
	free(s);
	free(in);
}

void delete_bor(tree *p)
{
	Qu *q = q_create();
	push_qu(q, p->root);
	while (!is_empty(q)) {
		node *n = top_qu(q);
		pop_qu(q);
		if (n->links->root) {
			toq(q, n->links->root);
		}
		delet(n->links);
		free(n->to_parent);
		free(n);
	}
	qu_delete(q);
	free(p);
	
}


		

int main() {
	tree *p = create();
	char str[1024];
	int i = 0;
	char ** obr = (char**)malloc(sizeof(char*) * 5);
	i = 0;
	char c;
	int size = 0;
	while (1) {
		c = getchar();
		if (i % 5 == 0) {
			obr = (char**)realloc(obr, sizeof(char*) * (i + 5));
		}
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
			ungetc(c, stdin);
			scanf("%s", str);
			to_str(str);
			obr[i] = (char*)malloc(sizeof(char) * (strlen(str) + 1));
			memcpy(obr[i], str, strlen(str) + 1);
			i++;
		}
		if (c == '\n') {
			add_to_bor(p, obr, i);
			size = i;
			for (i = 0; i < size; i++) {
				free(obr[i]);
				obr[i] = NULL;
			}
			i = 0;
			c = getchar();
			if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
				ungetc(c, stdin);
			} else if (c == '\n') {
				for (i = 0; i < size; i++) {
					free(obr[i]);
					obr[i] = NULL;
				}
				break;
			}
		}
	}
	free(obr);

	passlink(p);
	exitlink(p);

	searching(p);
	delete_bor(p);

	return 0;
}
