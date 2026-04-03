#ifndef GRAPH_ORIENTATION_BRIDGE_H
#define GRAPH_ORIENTATION_BRIDGE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    size_t n;
    size_t m;
    const int64_t *bounds;
    const size_t *edges_u;
    const size_t *edges_v;
} go_raw_problem;

typedef struct {
    uint8_t feasible;
    size_t *directed_u;
    size_t *directed_v;
    size_t directed_capacity;
    size_t directed_len;
    size_t *violators;
    size_t violators_capacity;
    size_t violators_len;
} go_raw_solution;

int graph_orientation_solve(const go_raw_problem *problem, go_raw_solution *out);

#ifdef __cplusplus
}
#endif

#endif
