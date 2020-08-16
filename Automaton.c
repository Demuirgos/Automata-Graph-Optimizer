#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<conio.h>
#define max(a,b) ((a > b) ? a : b)
#define input(s,p,n) system("cls");char* p = (char*)malloc(n * sizeof(char));printf(s);scanf_s("%s",p,n)
#define isdigit(d) (d<59 && d>47)
typedef struct graph graph;
typedef struct node node;
typedef struct edge edge;
struct graph {
	char* name;
	node** map;
	int* map_check;
	char* lang;
	int len_lang;
	int count_n;
	int* starting_nodes;
	int* end_nodes;
};
struct node {
	int n = 0;
	int count_e;
	edge** e = NULL;
	bool processed = 0;
	int* clos;
	int clos_n;
};
struct edge {
	char c;
	node* n = NULL;
};
graph* create_g(graph* g) {
	g = (graph*)malloc(sizeof(graph));
	g->name = (char*)malloc(sizeof(char));
	g->count_n = 1;
	g->map = (node**)malloc(sizeof(node*));
	g->map_check = (int*)calloc(1, sizeof(int));
	g->lang = NULL;
	g->starting_nodes = (int*)malloc(sizeof(int)); g->starting_nodes[0] = 0;// g->starting_nodes[1] = 0;
	g->end_nodes = (int*)malloc(sizeof(int)); g->end_nodes[0] = 0;
	g->len_lang = 0;
	return g;
}
node* create_n(node* n) {
	n = (node*)malloc(sizeof(node));
	n->count_e = 0;
	n->e = NULL;
	n->processed = 0;
	n->clos = (int*)malloc(sizeof(int));
	n->clos[0] = n->n;
	n->clos_n = 1;
	return n;
}
edge* create_e(edge* e) {
	e = (edge*)malloc(sizeof(edge));
	e->c = '\0';
	e->n = NULL;
	return e;
}
int* expand(int* check, int l, int L) {
	int* t = (int*)calloc(L + 1, sizeof(int));
	for (int i = 0; i < l; i++) {
		t[i] = check[i];
	}
	return t;
}
void add_alph(graph* g, char c) {
	for (int i = 0; i < g->len_lang; i++) if (g->lang[i] == c) return;
	g->lang = (char*)realloc(g->lang, ++g->len_lang * sizeof(char));
	g->lang[g->len_lang - 1] = c;
}
void insert(graph* g, int s, int f, int c) {
	int old_ount = g->count_n;
	g->count_n = max(g->count_n, max(s, f) + 1);
	g->map = (node**)realloc(g->map, (g->count_n) * sizeof(node*));
	g->map_check = expand(g->map_check, old_ount, g->count_n);
	if (g->map_check[s] == 0) {
		g->map[s] = create_n(g->map[s]);
		g->map[s]->n = s;
		g->map[s]->clos[0] = s;
		g->map_check[s] = 1;
		g->map[s]->e = (edge**)realloc(g->map[s]->e, ++g->map[s]->count_e * sizeof(edge*));
		g->map[s]->e[g->map[s]->count_e - 1] = (edge*)malloc(sizeof(edge));
	}
	else {
		g->map[s]->e = (edge**)realloc(g->map[s]->e, ++g->map[s]->count_e * sizeof(edge*));
		g->map[s]->e[g->map[s]->count_e - 1] = (edge*)malloc(sizeof(edge));
	}
	if (g->map_check[f] == 0) {
		g->map[f] = create_n(g->map[f]);
		g->map_check[f] = 1;
		g->map[f]->clos[0] = f;
		g->map[f]->n = f;
	}
	g->map[s]->e[g->map[s]->count_e - 1]->c = c;
	g->map[s]->e[g->map[s]->count_e - 1]->n = g->map[f];
}
void remove(node* n) {
	for (int i = 0; i < n->count_e - 1; i++) {
		if (n->e[i]->c == '0') {
			n->e[i] = n->e[n->count_e - 1];
			n->count_e--;
			i--;
		}
	}
	for (int i = 0; i < n->count_e - 1; i++) {
		for (int j = i + 1; j < n->count_e; j++) {
			if (n->e[i]->c == n->e[j]->c && n->e[i]->n == n->e[j]->n) {
				n->e[j] = n->e[n->count_e - 1];
				n->count_e--;
				j--;
			}
		}
	}
}
bool not_in_clos(node* s, node* f) {
	for (int i = 0; i < s->clos_n; i++) {
		if (s->clos[i] == f->n) return 0;
	}
	return 1;
}
void Optimise_n(graph* g, int s, int c = 48) {
	if (g->map_check[s] && g->map[s]->processed == false) {
		int l = g->map[s]->count_e;
		for (int i = 0; i < l && !g->map[s]->processed; i++) {
			if (g->map[s]->e[i]->c == c && not_in_clos(g->map[s], g->map[s]->e[i]->n)) {
				g->map[s]->clos = (int*)realloc(g->map[s]->clos, ++g->map[s]->clos_n * sizeof(int));
				g->map[s]->clos[g->map[s]->clos_n - 1] = g->map[s]->e[i]->n->n;
				for (int l = 1; l <= g->end_nodes[0]; l++) {
					if (g->end_nodes[l] == g->map[s]->e[i]->n->n) {
						g->end_nodes = (int*)realloc(g->end_nodes, (++g->end_nodes[0]+1) * sizeof(int));
						g->end_nodes[g->end_nodes[0]] = s;
					}
				}
				if (g->map[s]->e[i]->n->count_e > 0) {
					for (int j = 0; j < g->map[s]->e[i]->n->count_e - 1; j++) {
						if (g->map[s]->e[i] != g->map[s]->e[i]->n->e[j]) {
							g->map[s]->e = (edge**)realloc(g->map[s]->e, ++g->map[s]->count_e * sizeof(edge*));
							g->map[s]->e[g->map[s]->count_e - 1] = g->map[s]->e[i]->n->e[j];
						}
					}
					g->map[s]->e[i] = g->map[s]->e[i]->n->e[g->map[s]->e[i]->n->count_e - 1];
				}
				Optimise_n(g, s);
			}
		}
		g->map[s]->processed = 1;
	}
}
void optimise(graph* g) {
	printf("Optimizing : ...\n");
	for (int i = 0; i < g->count_n; i++) {
		if (g->map_check[i]) {
			printf("	Processing Node  %d : ...\n", i);
			Optimise_n(g, i);
			printf("\t\t0_clos(%d): ", g->map[i]->n);
			for (int j = 0; j < g->map[i]->clos_n; j++) {
				printf(" %d %c", g->map[i]->clos[j], j != g->map[i]->clos_n - 1 ? '-' : '\0');
			}
			printf("\n	-Processing Node %d : Done!\n", i);
		}
	}
	printf("	Post_Processing Nodes : ...\n");
	for (int i = 0; i < g->count_n; i++) {
		if (g->map_check[i]) remove(g->map[i]);
	}
	printf("	Post_Processing Nodes : DONE!\nDONE!\n");
}
void traverse(graph* g, node* s, FILE* f) {
	if (s->processed) {
		s->processed = 0;
		for (int i = 0; i < s->count_e; i++) {
			if (s->e[i] != NULL && s->e[i]->c != '0') {
				add_alph(g, s->e[i]->c);
				printf("	Writing relation : %d->%d weight = %c\n", s->n, s->e[i]->n->n, s->e[i]->c);
				fprintf(f, "%d->%d[label=\"%c\"];\n", s->n, s->e[i]->n->n, s->e[i]->c);
				traverse(g, s->e[i]->n, f);
			}
		}
	}
}
int in_superset(int** super, int* set, int l) {
	for (int i = 0; i < l; i++) {
		int count = set[0];
		if (set[0] == super[i][0]) {
			for (int j = 1; j <= set[0]; j++) {
				for (int k = 1; k <= super[i][0]; k++) {
					if (super[i][k] == set[j]) count--;
				}
			}
		}
		if (count == 0 && set[0] != 0) return i;
	}
	return -1;
}
int*** determining(graph* g, int*** arr, int* set, int& n) {
	arr = (int***)realloc(arr, (++n) * sizeof(int*));
	arr[n - 1] = (int**)malloc((g->len_lang + 1) * sizeof(int*));
	arr[n - 1][0] = (int*)malloc((set[0] + 1) * sizeof(int));
	for (int i = 0; i <= set[0]; i++) {
		arr[n - 1][0][i] = set[i];
	}
	for (int i = 0; i < g->len_lang; i++) {
		arr[n - 1][i + 1] = (int*)malloc(sizeof(int));
		arr[n - 1][i + 1][0] = 0;
		for (int j = 1; j <= set[0]; j++) {
			for (int k = 0; k < g->map[set[j]]->count_e; k++) {
				if (g->map[set[j]]->e[k]->c == g->lang[i]) {
					int found = 0;
					for (int l = 1; l <= arr[n - 1][i + 1][0]; l++) {
						if (arr[n - 1][i + 1][l] == g->map[set[j]]->e[k]->n->n) {
							found = 1;
							break;
						}
					}
					if (!found) {
						arr[n - 1][i + 1] = (int*)realloc(arr[n - 1][i + 1], (++arr[n - 1][i + 1][0] + 1) * sizeof(int));
						arr[n - 1][i + 1][arr[n - 1][i + 1][0]] = g->map[set[j]]->e[k]->n->n;
					}

				}
			}
		}
	}
	return arr;
}
graph* determine(graph* g) {
	printf("Transforming NFA to DFA :\n");
	int*** arr = (int***)malloc(sizeof(int**));
	int n = 0;
	int** memo = (int**)malloc(sizeof(int*));;
	int l = 0;
	arr = determining(g, arr, g->starting_nodes, n);
	memo = (int**)realloc(memo, ++l * sizeof(int*));
	memo[l - 1] = g->starting_nodes;
	int level = 1;
	for (int i = 0; i < g->len_lang; i++) {
		if (arr[level - 1][i + 1][0] != 0) {
			int index = in_superset(memo, arr[level - 1][i + 1], l);
			if (index == -1) {
				memo = (int**)realloc(memo, ++l * sizeof(int*));
				memo[l - 1] = arr[level - 1][i + 1];
				arr = determining(g, arr, arr[level - 1][i + 1], n);
			}
		}
		if (i == g->len_lang - 1 && level < n) {
			level++; i = -1;
		}
	}
	printf("\tCreating new DFA graph:\n");
	graph* r = NULL;
	r = create_g(r); r->name = g->name;
	for (int i = 0; i < n; i++) {
		for (int j = 1; j <= g->len_lang; j++) {
			if (arr[i][j][0] != 0) {
				int s, f; char c;
				s = i; f = in_superset(memo, arr[i][j], l); c = g->lang[j - 1];
				printf("\t\tInserting Relation :%d-(%c)->%d\n", s, c, f);
				insert(r, s, f, c);
				add_alph(r, c);
				r->map[s]->processed = true;
			}
			int b = 1;
			for (int it = 1; it <= r->end_nodes[0]; it++) {
				if (r->end_nodes[it] == i) {
					b = 0; break;
				}
			} 
			for (int k = 1; k <= g->end_nodes[0] && b; k++) {
				for (int l = 1; l <= memo[i][0] && b; l++) {
					if (g->end_nodes[k] == memo[i][l]) {
						r->end_nodes = (int*)realloc(r->end_nodes, (++r->end_nodes[0]+1) * sizeof(int));
						r->end_nodes[r->end_nodes[0]] = i;
						b = 0;
					}
				}
			}
		}
	}
	r->starting_nodes = (int*)realloc(r->starting_nodes, 2 * sizeof(int));
	r->starting_nodes[0] = 1; r->starting_nodes[1] = 0;
	printf("\tDONE!\n");
	printf("DONE!\n");
	return r;
}
graph* reverse(graph* g) {
	graph* r = NULL; r = create_g(r);
	printf("Reversing Graph :\n");
	r->name = g->name;
	for (int i = 0; i < g->count_n; i++) {
		if (g->map_check[i]) {
			for (int j = 0; j < g->map[i]->count_e; j++) {
				printf("\tProcessing : %d %d %c\n", g->map[i]->e[j]->n->n, i, g->map[i]->e[j]->c);
				insert(r, g->map[i]->e[j]->n->n, i, g->map[i]->e[j]->c);
				add_alph(r, g->map[i]->e[j]->c);
				r->map[g->map[i]->e[j]->n->n]->processed = true;
			}
		}
	}
	r->starting_nodes = (int*)realloc(r->starting_nodes, (g->end_nodes[0] + 1) * sizeof(int));
	for (int i = 0; i <= g->end_nodes[0]; i++) {
		r->starting_nodes[i] = g->end_nodes[i];
	}
	r->end_nodes = (int*)realloc(r->end_nodes, (g->starting_nodes[0] + 1) * sizeof(int));
	for (int i = 0; i <= g->starting_nodes[0]; i++) {
		r->end_nodes[i] = g->starting_nodes[i];
	}
	printf("DONE!\n");
	return r;
}
graph* minimize(graph* g, int m = 2) {
	graph* temp = NULL;
	temp = create_g(temp);
	temp = reverse(g);
	temp = determine(temp);
	graph* final = NULL;
	final = create_g(final);
	final = reverse(temp);
	final = determine(final);
	return final;
}
void g_write(graph* g, FILE* f) {
	g->lang = NULL; g->len_lang = 0;
	printf("Writing to FILE : ...\n");
	fprintf(f, "digraph %s", g->name);
	fprintf(f, "{\n");
	for (int i = 0; i < g->starting_nodes[0]; i++) {
		printf("%d\n", g->starting_nodes[1+i]);
		traverse(g, g->map[g->starting_nodes[i + 1]], f);
		
	}
	fprintf(f, "}");
	printf("\t-Graph %s alphabets are : ", g->name);
	for (int i = 0; i < g->len_lang; i++) {
		printf("%c%s", g->lang[i], i == g->len_lang - 1 ? "\n" : ", ");
	}
	printf("DONE!\n");
}
void get_start(graph* g) {
	int n;
	printf("\tPlease enter the number of starting nodes :"); scanf_s("%d", &n);
	g->starting_nodes = (int*)realloc(g->starting_nodes, (n + 1) * sizeof(int));
	g->starting_nodes[0] = n;
	printf("\t\tplease enter the %d start nodes :", n);
	for (int i = 1; i <= n; i++) {
		scanf_s("%d", g->starting_nodes+i);
	}
}
void get_end(graph* g) {
	int n;
	printf("\tPlease enter the number of end nodes :"); scanf_s("%d", &n);
	g->end_nodes = (int*)realloc(g->end_nodes, (n + 1) * sizeof(int));
	g->end_nodes[0] = n;
	printf("\t\tplease enter the %d end nodes :", n);
	for (int i = 1; i <= n; i++) {
		scanf_s("%d", g->end_nodes+i);
	}
}
void g_read(graph* g, FILE* file) {
	printf("Reading : ...\n");
	int c;
	while ((c = fgetc(file)) != ' ');
	for (int i = 1; ((c = fgetc(file)) != '{'); i++) {
		if (c == '\n') continue;
		g->name = (char*)realloc(g->name, (i + 1) * sizeof(char));
		g->name[i - 1] = c;
		g->name[i] = '\0';
	}
	while ((c = fgetc(file)) != '}') {
		int s = 0;
		int f = 0;
		int w;
		while (!isdigit(c)) {
			if (c == '}') return;
			c=fgetc(file);
		}
		do {
			s = s * 10 + (c - '0');
		} while ((c = fgetc(file)) && isdigit(c));
		while ((c = fgetc(file)) && !isdigit(c));
		do
			f = f * 10 + (c - '0');
		while ((c = fgetc(file)) && isdigit(c));
		while ((c = fgetc(file)) != '\"');
		w = fgetc(file);
		while ((c = fgetc(file)) != ';');
		printf("	Inserting Relation :%d-(%c)->%d\n", s, w, f);
		insert(g, s, f, w); add_alph(g, (char)w);
	}
	printf("\t-Graph %s alphabets are : ", g->name);
	for (int i = 0; i < g->len_lang; i++) {
		printf("%c%s", g->lang[i], i == g->len_lang - 1 ? "\n" : ", ");
	}
	get_start(g); get_end(g);
	printf("DONE!\n");
}
int check_v(char* path) {
	int n = strlen(path) - 1; bool valid = false;
	if (path[n] == 'v' && path[n - 1] == 'g' && path[n - 2] == '.') valid = true;
	if (path[n] == 't' && path[n - 1] == 'x' && path[n - 2] == 't' && path[n - 3] == '.') valid = true;
	if (path[n] == 't' && path[n - 1] == 'o' && path[n - 2] == 'd' && path[n - 3] == '.') valid = true;
	if (valid) {
		system("cls"); return valid;
	}
	printf("WARNING : %s is not a VALID gv/txt File!\n", path);
	return 0;
}
bool ask(char* c) {
	int b = 1;
	printf("are you sure you want to open this file : %s ?\n", c);
	printf("	   print 0 for NO and 1 for yes.\n");
	scanf_s("%d", &b);
	return b && check_v(c);
}
char* extract_location(char* path, const char* mode) {
	for (int i = strlen(path); i >= 0 && path[i] != '\\'; i--) path[i] = '\0';
	strcat_s(path, 50, "output"); strcat_s(path, 50, mode); strcat_s(path, 50, ".dot");
	return path;
}
bool confirm(bool& b) {
	if (b) { system("cls"); b = !b; return !b; }
	printf("would you like to process another GraphViz file ?\n1->\"YES\"  0->\"NO\" : ");
	int ans; scanf_s("%d", &ans);
	return ans == 1 ? 1 : (ans == 0 ? 0 : confirm(b));
}
void close_f(FILE* in, FILE* during, FILE* out, FILE* last) {
	fclose(in); fclose(during); fclose(out); fclose(last);
}
void process(graph* graph_in, FILE* file_in, FILE* file_out, FILE* fileout2, FILE* fileout3, void read(graph*, FILE*), void opt(graph*), graph* det(graph*), graph* min(graph*, int), void write(graph*, FILE*), void rendr(void)) {
	read(graph_in, file_in);
	opt(graph_in);
	write(graph_in, file_out);
	graph_in = det(graph_in);
	write(graph_in, fileout2);
	graph_in = min(graph_in, 2);
	write(graph_in, fileout3);
	close_f(file_in, file_out, fileout2, fileout3);
	rendr();
}
FILE** open_files(char* path) {
	FILE** files = (FILE**)malloc(4 * sizeof(FILE*));
	files[0] = NULL;
	fopen_s(&files[0], path, "r");
	char* output_f = extract_location(path, "1");
	files[1] = NULL;
	fopen_s(&files[1], output_f, "w");
	char* output_f_2 = extract_location(path, "2");
	files[2] = NULL;
	fopen_s(&files[2], output_f, "w");
	char* output_f_3 = extract_location(path, "3");
	files[3] = NULL;
	fopen_s(&files[3], output_f, "w");
	return files;
}
void Render() {
	printf("Rendering PNG of th result Graph :\n");
	printf("\t-Running shell :\n");
	printf("\t\tpassing args : dot -Tpng output3.dot -o output3.png\n");
	system("dot -Tpng output3.dot -o output3.png\n");
	printf("Done!\n");
}
int main() {
	bool start = true;
	while (confirm(start)) {
		input("please enter the path of the file/name of file : ", path, 50);
		if (ask(path))
		{
			FILE** files = open_files(path);
			if (files[0] != NULL && files[1] != NULL && files[2] != NULL && files[3]!=NULL) {
				graph* g = NULL;
				g = create_g(g);
				process(g, files[0], files[1], files[2], files[3], g_read, optimise, determine, minimize, g_write, Render);
				free(g);
			}
			else {
				printf("failed to open FILE\n");
			}
		}
	}
}