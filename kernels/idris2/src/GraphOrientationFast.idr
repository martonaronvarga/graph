module GraphOrientationFast

import Data.IOArray.Prims
import Data.Vect
import Data.List

%default covering

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

export
record IntArray where
  constructor MkIntArray
  array : ArrayData Int

export
newIntArray : Int -> Int -> IO IntArray
newIntArray s val = do
  array <- primIO $ prim__newArray (cast s) val
  pure $ MkIntArray array

export
%inline
readIntArray : IntArray -> Int -> IO Int
readIntArray (MkIntArray arr) i =
  primIO $ prim__arrayGet arr (cast i)

export
%inline
writeIntArray : IntArray -> Int -> Int -> IO ()
writeIntArray (MkIntArray arr) i val =
  primIO $ prim__arraySet arr (cast i) val

fillIntArray : IntArray -> Int -> Int -> IO ()
fillIntArray arr sz val = do
  let go : Int -> IO ()
      go i = if i >= sz then pure () else do
        writeIntArray arr i val
        go (i + 1)
  go 0

-- Forward Star representation
export
record Graph1D where
  constructor MkGraph1D
  numNodes : Int
  firstOut : IntArray
  nextOut : IntArray
  edgeTo : IntArray
  edgeCap : IntArray
  edgeFlow : IntArray
  edgeRev : IntArray
  edgeCount : IntArray

addEdge : Graph1D -> Int -> Int -> Int -> IO ()
addEdge g u v cap = do
  e <- readIntArray g.edgeCount 0
  
  fstU <- readIntArray g.firstOut u
  writeIntArray g.edgeTo e v
  writeIntArray g.edgeCap e cap
  writeIntArray g.edgeFlow e 0
  writeIntArray g.nextOut e fstU
  writeIntArray g.firstOut u e
  
  let revE = e + 1
  
  fstV <- readIntArray g.firstOut v
  writeIntArray g.edgeTo revE u
  writeIntArray g.edgeCap revE 0
  writeIntArray g.edgeFlow revE 0
  writeIntArray g.nextOut revE fstV
  writeIntArray g.firstOut v revE
  
  writeIntArray g.edgeRev e revE
  writeIntArray g.edgeRev revE e
  
  writeIntArray g.edgeCount 0 (e + 2)

export
scanNeighborsTop : IntArray -> IntArray -> IntArray -> IntArray -> IntArray -> IntArray -> IntArray -> Int -> Int -> Int -> Int -> IO Int
scanNeighborsTop edgeTo edgeCap edgeFlow nextOut visited pred queue pass t e qTail = do
  if e == -1 then pure qTail
    else do
    v <- readIntArray edgeTo e
    vis <- readIntArray visited v
    if vis == pass then do
      nxt <- readIntArray nextOut e
      scanNeighborsTop edgeTo edgeCap edgeFlow nextOut visited pred queue pass t nxt qTail
      else do
      c <- readIntArray edgeCap e
      f <- readIntArray edgeFlow e
      if c - f > 0 then do
        writeIntArray visited v pass
        writeIntArray pred v e
        writeIntArray queue qTail v
        if v == t then pure (qTail + 1)
          else do
          nxt <- readIntArray nextOut e
          scanNeighborsTop edgeTo edgeCap edgeFlow nextOut visited pred queue pass t nxt (qTail + 1)
        else do
        nxt <- readIntArray nextOut e
        scanNeighborsTop edgeTo edgeCap edgeFlow nextOut visited pred queue pass t nxt qTail

export
bfsLoopTop : Graph1D -> IntArray -> IntArray -> IntArray -> Int -> Int -> Int -> Int -> IO Bool
bfsLoopTop g visited pred queue pass t head tail = do
  if head >= tail then pure False
    else do
    u <- readIntArray queue head
    fstU <- readIntArray g.firstOut u
    -- passing unboxed arrays directly to the top-level loop avoids closure captures
    newTail <- scanNeighborsTop g.edgeTo g.edgeCap g.edgeFlow g.nextOut visited pred queue pass t fstU tail
    visT <- readIntArray visited t
    if visT == pass then pure True
      else bfsLoopTop g visited pred queue pass t (head + 1) newTail

