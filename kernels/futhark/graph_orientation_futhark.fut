-- Futhark kernel
-- source -> edge-node -> endpoint-vertex -> sink
-- feasible iff maxflow == m
-- if feasible: decode orientation from chosen endpoint per edge-node
-- else: return violating set from residual reachability on vertex nodes.

def node_count (n: i32) (m: i32): i32 = 2i32 + m + n
def source (): i32 = 0i32
def edge_node (k: i32): i32 = 1i32 + k
def vertex_offset (m: i32): i32 = 1i32 + m
def vertex_node (m: i32) (v: i32): i32 = vertex_offset m + v
def sink (n: i32) (m: i32): i32 = 1i32 + m + n

def build_edges (eu: []i32) (ev: []i32): [][2]i32 =
  map2 (\(u: i32) (v: i32) -> [u, v]) eu ev

def empty_mat (n: i32): [][]i64 =
  replicate (i64.i32 n) (replicate (i64.i32 n) 0i64)

def add_arc (mat: [][]i64) (u: i32) (v: i32) (c: i64): [][]i64 =
  copy mat with [u, v] = (mat[u][v] + c)

def build_capacity (n: i32) (m: i32) (bounds: []i64) (eu: []i32) (ev: []i32): [][]i64 =
  let nn = node_count n m
  let s = source()
  let t = sink n m
  let vo = vertex_offset m
  let cap0 = empty_mat nn

  let cap1 =
    loop cap = cap0 for k64 in 0..<i64.i32 m do
      let k = i32.i64 k64
      let ek = edge_node k
      let u = eu[k]
      let v = ev[k]
      let c1 = add_arc cap s ek 1i64
      let c2 = add_arc c1 ek (vo + u) 1i64
      let c3 = add_arc c2 ek (vo + v) 1i64
      in c3

  let cap2 =
    loop cap = cap1 for i64v in 0..<i64.i32 n do
      let i = i32.i64 i64v
      in add_arc cap (vo + i) t bounds[i]

  in cap2

