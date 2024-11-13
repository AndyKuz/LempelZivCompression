#include "trie.h"
#include "code.h"

#include <stdlib.h>

TrieNode *trie_node_create(uint16_t index) {
    TrieNode *trie_node;
    trie_node = (struct TrieNode *) malloc(ALPHABET * sizeof(struct TrieNode));
    trie_node->code = index;

    for (int i = 0; i < ALPHABET; i++) {
        trie_node->children[i] = NULL;
    }

    return trie_node;
}

void trie_node_delete(TrieNode *n) {
    free(n);
}

TrieNode *trie_create(void) {
    TrieNode *trie_root;
    trie_root = (struct TrieNode *) malloc(ALPHABET * sizeof(struct TrieNode));
    trie_root->code = EMPTY_CODE;

    for (int i = 0; i < ALPHABET; i++) {
        trie_root->children[i] = NULL;
    }

    if (trie_root) {
        return trie_root;
    }
    return NULL;
}

void trie_reset(TrieNode *root) {
    for (int i = 0; i < ALPHABET; i++) {
        if (root->children[i] != NULL) {
            trie_delete(root->children[i]);
            root->children[i] = NULL;
        }
    }
}

void trie_delete(TrieNode *n) {
    for (int i = 0; i < ALPHABET; i++) {
        if (n->children[i] != NULL) {
            trie_node_delete(n->children[i]);
        }
    }
    free(n);
}

TrieNode *trie_step(TrieNode *n, uint8_t sym) {
    if (n->children[sym]) {
        return n->children[sym];
    } else {
        return NULL;
    }
}
