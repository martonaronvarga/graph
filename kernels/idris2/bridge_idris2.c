#include "../graph_orientation_bridge.h"

extern int idris2_graph_orientation_solve(const go_raw_problem*, go_raw_solution*);

int graph_orientation_solve(const go_raw_problem *problem, go_raw_solution *out) {
    return idris2_graph_orientation_solve(problem, out);
}
