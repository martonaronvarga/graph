module GraphOrientation

import Data.Vect
import Data.List
import Data.Nat
import Data.Fin
import Data.Maybe
import Control.Monad.State

%default total

public export
record Problem where
  constructor MkProblem
  n : Nat
  bounds : Vect n Int
  m : Nat
  eu : Vect m Int
  ev : Vect m Int

-- Utility: make empty adjacency/flow matrix
mkMatrix : (n : Nat) -> Int -> Vect n (Vect n Int)
mkMatrix n v = replicate n (replicate n v)

-- Utility: add all edges (capacity 1)
buildCapacity : Problem -> Vect n (Vect n Int)
buildCapacity pb =
  let edges = zip pb.eu pb.ev
   in foldl (\mat, (u, v) => replaceAt u (replaceAt v ((index (index mat u) v) + 1) (index mat u)) mat) (mkMatrix pb.n 0) edges

-- ----------- Edmonds-Karp (BFS) --------------

-- Simple BFS for finding augmenting paths
bfs : Vect n (Vect n Int) -> Vect n (Vect n Int) -> Fin n -> Fin n -> Maybe (List (Fin n))
bfs cap flow s t =
  let
    n = length cap
    pred : Vect n (Maybe (Fin n))
    pred = replicate n Nothing

    bfs' : List (Fin n) -> Vect n (Maybe (Fin n)) -> Maybe (List (Fin n))
    bfs' []      pred = Nothing
    bfs' (u::us) pred =
      let vs = toList (zip (indices cap) (index cap u))
      in
        let newPairs = filter (\(v', _) => isNothing (index pred v') && index cap u !! v' - index flow u !! v' > 0)
                              vs
        in
          case find (\(v', _) => v' == t) newPairs of
            Just (found, _) =>
              -- reconstruct path
              let
                recPath : Fin n -> List (Fin n) -> List (Fin n)
                recPath v acc =
                  case index pred v of
                    Nothing => reverse (v :: acc)
                    Just pv => recPath pv (v :: acc)
              in
                Just (recPath found [])
            Nothing =>
              let pred' = foldl (\pr, (v',_) => replaceAt v' (Just u) pr) pred newPairs
                  q'    = us ++ map fst newPairs
              in bfs' q' pred'
  in bfs' [s] (replaceAt s (Just s) pred)

findBottleneck : Vect n (Vect n Int) -> Vect n (Vect n Int) -> List (Fin n) -> Int
findBottleneck cap flow (a :: b :: xs) =
  let res = (index cap a !! b) - (index flow a !! b)
  in min res (findBottleneck cap flow (b::xs))
findBottleneck _ _ _ = maxBound

