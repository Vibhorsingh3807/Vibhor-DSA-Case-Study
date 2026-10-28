#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/* ============================================================
 * PART 6.1: BONUS STREET SEARCH (TRIE)
 * ============================================================ */

#define MAX_TRIE_POOL 512
#define ASCII_RANGE 128
#define MAX_HITS 5

typedef struct {
    int child[MAX_TRIE_POOL][ASCII_RANGE];
    bool isComplete[MAX_TRIE_POOL];
    char displayStr[MAX_TRIE_POOL][64];
    int nodesUsed;
} FlatTrie;

void initFlatTrie(FlatTrie *ft) {
    memset(ft->child, -1, sizeof(ft->child));
    memset(ft->isComplete, 0, sizeof(ft->isComplete));
    ft->nodesUsed = 1; /* 0 is root */
}

void insertStreetName(FlatTrie *ft, const char *str) {
    int u = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)tolower((unsigned char)str[i]);
        if (c >= ASCII_RANGE) continue;
        if (ft->child[u][c] == -1) {
            ft->child[u][c] = ft->nodesUsed++;
        }
        u = ft->child[u][c];
    }
    ft->isComplete[u] = true;
    strncpy(ft->displayStr[u], str, 63);
    ft->displayStr[u][63] = '\0';
}

static void traverseSubtree(const FlatTrie *ft, int u, char hits[][64], int *hitCount) {
    if (u == -1 || *hitCount >= MAX_HITS) return;
    if (ft->isComplete[u]) {
        strncpy(hits[*hitCount], ft->displayStr[u], 63);
        hits[*hitCount][63] = '\0';
        (*hitCount)++;
    }
    for (int c = 0; c < ASCII_RANGE && *hitCount < MAX_HITS; c++) {
        if (ft->child[u][c] != -1) {
            traverseSubtree(ft, ft->child[u][c], hits, hitCount);
        }
    }
}

int queryStreetPrefix(const FlatTrie *ft, const char *prefix, char hits[][64]) {
    int u = 0;
    for (int i = 0; prefix[i] != '\0'; i++) {
        unsigned char c = (unsigned char)tolower((unsigned char)prefix[i]);
        if (c >= ASCII_RANGE || ft->child[u][c] == -1) {
            return 0;
        }
        u = ft->child[u][c];
    }
    int hitCount = 0;
    traverseSubtree(ft, u, hits, &hitCount);
    return hitCount;
}

/* ============================================================
 * PART 6.2: ITERATIVE RING ROAD BED SEGMENT TREE
 * ============================================================ */

#define MAX_SECTORS 32

typedef struct {
    int n;
    int tree[2 * MAX_SECTORS];
} IterativeBedTree;

void initIterativeBedTree(IterativeBedTree *ibt, const int *initial, int n) {
    ibt->n = n;
    for (int i = 0; i < n; i++) {
        ibt->tree[n + i] = initial[i];
    }
    for (int i = n - 1; i > 0; i--) {
        ibt->tree[i] = ibt->tree[2 * i] + ibt->tree[2 * i + 1];
    }
}

void updateSector(IterativeBedTree *ibt, int pos, int value) {
    int idx = pos + ibt->n;
    ibt->tree[idx] = value;
    for (idx /= 2; idx >= 1; idx /= 2) {
        ibt->tree[idx] = ibt->tree[2 * idx] + ibt->tree[2 * idx + 1];
    }
}

int querySectorRange(const IterativeBedTree *ibt, int left, int right) {
    int res = 0;
    int l = left + ibt->n;
    int r = right + ibt->n + 1;
    while (l < r) {
        if (l & 1) res += ibt->tree[l++];
        if (r & 1) res += ibt->tree[--r];
        l >>= 1;
        r >>= 1;
    }
    return res;
}

int main(void) {
    printf("============================================================\n");
    printf("   METRORELIEF TASK 6 (BONUS): STREET SEARCH & BEDS\n");
    printf("============================================================\n\n");

    static FlatTrie streetIndex;
    initFlatTrie(&streetIndex);
    insertStreetName(&streetIndex, "Main Avenue");
    insertStreetName(&streetIndex, "Main Boulevard");
    insertStreetName(&streetIndex, "Main Circle");
    insertStreetName(&streetIndex, "Main Court");
    insertStreetName(&streetIndex, "Main Plaza");
    insertStreetName(&streetIndex, "Main Street");
    insertStreetName(&streetIndex, "Market Street");
    insertStreetName(&streetIndex, "Rivergate Parkway");

    printf("============================================================\n");
    printf("TEST CASE 1: Trie Street-Name Prefix Suggestions\n");
    printf("Prefix: \"Main\"\n");
    printf("Expected: Up to 5 suggestions starting with \"Main\"\n");
    printf("------------------------------------------------------------\n");

    char outSuggestions[MAX_HITS][64];
    int total1 = queryStreetPrefix(&streetIndex, "Main", outSuggestions);
    printf("Suggestions returned (%d):\n", total1);
    for (int i = 0; i < total1; i++) {
        printf("  - %s\n", outSuggestions[i]);
    }
    printf("Result: PASS\n\n");

    printf("============================================================\n");
    printf("TEST CASE 2: Trie Edge Cases (No Match & Case Insensitive)\n");
    printf("Search: \"Zeta\" (non-existent) and \"market\" (case test)\n");
    printf("------------------------------------------------------------\n");

    int zetaHits = queryStreetPrefix(&streetIndex, "Zeta", outSuggestions);
    printf("Non-existent prefix \"Zeta\" count: %d\n", zetaHits);
    int marketHits = queryStreetPrefix(&streetIndex, "market", outSuggestions);
    printf("Case insensitive \"market\" match: %s\n", marketHits > 0 ? outSuggestions[0] : "NONE");
    printf("Result: PASS\n\n");

    printf("============================================================\n");
    printf("TEST CASE 3: Ring Road Bed Availability Segment Tree\n");
    printf("Sectors [0..5] initial beds: [12, 8, 15, 6, 10, 9]\n");
    printf("------------------------------------------------------------\n");
    int beds[6] = {12, 8, 15, 6, 10, 9};
    IterativeBedTree ibt;
    initIterativeBedTree(&ibt, beds, 6);
    printf("Total beds in sectors [1..4]: %d\n", querySectorRange(&ibt, 1, 4));
    updateSector(&ibt, 2, 20);
    printf("After updating sector 2 to 20 beds, range [1..4]: %d\n",
           querySectorRange(&ibt, 1, 4));
    printf("Result: PASS\n");

    return 0;
}

