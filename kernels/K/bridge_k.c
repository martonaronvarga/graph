#include "k.h"
#include "graph_orientation_bridge.h"

static int k_initialized = 0;

static void k_init_once() {
    if (!k_initialized) {
        kinit();
        K r = k(0, (S)"\\l graph_orientation.k", (K)0);
        r0(r);
        k_initialized = 1;
    }
}

int graph_orientation_solve(const go_raw_problem *problem, go_raw_solution *out) {
    k_init_once();
    J n = (J)problem->n, m = (J)problem->m;
    K kmode = ki(0); // Or set as desired from environment or parameter
    K kn = ki(n);
    K kb = ktn(KJ, n);
    for (J i = 0; i < n; ++i) kK(kb)[i] = kj(problem->bounds[i]);
    K keu = ktn(KJ, m);
    K kev = ktn(KJ, m);
    for (J i = 0; i < m; ++i) {
        kK(keu)[i] = kj(problem->edges_u[i]);
        kK(kev)[i] = kj(problem->edges_v[i]);
    }
    K args = knk(5, kmode, kn, kb, keu, kev);
    K res = k(0, (S)"solve", args, (K)0);

    if (!res || res->t != 0 || res->n != 3) { r0(res); return 2; }

    K kok = kK(res)[0], korient = kK(res)[1], kviol = kK(res)[2];
    out->feasible = (kok->i)?1:0;
    size_t out_len = m < out->directed_capacity ? m : out->directed_capacity;
    for (size_t i = 0; i < out_len; ++i) {
        J o = kK(korient)[i]->i;
        if (o==0) {
            out->directed_u[i] = problem->edges_u[i];
            out->directed_v[i] = problem->edges_v[i];
        } else {
            out->directed_u[i] = problem->edges_v[i];
            out->directed_v[i] = problem->edges_u[i];
        }
    }
    out->directed_len = out_len;

    out->violators_len = kviol->n < out->violators_capacity ? kviol->n : out->violators_capacity;
    for (size_t i = 0; i < out->violators_len; ++i)
        out->violators[i] = kK(kviol)[i]->i;

    r0(res);
    return 0;
}