export
bfsEK : Graph1D -> Int -> Int -> IntArray -> IntArray -> IntArray -> Int -> IO Bool 
bfsEK g s t pred queue visited pass = do
  writeIntArray visited s pass
  writeIntArray queue 0 s
  bfsLoopTop g visited pred queue pass t 0 1

export
findBottleneckTop : Graph1D -> IntArray -> Int -> Int -> Int -> IO Int
findBottleneckTop g pred s curr minF = do
  if curr == s then pure minF
    else do
    e <- readIntArray pred curr
    c <- readIntArray g.edgeCap e
    f <- readIntArray g.edgeFlow e
    revE <- readIntArray g.edgeRev e
    p <- readIntArray g.edgeTo revE
    findBottleneckTop g pred s p (min minF (c - f))

export
augmentFlowTop : Graph1D -> IntArray -> Int -> Int -> Int -> IO ()
augmentFlowTop g pred s curr delta = do
  if curr == s then pure ()
    else do
    e <- readIntArray pred curr
    fwdF <- readIntArray g.edgeFlow e
    writeIntArray g.edgeFlow e (fwdF + delta)
    revE <- readIntArray g.edgeRev e
    revF <- readIntArray g.edgeFlow revE
    writeIntArray g.edgeFlow revE (revF - delta)
    p <- readIntArray g.edgeTo revE
    augmentFlowTop g pred s p delta

export
edmondsKarpLoop : Graph1D -> IntArray -> IntArray -> IntArray -> Int -> Int -> Int -> Int -> IO Int
edmondsKarpLoop g pred queue visited s t tot pass = do
  found <- bfsEK g s t pred queue visited pass
  if found then do
    delta <- findBottleneckTop g pred s t 999999999
    augmentFlowTop g pred s t delta
    edmondsKarpLoop g pred queue visited s t (tot + delta) (pass + 1)
    else pure tot

export
edmondsKarp : Graph1D -> Int -> Int -> IO Int
edmondsKarp g s t = do
  let sz = g.numNodes
  pred <- newIntArray sz (-1)
  queue <- newIntArray sz 0
  visited <- newIntArray sz 0
  edmondsKarpLoop g pred queue visited s t 0 1

bfsDinic : Graph1D  -> Int -> Int -> IntArray -> IntArray -> IO Bool
bfsDinic g s t level queue = do
  fillIntArray level g.numNodes (-1)
  writeIntArray level s 0
  writeIntArray queue 0 s

  let loop : Int -> Int -> IO Bool
      loop head tail = do
        if head >= tail then do
          lt <- readIntArray level t
          pure (lt /= -1)
          else do
          u <- readIntArray queue head
          lu <- readIntArray level u
          let scanNeighbors : Int -> Int -> IO Int
              scanNeighbors e qTail = do
                if e == -1 then pure qTail
                  else do
                  v <- readIntArray g.edgeTo e
                  lv <- readIntArray level v
                  if lv == -1 then do
                    c <- readIntArray g.edgeCap e
                    f <- readIntArray g.edgeFlow e
                    if c - f > 0 then do
                      writeIntArray level v (lu + 1)
                      writeIntArray queue qTail v
                      nxt <- readIntArray g.nextOut e
                      scanNeighbors nxt (qTail + 1)
                      else do
                      nxt <- readIntArray g.nextOut e
                      scanNeighbors nxt qTail
                    else do
                    nxt <- readIntArray g.nextOut e
                    scanNeighbors nxt qTail
          fstU <- readIntArray g.firstOut u
          newTail <- scanNeighbors fstU tail
          loop (head + 1) newTail
  loop 0 1

