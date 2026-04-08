-- Futhark kernel

def source (): i32 = 0i32
def edge_node (k: i32): i32 = 1i32 + k
def vertex_node (m: i32) (v: i32): i32 = 1i32 + m + v
def sink (n: i32) (m: i32): i32 = 1i32 + m + n

-- forward star storage
def build_graph [n] [m] (bounds: [n]i64) (eu: [m]i32) (ev: [m]i32):
  (i32, i32, *[]i32, *[]i32, *[]i32, *[]i64, *[]i64, *[]i32) =
  let n32 = i32.i64 n
  let m32 = i32.i64 m
  let V = 2i32 + m32 + n32
  let E = (m32 * 3i32 + n32) * 2i32
  
  let first_out = replicate (i64.i32 V) (-1i32)
  let next_out = replicate (i64.i32 E) 0i32
  let edge_to = replicate (i64.i32 E) 0i32
  let edge_cap = replicate (i64.i32 E) 0i64
  let edge_flow = replicate (i64.i32 E) 0i64
  let edge_rev = replicate (i64.i32 E) 0i32
  
  let s = source()
  let t = sink n32 m32
  
  let (first_out1, next_out1, edge_to1, edge_cap1, edge_flow1, edge_rev1, e_idx) =
    loop (fo, no, et, ec, ef, er, e) = (first_out, next_out, edge_to, edge_cap, edge_flow, edge_rev, 0i32)
    for k in 0i32..<m32 do
      let u = eu[i64.i32 k]
      let v = ev[i64.i32 k]
      let eNode = edge_node k
      let uNode = vertex_node m32 u
      let vNode = vertex_node m32 v
      
      let add_edge (foi: *[]i32) (noi: *[]i32) (eti: *[]i32) (eci: *[]i64) (efi: *[]i64) (eri: *[]i32) (ei: i32) (from: i32) (to: i32) (cap: i64) =
        let fstU = foi[i64.i32 from]
        let eti[i64.i32 ei] = to
        let eci[i64.i32 ei] = cap
        let efi[i64.i32 ei] = 0i64
        let noi[i64.i32 ei] = fstU
        let foi[i64.i32 from] = ei
        let revE = ei + 1i32
        let fstV = foi[i64.i32 to]
        let eti[i64.i32 revE] = from
        let eci[i64.i32 revE] = 0i64
        let efi[i64.i32 revE] = 0i64
        let noi[i64.i32 revE] = fstV
        let foi[i64.i32 to] = revE
        let eri[i64.i32 ei] = revE
        let eri[i64.i32 revE] = ei
        in (foi, noi, eti, eci, efi, eri, ei + 2i32)

      let (fo1, no1, et1, ec1, ef1, er1, e1) = add_edge fo no et ec ef er e s eNode 1i64
      let (fo2, no2, et2, ec2, ef2, er2, e2) = add_edge fo1 no1 et1 ec1 ef1 er1 e1 eNode uNode 1i64
      let (fo3, no3, et3, ec3, ef3, er3, e3) = add_edge fo2 no2 et2 ec2 ef2 er2 e2 eNode vNode 1i64
      in (fo3, no3, et3, ec3, ef3, er3, e3)

  let (first_out2, next_out2, edge_to2, edge_cap2, edge_flow2, edge_rev2, _) =
    loop (fo, no, et, ec, ef, er, e) = (first_out1, next_out1, edge_to1, edge_cap1, edge_flow1, edge_rev1, e_idx)
    for k in 0i32..<n32 do
      let vNode = vertex_node m32 k
      let bound = bounds[i64.i32 k]
      let from = vNode
      let to = t
      let ei = e
      let fstU = fo[i64.i32 from]
      let et[i64.i32 ei] = to
      let ec[i64.i32 ei] = bound
      let ef[i64.i32 ei] = 0i64
      let no[i64.i32 ei] = fstU
      let fo[i64.i32 from] = ei
      let revE = ei + 1i32
      let fstV = fo[i64.i32 to]
      let et[i64.i32 revE] = from
      let ec[i64.i32 revE] = 0i64
      let ef[i64.i32 revE] = 0i64
      let no[i64.i32 revE] = fstV
      let fo[i64.i32 to] = revE
      let er[i64.i32 ei] = revE
      let er[i64.i32 revE] = ei
      in (fo, no, et, ec, ef, er, e + 2i32)

  in (V, E, first_out2, next_out2, edge_to2, edge_cap2, edge_flow2, edge_rev2)

