module GraphOrientationFFIBridge

import GraphOrientationFast
import Data.Vect

%default total

public export
record Problem where
  constructor MkProblem
  n : Int
  bounds : List Int64
  m : Int
  eu : List Int
  ev : List Int

public export
record Solution where
  constructor MkSolution
  feasible : Bool
  directed_u : List Int
  directed_v : List Int
  violators : List Int

export
ffiToKernel : GraphOrientationFFIBridge.Problem -> Maybe GraphOrientationFast.Problem
ffiToKernel (MkProblem n bounds m eu ev) = do
  boundsV <- listToVect (cast n) (map cast bounds)
  euV     <- listToVect (cast m) eu
  evV     <- listToVect (cast m) ev
  pure (GraphOrientationFast.MkProblem (cast n) boundsV (cast m) euV evV)

export
kernelToFFI : GraphOrientationFast.Solution -> GraphOrientationFFIBridge.Solution
kernelToFFI (MkSolution feasible dus dvs viols) =
  MkSolution feasible dus dvs viols