dfsDinic : Graph1D -> IntArray -> IntArray -> Int -> Int -> Int -> IO Int
dfsDinic g level ptr u t amt = do
   if u == t then pure amt
    else do
    lu <- readIntArray level u
    let loop : IO Int
        loop = do
          e <- readIntArray ptr u
          if e == -1 then pure 0
            else do
            v <- readIntArray g.edgeTo e
            lv <- readIntArray level v
            if lv == lu + 1 then do
              c <- readIntArray g.edgeCap e
              f <- readIntArray g.edgeFlow e
              if c - f > 0 then do
                let pushAmt = min amt (c - f)
                pushed <- dfsDinic g level ptr v t pushAmt
                if pushed > 0 then do
                  writeIntArray g.edgeFlow e (f + pushed)
                  revE <- readIntArray g.edgeRev e
                  revF <- readIntArray g.edgeFlow revE
                  writeIntArray g.edgeFlow revE (revF - pushed)
                  pure pushed
                  else do
                  nxt <- readIntArray g.nextOut e
                  writeIntArray ptr u nxt
                  loop
                else do
                nxt <- readIntArray g.nextOut e
                writeIntArray ptr u nxt
                loop
              else do
              nxt <- readIntArray g.nextOut e
              writeIntArray ptr u nxt
              loop
    loop

dinic : Graph1D -> Int -> Int -> IO Int
dinic g s t = do
  let sz = g.numNodes
  level <- newIntArray sz (-1)
  ptr <- newIntArray sz 0
  queue <- newIntArray sz 0

  let go : Int -> IO Int
      go tot = do
        found <- bfsDinic g s t level queue
        if not found then pure tot
          else do
          let initPtr : Int -> IO ()
              initPtr i = if i >= sz then pure () else do
                fst <- readIntArray g.firstOut i
                writeIntArray ptr i fst
                initPtr (i + 1)
          initPtr 0
          
          let pushLoop : Int -> IO Int
              pushLoop acc = do
                pushed <- dfsDinic g level ptr s t 999999999
                if pushed == 0 then pure acc
                  else pushLoop (acc + pushed)
          added <- pushLoop 0
          go (tot + added)
          
  go 0
          

