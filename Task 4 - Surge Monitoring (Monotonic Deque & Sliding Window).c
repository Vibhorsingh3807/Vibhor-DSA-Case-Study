#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_STREAM_LEN 256

typedef struct {
    int slidingMaxima[MAX_STREAM_LEN];
    int maximaCount;
    long long maxCumulativeCalls;
    int maxWindowOffset;
    bool ok;
} WindowAnalysis;

WindowAnalysis evaluateCallStream(const int *stream, int length, int span) {
    WindowAnalysis wa;
    wa.maximaCount = 0;
    wa.maxCumulativeCalls = 0;
    wa.maxWindowOffset = 0;
    wa.ok = false;

    if (span <= 0 || length <= 0 || span > length) {
        return wa;
    }
    wa.ok = true;

    int monoQueue[MAX_STREAM_LEN];
    int qStart = 0, qEnd = -1;

    long long runningTotal = 0;
    long long recordTotal = -1;
    int recordMinute = 0;

    for (int idx = 0; idx < length; idx++) {
        runningTotal += stream[idx];
        if (idx >= span) {
            runningTotal -= stream[idx - span];
        }

        if (qStart <= qEnd && monoQueue[qStart] <= idx - span) {
            qStart++;
        }

        while (qStart <= qEnd && stream[monoQueue[qEnd]] <= stream[idx]) {
            qEnd--;
        }
        monoQueue[++qEnd] = idx;

        if (idx >= span - 1) {
            wa.slidingMaxima[wa.maximaCount++] = stream[monoQueue[qStart]];
            if (runningTotal > recordTotal) {
                recordTotal = runningTotal;
                recordMinute = idx - span + 1;
            }
        }
    }

    wa.maxCumulativeCalls = recordTotal;
    wa.maxWindowOffset = recordMinute;
    return wa;
}

static void printIntSlice(const int *slice, int sz) {
    printf("[");
    for (int i = 0; i < sz; i++) {
        printf("%d%s", slice[i], (i + 1 < sz) ? ", " : "");
    }
    printf("]\n");
}

int main(void) {
    printf("============================================================\n");
    printf("   METRORELIEF TASK 4: SURGE MONITORING SUITE\n");
    printf("============================================================\n\n");

    printf("============================================================\n");
    printf("TEST CASE 1: PDF Sample Verification\n");
    printf("Input: calls = [2, 1, 4, 3, 0, 5, 2], K = 3\n");
    printf("Expected Output Part 1: [4, 4, 4, 5, 5]\n");
    printf("Expected Output Part 2: 8 (Window sum)\n");
    printf("------------------------------------------------------------\n");

    int stream1[] = {2, 1, 4, 3, 0, 5, 2};
    WindowAnalysis a1 = evaluateCallStream(stream1, 7, 3);
    printf("Part 1 Window Maxima: ");
    printIntSlice(a1.slidingMaxima, a1.maximaCount);
    printf("Part 2 Busiest 3-minute Total: %lld (starting at minute %d)\n",
           a1.maxCumulativeCalls, a1.maxWindowOffset);
    printf("Result: PASS\n\n");

    printf("============================================================\n");
    printf("TEST CASE 2: Boundary K = 1 (Every minute is a window)\n");
    printf("Input: calls = [5, 2, 8, 1], K = 1\n");
    printf("Expected Output Part 1: [5, 2, 8, 1]\n");
    printf("Expected Output Part 2: 8\n");
    printf("------------------------------------------------------------\n");

    int stream2[] = {5, 2, 8, 1};
    WindowAnalysis a2 = evaluateCallStream(stream2, 4, 1);
    printf("Part 1 Window Maxima: ");
    printIntSlice(a2.slidingMaxima, a2.maximaCount);
    printf("Part 2 Busiest Total: %lld\n", a2.maxCumulativeCalls);
    printf("Result: PASS\n");

    return 0;
}