augmentFlow : Vect n (Vect n Int) -> List (Fin n) -> Int -> Vect n (Vect n Int)
augmentFlow flow (a :: b :: xs) delta =
  let rowA = replaceAt b (index (index flow a) b + delta) (index flow a)
      flow' = replaceAt a rowA flow
      rowB = replaceAt a (index (index flow' b) a - delta) (index flow' b)
      flow'' = replaceAt b rowB flow'
  in augmentFlow flow'' (b::xs) delta
augmentFlow flow _ _ = flow

EdmondsKarp : Problem -> (Vect n (Vect n Int), Int)
EdmondsKarp pb =
  let cap = buildCapacity pb
      flow0 = mkMatrix pb.n 0
      s = FZ
      t = last (indices (S pb.n))
      go : Vect n (Vect n Int) -> Int -> (Vect n (Vect n Int), Int)
      go flow tot =
        case bfs cap flow s t of
          Nothing => (flow, tot)
          Just path =>
            let delta = findBottleneck cap flow path
                flow' = augmentFlow flow path delta
            in go flow' (tot + delta)
  in go flow0 0

-- ---------- Dinic's Algorithm (Level BFS + blocking flow) ------------

levelBFS : Vect n (Vect n Int) -> Vect n (Vect n Int) -> Fin n -> Vect n (Maybe Nat)
levelBFS cap flow s =
  let n = length cap
      level = replicate n Nothing
      go : List (Fin n) -> Vect n (Maybe Nat) -> Vect n (Maybe Nat)
      go [] level = level
      go (u::us) level =
        let currlvl = case index level u of
                        Just l => l
                        Nothing => 0
            vs = toList (zip (indices cap) (index cap u))
            newVs = filter (\(v,_) => isNothing (index level v) && index cap u !! v - index flow u !! v > 0) vs
            level' = foldl (\l', (v,_) => replaceAt v (Just (currlvl + 1)) l') level newVs
            us' = us ++ map fst newVs
        in go us' level'
  in go [s] (replaceAt s (Just 0) level)

dinicDFS : Vect n (Vect n Int) -> Vect n (Vect n Int) -> Vect n (Maybe Nat) ->
             Fin n -> Fin n -> Int -> (Vect n (Vect n Int), Int)
dinicDFS cap flow level u t amt =
  if u == t then (flow, amt)
  else
    let n = length cap
        vs = indices cap
        go : List (Fin n) -> Vect n (Vect n Int) -> Int -> (Vect n (Vect n Int), Int)
        go [] flow amt = (flow, 0)
        go (v::vs) flow amt =
          case (index level v, index level u) of
            (Just lv, Just lu) =>
              if lv == lu + 1 && index cap u !! v - index flow u !! v > 0 then
                let delta = min amt (index cap u !! v - index flow u !! v)
                    (flow', pushed) = dinicDFS cap flow level v t delta
                in if pushed > 0 then
                     let rowU = replaceAt v (index (index flow' u) v + pushed) (index flow' u)
                         flow'' = replaceAt u rowU flow'
                         rowV = replaceAt u (index (index flow'' v) u - pushed) (index flow'' v)
                         flow''' = replaceAt v rowV flow''
                     in (flow''', pushed)
                   else go vs flow amt
              else go vs flow amt
            _ => go vs flow amt
    in go (indices cap) flow amt

Dinic : Problem -> (Vect n (Vect n Int), Int)
Dinic pb =
  let cap = buildCapacity pb
      n = pb.n
      flow0 = mkMatrix n 0
      s = FZ
      t = last (indices (S n))
      go : Vect n (Vect n Int) -> Int -> (Vect n (Vect n Int), Int)
      go flow tot =
        let level = levelBFS cap flow s
        in case index level t of
            Nothing => (flow, tot)
            Just _ =>
              let
                block : Vect n (Vect n Int) -> Int -> (Vect n (Vect n Int), Int)
                block fl tot' =
                  let (fl', pushed) = dinicDFS cap fl level s t maxBound
                  in if pushed == 0 then (fl, tot')
                     else block fl' (tot' + pushed)
                (flow', tot') = block flow 0
              in go flow' (tot + tot')
  in go flow0 0

-- ------------ Push-Relabel ---------------------------

PushRelabel : Problem -> (Vect n (Vect n Int), Int)
PushRelabel pb =
  let cap = buildCapacity pb
      n = pb.n
      s = FZ
      t = last (indices (S n))
      h0 = replaceAt s n (replicate n 0)
      flow0 = mkMatrix n 0
      -- saturate source edges
      flow1 =
        foldl (\fl, v =>
                 let capSv = index cap s !! v
                 in if capSv > 0
                      then let rowS = replaceAt v capSv (index fl s)
                               fl' = replaceAt s rowS fl
                               rowV = replaceAt s (-capSv) (index fl' v)
                           in replaceAt v rowV fl'
                      else fl) flow0 (indices cap)
      excess0 =
        foldl (\ex, v =>
           let capSv = index cap s !! v
           in if capSv > 0
                then replaceAt v capSv ex
                else ex
           ) (replicate n 0) (indices cap)
      queue0 = filter (\v => v /= s && v /= t && index excess0 v > 0) (indices cap)
      step : Vect n (Vect n Int) -> Vect n Int -> Vect n Int -> Vect n (Vect n Int) -> List (Fin n) ->
               (Vect n (Vect n Int), Int)
      step cap h excess flow [] = (flow, index excess t)
      step cap h excess flow (u::qs) =
        let n = length cap
            vs = indices cap
            push (fl, ex, q) v =
              let resid = index cap u !! v - index fl u !! v
              in if resid > 0 && (index h u) == (index h v) + 1 && index ex u > 0 then
                  let amt = min (index ex u) resid
                      rowU = replaceAt v (index (index fl u) v + amt) (index fl u)
                      fl' = replaceAt u rowU fl
                      rowV = replaceAt u (index (index fl' v) u - amt) (index fl' v)
                      fl'' = replaceAt v rowV fl'
                      exU' = index ex u - amt
                      exV' = index ex v + amt
                      ex' = replaceAt u exU' (replaceAt v exV' ex)
                      q' = if v /= s && v /= t && index ex v == 0 && exV' > 0 then q ++ [v] else q
                  in (fl'', ex', q')
                 else (fl, ex, q)
            (flow', excess', q') = foldl (\s,v => push s v) (flow,excess,qs) vs
        in
            if index excess' u > 0 then
              -- relabel
              let validVs = filter (\v => index cap u !! v - index flow' u !! v > 0) vs
                  minH = minimum (map (\w => index h w) validVs)
                  h' = replaceAt u (1 + minH) h
              in step cap h' excess' flow' (qs++[u])
            else
              step cap h excess' flow' q'
  in step cap h0 excess0 flow1 queue0

-- ---------- Orientation & Reporting -----------

orientEdges : Problem -> Vect n (Vect n Int) -> Vect m Int
orientEdges prob flow =
  -- For each edge: direction == 0 if flow along (u,v) is positive, else 1
  let m = prob.m
  in vmap (\i =>
    let u = prob.eu ! i; v = prob.ev ! i
    in if index (index flow u) v > 0 then 0 else 1
    ) (indices m)

indegrees : Problem -> Vect m Int -> Vect n Int
indegrees pb orient =
  foldl (\acc, i =>
    let tgt = if orient ! i == 0 then pb.ev ! i else pb.eu ! i
    in replaceAt tgt ((index acc tgt) + 1) acc
  ) (replicate pb.n 0) (indices pb.m)

violators : Problem -> Vect m Int -> List Int
violators pb orient =
  let indeg = indegrees pb orient
      n = pb.n
  in filter (\i => index indeg i > index pb.bounds i) [0..(n-1)]

public export
solve : Problem -> Int -> (Bool, Vect m Int, List Int)
solve prob mode =
  let (flow, maxf) =
        case mode of
          0 => EdmondsKarp prob
          1 => Dinic prob
          _ => PushRelabel prob
      orient = orientEdges prob flow
      ok = maxf == prob.m
      viols = if ok then [] else violators prob orient
  in (ok, orient, viols)