pushRelabel : Graph1D -> Int -> Int -> IO Int
pushRelabel g s t = do
  let sz = g.numNodes
  h <- newIntArray sz 0
  ex <- newIntArray sz 0
  queue <- newIntArray sz 0
  inQ <- newIntArray sz 0
  nextE <- newIntArray sz 0
  
  writeIntArray h s sz
  
  let initSrc : Int -> Int -> IO Int
      initSrc e qTail = if e == -1 then pure qTail else do
        v <- readIntArray g.edgeTo e
        c <- readIntArray g.edgeCap e
        if c > 0 then do
          writeIntArray g.edgeFlow e c
          revE <- readIntArray g.edgeRev e
          writeIntArray g.edgeFlow revE (-c)
          writeIntArray ex v c
          if v /= s && v /= t then do
            writeIntArray queue (qTail `mod` sz) v
            writeIntArray inQ v 1
            nxt <- readIntArray g.nextOut e
            initSrc nxt (qTail + 1)
            else do
            nxt <- readIntArray g.nextOut e
            initSrc nxt qTail
          else do
          nxt <- readIntArray g.nextOut e
          initSrc nxt qTail
        
  fstS <- readIntArray g.firstOut s
  tailStart <- initSrc fstS 0

  let initNext : Int -> IO ()
      initNext i = if i >= sz then pure () else do
        fst <- readIntArray g.firstOut i
        writeIntArray nextE i fst
        initNext (i + 1)
  initNext 0

  let enqueue : Int -> Int -> IO Int
      enqueue v tail = do
        if v /= s && v /= t then do
          inQueue <- readIntArray inQ v
          if inQueue == 0 then do
            writeIntArray queue (tail `mod` sz) v
            writeIntArray inQ v 1
            pure (tail + 1)
            else pure tail
          else pure tail

  let push : Int -> Int -> Int -> IO (Bool, Int)
      push u e tail = do
        ex_u <- readIntArray ex u
        c <- readIntArray g.edgeCap e
        f <- readIntArray g.edgeFlow e
        let resid = c - f
        if ex_u > 0 && resid > 0 then do
          v <- readIntArray g.edgeTo e
          hu <- readIntArray h u
          hv <- readIntArray h v
          if hu == hv + 1 then do
            let amt = min ex_u resid
            writeIntArray g.edgeFlow e (f + amt)
            revE <- readIntArray g.edgeRev e
            revF <- readIntArray g.edgeFlow revE
            writeIntArray g.edgeFlow revE (revF - amt)
            writeIntArray ex u (ex_u - amt)
            ex_v <- readIntArray ex v
            writeIntArray ex v (ex_v + amt)
            nTail <- enqueue v tail
            pure (True, nTail)
            else pure (False, tail)
          else pure (False, tail)

  let relabel : Int -> IO ()
      relabel u = do
        let findMinH : Int -> Int -> IO Int
            findMinH e minH = if e == -1 then pure minH else do
              c <- readIntArray g.edgeCap e
              f <- readIntArray g.edgeFlow e
              if c - f > 0 then do
                v <- readIntArray g.edgeTo e
                hv <- readIntArray h v
                nxt <- readIntArray g.nextOut e
                findMinH nxt (min minH hv)
                else do
                nxt <- readIntArray g.nextOut e
                findMinH nxt minH
        fstU <- readIntArray g.firstOut u
        mH <- findMinH fstU 999999999
        writeIntArray h u (mH + 1)

  let discharge : Int -> Int -> IO Int
      discharge u tail = do
        ex_u <- readIntArray ex u
        if ex_u == 0 then pure tail
          else do
          e <- readIntArray nextE u
          if e == -1 then do
            relabel u
            fstU <- readIntArray g.firstOut u
            writeIntArray nextE u fstU
            discharge u tail
            else do
            (pushed, nTail) <- push u e tail
            if not pushed then do
              nxt <- readIntArray g.nextOut e
              writeIntArray nextE u nxt
              discharge u nTail
              else discharge u nTail

  let runActive : Int -> Int -> IO ()
      runActive head tail = do
        if head >= tail then pure ()
          else do
          u <- readIntArray queue (head `mod` sz)
          writeIntArray inQ u 0
          newTail <- discharge u tail
          runActive (head + 1) newTail

  runActive 0 tailStart
  readIntArray ex t

totalNodes : Nat -> Nat -> Int
totalNodes n m = cast (S (S (m + n)))

buildGraph : Problem -> IO Graph1D
buildGraph (MkProblem n bounds m eu ev) = do
  let tN = totalNodes n m
  let maxE = (cast m * 3 + cast n) * 2 -- Total bidirectional edges
  
  firstOut <- newIntArray tN (-1)
  nextOut <- newIntArray maxE 0
  edgeTo <- newIntArray maxE 0
  edgeCap <- newIntArray maxE 0
  edgeFlow <- newIntArray maxE 0
  edgeRev <- newIntArray maxE 0
  edgeCount <- newIntArray 1 0
  
  let g = MkGraph1D tN firstOut nextOut edgeTo edgeCap edgeFlow edgeRev edgeCount
  let s = 0
  let t = tN - 1
  let mInt = cast m
  
  let buildEdges : Int -> List Int -> List Int -> IO ()
      buildEdges _ [] _ = pure ()
      buildEdges _ _ [] = pure ()
      buildEdges k (u :: us) (v :: vs) = do
        let eNode = 1 + k
        let uNode = 1 + mInt + u
        let vNode = 1 + mInt + v
        addEdge g s eNode 1
        addEdge g eNode uNode 1
        addEdge g eNode vNode 1
        buildEdges (k + 1) us vs
  buildEdges 0 (toList eu) (toList ev)

  let buildBounds : Int -> List Int -> IO ()
      buildBounds _ [] = pure ()
      buildBounds k (bound :: rest) = do
        let vNode = 1 + mInt + k
        addEdge g vNode t bound
        buildBounds (k + 1) rest
  buildBounds 0 (toList bounds)
  
  pure g

