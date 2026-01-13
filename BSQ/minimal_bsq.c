// minimal_bsq.c - compact, single-file BSQ with clearer functions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static void fail(const char *msg){ fprintf(stderr, "%s\n", msg); }

// Read whole stream into a memory buffer (returns NULL on error)
static char *read_all(FILE *f, long *out_size){
    if(fseek(f, 0, SEEK_END) != 0) return NULL;
    long sz = ftell(f);
    if(sz < 0) return NULL;
    rewind(f);
    char *buf = malloc(sz + 1);
    if(!buf) return NULL;
    if(fread(buf, 1, sz, f) != (size_t)sz && ferror(f)) { free(buf); return NULL; }
    buf[sz] = '\0';
    if(out_size) *out_size = sz;
    return buf;
}

// Parse header: "<number><empty><obstacle><full>\n"
static int parse_header(const char *buf, int *out_h, char *out_empty, char *out_obstacle, char *out_full, const char **out_mapstart){
    const char *nl = strchr(buf, '\n');
    if(!nl) return -1;
    int header_len = (int)(nl - buf);
    if(header_len < 4) return -1;
    *out_full = nl[-1];
    *out_obstacle = nl[-2];
    *out_empty = nl[-3];
    int digits = header_len - 3;
    if(digits <= 0 || digits >= 32) return -1;
    for(int i = 0; i < digits; ++i) if(!isdigit((unsigned char)buf[i])) return -1;
    char tmp[32]; memcpy(tmp, buf, digits); tmp[digits] = '\0';
    *out_h = atoi(tmp);
    *out_mapstart = nl + 1;
    return 0;
}

// Split map into lines, validate width and allowed chars. Returns array of lines (NULL terminated) or NULL on error.
static char **collect_lines(const char *mapstart, int expected_h, int *out_w, char empty, char obstacle){
    // Count lines and width
    int h = 0, w = -1;
    const char *p = mapstart;
    while(p && *p){
        const char *e = strchr(p, '\n');
        int len = e ? (int)(e - p) : (int)strlen(p);
        if(len == 0) { p = e ? e + 1 : NULL; continue; }
        if(w == -1) w = len;
        if(len != w) return NULL;
        for(int i = 0; i < len; ++i) if(p[i] != empty && p[i] != obstacle) return NULL;
        h++;
        p = e ? e + 1 : NULL;
    }
    if(expected_h != 0 && expected_h != h) return NULL;
    if(h == 0 || w <= 0) return NULL;

    // allocate and copy
    char **lines = malloc((h + 1) * sizeof(char*));
    if(!lines) return NULL;
    p = mapstart; int r = 0;
    while(p && *p && r < h){
        const char *e = strchr(p, '\n');
        int len = e ? (int)(e - p) : (int)strlen(p);
        if(len == 0){ p = e ? e + 1 : NULL; continue; }
        char *s = malloc(len + 1);
        if(!s){ // cleanup
            for(int k = 0; k < r; ++k) free(lines[k]); free(lines); return NULL;
        }
        memcpy(s, p, len); s[len] = '\0';
        lines[r++] = s;
        p = e ? e + 1 : NULL;
    }
    lines[r] = NULL;
    *out_w = w;
    return lines;
}

// Solve using DP with two rows, return best size and bottom-right coords via pointers
static void solve_bsq(char **lines, int h, int w, char obstacle, int *out_size, int *out_bi, int *out_bj){
    int *prev = calloc(w + 1, sizeof(int));
    int *cur  = calloc(w + 1, sizeof(int));
    if(!prev || !cur){ free(prev); free(cur); return; }
    int best = 0, bi = 0, bj = 0;
    for(int i = 0; i < h; ++i){
        for(int j = 0; j < w; ++j){
            if(lines[i][j] == obstacle) cur[j+1] = 0;
            else {
                int a = prev[j+1], b = prev[j], c = cur[j];
                int m = a < b ? (a < c ? a : c) : (b < c ? b : c);
                cur[j+1] = m + 1;
                if(cur[j+1] > best){ best = cur[j+1]; bi = i; bj = j; }
            }
        }
        // swap
        int *t = prev; prev = cur; cur = t;
        memset(cur, 0, (w + 1) * sizeof(int));
    }
    free(prev); free(cur);
    *out_size = best; *out_bi = bi; *out_bj = bj;
}

static void fill_and_print(char **lines, int h, int w, int size, int bi, int bj, char full){
    if(size > 0){
        int top = bi - size + 1, left = bj - size + 1;
        for(int i = top; i <= bi; ++i) for(int j = left; j <= bj; ++j) lines[i][j] = full;
    }
    for(int i = 0; i < h; ++i){ puts(lines[i]); }
}

static void free_lines(char **lines){ if(!lines) return; for(int i = 0; lines[i]; ++i) free(lines[i]); free(lines); }

int main(int argc, char **argv){
    FILE *f = argc > 1 ? fopen(argv[1], "rb") : stdin;
    if(!f){ fail("cannot open file"); return 1; }

    long sz; char *buf = read_all(f, &sz);
    if(f != stdin) fclose(f);
    if(!buf){ fail("read error"); return 1; }

    int expected_h; char empty, obstacle, full;
    const char *mapstart;
    if(parse_header(buf, &expected_h, &empty, &obstacle, &full, &mapstart) != 0){ free(buf); fail("bad header"); return 1; }

    int w; char **lines = collect_lines(mapstart, expected_h, &w, empty, obstacle);
    if(!lines){ free(buf); fail("bad map"); return 1; }

    int size, bi, bj;
    solve_bsq(lines, expected_h == 0 ? 0 : expected_h, w, obstacle, &size, &bi, &bj);
    // Note: if expected_h was 0 we still determined h by lines array length; compute h
    int h = 0; while(lines[h]) ++h;
    if(expected_h == 0) expected_h = h;
    // adjust bi/bj if they are uninitialized when using expected_h==0

    fill_and_print(lines, expected_h, w, size, bi, bj, full);

    free_lines(lines); free(buf);
    return 0;
}
