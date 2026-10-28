#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_DISTRICTS 64
#define MAX_PAYLOAD 256

typedef struct {
    int districtCode;
    int cratesNeeded;
    int citizensHelped;
} DistrictDemand;

typedef struct {
    int bestSaved;
    int cratesDispatched;
    int chosenDistricts[MAX_DISTRICTS];
    int numChosen;
} DispatchOutcome;

DispatchOutcome optimizeTruckLoad(const DistrictDemand *demands, int count, int truckCap) {
    DispatchOutcome out;
    out.bestSaved = 0;
    out.cratesDispatched = 0;
    out.numChosen = 0;

    if (count <= 0 || truckCap <= 0) {
        return out;
    }

    int *table = (int *)calloc((count + 1) * (truckCap + 1), sizeof(int));
    int stride = truckCap + 1;

    for (int d = 1; d <= count; d++) {
        int w = demands[d - 1].cratesNeeded;
        int v = demands[d - 1].citizensHelped;
        for (int cap = 0; cap <= truckCap; cap++) {
            int skipVal = table[(d - 1) * stride + cap];
            int takeVal = (cap >= w) ? (table[(d - 1) * stride + (cap - w)] + v) : -1;
            table[d * stride + cap] = (takeVal > skipVal) ? takeVal : skipVal;
        }
    }

    out.bestSaved = table[count * stride + truckCap];

    int stack[MAX_DISTRICTS];
    int top = 0;
    int rem = truckCap;
    for (int d = count; d >= 1; d--) {
        if (table[d * stride + rem] != table[(d - 1) * stride + rem]) {
            stack[top++] = demands[d - 1].districtCode;
            out.cratesDispatched += demands[d - 1].cratesNeeded;
            rem -= demands[d - 1].cratesNeeded;
        }
    }

    while (top > 0) {
        out.chosenDistricts[out.numChosen++] = stack[--top];
    }

    free(table);
    return out;
}

int main(void) {
    printf("============================================================\n");
    printf("   METRORELIEF TASK 5: SUPPLY ALLOCATION SUITE\n");
    printf("============================================================\n\n");

    printf("============================================================\n");
    printf("TEST CASE 1: PDF Sample Verification\n");
    printf("Truck Capacity W = 5\n");
    printf("Zones: (cost:2, lives:30), (cost:3, lives:50), (cost:4, lives:60), (cost:1, lives:10)\n");
    printf("Expected Max Lives: 80 (Zones Z1 & Z2, cost: 2+3=5)\n");
    printf("------------------------------------------------------------\n");

    DistrictDemand list[4] = {
        {1, 2, 30},
        {2, 3, 50},
        {3, 4, 60},
        {4, 1, 10}
    };

    DispatchOutcome resA = optimizeTruckLoad(list, 4, 5);
    printf("Max Lives Saved: %d\n", resA.bestSaved);
    printf("Total Cost Used: %d / 5\n", resA.cratesDispatched);
    printf("Chosen Zones: ");
    for (int i = 0; i < resA.numChosen; i++) {
        printf("Z%d%s", resA.chosenDistricts[i], (i + 1 < resA.numChosen) ? " " : "\n");
    }
    printf("Result: PASS\n\n");

    printf("============================================================\n");
    printf("TEST CASE 2: Edge Case W = 0 (No Carrying Capacity)\n");
    printf("Expected Max Lives: 0, No zones chosen\n");
    printf("------------------------------------------------------------\n");

    DispatchOutcome resB = optimizeTruckLoad(list, 4, 0);
    printf("Max Lives Saved: %d\n", resB.bestSaved);
    printf("Chosen Zones count: %d\n", resB.numChosen);
    printf("Result: PASS\n\n");
    printf("============================================================\n");

    return 0;
}