orientEdges : Problem -> Graph1D -> IO (List (Int, Int))
orientEdges (MkProblem n bounds m eu ev) g = do
  let mInt = cast m
  let go : Int -> List Int -> List Int -> List (Int, Int) -> IO (List (Int, Int))
      go _ [] _ acc = pure (reverse acc)
      go _ _ [] acc = pure (reverse acc)
      go k (u :: us) (v :: vs) acc = do
        let eNode = 1 + k
        let uNode = 1 + mInt + u
        
        let findEdge : Int -> IO Int
            findEdge e = if e == -1 then pure 0 else do
              toNode <- readIntArray g.edgeTo e
              if toNode == uNode then readIntArray g.edgeFlow e
                else do
                nxt <- readIntArray g.nextOut e
                findEdge nxt
                
        fstENode <- readIntArray g.firstOut eNode
        fVal <- findEdge fstENode
        if fVal > 0 
          then go (k + 1) us vs ((u, v) :: acc)
            else go (k + 1) us vs ((v, u) :: acc)
  go 0 (toList eu) (toList ev) []

residualReachable : Graph1D -> Int -> IO IntArray
residualReachable g s = do
  let sz = g.numNodes
  seen <- newIntArray sz 0
  queue <- newIntArray sz 0
  writeIntArray seen s 1
  writeIntArray queue 0 s

  let loop : Int -> Int -> IO ()
      loop head tail = do
        if head >= tail then pure ()
          else do
          u <- readIntArray queue head
          let scanNeighbors : Int -> Int -> IO Int
              scanNeighbors e qTail = do
                if e == -1 then pure qTail
                  else do
                  v <- readIntArray g.edgeTo e
                  isSeen <- readIntArray seen v
                  if isSeen == 0 then do
                    c <- readIntArray g.edgeCap e
                    f <- readIntArray g.edgeFlow e
                    if c - f > 0 then do
                      writeIntArray seen v 1
                      writeIntArray queue qTail v
                      nxt <- readIntArray g.nextOut e
                      scanNeighbors nxt (qTail + 1)
                      else do
                      nxt <- readIntArray g.nextOut e
                      scanNeighbors nxt qTail
                    else do
                    nxt <- readIntArray g.nextOut e
                    scanNeighbors nxt qTail
          fstU <- readIntArray g.firstOut u
          newTail <- scanNeighbors fstU tail
          loop (head + 1) newTail
  loop 0 1
  pure seen

violators : Problem -> Graph1D -> IO (List Int)
violators (MkProblem n bounds m eu ev) g = do
  let s = 0
  let mInt = cast m
  seen <- residualReachable g s
  
  let go : Int -> List Int -> IO (List Int)
      go k acc = if k >= cast n then pure (reverse acc) else do
        let vNode = 1 + mInt + k
        isSeen <- readIntArray seen vNode
        if isSeen == 1 
          then go (k + 1) (k :: acc)
          else go (k + 1) acc
  go 0 []

solveIO : Problem -> Int -> IO Solution
solveIO pb mode = do
  let tN = totalNodes pb.n pb.m
  g <- buildGraph pb
  let s = 0
  let t = tN - 1
  
  maxf <- case mode of
    0 => edmondsKarp g s t
    1 => dinic g s t
    _ => pushRelabel g s t
  
  dirs <- orientEdges pb g
  let ok = maxf == cast pb.m
  let dus = map fst dirs
  let dvs = map snd dirs
  
  viols <- if ok then pure [] else violators pb g
  pure (MkSolution ok dus dvs viols)

public export
solve : (pb : Problem) -> Int -> Solution
solve pb mode = unsafePerformIO (solveIO pb mode)
