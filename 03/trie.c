#include "trie.h"
#include <stdio.h>
#include <stdbool.h>

int char_to_index(char c) {
    c = tolower(c);
    if ('0' <= c && c <= '9')
        return c - '0';
    if ('a' <= c && c <= 'z')
        return c - 'a' + 10;
    return -1;
}

char index_to_char(int i) {
    if (0 <= i && i <= 9)
        return i + '0';
    if (10 <= i && i <= 36)
        return i - 10 + 'a';
    return -1;
}

int trie_remove_rec(Trie *t, const char *k) {
    if (*k == 0)
        return S_OK;
    trie_remove_rec(t->children[char_to_index(*k)], k + 1);
    free(t);
    return S_OK;
}

int trie_remove(Trie *t, const char *k) {
    if (trie_get(t, k) == 0)
        return S_OK;
    return trie_remove_rec(t->children[char_to_index(*k)], k + 1);
}

int trie_set(Trie *t, const char *k, int v) {
    if (v == 0)
        return trie_remove(t, k);
    Trie **tmp = &t;
    for (const char *c = k; *c; c++) {
        int i = char_to_index(*c);
        if (i < 0)
            return S_INVALID_INPUT;
        tmp = &(*tmp)->children[i];
        if (*tmp == NULL) {
            *tmp = malloc(sizeof(Trie));
            if (*tmp == NULL)
                return S_MALLOC;
            memset(*tmp, 0, sizeof(Trie));
        }
    }
    (*tmp)->val = v;
    return S_OK;
}

int trie_get(const Trie *t, const char *k) {
    for (const char *c = k; *c; c++) {
        int i = char_to_index(*c);
        if (i < 0)
            return 0;
        t = t->children[i];
        if (t == NULL) {
            return 0;
        }
    }
    return t->val;
}

int trie_free(Trie *t) {
    if (!t)
        return 0;
    for (int i = 0; i < TRIE_SIZE; i++)
        trie_free(t->children[i]);
    return 0;
}

int trie_for_each_rec(const Trie *t, trie_callback call, size_t level,
                      char **buf, size_t *buf_size, void *ptr) {
    if (t == NULL)
        return S_OK;
    if (*buf == NULL) {
        *buf_size = 128;
        *buf = malloc(*buf_size);
        if (*buf == NULL)
            return S_MALLOC;
    }
    if (level >= *buf_size - 1) {
        *buf_size += 128;
        char *new_buf = realloc(*buf, *buf_size);
        if (new_buf == NULL) {
            free(*buf);
            *buf_size = 0;
            return S_MALLOC;
        }
        *buf = new_buf;
    }
    (*buf)[level] = 0;
    if (t->val > 0) {
        (*buf)[level] = 0;
        int r = call(*buf, t->val, ptr);
        if (r != S_OK)
            return r;
    }
    for (int i = 0; i < TRIE_SIZE; i++) {
        (*buf)[level] = index_to_char(i);
        int r = trie_for_each_rec(t->children[i], call, level + 1, buf,
                                  buf_size, ptr);
        if (r != S_OK)
            return r;
    }

    return S_OK;
}

int trie_for_each(const Trie *t, trie_callback call, void *ptr) {
    char *s = NULL;
    size_t s_size = 0;
    int r = trie_for_each_rec(t, call, 0, &s, &s_size, ptr);
    if (s)
        free(s);
    return r;
}

int trie_len_callback(const char *c, int val, void *ptr) {
    (void)c;
    (void)val;
    int *res = ptr;
    (*res)++;
    return S_OK;
}

int trie_len(const Trie *t) {
    int len = 0;
    return trie_for_each(t, trie_len_callback, &len);
}

struct TrieEqCallbackArg {
    Trie *t;
    bool *res;
};

int trie_eq_callback(const char *c, int val, void *ptr) {
    struct TrieEqCallbackArg *b = ptr;
    if (trie_get(b->t, c) != val)
        b->res = false;
    return S_OK;
}

int trie_eq(const Trie *a, const Trie *b) {
    int len_a, len_b;
    if ((len_a = trie_len(a)) < 0 || (len_b = trie_len(b)) < 0)
        return -1;
    if (len_a != len_b)
        return 0;
    return 1;
}