def bfs [nn](cap: [nn][nn]i64) (flow: [nn][nn]i64) (s: i32) (t: i32): (*[nn]i32, bool) =
  let pred0 = replicate nn (-1i32) with [i64.i32 s] = s
  let queue0 = replicate nn 0i32 with [0] = s

  let (_, _, predf, foundf, _) =
    loop (qstart, qend, pred:*[nn]i32, found, queue: *[nn]i32) = (0i32, 1i32, pred0, false, queue0)
      while qstart < qend && not found
    do
      let u = queue[i64.i32 qstart]
      let u64 = i64.i32 u
      let cap_u = cap[u64]
      let flow_u = flow[u64]
      let qstart' = qstart + 1i32

      let (pred', queue', qend', found', _) =
        loop (p: *[nn]i32, q: *[nn]i32, qe, f, i64v) = (pred, queue, qend, found, 0i64)
          while i64v < nn && not f
          do
            if p[i64v] == -1i32 && cap_u[i64v] > flow_u[i64v] then
              let p' = p with [i64v] = u
              let q' = q with [i64.i32 qe] = i32.i64 i64v
              let is_t = (i32.i64 i64v == t)
              in (p', q', qe + 1i32, f || is_t, i64v + 1i64)
            else (p, q, qe, f, i64v + 1i64)
      in (qstart', qend', pred', found', queue')
  in (predf, foundf)

def edmonds_karp (V: i32) (E: i32) (first_out: []i32) (next_out: []i32) (edge_to: []i32) (edge_cap: []i64) (edge_flow: *[]i64) (edge_rev: []i32) (s: i32) (t: i32): (i64, *[]i64) =
  let pred0 = replicate (i64.i32 V) (-1i32)
  let queue0 = replicate (i64.i32 V) 0i32
  
  let (_, flowf, totf) = 
    loop (done, flow: *[]i64, tot) = (false, edge_flow, 0i64)
    while not done do
      let (_,_,pred,found,_) = 
        loop (qstart, qend, p: *[]i32, f, q: *[]i32) = (0i32, 1i32, copy pred0, false, copy queue0 with [0] = s)
        while qstart < qend && not f do
          let u = q[i64.i32 qstart]
          let e_init = first_out[i64.i32 u]
          
          let (p', q', qend', f', _) = 
            loop (pr: *[]i32, qr: *[]i32, qer, fr, e) = (p, q, qend, f, e_init)
            while e != -1i32 && not fr do
              let v = edge_to[i64.i32 e]
              in if pr[i64.i32 v] == -1i32 && v != s && edge_cap[i64.i32 e] - flow[i64.i32 e] > 0i64 then
                   let pr' = pr with [i64.i32 v] = e
                   let qr' = qr with [i64.i32 qer] = v
                   in (pr', qr', qer + 1i32, v == t, next_out[i64.i32 e])
                 else (pr, qr, qer, fr, next_out[i64.i32 e])
          in (qstart + 1i32, qend', p', f', q')

      in if not found then (true, flow, tot)
      else
        let (minF, _) = 
          loop (m, curr) = (9223372036854775807i64, t)
          while curr != s do
            let e = pred[i64.i32 curr]
            let m' = i64.min m (edge_cap[i64.i32 e] - flow[i64.i32 e])
            let rE = edge_rev[i64.i32 e]
            let prev = edge_to[i64.i32 rE]
            in (m', prev)
        
        let flow' =
          let (_, final_fl) = 
            loop (curr, fll: *[]i64) = (t, flow) 
            while curr != s do
              let e = pred[i64.i32 curr]
              let rE = edge_rev[i64.i32 e]
              
              let fll' = fll with [i64.i32 e] = fll[i64.i32 e] + minF
              let fll'' = fll' with [i64.i32 rE] = fll'[i64.i32 rE] - minF
              
              let prev = edge_to[i64.i32 rE]
              in (prev, fll'')
          in final_fl
          
        in (false, flow', tot + minF)
  in (totf, flowf)

def dinic_bfs_level (V: i32) (first_out: []i32) (next_out: []i32) (edge_to: []i32) (edge_cap: []i64) (flow: []i64) (s: i32) (t: i32): *[]i32 =
  let level0 = replicate (i64.i32 V) (-1i32) with [i64.i32 s] = 0i32
  let queue0 = replicate (i64.i32 V) 0i32 with [0] = s

  let (_, _, levelf, _) =
    loop (qstart, qend, level: *[]i32, queue: *[]i32) = (0i32, 1i32, level0, queue0)
    while qstart < qend do
      let u = queue[i64.i32 qstart]
      let (level', queue', qend', _) =
        loop (lvl: *[]i32, q: *[]i32, qe, e) = (level, queue, qend, first_out[i64.i32 u])
        while e != -1i32 do
          let v = edge_to[i64.i32 e]
          in if lvl[i64.i32 v] == -1i32 && edge_cap[i64.i32 e] - flow[i64.i32 e] > 0i64 then
               let lvl' = lvl with [i64.i32 v] = lvl[i64.i32 u] + 1i32
               let q' = q with [i64.i32 qe] = v
               in (lvl', q', qe + 1i32, next_out[i64.i32 e])
             else (lvl, q, qe, next_out[i64.i32 e])
      in (qstart + 1i32, qend', level', queue')
  in levelf

def dinic_find_path (V: i32) (next_out: []i32) (edge_to: []i32) (edge_cap: []i64) (flow: []i64) (level: []i32) (s: i32) (t: i32) (ptr: *[]i32) (stack_v: *[]i32) (stack_e: *[]i32): (bool, i64, *[]i32, *[]i32, *[]i32) =
  let stack_v' = stack_v with [0] = s
  
  let (found, sv, se, topf, ptrf) =
    loop (found, sv_i: *[]i32, se_i: *[]i32, top, p: *[]i32) = (false, stack_v', stack_e, 1i32, ptr)
    while not found && top > 0i32 do
      let u = sv_i[i64.i32 (top - 1i32)]
      in if u == t then
           (true, sv_i, se_i, top, p)
         else
           let e = p[i64.i32 u]
           in if e == -1i32 then
                -- u is fully exhausted. Pop it.
                let top' = top - 1i32
                let p' = if top' > 0i32 then
                           let parent_u = sv_i[i64.i32 (top' - 1i32)]
                           let parent_e = p[i64.i32 parent_u]
                           in p with [i64.i32 parent_u] = next_out[i64.i32 parent_e]
                         else p
                in (false, sv_i, se_i, top', p')
              else
                -- Evaluate active edge
                let v = edge_to[i64.i32 e]
                in if level[i64.i32 v] == level[i64.i32 u] + 1i32 && edge_cap[i64.i32 e] - flow[i64.i32 e] > 0i64 then
                     let sv_j = sv_i with [i64.i32 top] = v
                     let se_j = se_i with [i64.i32 top] = e
                     in (false, sv_j, se_j, top + 1i32, p)
                   else
                     let p' = p with [i64.i32 u] = next_out[i64.i32 e]
                     in (false, sv_i, se_i, top, p')

  let path_len = if found then i64.i32 (topf - 1i32) else 0i64
  in (found, path_len, ptrf, sv, se)

def dinic (V: i32) (E: i32) (first_out: []i32) (next_out: []i32) (edge_to: []i32) (edge_cap: []i64) (edge_flow: *[]i64) (edge_rev: []i32) (s: i32) (t: i32): (i64, *[]i64) =
  let stack_v_init = replicate (i64.i32 V) 0i32
  let stack_e_init = replicate (i64.i32 V) (-1i32)
  
  let (_, flowf, totf, _, _) =
    loop (done_outer, flow: *[]i64, tot, sv: *[]i32, se: *[]i32) = (false, edge_flow, 0i64, stack_v_init, stack_e_init)
    while not done_outer do
      let level = dinic_bfs_level V first_out next_out edge_to edge_cap flow s t
      in if level[i64.i32 t] < 0i32 then
           (true, flow, tot, sv, se)
         else
           let ptr0 = copy first_out
           let (_, flow', dsum, _, sv_next, se_next) =
             loop (done_inner, flowi: *[]i64, ds, ptr: *[]i32, sv_i: *[]i32, se_i: *[]i32) = (false, flow, 0i64, ptr0, sv, se)
             while not done_inner do
               let (found_path, path_len, ptr', sv_j, se_j) =
                 dinic_find_path V next_out edge_to edge_cap flowi level s t ptr sv_i se_i
               in if not found_path then
                    (true, flowi, ds, ptr', sv_j, se_j)
                  else
                    let minF = 
                      loop m = 9223372036854775807i64 for i in 0i64..<path_len do
                        let e = se_j[i + 1i64]
                        in i64.min m (edge_cap[i64.i32 e] - flowi[i64.i32 e])
                        
                    let flowj = 
                      loop fl = flowi for i in 0i64..<path_len do
                        let e = se_j[i + 1i64]
                        let rE = edge_rev[i64.i32 e]
                        let fl' = fl with [i64.i32 e] = fl[i64.i32 e] + minF
                        in fl' with [i64.i32 rE] = fl'[i64.i32 rE] - minF
                        
                    in (false, flowj, ds + minF, ptr', sv_j, se_j)
           in if dsum == 0i64 then (true, flow', tot, sv_next, se_next)
              else (false, flow', tot + dsum, sv_next, se_next)
  in (totf, flowf)

def add_to_array [n] (arr: *[n]i64) (indices: []i64) (values: []i64): *[n]i64 =
  reduce_by_index arr (+) 0i64 indices values

def global_relabel (V: i32) (first_out: []i32) (next_out: []i32) (edge_to: []i32) (edge_cap: []i64) (fl: []i64) (s: i32) (t: i32) (ex: []i64): (*[]i32, *[]i32) =
  let h0 = replicate (i64.i32 V) V with [i64.i32 t] = 0i32
  
  let (_, h_final, _) =
    loop (front_len: i32, h: *[]i32, d: i32) = (1i32, h0, 0i32)
    while front_len > 0i32 do
      let next_front_bool = map (\u64 ->
        let u = i32.i64 u64
        in if u != s && h[u64] == V then
             let (found, _) = 
               loop (f, e) = (false, first_out[u64])
               while e != -1i32 && not f do
                 let v = edge_to[i64.i32 e]
                 let resid = edge_cap[i64.i32 e] - fl[i64.i32 e]
                 in if resid > 0i64 && h[i64.i32 v] == d then
                      (true, -1i32)
                    else (false, next_out[i64.i32 e])
             in found
           else false
      ) (iota (i64.i32 V))
      
      let next_front = map i32.i64 (filter (\i -> next_front_bool[i]) (iota (i64.i32 V)))
      let h_next = scatter h (map i64.i32 next_front) (map (\_ -> d + 1i32) next_front)
      in (i32.i64 (length next_front), h_next, d + 1i32)
      
  let active = map i32.i64 (filter (\u64 -> 
    let u = i32.i64 u64
    in u != s && u != t && h_final[u64] < V && ex[u64] > 0i64
  ) (iota (i64.i32 V)))
  
  in (h_final, active)

def parallel_global_push_relabel (V: i32) (E: i32) 
                          (first_out: []i32) (next_out: []i32) 
                          (edge_to: []i32) (edge_cap: []i64) 
                          (edge_flow: *[]i64) (edge_rev: []i32) 
                          (s: i32) (t: i32): (i64, *[]i64) =
  let ex0 = replicate (i64.i32 V) 0i64

  let (fl1, ex1, _) =
    let e_s = first_out[i64.i32 s]
    in loop (fl, ex, e) = (edge_flow, ex0, e_s)
       while e != -1i32 do
         let v = edge_to[i64.i32 e]
         let c = edge_cap[i64.i32 e]
         in if c > 0i64 then
              let fl' = fl with [i64.i32 e] = c
              let rE = edge_rev[i64.i32 e]
              let fl'' = fl' with [i64.i32 rE] = -c
              let ex' = ex with [i64.i32 v] = c
              in (fl'', ex', next_out[i64.i32 e])
            else 
              (fl, ex, next_out[i64.i32 e])

  let (h_init, active_init) = global_relabel V first_out next_out edge_to edge_cap fl1 s t ex1
  
  let track_arr0 = replicate (i64.i32 V) 999999999i64
  let PGR_THRESH = V / 2i32  -- heuristic: relabel every time we process V/2 nodes
  
  let (_, flf, exf, _, _, _) =
    loop (active: []i32, fl: *[]i64, ex: *[]i64, h: *[]i32, track_arr: *[]i64, work_done: i32) = 
         (active_init, fl1, ex1, h_init, track_arr0, 0i32)
    while length active > 0 do
      
      if work_done >= PGR_THRESH then
        let (h_pgr, act_pgr) = global_relabel V first_out next_out edge_to edge_cap fl s t ex
        in (act_pgr, fl, ex, h_pgr, track_arr, 0i32)
      else
        
        let intents = map (\u ->
          let (found, target_e, target_v, amt, min_h, _) =
            loop (f, te, tv, a, mh, e) = (false, -1i32, -1i32, 0i64, 2147483647i32, first_out[i64.i32 u])
            while e != -1i32 && not f do
              let v = edge_to[i64.i32 e]
              let resid = edge_cap[i64.i32 e] - fl[i64.i32 e]
              in if resid > 0i64 then
                   if h[i64.i32 u] == h[i64.i32 v] + 1i32 then
                     (true, e, v, i64.min ex[i64.i32 u] resid, mh, -1i32)
                   else
                     (false, -1i32, -1i32, 0i64, i32.min mh h[i64.i32 v], next_out[i64.i32 e])
                 else (false, -1i32, -1i32, 0i64, mh, next_out[i64.i32 e])
          
          in if found then
               (target_e, edge_rev[i64.i32 target_e], target_v, u, amt, -1i32, -1i32)
             else
               let new_h = if min_h == 2147483647i32 then V else min_h + 1i32
               in (-1i32, -1i32, -1i32, -1i32, 0i64, u, new_h)
        ) active

        let p_e     = map (.0) intents
        let p_rev_e = map (.1) intents
        let p_v     = map (.2) intents
        let p_u     = map (.3) intents
        let p_amt   = map (.4) intents
        let r_u     = map (.5) intents
        let r_h     = map (.6) intents
        let neg_amt = map (\a -> -a) p_amt

        let comb_e   = p_e ++ p_rev_e
        let comb_amt = p_amt ++ neg_amt
        let comb_val = map2 (\e amt -> if e != -1i32 then fl[i64.i32 e] + amt else 0i64) comb_e comb_amt
        let fl_next  = scatter fl (map i64.i32 comb_e) comb_val

        let comb_v   = p_u ++ p_v
        let comb_ex_amt = neg_amt ++ p_amt
        let ex_next  = reduce_by_index ex (+) 0i64 (map i64.i32 comb_v) comb_ex_amt

        let h_next   = scatter h (map i64.i32 r_u) r_h

        let candidates = active ++ p_v
        let c_idx = indices candidates
        
        let track_arr_1 = reduce_by_index track_arr i64.min 999999999i64 (map i64.i32 candidates) c_idx
        
        let mask = map (\i -> 
          let c = candidates[i]
          in if c != -1i32 then track_arr_1[i64.i32 c] == i else false
        ) c_idx
        
        let unique_valid_c = map (.0) (filter (\(u, m) -> m) (zip candidates mask))
        
        let next_active = filter (\u -> u != s && u != t && ex_next[i64.i32 u] > 0i64) unique_valid_c
        
        let track_arr_next = scatter track_arr_1 (map i64.i32 unique_valid_c) (map (\_ -> 999999999i64) unique_valid_c)

        in (next_active, fl_next, ex_next, h_next, track_arr_next, work_done + i32.i64 (length active))

  in (exf[i64.i32 t], flf)

def parallel_push_relabel (V: i32) (E: i32) 
                          (first_out: []i32) (next_out: []i32) 
                          (edge_to: []i32) (edge_cap: []i64) 
                          (edge_flow: *[]i64) (edge_rev: []i32) 
                          (s: i32) (t: i32): (i64, *[]i64) =
  let h0 = replicate (i64.i32 V) 0i32 with [i64.i32 s] = V
  let ex0 = replicate (i64.i32 V) 0i64

  let (fl1, ex1, _) =
    let e_s = first_out[i64.i32 s]
    in loop (fl, ex, e) = (edge_flow, ex0, e_s)
       while e != -1i32 do
         let v = edge_to[i64.i32 e]
         let c = edge_cap[i64.i32 e]
         in if c > 0i64 then
              let fl' = fl with [i64.i32 e] = c
              let rE = edge_rev[i64.i32 e]
              let fl'' = fl' with [i64.i32 rE] = -c
              let ex' = ex with [i64.i32 v] = c
              in (fl'', ex', next_out[i64.i32 e])
            else 
              (fl, ex, next_out[i64.i32 e])

  let active0 = filter (\u -> u != s && u != t && ex1[i64.i32 u] > 0i64) (map i32.i64 (iota (i64.i32 V)))
  let track_arr0 = replicate (i64.i32 V) 999999999i64

  let (_, flf, exf, _, _) =
    loop (active: []i32, fl: *[]i64, ex: *[]i64, h: *[]i32, track_arr: *[]i64) = 
         (active0, fl1, ex1, h0, track_arr0)
    while length active > 0 do
      
      let intents = map (\u ->
        let (found, target_e, target_v, amt, min_h, _) =
          loop (f, te, tv, a, mh, e) = (false, -1i32, -1i32, 0i64, 2147483647i32, first_out[i64.i32 u])
          while e != -1i32 && not f do
            let v = edge_to[i64.i32 e]
            let resid = edge_cap[i64.i32 e] - fl[i64.i32 e]
            in if resid > 0i64 then
                 if h[i64.i32 u] == h[i64.i32 v] + 1i32 then
                   (true, e, v, i64.min ex[i64.i32 u] resid, mh, -1i32)
                 else
                   (false, -1i32, -1i32, 0i64, i32.min mh h[i64.i32 v], next_out[i64.i32 e])
               else (false, -1i32, -1i32, 0i64, mh, next_out[i64.i32 e])
        
        in if found then
            (target_e, edge_rev[i64.i32 target_e], target_v, u, amt, -1i32, -1i32)
           else
             let new_h = if min_h == 2147483647i32 then h[i64.i32 u] else min_h + 1i32
             in (-1i32, -1i32, -1i32, -1i32, 0i64, u, new_h)
      ) active

      let p_e     = map (.0) intents
      let p_rev_e = map (.1) intents
      let p_v     = map (.2) intents
      let p_u     = map (.3) intents
      let p_amt   = map (.4) intents
      let r_u     = map (.5) intents
      let r_h     = map (.6) intents
      
      let neg_amt = map (\a -> -a) p_amt

      let comb_e   = p_e ++ p_rev_e
      let comb_amt = p_amt ++ neg_amt
      let comb_val = map2 (\e amt -> if e != -1i32 then fl[i64.i32 e] + amt else 0i64) comb_e comb_amt
      let fl_next  = scatter fl (map i64.i32 comb_e) comb_val

      let comb_v   = p_u ++ p_v
      let comb_ex_amt = neg_amt ++ p_amt
      let ex_next  = reduce_by_index ex (+) 0i64 (map i64.i32 comb_v) comb_ex_amt

      let h_next   = scatter h (map i64.i32 r_u) r_h

      let candidates = active ++ p_v
      let c_idx = indices candidates
      
      let track_arr_1 = reduce_by_index track_arr i64.min 999999999i64 (map i64.i32 candidates) c_idx
      
      let mask = map (\i -> 
        let c = candidates[i]
        in if c != -1i32 then track_arr_1[i64.i32 c] == i else false
      ) c_idx
      
      let unique_valid_c = map (.0) (filter (\(u, m) -> m) (zip candidates mask))
      
      let next_active = filter (\u -> u != s && u != t && ex_next[i64.i32 u] > 0i64) unique_valid_c
      
      let track_arr_next = scatter track_arr_1 (map i64.i32 unique_valid_c) (map (\_ -> 999999999i64) unique_valid_c)

      in (next_active, fl_next, ex_next, h_next, track_arr_next)

  in (exf[i64.i32 t], flf)

def push_relabel (V: i32) (E: i32) (first_out: []i32) (next_out: []i32) (edge_to: []i32) (edge_cap: []i64) (edge_flow: *[]i64) (edge_rev: []i32) (s: i32) (t: i32): (i64, *[]i64) =
  let h0 = replicate (i64.i32 V) 0i32 with [i64.i32 s] = V
  let ex0 = replicate (i64.i32 V) 0i64
  let q0 = replicate (i64.i32 V) 0i32
  let inq0 = replicate (i64.i32 V) false

  let (fl1, ex1, q1, inq1, tail1, _) =
    let e_s = first_out[i64.i32 s]
    in loop (fl, ex, q, inq, tail, e) = (edge_flow, ex0, q0, inq0, 0i32, e_s)
       while e != -1i32 do
         let v = edge_to[i64.i32 e]
         let c = edge_cap[i64.i32 e]
         in if c > 0i64 then
              let fl' = fl with [i64.i32 e] = c
              let rE = edge_rev[i64.i32 e]
              let fl'' = fl' with [i64.i32 rE] = -c
              let ex' = ex with [i64.i32 v] = c
              let (q', inq', tail') = 
                if v != s && v != t then
                  let qi = q with [i64.i32 (tail %% V)] = v
                  let inqi = inq with [i64.i32 v] = true
                  in (qi, inqi, tail + 1i32)
                else (q, inq, tail)
              in (fl'', ex', q', inq', tail', next_out[i64.i32 e])
            else (fl, ex, q, inq, tail, next_out[i64.i32 e])

  let (_, _, flf, exf, _, _, _) =
    loop (head, tail, fl: *[]i64, ex: *[]i64, h: *[]i32, q: *[]i32, inq: *[]bool) = 
         (0i32, tail1, fl1, ex1, h0, q1, inq1)
    while head < tail do
      let u = q[i64.i32 (head %% V)]
      let inq' = inq with [i64.i32 u] = false
      
      let (fl2, ex2, h2, q2, inq2, tail2) =
        loop (flc: *[]i64, exc: *[]i64, hc: *[]i32, qc: *[]i32, inqc: *[]bool, tc) = 
             (fl, ex, h, q, inq', tail)
        while exc[i64.i32 u] > 0i64 do
          let e_u = first_out[i64.i32 u]
          let (fln, exn, qn, inqn, tn, pushed_any, _) =
            loop (fl_i: *[]i64, ex_i: *[]i64, q_i: *[]i32, inq_i: *[]bool, t_i, pa, e) = 
                 (flc, exc, qc, inqc, tc, false, e_u)
            while e != -1i32 do
              let v = edge_to[i64.i32 e]
              let resid = edge_cap[i64.i32 e] - fl_i[i64.i32 e]
              in if resid > 0i64 && hc[i64.i32 u] == hc[i64.i32 v] + 1i32 && ex_i[i64.i32 u] > 0i64 then
                   let amt = i64.min ex_i[i64.i32 u] resid
                   let fl_i' = fl_i with [i64.i32 e] = fl_i[i64.i32 e] + amt
                   let rE = edge_rev[i64.i32 e]
                   let fl_i'' = fl_i' with [i64.i32 rE] = fl_i'[i64.i32 rE] - amt
                   let ex_i' = ex_i with [i64.i32 u] = ex_i[i64.i32 u] - amt
                   let ex_i'' = ex_i' with [i64.i32 v] = ex_i'[i64.i32 v] + amt
                   let (q_i', inq_i', t_i') = 
                     if v != s && v != t && not inq_i[i64.i32 v] then
                       let nq = q_i with [i64.i32 (t_i %% V)] = v
                       let ninq = inq_i with [i64.i32 v] = true
                       in (nq, ninq, t_i + 1i32)
                     else (q_i, inq_i, t_i)
                   in (fl_i'', ex_i'', q_i', inq_i', t_i', true, next_out[i64.i32 e])
                 else (fl_i, ex_i, q_i, inq_i, t_i, pa, next_out[i64.i32 e])
          in if pushed_any then (fln, exn, hc, qn, inqn, tn)
             else 
               let minH = 
                 loop (m, e) = (2147483647i32, first_out[i64.i32 u])
                 while e != -1i32 do
                   let v = edge_to[i64.i32 e]
                   let r = edge_cap[i64.i32 e] - fln[i64.i32 e]
                   let m' = if r > 0i64 then i32.min m hc[i64.i32 v] else m
                   in (m', next_out[i64.i32 e])
               let newH = if minH.0 == 2147483647i32 then hc[i64.i32 u] else minH.0 + 1i32
               let hc' = hc with [i64.i32 u] = newH
               in (fln, exn, hc', qn, inqn, tn)

      in (head + 1i32, tail2, fl2, ex2, h2, q2, inq2)

  in (exf[i64.i32 t], flf)

entry graph_orientation_solve
  (mode: i32)
  (n: i32)
  (m: i32)
  (bounds: []i64)
  (eu: []i32)
  (ev: []i32)
  : (bool, []i64, []i64, []i64) =

  let (V, E, first_out, next_out, edge_to, edge_cap, edge_flow, edge_rev) = build_graph bounds eu ev
  let s = source()
  let t = sink n m
  let vo = 1i32 + m

  let (maxflow, flow_mat) =
    if mode == 0i32 then edmonds_karp V E first_out next_out edge_to edge_cap edge_flow edge_rev s t
    else if mode == 1i32 then dinic V E first_out next_out edge_to edge_cap edge_flow edge_rev s t
    else parallel_global_push_relabel V E first_out next_out edge_to edge_cap edge_flow edge_rev s t

  let ok = maxflow == i64.i32 m
 
let directed =
    map (\(k64: i64) ->
      let k = i32.i64 k64
      let u = eu[k]
      let v = ev[k]
      let ek = edge_node k
      let uNode = vertex_node m u
      
      let flow_val = 
        loop (res, e) = (0i64, first_out[i64.i32 ek])
        while e != -1i32 && res == 0i64 do
          if edge_to[i64.i32 e] == uNode then (flow_mat[i64.i32 e], -1i32)
          else (0i64, next_out[i64.i32 e])
          
      in if flow_val.0 > 0i64
         then [i64.i32 v, i64.i32 u]
         else [i64.i32 u, i64.i32 v]
    ) (iota (i64.i32 m))

  let directed_u = map (\(e: [2]i64) -> e[0]) directed
  let directed_v = map (\(e: [2]i64) -> e[1]) directed

let violators =
    if ok then []
    else
      let seen = replicate (i64.i32 V) false with [i64.i32 s] = true
      let queue = replicate (i64.i32 V) 0i32 with [0] = s
      
      let (_, _, seenf, _) = 
        loop (qstart, qend, sn: *[]bool, q: *[]i32) = (0i32, 1i32, seen, queue)
        while qstart < qend do
          let u = q[i64.i32 qstart]
          let (sn', q', qend', _) =
            loop (sni, qi, qei, e) = (sn, q, qend, first_out[i64.i32 u])
            while e != -1i32 do
              let nv = edge_to[i64.i32 e]
              in if not sni[i64.i32 nv] && edge_cap[i64.i32 e] - flow_mat[i64.i32 e] > 0i64 then
                   let sni' = sni with [i64.i32 nv] = true
                   let qi' = qi with [i64.i32 qei] = nv
                   in (sni', qi', qei + 1i32, next_out[i64.i32 e])
                 else (sni, qi, qei, next_out[i64.i32 e])
          in (qstart + 1i32, qend', sn', q')
          
      in filter (\(i64v: i64) -> seenf[i64.i32 (vo + i32.i64 i64v)]) (iota (i64.i32 n))

  in (ok, directed_u, directed_v, violators)
