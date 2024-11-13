#include "word.h"
#include "code.h"

#include <stdlib.h>

Word *word_create(uint8_t *syms, uint32_t len) {
    Word *w;
    w = (struct Word *) malloc(sizeof(*w));

    w->len = len;
    w->syms = (uint8_t *) malloc(len * sizeof(uint8_t));

    for (uint32_t i = 0; i < len; i++) {
        w->syms[i] = syms[i];
    }

    if (w != NULL) {
        return w;
    }
    return NULL;
}

Word *word_append_sym(Word *w, uint8_t sym) {
    uint32_t new_len = (w->len) + 1;
    uint8_t *new_syms = (uint8_t *) malloc(MAX_CODE * sizeof(uint8_t));

    for (uint32_t i = 0; i < w->len; i++) {
        new_syms[i] = w->syms[i];
    }
    new_syms[w->len] = sym;

    Word *new_w;
    new_w = word_create(new_syms, new_len);

    return new_w;
}

void word_delete(Word *w) {
    free(w->syms);
    free(w);
}

WordTable *wt_create(void) {
    WordTable *wt;

    wt = (WordTable *) malloc(MAX_CODE * sizeof(Word));

    wt[EMPTY_CODE] = word_create(NULL, 0);

    for (int i = START_CODE; i < MAX_CODE; i++) {
        wt[i] = NULL;
    }

    return wt;
}

void wt_reset(WordTable *wt) {
    for (int i = START_CODE; i < MAX_CODE; i++) {
        if (wt[i] != NULL) {
            free(wt[i]->syms);
            wt[i] = NULL;
            free(wt[i]);
        }
    }
}

void wt_delete(WordTable *wt) {
    wt_reset(wt);
    wt[EMPTY_CODE] = NULL;

    free(wt);
}
