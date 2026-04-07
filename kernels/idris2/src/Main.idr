module Main

import System
import GraphOrientationFFI

%foreign "C:get_global_problem"
prim_get_global_problem : PrimIO AnyPtr

%foreign "C:get_global_mode"
prim_get_global_mode : PrimIO Int

%foreign "C:get_global_sol"
prim_get_global_sol : PrimIO AnyPtr

%foreign "C:set_global_res"
prim_set_global_res : Int -> PrimIO ()

get_global_problem : IO AnyPtr
get_global_problem = primIO prim_get_global_problem

get_global_mode : IO Int
get_global_mode = primIO prim_get_global_mode

get_global_sol : IO AnyPtr
get_global_sol = primIO prim_get_global_sol

set_global_res : Int -> IO ()
set_global_res res = primIO (prim_set_global_res res)

main : IO ()
main = do
  p_prob <- get_global_problem
  mode <- get_global_mode
  p_sol <- get_global_sol
  
  res <- ffi_solve p_prob mode p_sol
  
  set_global_res res