def bfs (cap: [][]i64) (flow: [][]i64) (s: i32) (t: i32): ([]i32, bool) =
  let n64 = length cap
  let n = i32.i64 n64
  let pred0 = (replicate n64 (-1i32)) with [s] = s
  let queue0 = (replicate n64 0i32) with [0] = s

  let (_, _, predf, foundf, _) =
    loop (qstart, qend, pred, found, queue) = (0i32, 1i32, pred0, false, queue0)
      while qstart < qend && not found
    do
      let u = queue[qstart]
      let qstart' = qstart + 1
      let (pred', queue', qend', found') =
        loop (p, q, qe, f) = (pred, queue, qend, found)
          for v in 0..<n do
            if p[v] == -1i32 && cap[u][v] - flow[u][v] > 0i64 then
              let p' = copy p with [v] = u
              let q' = copy q with [qe] = v
              let qe' = qe + 1
              let f' = f || (v == t)
              in (p', q', qe', f')
            else (p, q, qe, f)
      in (qstart', qend', pred', found', queue')
  in (predf, foundf)

def augment_path (pred: []i32) (s: i32) (t: i32): []i32 =
  let (x, acc) =
    loop (v, acc) = (t, []:[]i32)
      while v != s && v != -1i32
    do
      let p = pred[v]
      in (p, [v] ++ acc)
  in if x == s then [s] ++ acc else []

def min_residual (cap: [][]i64) (flow: [][]i64) (path: []i32): i64 =
  let len = length path
  in if len < 2 then 0i64 else
       let k = len - 1
       let us = take k path
       let vs = take k (drop 1 path)
       in reduce i64.min 9223372036854775807i64
            (map2 (\(u: i32) (v: i32) -> cap[u][v] - flow[u][v]) us vs)

def add_flow (flow: [][]i64) (path: []i32) (delta: i64): [][]i64 =
  let len = length path
  in if len < 2 || delta == 0i64 then flow else
       let k = len - 1
       let us = take k path
       let vs = take k (drop 1 path)
       in loop f = flow for i64j in 0..<k do
            let j = i32.i64 i64j
            let u = us[j]
            let v = vs[j]
            let f1 = copy f with [u, v] = (f[u][v] + delta)
            let f2 = copy f1 with [v, u] = (f1[v][u] - delta)
            in f2

def edmonds_karp (cap: [][]i64) (s: i32) (t: i32): (i64, [][]i64) =
  let n64 = length cap
  let f0 = replicate n64 (replicate n64 0i64)
  let (_, ff, tot) =
    loop (done, flow, total) = (false, f0, 0i64)
      while not done
    do
      let (pred, found) = bfs cap flow s t
      in if not found || pred[t] == -1i32 then
           (true, flow, total)
         else
           let path = augment_path pred s t
           let d = min_residual cap flow path
           in if length path < 2 || d <= 0i64 then
                (true, flow, total)
              else
                (false, add_flow flow path d, total + d)
  in (tot, ff)

def dinic_bfs_level (cap: [][]i64) (flow: [][]i64) (s: i32) (t: i32): []i32 =
  let n64 = length cap
  let n = i32.i64 n64
  let level0 = (replicate n64 (-1i32)) with [s] = 0i32
  let queue0 = (replicate n64 0i32) with [0] = s

  let (_, _, levelf, _) =
    loop (qstart, qend, level, queue) = (0i32, 1i32, level0, queue0)
      while qstart < qend
    do
      let u = queue[qstart]
      let qstart' = qstart + 1
      let (level', queue', qend') =
        loop (lvl, q, qe) = (level, queue, qend)
          for v in 0..<n do
            if lvl[v] == -1i32 && cap[u][v] - flow[u][v] > 0i64 then
              let lvl' = copy lvl with [v] = (lvl[u] + 1i32)
              let q' = copy q with [qe] = v
              let qe' = qe + 1
              in (lvl', q', qe')
            else (lvl, q, qe)
      in (qstart', qend', level', queue')
  in levelf

let dinic_blocking_flow (cap: [][]i64) (flow: [][]i64) (level: []i32) (s: i32) (t: i32): ([]i32, i64) =
  let n64 = length cap
  let n = i32.i64 n64
  let parent0 = (replicate n64 (-1i32)) with [s] = s
  let stack0 = (replicate n64 0i32) with [0] = s

  let (found, parentf, _, topf) =
    loop (found, parent, stack, top) = (false, parent0, stack0, 1i32)
      while not found && top > 0
    do
      let u = stack[top - 1]
      in if u == t then
           (true, parent, stack, top)
         else
           let (v_found, ok) =
             loop (v, ok) = (0i32, false)
               while v < n && not ok
             do
               if level[v] == level[u] + 1i32 &&
                  cap[u][v] - flow[u][v] > 0i64 &&
                  parent[v] == -1i32
               then (v, true)
               else (v + 1, false)
           in if ok then
                let parent' = copy parent with [v_found] = u
                let stack' = copy stack with [top] = v_found
                in (false, parent', stack', top + 1)
              else
                (false, parent, stack, top - 1)

  in if not found || topf <= 0i32 then
       ([], 0i64)
     else
       let rev =
         loop (node, acc) = (t, []:[]i32)
           while node != s && node != -1i32
         do
           let p = parentf[node]
           in (p, [node] ++ acc)
       let path = if rev.0 == s then [s] ++ rev.1 else []
       let len = length path
       let bottleneck =
         if len < 2 then 0i64 else
           let (_, b) =
             loop (i, b) = (1i64, 9223372036854775807i64)
               while i < len
             do
               let u = path[i - 1]
               let v = path[i]
               let rc = cap[u][v] - flow[u][v]
               let b' = if rc < b then rc else b
               in (i + 1, b')
           in b
       in (path, bottleneck)

def dinic (cap: [][]i64) (s: i32) (t: i32): (i64, [][]i64) =
  let n64 = length cap
  let flow0 = replicate n64 (replicate n64 0i64)

  let (_, flowf, totf) =
    loop (done_outer, flow, tot) = (false, flow0, 0i64)
      while not done_outer
    do
      let level = dinic_bfs_level cap flow s t
      in if level[t] < 0i32 then
           (true, flow, tot)
         else
           let (_, flow', dsum) =
             loop (done_inner, flowi, ds) = (false, flow, 0i64)
               while not done_inner
             do
               let (path, delta) = dinic_blocking_flow cap flowi level s t
               in if delta <= 0i64 || length path < 2 then
                    (true, flowi, ds)
                  else
                    let flowj = add_flow flowi path delta
                    in (false, flowj, ds + delta)
           in if dsum == 0i64 then (true, flow', tot)
              else (false, flow', tot + dsum)

  in (totf, flowf)

def push (fl: [][]i64) (exc: []i64) (u: i32) (v: i32) (cap_: [][]i64) (h_: []i32): ([][]i64, []i64) =
  let amt = i64.min exc[u] (cap_[u][v] - fl[u][v])
  in if amt > 0i64 && h_[u] == h_[v] + 1i32 then
       let fl1 = copy fl with [u, v] = (fl[u][v] + amt)
       let fl2 = copy fl1 with [v, u] = (fl1[v, u] - amt)
       let ex1 = copy exc with [u] = (exc[u] - amt)
       let ex2 = copy ex1 with [v] = (ex1[v] + amt)
       in (fl2, ex2)
     else (fl, exc)

def relabel (u: i32) (cap_: [][]i64) (fl: [][]i64) (h_: []i32): i32 =
  let n64 = length cap_
  let cand: []i32 =
    map i32.i64 (filter (\(v64: i64) -> cap_[u][i32.i64 v64] - fl[u][i32.i64 v64] > 0i64) (iota n64))
  in if length cand == 0 then h_[u]
     else 1i32 + reduce i32.min 2147483647i32 (map (\(v: i32) -> h_[v]) cand)

def discharge (u: i32) (cap: [][]i64) (fl0: [][]i64) (ex0: []i64) (h0: []i32): ([][]i64, []i64, []i32) =
  let n = i32.i64 (length cap)
  let (_, flf, exf, hf) =
    loop (done, fl, ex, h) = (false, fl0, ex0, h0)
      while not done
    do
      if ex[u] == 0i64 then (true, fl, ex, h) else
        let (_, pushed, fl1, ex1) =
          loop (v, pushed, fli, exi) = (0i32, false, fl, ex)
            while v < n && not pushed
          do
            if cap[u][v] - fli[u][v] > 0i64 && h[u] == h[v] + 1i32 then
              let (fln, exn) = push fli exi u v cap h
              in if fln != fli then (v + 1, true, fln, exn)
                 else (v + 1, false, fli, exi)
            else (v + 1, false, fli, exi)
        in if pushed then (false, fl1, ex1, h)
           else
             let hu = relabel u cap fl1 h
             let h1 = copy h with [u] = hu
             in (false, fl1, ex1, h1)
  in (flf, exf, hf)

def push_relabel (cap: [][]i64) (s: i32) (t: i32): (i64, [][]i64) =
  let n64 = length cap
  let h0 = (replicate n64 0i32) with [s] = (i32.i64 n64)
  let fl0 = replicate n64 (replicate n64 0i64)
  let ex0 = replicate n64 0i64

  let srcs: []i32 =
    map i32.i64 (filter (\(v64: i64) -> cap[s][i32.i64 v64] > 0i64) (iota n64))

  let ex1 =
    foldl (\ex v ->
      let exa = copy ex with [v] = (ex[v] + cap[s][v])
      let exb = copy exa with [s] = (exa[s] - cap[s][v])
      in exb
    ) ex0 srcs

  let fl1 =
    foldl (\f v ->
      let f1 = copy f with [s, v] = cap[s][v]
      let f2 = copy f1 with [v, s] = (-cap[s][v])
      in f2
    ) fl0 srcs

  let active0: []i32 =
    map i32.i64 (filter (\(v64: i64) ->
      let v = i32.i64 v64
      in v != s && v != t && ex1[v] > 0i64
    ) (iota n64))

  let (_, flf, exf, _) =
    loop (q, fl, ex, h) = (active0, fl1, ex1, h0)
      while length q > 0
    do
      let u = head q
      let rest = tail q
      let (fl2, ex2, h2) = discharge u cap fl ex h
      let new_active: []i32 =
        map i32.i64 (filter (\(v64: i64) ->
          let v = i32.i64 v64
          in v != s && v != t && ex[v] == 0i64 && ex2[v] > 0i64
        ) (iota n64))
      let q' = if ex2[u] > 0i64 then rest ++ new_active ++ [u]
               else rest ++ new_active
      in (q', fl2, ex2, h2)

  in (exf[t], flf)

  def residual_reachable (cap: [][]i64) (flow: [][]i64) (s: i32): []bool =
  let n64 = length cap
  let n = i32.i64 n64
  let seen0 = (replicate n64 false) with [s] = true
  let queue0 = (replicate n64 0i32) with [0] = s

  let (_, _, seenf, _) =
    loop (qstart, qend, seen, queue) = (0i32, 1i32, seen0, queue0)
      while qstart < qend
    do
      let u = queue[qstart]
      let qstart' = qstart + 1
      let (seen', queue', qend') =
        loop (sn, q, qe) = (seen, queue, qend)
          for v in 0..<n do
            if not sn[v] && cap[u][v] - flow[u][v] > 0i64 then
              let sn' = copy sn with [v] = true
              let q' = copy q with [qe] = v
              in (sn', q', qe + 1)
            else (sn, q, qe)
      in (qstart', qend', seen', queue')
  in seenf

entry graph_orientation_solve
  (mode: i32)
  (n: i32)
  (m: i32)
  (bounds: []i64)
  (eu: []i32)
  (ev: []i32)
  : (bool, []i64, []i64, []i64) =
  let s = source()
  let t = sink n m
  let s = source()
  let t = sink n m
  let vo = vertex_offset m
  let cap = build_capacity n m bounds eu ev
   let (maxflow, flow_mat) =
    if mode == 0i32 then edmonds_karp cap s t
    else if mode == 1i32 then dinic cap s t
    else push_relabel cap s t

  let ok = maxflow == i64.i32 m
 
let directed =
    map (\(k64: i64) ->
      let k = i32.i64 k64
      let u = eu[k]
      let v = ev[k]
      let ek = edge_node k
      let vu = vertex_node m u
      let chosen_u = flow_mat[ek][vu] > 0i64
      in if chosen_u
         then [i64.i32 v, i64.i32 u]  -- head=u => orient v->u
         else [i64.i32 u, i64.i32 v]  -- head=v => orient u->v
    ) (iota (i64.i32 m))

  let directed_u = map (\(e: [2]i64) -> e[0]) directed
  let directed_v = map (\(e: [2]i64) -> e[1]) directed

let violators =
  if ok then []
  else
    let seen = residual_reachable cap flow_mat s
    in filter (\(i64v: i64) ->
         let i = i32.i64 i64v
         in seen[vo + i]
       ) (iota (i64.i32 n))

  in (ok, directed_u, directed_v, violators)
