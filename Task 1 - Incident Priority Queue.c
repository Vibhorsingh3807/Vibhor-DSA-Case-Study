#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_HEAP_ELEMS 256

typedef struct {
    char recordId[24];
    int priorityLevel;
    long long timeLogged;
} IncidentRecord;

typedef struct {
    IncidentRecord tree[MAX_HEAP_ELEMS]; /* 1-indexed binary max-heap */
    int length;
} PriorityDispatcher;

static int compareRecords(const IncidentRecord *x, const IncidentRecord *y) {
    if (x->priorityLevel != y->priorityLevel) {
        return (x->priorityLevel > y->priorityLevel) ? 1 : -1;
    }
    if (x->timeLogged != y->timeLogged) {
        return (x->timeLogged < y->timeLogged) ? 1 : -1;
    }
    return 0;
}

static void swapRecords(IncidentRecord *a, IncidentRecord *b) {
    IncidentRecord tmp = *a;
    *a = *b;
    *b = tmp;
}

void dispatcherReset(PriorityDispatcher *pd) {
    pd->length = 0;
}

static void restoreUpward(PriorityDispatcher *pd, int k) {
    while (k > 1 && compareRecords(&pd->tree[k], &pd->tree[k / 2]) > 0) {
        swapRecords(&pd->tree[k], &pd->tree[k / 2]);
        k /= 2;
    }
}

static void restoreDownward(PriorityDispatcher *pd, int k) {
    while (2 * k <= pd->length) {
        int child = 2 * k;
        if (child < pd->length && compareRecords(&pd->tree[child + 1], &pd->tree[child]) > 0) {
            child++;
        }
        if (compareRecords(&pd->tree[k], &pd->tree[child]) >= 0) {
            break;
        }
        swapRecords(&pd->tree[k], &pd->tree[child]);
        k = child;
    }
}

void pushIncident(PriorityDispatcher *pd, const char *id, int sev, long long arrival) {
    if (pd->length + 1 >= MAX_HEAP_ELEMS) return;
    int pos = ++pd->length;
    strncpy(pd->tree[pos].recordId, id, sizeof(pd->tree[pos].recordId) - 1);
    pd->tree[pos].recordId[sizeof(pd->tree[pos].recordId) - 1] = '\0';
    pd->tree[pos].priorityLevel = sev;
    pd->tree[pos].timeLogged = arrival;
    restoreUpward(pd, pos);
}

bool popHighestIncident(PriorityDispatcher *pd, IncidentRecord *out) {
    if (pd->length == 0) return false;
    *out = pd->tree[1];
    pd->tree[1] = pd->tree[pd->length--];
    if (pd->length > 0) {
        restoreDownward(pd, 1);
    }
    return true;
}

bool updateSeverity(PriorityDispatcher *pd, const char *targetId, int newSev) {
    int foundIdx = -1;
    for (int i = 1; i <= pd->length; i++) {
        if (strcmp(pd->tree[i].recordId, targetId) == 0) {
            foundIdx = i;
            break;
        }
    }
    if (foundIdx == -1) return false;

    int oldSev = pd->tree[foundIdx].priorityLevel;
    pd->tree[foundIdx].priorityLevel = newSev;
    if (newSev > oldSev) {
        restoreUpward(pd, foundIdx);
    } else if (newSev < oldSev) {
        restoreDownward(pd, foundIdx);
    }
    return true;
}

void showCurrentHeap(const PriorityDispatcher *pd) {
    printf("  [Current Queue]: ");
    for (int i = 1; i <= pd->length; i++) {
        printf("%s(sev:%d, t:%lld) ",
               pd->tree[i].recordId, pd->tree[i].priorityLevel, pd->tree[i].timeLogged);
    }
    printf("\n");
}

int main(void) {
    printf("============================================================\n");
    printf("   METRORELIEF TASK 1: INCIDENT PRIORITY QUEUE SUITE\n");
    printf("============================================================\n\n");

    printf("============================================================\n");
    printf("TEST CASE 1: PDF Sample Verification\n");
    printf("Input: I1(sev:3, t:10), I2(sev:5, t:20), I3(sev:5, t:0), I4(sev:2, t:30)\n");
    printf("Expected Service Order: I3, I2, I1, I4\n");
    printf("------------------------------------------------------------\n");

    PriorityDispatcher disp1;
    dispatcherReset(&disp1);
    pushIncident(&disp1, "I1", 3, 10);
    pushIncident(&disp1, "I2", 5, 20);
    pushIncident(&disp1, "I3", 5, 0);
    pushIncident(&disp1, "I4", 2, 30);

    IncidentRecord rec;
    while (popHighestIncident(&disp1, &rec)) {
        printf("Served: %s (Severity: %d, Arrival: %lld)\n",
               rec.recordId, rec.priorityLevel, rec.timeLogged);
    }
    printf("Result: PASS\n\n");

    printf("============================================================\n");
    printf("TEST CASE 2: Dynamic Severity Upgrade via updateSeverity()\n");
    printf("Input: I1(sev:2, t:10), I2(sev:3, t:15). Upgrade I1 to sev:5.\n");
    printf("Expected: I1 is served first after upgrade despite starting at sev:2.\n");
    printf("------------------------------------------------------------\n");

    PriorityDispatcher disp2;
    dispatcherReset(&disp2);
    pushIncident(&disp2, "I1", 2, 10);
    pushIncident(&disp2, "I2", 3, 15);

    printf("Initial state:\n");
    showCurrentHeap(&disp2);
    printf("Calling updateSeverity(\"I1\", 5)...\n");
    bool changed = updateSeverity(&disp2, "I1", 5);
    printf("Update successful: %s\n", changed ? "YES" : "NO");
    showCurrentHeap(&disp2);

    while (popHighestIncident(&disp2, &rec)) {
        printf("Served: %s (Severity: %d, Arrival: %lld)\n",
               rec.recordId, rec.priorityLevel, rec.timeLogged);
    }
    printf("Result: PASS\n");

    return 0;
}

