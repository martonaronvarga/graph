module GraphOrientationFFI

import GraphOrientation
import GraphOrientationFFIBridge
import Data.List

%default total

-- Foreign imports to read arrays
%foreign "C:c_read_i64_array, idris_bridge"
c_read_i64 : AnyPtr -> Int -> Int64

%foreign "C:c_read_size_array, idris_bridge"
c_read_size : AnyPtr -> Int -> Int

%foreign "C:c_write_size_array, idris_bridge"
c_write_size : AnyPtr -> Int -> Int -> PrimIO ()

-- Foreign imports to read problem struct
%foreign "C:c_get_problem_n, idris_bridge"
c_get_n : AnyPtr -> Int

%foreign "C:c_get_problem_m, idris_bridge"
c_get_m : AnyPtr -> Int

%foreign "C:c_get_problem_bounds, idris_bridge"
c_get_bounds : AnyPtr -> AnyPtr

%foreign "C:c_get_problem_eu, idris_bridge"
c_get_eu : AnyPtr -> AnyPtr

%foreign "C:c_get_problem_ev, idris_bridge"
c_get_ev : AnyPtr -> AnyPtr

-- Foreign imports to write solution struct
%foreign "C:c_set_solution_feasible, idris_bridge"
c_set_feasible : AnyPtr -> Int -> PrimIO ()

%foreign "C:c_get_solution_du, idris_bridge"
c_get_du : AnyPtr -> AnyPtr

%foreign "C:c_get_solution_dv, idris_bridge"
c_get_dv : AnyPtr -> AnyPtr

%foreign "C:c_get_solution_d_cap, idris_bridge"
c_get_dcap : AnyPtr -> Int

%foreign "C:c_set_solution_d_len, idris_bridge"
c_set_dlen : AnyPtr -> Int -> PrimIO ()

%foreign "C:c_get_solution_viols, idris_bridge"
c_get_viols : AnyPtr -> AnyPtr

%foreign "C:c_get_solution_v_cap, idris_bridge"
c_get_vcap : AnyPtr -> Int

%foreign "C:c_set_solution_v_len, idris_bridge"
c_set_vlen : AnyPtr -> Int -> PrimIO ()


-- Read a List safely by calling the C array reader multiple times
readI64List : AnyPtr -> Int -> List Int64
readI64List ptr 0 = []
readI64List ptr n = map (\i => c_read_i64 ptr i) [0 .. (n-1)]

readSizeList : AnyPtr -> Int -> List Int
readSizeList ptr 0 = []
readSizeList ptr n = map (\i => c_read_size ptr i) [0 .. (n-1)]

-- Write a List safely by calling the C array writer multiple times
writeSizeList : AnyPtr -> List Int -> IO ()
writeSizeList ptr xs = 
  let indexed = zip [0 .. (cast (length xs) - (the Int 1))] xs
  in traverse_ (\(i, val) => primIO (c_write_size ptr (cast i) val)) indexed

export
readRawProblem : AnyPtr -> GraphOrientationFFIBridge.Problem
readRawProblem p =
  let n = c_get_n p
      m = c_get_m p
      b_ptr = c_get_bounds p
      eu_ptr = c_get_eu p
      ev_ptr = c_get_ev p
      
      bounds = readI64List b_ptr n
      eus = readSizeList eu_ptr m
      evs = readSizeList ev_ptr m
  in MkProblem n bounds m eus evs

export
writeRawSolution : AnyPtr -> GraphOrientationFFIBridge.Solution -> IO ()
writeRawSolution p sol = do
  primIO (c_set_feasible p (if sol.feasible then 1 else 0))
  
  let du_ptr = c_get_du p
  let dv_ptr = c_get_dv p
  let d_cap = c_get_dcap p
  let d_len = min d_cap (cast (length sol.directed_u))
  
  writeSizeList du_ptr (take (cast d_len) sol.directed_u)
  writeSizeList dv_ptr (take (cast d_len) sol.directed_v)
  primIO (c_set_dlen p d_len)
  
  let viols_ptr = c_get_viols p
  let v_cap = c_get_vcap p
  let v_len = min v_cap (cast (length sol.violators))
  
  writeSizeList viols_ptr (take (cast v_len) sol.violators)
  primIO (c_set_vlen p v_len)

-- %export "refc:idris_graph_orientation_solve" unsupported
export
covering
ffi_solve : AnyPtr -> Int -> AnyPtr -> IO Int
ffi_solve p_problem mode p_sol = do
  let ffiPb = readRawProblem p_problem
  case ffiToKernel ffiPb of
    Just kernelProb => do
      let sol = GraphOrientation.solve kernelProb mode
      writeRawSolution p_sol (kernelToFFI sol)
      pure 0
    Nothing => pure 1
