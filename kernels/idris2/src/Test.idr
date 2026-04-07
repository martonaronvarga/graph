module Main

import GraphOrientation

export
Show Solution where
  show (MkSolution f u v viols) =
    "MkSolution { feasible = " ++ show f ++ 
    ", directed_u = " ++ show u ++ 
    ", directed_v = " ++ show v ++ 
    ", violators = " ++ show viols ++ " }"


main : IO ()
main = do
  let n     = 3
      m     = 3
      boundsL = [1,1,1]
      euL    = [0,1,2]
      evL    = [1,2,0]
      mbProb = do
        euVect <- listToVect m euL
        evVect <- listToVect m evL
        boundsVect <- listToVect n boundsL
        pure (MkProblem n boundsVect m euVect evVect)
  case mbProb of
    Just p => printLn $ solve p 0
    Nothing => putStrLn "Invalid test problem."
