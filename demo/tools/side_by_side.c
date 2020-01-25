#include "ac_buffer.h"
#include "ac_in.h"
#include "ac_io.h"
#include "ac_pool.h"
#include "ac_sort.h"

typedef struct {
  char *header;
  char *key;
  char *label;
  char *name;
  char *value;
  int column;
  bool reversed;
} node_t;

int *column_widths(ac_pool_t *pool, int num_cols, node_t *n, size_t num_n) {
  node_t *np = n;
  node_t *ep = n + num_n;
  int *res = (int *)ac_pool_calloc(pool, sizeof(int) * (num_cols + 1));
  res[num_cols] = 12;
  while (np < ep) {
    node_t *cur = np;
    int max_length = strlen(np->key);
    if (max_length < strlen(np->label))
      max_length = strlen(np->label);
    if (res[0] < max_length)
      res[0] = max_length;

    while (np < ep && !strcmp(np->header, cur->header) &&
           !strcmp(np->key, cur->key) && !strcmp(np->label, cur->label)) {
      int max_length = strlen(np->name);
      if (max_length < strlen(np->value))
        max_length = strlen(np->value);
      if (res[np->column] < max_length)
        res[np->column] = max_length;
      np++;
    }
  }
  for (int i = 0; i < num_cols; i++) {
    if (res[i] > 0)
      res[i] += 2;
  }
  return res;
}

int compare_node(node_t *v1, node_t *v2) {
  int n = strcmp(v1->header, v2->header);
  if (n)
    return n;
  n = strcmp(v1->key, v2->key);
  if (n)
    return n;
  n = strcmp(v1->label, v2->label);
  if (n)
    return n;
  if (v1->column != v2->column)
    return (v1->column < v2->column) ? -1 : 1;
  return 0;
}

ac_sort_m(sort_nodes, node_t, compare_node);

void append_node(ac_buffer_t *bh, char **a, int column) {
  node_t n;
  if (a[0][0] == '-') {
    n.reversed = true;
    n.header = a[0] + 1;
  } else {
    n.reversed = false;
    n.header = a[0];
  }
  n.key = a[1];
  n.label = a[2];
  n.name = a[3];
  n.value = a[4];
  n.column = column;
  ac_buffer_append(bh, &n, sizeof(n));
}

void print_left_string(int width, char *s, int pad) {
  // return;
  char fmt[20];
  sprintf(fmt, "%%%ds%%-%ds", pad, width - pad);
  printf(fmt, "", s);
}

void print_right_string(int width, char *s, int pad) {
  // return;
  char fmt[20];
  sprintf(fmt, "%%%ds%%%ds", width - pad, pad);
  printf(fmt, s, "");
}

void print_header(int *widths, int num_cols, node_t *n, node_t *ep) {
  // return;
  if (n->header[0] && n->header[1])
    printf("%s\n", n->header + 1);
  print_left_string(widths[0], n->key + 1, 0);
  for (int i = 1; i < num_cols; i++) {
    node_t *np = n;
    while (np < ep) {
      if (np->column == i) {
        print_left_string(widths[i], np->name, 0);
        break;
      }
      np++;
    }
    if (np == ep)
      print_left_string(widths[i], "", 0);
  }
  if (num_cols == 3)
    print_right_string(widths[num_cols], "% gain", 2);
  printf("\n");
}

bool extract_double(const char *s, double *r) {
  char tmp[30];
  const char *p = s;
  char *wp = tmp;
  while (*p) {
    if (*p != ',')
      *wp++ = *p++;
    else
      p++;
  }
  *wp = 0;
  // printf("%s\n", tmp);
  if (sscanf(tmp, "%lf", r) == 1)
    return true;
  return false;
}

void print_rows(int *widths, int num_cols, node_t *n, node_t *ep) {
  print_left_string(widths[0], n->label + 1, 0);
  double a = 0.0, b = 0.0;
  bool a_found = false, b_found = false;
  for (int i = 1; i < num_cols; i++) {
    node_t *np = n;
    while (np < ep) {
      if (np->column == i) {
        if (i == 1 && extract_double(np->value, &a))
          a_found = true;
        else if (i == 2 && extract_double(np->value, &b))
          b_found = true;
        print_right_string(widths[i], np->value, 2);
        break;
      }
      np++;
    }
    if (np == ep)
      print_right_string(widths[i], "", 2);
  }
  if (a_found && b_found && num_cols == 3) {
    char tmp[20];
    if (a > b)
      sprintf(tmp, "%s%'0.3f%%", n->reversed ? "-" : "", ((a / b) - 1) * 100.0);
    else
      sprintf(tmp, "%s%'0.3f%%", n->reversed ? "" : "-", ((b / a) - 1) * 100.0);
    print_right_string(widths[num_cols], tmp, 0);
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  ac_in_options_t opts;
  ac_in_options_init(&opts);
  ac_in_options_format(&opts, ac_io_delimiter('\n'));
  ac_pool_t *pool = ac_pool_init(4096);
  ac_buffer_t *bh = ac_buffer_init(1024);
  for (int i = 1; i < argc; i++) {
    ac_in_t *in = ac_in_init(argv[i], &opts);
    ac_io_record_t *r;
    while ((r = ac_in_advance(in)) != NULL) {
      size_t num_a = 0;
      char **a = ac_pool_split(pool, &num_a, '\t', r->record);
      if (num_a == 5)
        append_node(bh, a, i);
    }
    ac_in_destroy(in);
  }
  node_t *nodes = (node_t *)ac_buffer_data(bh);
  size_t num_nodes = ac_buffer_length(bh) / sizeof(node_t);
  sort_nodes(nodes, num_nodes);
  node_t *np = nodes;
  node_t *ep = nodes + num_nodes;

  int *widths = column_widths(pool, argc, nodes, num_nodes);
  while (np < ep) {
    node_t *cur = np;
    np++;
    while (np < ep && !strcmp(np->header, cur->header) &&
           !strcmp(np->key, cur->key))
      np++;

    print_header(widths, argc, cur, np);
    while (cur < np) {
      node_t *cur2 = cur;
      cur++;
      while (cur < np && !strcmp(cur->label, cur2->label))
        cur++;
      print_rows(widths, argc, cur2, cur);
    }
    printf("\n");
  }

  ac_pool_destroy(pool);
  ac_buffer_destroy(bh);
  return 0;
}
