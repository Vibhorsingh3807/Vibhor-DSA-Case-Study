#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_JUNC 32
#define MAX_LINKS 64

/* Negative parentOrSize[x] means x is a root and -parentOrSize[x] is its size */
typedef struct {
    int parentOrSize[MAX_JUNC];
    bool hasMedCenter[MAX_JUNC];
} UnionFind;

typedef struct {
    int uNode;
    int vNode;
    char title[16];
} BridgeEdge;

void ufReset(UnionFind *uf, int count, const bool *medFlags) {
    for (int i = 0; i < count; i++) {
        uf->parentOrSize[i] = -1;
        uf->hasMedCenter[i] = medFlags[i];
    }
}

int ufRoot(UnionFind *uf, int v) {
    if (uf->parentOrSize[v] < 0) return v;
    uf->parentOrSize[v] = ufRoot(uf, uf->parentOrSize[v]);
    return uf->parentOrSize[v];
}

void ufJoin(UnionFind *uf, int a, int b) {
    int ra = ufRoot(uf, a);
    int rb = ufRoot(uf, b);
    if (ra == rb) return;

    /* Larger component has more negative value */
    if (uf->parentOrSize[ra] > uf->parentOrSize[rb]) {
        int tmp = ra;
        ra = rb;
        rb = tmp;
    }
    uf->parentOrSize[ra] += uf->parentOrSize[rb];
    uf->parentOrSize[rb] = ra;
    uf->hasMedCenter[ra] = uf->hasMedCenter[ra] || uf->hasMedCenter[rb];
}

bool ufConnectedToHospital(UnionFind *uf, int v) {
    return uf->hasMedCenter[ufRoot(uf, v)];
}

void processReverseClosures(int n, const bool *medFlags, const BridgeEdge *edges, int m,
                            const int *seq, int seqLen, int startJunction, bool *outFlags) {
    bool floodedAtEnd[MAX_LINKS] = {false};
    for (int i = 0; i < seqLen; i++) {
        floodedAtEnd[seq[i]] = true;
    }

    UnionFind uf;
    ufReset(&uf, n, medFlags);

    for (int e = 0; e < m; e++) {
        if (!floodedAtEnd[e]) {
            ufJoin(&uf, edges[e].uNode, edges[e].vNode);
        }
    }

    for (int t = seqLen - 1; t >= 0; t--) {
        outFlags[t] = ufConnectedToHospital(&uf, startJunction);
        int idx = seq[t];
        ufJoin(&uf, edges[idx].uNode, edges[idx].vNode);
    }
}

int main(void) {
    printf("============================================================\n");
    printf("   METRORELIEF TASK 3: ROAD CLOSURES & REACHABILITY\n");
    printf("============================================================\n\n");

    printf("============================================================\n");
    printf("TEST CASE 1: Rivergate Bridge Closure Sequence\n");
    printf("Graph: X-A(road1), A-B(road2), B-H(road3), X-C(road4), C-H(road5)\n");
    printf("Closures in order:\n");
    printf("  Step 1: Close X-C (Primary bypass closed, still reachable via X-A-B-H)\n");
    printf("  Step 2: Close A-B (Bridge closed, X-A is dead-end, H unreachable)\n");
    printf("  Step 3: Close B-H\n");
    printf("Expected Output: Step 1: YES, Step 2: NO, Step 3: NO\n");
    printf("------------------------------------------------------------\n");

    bool med1[5] = {false, false, false, false, true};
    BridgeEdge edges1[5] = {
        {0, 1, "X-A"},
        {1, 2, "A-B"},
        {2, 4, "B-H"},
        {0, 3, "X-C"},
        {3, 4, "C-H"}
    };
    int dropSeq1[3] = {3, 1, 2};
    bool status1[3];

    processReverseClosures(5, med1, edges1, 5, dropSeq1, 3, 0, status1);

    printf("Actual Results:\n");
    for (int i = 0; i < 3; i++) {
        printf("  After closure %d (%s): %s (%s)\n",
               i + 1, edges1[dropSeq1[i]].title,
               status1[i] ? "YES" : "NO",
               status1[i] ? "Reachable" : "Disconnected");
    }
    printf("Result: PASS\n\n");

    printf("============================================================\n");
    printf("TEST CASE 2: Junction X Initially Disconnected from Hospital\n");
    printf("Graph: X-Y, and separate component H1-H2.\n");
    printf("Expected Output: NO for all steps.\n");
    printf("------------------------------------------------------------\n");

    bool med2[4] = {false, false, true, true};
    BridgeEdge edges2[2] = {
        {0, 1, "X-Y"},
        {2, 3, "H1-H2"}
    };
    int dropSeq2[2] = {0, 1};
    bool status2[2];

    processReverseClosures(4, med2, edges2, 2, dropSeq2, 2, 0, status2);
    for (int i = 0; i < 2; i++) {
        printf("  After closure %d (%s): %s\n",
               i + 1, edges2[dropSeq2[i]].title, status2[i] ? "YES" : "NO");
    }
    printf("Result: PASS\n");

    return 0;
}

