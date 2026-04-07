#include <stdint.h>
#include <stddef.h>
#include "idris_bridge.h"
#include "../graph_orientation_ffi.h"

void* global_problem = NULL;
int64_t global_mode = 0;
void* global_sol = NULL;
int64_t global_res = 0;

void* get_global_problem(void) { return global_problem; }
int64_t get_global_mode(void) { return (int64_t)global_mode; }
void* get_global_sol(void) { return global_sol; }
void set_global_res(int64_t res) { global_res = res; }

//  call the renamed Idris main function
extern int idris_main(int argc, char *argv[]);

int64_t c_read_i64_array(const int64_t* arr, int64_t index) {
    return arr[index];
}

int64_t c_read_size_array(const size_t* arr, int64_t index) {
    return (int64_t)arr[index];
}

void c_write_size_array(size_t* arr, int64_t index, int64_t val) {
    arr[index] = (size_t)val;
}

int64_t c_get_problem_n(const void* p) { return (int64_t)((const RawProblem*)p)->n; }
int64_t c_get_problem_m(const void* p) { return (int64_t)((const RawProblem*)p)->m; }
void* c_get_problem_bounds(const void* p) { return (void*)((const RawProblem*)p)->bounds; }
void* c_get_problem_eu(const void* p) { return (void*)((const RawProblem*)p)->edges_u; }
void* c_get_problem_ev(const void* p) { return (void*)((const RawProblem*)p)->edges_v; }

void c_set_solution_feasible(void* p, int64_t val) { ((RawSolution*)p)->feasible = (uint8_t)val; }
void* c_get_solution_du(void* p) { return (void*)((RawSolution*)p)->directed_u; }
void* c_get_solution_dv(void* p) { return (void*)((RawSolution*)p)->directed_v; }
int64_t c_get_solution_d_cap(const void* p) { return (int64_t)((const RawSolution*)p)->directed_capacity; }
void c_set_solution_d_len(void* p, int64_t val) { ((RawSolution*)p)->directed_len = (size_t)val; }

void* c_get_solution_viols(void* p) { return (void*)((RawSolution*)p)->violators; }
int64_t c_get_solution_v_cap(const void* p) { return (int64_t)((const RawSolution*)p)->violators_capacity; }
void c_set_solution_v_len(void* p, int64_t val) { ((RawSolution*)p)->violators_len = (size_t)val; }


int graph_orientation_solve(const RawProblem* p_problem, int mode, RawSolution* p_sol) {
    global_problem = (void*)p_problem;
    global_mode = (int64_t)mode;
    global_sol = (void*)p_sol;
    
    char* dummy_argv[] = {"idris_solver", NULL};
    idris_main(1, dummy_argv);
    
    return (int)global_res;
}
