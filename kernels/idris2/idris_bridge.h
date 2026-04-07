#ifndef GRAPH_ORIENTATION_C_H
#define GRAPH_ORIENTATION_C_H

#include <stdint.h>
#include <stddef.h>

void* get_global_problem(void);
int64_t get_global_mode(void);
void* get_global_sol(void);
void set_global_res(int64_t res);

int64_t c_read_i64_array(const int64_t* arr, int64_t index);
int64_t c_read_size_array(const size_t* arr, int64_t index);
void c_write_size_array(size_t* arr, int64_t index, int64_t val);

int64_t c_get_problem_n(const void* p);
int64_t c_get_problem_m(const void* p);
void* c_get_problem_bounds(const void* p);
void* c_get_problem_eu(const void* p);
void* c_get_problem_ev(const void* p);

void c_set_solution_feasible(void* p, int64_t val);
void* c_get_solution_du(void* p);
void* c_get_solution_dv(void* p);
int64_t c_get_solution_d_cap(const void* p);
void c_set_solution_d_len(void* p, int64_t val);

void* c_get_solution_viols(void* p);
int64_t c_get_solution_v_cap(const void* p);
void c_set_solution_v_len(void* p, int64_t val);

#endif
