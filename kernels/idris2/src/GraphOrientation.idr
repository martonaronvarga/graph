module GraphOrientation

import Data.Vect
import Data.List
import Data.Nat
import Data.Fin
import Data.Maybe

%default total

public export
record Problem where
  constructor MkProblem
  n : Nat
  bounds : Vect n Int
  m : Nat
  eu : Vect m Int
  ev : Vect m Int

public export
record Solution where
  constructor MkSolution
  feasible : Bool
  directed_u : List Int
  directed_v : List Int
  violators  : List Int

export
listToVect : (k: Nat) -> List a -> Maybe (Vect k a)
listToVect k xs = exactLength k (fromList xs)

finRange : (k : Nat) -> Vect k (Fin k)
finRange Z = []
finRange (S j) = FZ :: map FS (finRange j)
 
mkMatrix : (k : Nat) -> Int -> Vect k (Vect k Int)
mkMatrix k v = replicate k (replicate k v)

addArc : {k : Nat} -> Vect k (Vect k Int) -> Fin k -> Fin k -> Int -> Vect k (Vect k Int)
addArc mat u v cap =
  let row = index u mat
      updated = replaceAt v (index v row + cap) row
  in replaceAt u updated mat

covering
bfs : {size:Nat} -> Vect size (Vect size Int) -> Vect size (Vect size Int) -> Fin size -> Fin size -> Maybe (List (Fin size))
bfs {size} cap flow s t =
  let
    pred : Vect size (Maybe (Fin size))
    pred = replicate size Nothing

    bfs' : List (Fin size) -> Vect size (Maybe (Fin size)) -> Maybe (List (Fin size))
    bfs' []      _ = Nothing
    bfs' (u::us) prd =
      let
        vs = toList (zip (finRange size) (index u cap))
        newPairs = filter (\(v', _) => isNothing (index v' prd) && index v' (index u cap) - index v' (index u flow) > 0) vs
        prd' = foldl (\p, (v',_) => replaceAt v' (Just u) p) prd newPairs
      in case find (\(v', _) => v' == t) newPairs of
          Just _ =>
            let
              recPath : Fin size -> List (Fin size) -> List (Fin size)
              recPath v acc =
                if v == s then (v :: acc)
                else case index v prd' of
                  Nothing => (v :: acc)
                  Just pv => recPath pv (v :: acc)
            in Just (recPath t [])
          Nothing =>
            let
              q' = us ++ map fst newPairs
            in bfs' q' prd'
  in bfs' [s] (replaceAt s (Just s) pred)

covering
findBottleneck : {size:Nat} -> Vect size (Vect size Int) -> Vect size (Vect size Int) -> List (Fin size) -> Int
findBottleneck {size} cap flow (a :: b :: xs) =
  let res = index b (index a cap) - index b (index a flow)
  in min res (findBottleneck cap flow (b::xs))
findBottleneck _ _ _ = the Int 999999999

augmentFlow : {size:Nat} -> Vect size (Vect size Int) -> List (Fin size) -> Int -> Vect size (Vect size Int)
augmentFlow {size} flow (a :: b :: xs) delta =
  let
    rowA = replaceAt b (index b (index a flow) + delta) (index a flow)
    flow' = replaceAt a rowA flow
    rowB = replaceAt a (index a (index b flow') - delta) (index b flow')
    flow'' = replaceAt b rowB flow'
  in augmentFlow flow'' (b::xs) delta
augmentFlow {size} flow _ _ = flow

covering
edmondsKarp : {size: Nat} -> Vect size (Vect size Int) -> Fin size -> Fin size -> (Vect size (Vect size Int), Int)
edmondsKarp {size} cap s t =
  let
    flow0 = mkMatrix size 0
    go : Vect size (Vect size Int) -> Int -> (Vect size (Vect size Int), Int)
    go flow tot =
      case bfs cap flow s t of
        Nothing => (flow, tot)
        Just path =>
          let
            delta = findBottleneck cap flow path
            flow' = augmentFlow flow path delta
          in go flow' (tot + delta)
  in go flow0 0

covering
levelBFS : {size:Nat} -> Vect size (Vect size Int) -> Vect size (Vect size Int) -> Fin size -> Vect size (Maybe Nat)
levelBFS {size} cap flow s =
  let
    level = replicate size Nothing
    go : List (Fin size) -> Vect size (Maybe Nat) -> Vect size (Maybe Nat)
    go [] lvl = lvl
    go (u::us) lvl =
      let
        currlvl = case index u lvl of
          Just l => l
          Nothing => 0
        vs = toList (zip (finRange size) (index u cap))
        newVs = filter (\(v,_) => isNothing (index v lvl) && index v (index u cap) - index v (index u flow) > 0) vs
        lvl' = foldl (\l', (v,_) => replaceAt v (Just (currlvl + 1)) l') lvl newVs
        us' = us ++ map fst newVs
      in go us' lvl'
  in go [s] (replaceAt s (Just 0) level)

covering
dinicDFS : {size:Nat} -> Vect size (Vect size Int) -> Vect size (Vect size Int) -> Vect size (Maybe Nat) -> Fin size -> Fin size -> Int -> (Vect size (Vect size Int), Int)
dinicDFS {size} cap flow level u t amt =
  if u == t then (flow, amt)
  else
    let
      vs = toList (finRange size)
      go : List (Fin size) -> Vect size (Vect size Int) -> Int -> (Vect size (Vect size Int), Int)
      go [] fl _ = (fl, 0)
      go (v::vs') fl a =
        case (index v level, index u level) of
          (Just lv, Just lu) =>
            if lv == lu + 1 && index v (index u cap) - index v (index u fl) > 0 then
              let
                delta = min a (index v (index u cap) - index v (index u fl))
              in case dinicDFS cap fl level v t delta of
                (flow', pushed) =>
                  if pushed > 0 then
                    let
                      rowU = replaceAt v (index v (index u flow') + pushed) (index u flow')
                      flow'' = replaceAt u rowU flow'
                      rowV = replaceAt u (index u (index v flow'') - pushed) (index v flow'')
                      flow''' = replaceAt v rowV flow''
                    in (flow''', pushed)
                  else go vs' fl a
            else go vs' fl a
          _ => go vs' fl a
    in go vs flow amt

covering
dinic : {size : Nat} -> Vect size (Vect size Int) -> Fin size -> Fin size -> (Vect size (Vect size Int), Int)
dinic {size} cap s t =
  let
    flow0 = mkMatrix size 0
    go : Vect size (Vect size Int) -> Int -> (Vect size (Vect size Int), Int)
    go flow tot =
      let
        level = levelBFS cap flow s
      in case index t level of
          Nothing => (flow, tot)
          Just _ =>
            let
              block : Vect size (Vect size Int) -> Int -> (Vect size (Vect size Int), Int)
              block fl tot' =
                case dinicDFS cap fl level s t (the Int 999999999) of
                  (fl', pushed) =>
                    if pushed == 0 then (fl, tot')
                    else block fl' (tot' + pushed)
            in case block flow 0 of
              (flow', tot') => go flow' (tot + tot')
  in go flow0 0

-- ------------ Push-Relabel ---------------------------

covering
pushRelabel : {size:Nat} -> Vect size (Vect size Int) -> Fin size -> Fin size -> (Vect size (Vect size Int), Int)
pushRelabel {size} cap s t =
  let
    h0 = replaceAt s (cast size) (replicate size (the Int 0))
    flow0 = mkMatrix size 0
    flow1 =
      foldl (\fl, v =>
        let capSv = index v (index s cap)
        in if capSv > 0 then
          let
            rowS = replaceAt v capSv (index s fl)
            fl' = replaceAt s rowS fl
            rowV = replaceAt s (-capSv) (index v fl')
          in replaceAt v rowV fl'
        else fl) flow0 (finRange size)
    excess0 =
      foldl (\ex, v =>
        let capSv = index v (index s cap)
        in if capSv > 0
          then replaceAt v capSv ex
          else ex
      ) (the (Vect size Int) (replicate size (the Int 0))) (finRange size)
    queue0 = filter (\v => v /= s && v /= t && index v excess0 > 0) (toList (finRange size))
    
    step : Vect size (Vect size Int) -> Vect size Int -> Vect size Int -> Vect size (Vect size Int) -> List (Fin size) -> (Vect size (Vect size Int), Int)
    step cap h excess flow [] = (flow, index t excess)
    step cap h excess flow (u::qs) =
      let
        vs = toList (finRange size)
        push : (Vect size (Vect size Int), Vect size Int, List (Fin size)) -> Fin size -> (Vect size (Vect size Int), Vect size Int, List (Fin size))
        push (fl, ex, q) v =
          let resid = index v (index u cap) - index v (index u fl)
          in if resid > 0 && index u h == index v h + 1 && index u ex > 0 then
            let
              amt = min (index u ex) resid
              rowU = replaceAt v (index v (index u fl) + amt) (index u fl)
              fl' = replaceAt u rowU fl
              rowV = replaceAt u (index u (index v fl') - amt) (index v fl')
              fl'' = replaceAt v rowV fl'
              exU' = index u ex - amt
              exV' = index v ex + amt
              ex' = replaceAt u exU' (replaceAt v exV' ex)
              q' = if v /= s && v /= t && index v ex == 0 && exV' > 0 then q ++ [v] else q
            in (fl'', ex', q')
          else (fl, ex, q)
      in case foldl push (flow, excess, qs) vs of
        (flow', excess', q') =>
          if index u excess' > 0 then
            let
              validVs = filter (\v => index v (index u cap) - index v (index u flow') > 0) vs
              minH = foldl (\acc, w => min acc (index w h)) (the Int 999999999) validVs
            in step cap (replaceAt u (1 + minH) h) excess' flow' (qs++[u])
          else
            step cap h excess' flow' q'
  in step cap h0 excess0 flow1 queue0

-- ---------- Orientation -------------

totalNodes : Nat -> Nat -> Nat
totalNodes n m = S (S (m + n))

buildCapacity : (pb : Problem) -> Vect (totalNodes pb.n pb.m) (Vect (totalNodes pb.n pb.m) Int)
buildCapacity (MkProblem n bounds m eu ev) =
  let
    mat0 = mkMatrix (totalNodes n m) 0
    s : Fin (totalNodes n m) = FZ
    t : Fin (totalNodes n m) = last
    
    mat1 = foldl (\acc, i =>
             let
               k = finToNat i
               eNodeF = fromMaybe FZ (natToFin (1 + k) (totalNodes n m))
               
               u = index i eu
               v = index i ev
               uNodeF = fromMaybe FZ (natToFin (1 + m + cast u) (totalNodes n m))
               vNodeF = fromMaybe FZ (natToFin (1 + m + cast v) (totalNodes n m))
               
               a1 = addArc acc s eNodeF 1
               a2 = addArc a1 eNodeF uNodeF 1
               a3 = addArc a2 eNodeF vNodeF 1
             in a3
           ) mat0 (finRange m)

    mat2 = foldl (\acc, i =>
             let
               k = finToNat i
               vNodeF = fromMaybe FZ (natToFin (1 + m + k) (totalNodes n m))
               bound = index i bounds
             in addArc acc vNodeF t bound
           ) mat1 (finRange n)
  in mat2

orientEdges : (pb : Problem) -> Vect (totalNodes pb.n pb.m) (Vect (totalNodes pb.n pb.m) Int) -> Vect pb.m (Int, Int)
orientEdges (MkProblem n bounds m eu ev) flow =
  map (\i =>
    let
      k = finToNat i
      eNodeF = fromMaybe FZ (natToFin (1 + k) (totalNodes n m))
      u = index i eu
      v = index i ev
      uNodeF = fromMaybe FZ (natToFin (1 + m + cast u) (totalNodes n m))
    in if index uNodeF (index eNodeF flow) > 0 then (u,v) else (v,u)
  ) (finRange m)

covering
residualReachable : {size : Nat} -> Vect size (Vect size Int) -> Vect size (Vect size Int) -> Fin size -> Vect size Bool
residualReachable {size} cap flow s =
  let
    seen0 = replaceAt s True (replicate size False)
    go : List (Fin size) -> Vect size Bool -> Vect size Bool
    go [] seen = seen
    go (u::us) seen =
      let vs = toList (zip (finRange size) (index u cap))
          newVs = filter (\(v,_) => not (index v seen) && index v (index u cap) - index v (index u flow) > 0) vs
          seen' = foldl (\sn, (v,_) => replaceAt v True sn) seen newVs
          us' = us ++ map fst newVs
      in go us' seen'
  in go [s] seen0

covering
violators : (pb : Problem) -> Vect (totalNodes pb.n pb.m) (Vect (totalNodes pb.n pb.m) Int) -> Vect (totalNodes pb.n pb.m) (Vect (totalNodes pb.n pb.m) Int) -> List Int
violators (MkProblem n bounds m eu ev) cap flow =
  let
    s : Fin (totalNodes n m) = FZ
    seen = residualReachable cap flow s
    
    isViolator : Fin n -> Bool
    isViolator i = 
      let
        k = finToNat i
        vNodeF = fromMaybe FZ (natToFin (1 + m + k) (totalNodes n m))
      in index vNodeF seen
      
    viols = filter isViolator (toList (finRange n))
  in map (\i => cast (finToNat i)) viols

public export
covering
solve : (pb : Problem) -> Int -> Solution
solve (MkProblem n bounds m eu ev) mode =
  let
    cap = buildCapacity (MkProblem n bounds m eu ev)
    s : Fin (totalNodes n m) = FZ
    t : Fin (totalNodes n m) = last

    (flow, maxf) = case mode of
      0 => edmondsKarp cap s t
      1 => dinic cap s t
      _ => pushRelabel cap s t

    dirs = orientEdges (MkProblem n bounds m eu ev) flow
    ok = maxf == cast m
    dus = toList (map fst dirs)
    dvs = toList (map snd dirs)
    viols = if ok then [] else violators (MkProblem n bounds m eu ev) cap flow
  in MkSolution ok dus dvs viols
