use std::collections::VecDeque;
use std::env;
use std::fs;
use std::io::{self};

pub const INPUT_PATH: &str = "input.txt";
pub const OUTPUT_PATH: &str = "output.txt";

#[cfg(feature = "futhark-backend")]
pub mod graph_orientation_futhark {
    include!(concat!(env!("OUT_DIR"), "/graph_orientation_futhark.rs"));
}

#[cfg(not(feature = "futhark-backend"))]
fn solve_via_futhark_bindgen(_: &Problem, _: RustAlgorithm) -> io::Result<Solution> {
    Err(io::Error::new(
        io::ErrorKind::Unsupported,
        "futhark-backend feature is disabled",
    ))
}

pub use RustAlgorithm as Algorithm;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum RustAlgorithm {
    AugmentingPath,
    Leveling,
    PushRelabel,
}

#[derive(Debug, Clone)]
pub struct Problem {
    pub n: usize,
    pub bounds: Vec<i64>,
    pub edges: Vec<(usize, usize)>,
}

#[derive(Debug, Clone)]
pub struct Solution {
    pub feasible: bool,
    pub directed_edges: Vec<(usize, usize)>,
    pub violators: Vec<usize>,
}

#[derive(Debug, Clone)]
pub struct Edge {
    pub to: usize,
    pub rev: usize,
    pub cap: i64,
}

#[derive(Debug, Clone)]
pub struct FlowGraph {
    pub adj: Vec<Vec<Edge>>,
}

impl FlowGraph {
    pub fn new(n: usize) -> Self {
        Self {
            adj: vec![Vec::new(); n],
        }
    }

    pub fn len(&self) -> usize {
        self.adj.len()
    }

    #[must_use]
    pub fn is_empty(&self) -> bool {
        self.adj.is_empty()
    }

    pub fn add_edge(&mut self, u: usize, v: usize, cap: i64) {
        let u_rev = self.adj[u].len();
        let v_rev = self.adj[v].len();
        self.adj[u].push(Edge {
            to: v,
            rev: v_rev,
            cap,
        });
        self.adj[v].push(Edge {
            to: u,
            rev: u_rev,
            cap: 0,
        });
    }

    pub fn reachable_from(&self, s: usize) -> Vec<bool> {
        let mut seen = vec![false; self.adj.len()];
        let mut q = VecDeque::new();
        seen[s] = true;
        q.push_back(s);
        while let Some(u) = q.pop_front() {
            for e in &self.adj[u] {
                if e.cap > 0 && !seen[e.to] {
                    seen[e.to] = true;
                    q.push_back(e.to);
                }
            }
        }
        seen
    }
}

pub fn parse_problem(text: &str) -> Result<Problem, String> {
    let mut it = text.split_whitespace();

    let n: usize = it
        .next()
        .ok_or_else(|| "missing vertex count".to_string())?
        .parse()
        .map_err(|_| "invalid vertex count".to_string())?;

    let mut bounds = Vec::with_capacity(n);
    for _ in 0..n {
        let g: i64 = it
            .next()
            .ok_or_else(|| "missing degree bound".to_string())?
            .parse()
            .map_err(|_| "invalid degree bound".to_string())?;
        if g < 0 {
            return Err("degree bounds must be nonnegative".to_string());
        }
        bounds.push(g);
    }

    let mut edges = Vec::new();
    while let Some(a) = it.next() {
        let Some(b) = it.next() else {
            return Err("dangling edge endpoint".to_string());
        };
        let u: usize = a.parse().map_err(|_| "invalid edge endpoint".to_string())?;
        let v: usize = b.parse().map_err(|_| "invalid edge endpoint".to_string())?;
        if u >= n || v >= n {
            return Err("edge endpoint out of range".to_string());
        }
        edges.push((u, v));
    }

    Ok(Problem { n, bounds, edges })
}

pub fn serialize_problem(problem: &Problem) -> String {
    let mut out = String::new();
    out.push_str(&problem.n.to_string());
    out.push('\n');
    for (i, g) in problem.bounds.iter().enumerate() {
        if i > 0 {
            out.push(' ');
        }
        out.push_str(&g.to_string());
    }
    out.push('\n');
    for &(u, v) in &problem.edges {
        out.push_str(&u.to_string());
        out.push(' ');
        out.push_str(&v.to_string());
        out.push('\n');
    }
    out
}

#[derive(Debug, Clone)]
struct BuiltNetwork {
    graph: FlowGraph,
    source: usize,
    sink: usize,
    vertex_offset: usize,
    edge_nodes: Vec<usize>,
}

fn build_network(problem: &Problem) -> BuiltNetwork {
    let m = problem.edges.len();
    let source = 0usize;
    let edge_offset = 1usize;
    let vertex_offset = 1usize + m;
    let sink = 1usize + m + problem.n;
    let mut graph = FlowGraph::new(sink + 1);
    let mut edge_nodes = Vec::with_capacity(m);

    for (k, &(u, v)) in problem.edges.iter().enumerate() {
        let e_node = edge_offset + k;
        edge_nodes.push(e_node);
        graph.add_edge(source, e_node, 1);
        graph.add_edge(e_node, vertex_offset + u, 1);
        graph.add_edge(e_node, vertex_offset + v, 1);
    }

    for (i, &bound) in problem.bounds.iter().enumerate() {
        graph.add_edge(vertex_offset + i, sink, bound);
    }

    BuiltNetwork {
        graph,
        source,
        sink,
        vertex_offset,
        edge_nodes,
    }
}

pub trait MaxFlow {
    fn max_flow(&mut self, s: usize, t: usize) -> i64;
    fn graph(&self) -> &FlowGraph;
}

pub struct EdmondsKarp {
    pub graph: FlowGraph,
}

impl EdmondsKarp {
    pub fn new(graph: FlowGraph) -> Self {
        Self { graph }
    }
}

impl MaxFlow for EdmondsKarp {
    fn max_flow(&mut self, s: usize, t: usize) -> i64 {
        let n = self.graph.len();
        let mut total = 0i64;
        let mut parent: Vec<Option<(usize, usize)>> = vec![None; n];

        loop {
            parent.fill(None);
            let mut q = VecDeque::new();
            parent[s] = Some((s, usize::MAX));
            q.push_back(s);

            while let Some(u) = q.pop_front() {
                for (idx, e) in self.graph.adj[u].iter().enumerate() {
                    if e.cap > 0 && parent[e.to].is_none() {
                        parent[e.to] = Some((u, idx));
                        if e.to == t {
                            break;
                        }
                        q.push_back(e.to);
                    }
                }
                if parent[t].is_some() {
                    break;
                }
            }

            if parent[t].is_none() {
                break;
            }

            let mut delta = i64::MAX;
            let mut v = t;
            while v != s {
                let (u, idx) = parent[v].expect("path should exist");
                delta = delta.min(self.graph.adj[u][idx].cap);
                v = u;
            }

            v = t;
            while v != s {
                let (u, idx) = parent[v].expect("path should exist");
                let rev = self.graph.adj[u][idx].rev;
                self.graph.adj[u][idx].cap -= delta;
                self.graph.adj[v][rev].cap += delta;
                v = u;
            }

            total += delta;
        }

        total
    }

    fn graph(&self) -> &FlowGraph {
        &self.graph
    }
}

pub struct Dinic {
    pub graph: FlowGraph,
    level: Vec<i32>,
    it: Vec<usize>,
}

impl Dinic {
    pub fn new(graph: FlowGraph) -> Self {
        let n = graph.len();
        Self {
            graph,
            level: vec![-1; n],
            it: vec![0; n],
        }
    }

    fn bfs(&mut self, s: usize, t: usize) -> bool {
        self.level.fill(-1);
        let mut q = VecDeque::new();
        self.level[s] = 0;
        q.push_back(s);
        while let Some(u) = q.pop_front() {
            for e in &self.graph.adj[u] {
                if e.cap > 0 && self.level[e.to] < 0 {
                    self.level[e.to] = self.level[u] + 1;
                    q.push_back(e.to);
                }
            }
        }
        self.level[t] >= 0
    }

    fn dfs(&mut self, v: usize, t: usize, pushed: i64) -> i64 {
        if v == t {
            return pushed;
        }
        while self.it[v] < self.graph.adj[v].len() {
            let i = self.it[v];
            let to = self.graph.adj[v][i].to;
            let cap = self.graph.adj[v][i].cap;
            if cap > 0 && self.level[v] + 1 == self.level[to] {
                let tr = self.dfs(to, t, pushed.min(cap));
                if tr > 0 {
                    let rev = self.graph.adj[v][i].rev;
                    self.graph.adj[v][i].cap -= tr;
                    self.graph.adj[to][rev].cap += tr;
                    return tr;
                }
            }
            self.it[v] += 1;
        }
        0
    }
}

impl MaxFlow for Dinic {
    fn max_flow(&mut self, s: usize, t: usize) -> i64 {
        let mut flow = 0i64;
        let inf = i64::MAX / 4;
        while self.bfs(s, t) {
            self.it.fill(0);
            loop {
                let pushed = self.dfs(s, t, inf);
                if pushed == 0 {
                    break;
                }
                flow += pushed;
            }
        }
        flow
    }

    fn graph(&self) -> &FlowGraph {
        &self.graph
    }
}

pub struct PushRelabel {
    pub graph: FlowGraph,
    height: Vec<usize>,
    excess: Vec<i64>,
    next: Vec<usize>,
    active: VecDeque<usize>,
    queued: Vec<bool>,
}

impl PushRelabel {
    pub fn new(graph: FlowGraph) -> Self {
        let n = graph.len();
        Self {
            graph,
            height: vec![0; n],
            excess: vec![0; n],
            next: vec![0; n],
            active: VecDeque::new(),
            queued: vec![false; n],
        }
    }

    fn enqueue(&mut self, v: usize, s: usize, t: usize) {
        if v != s && v != t && self.excess[v] > 0 && !self.queued[v] {
            self.queued[v] = true;
            self.active.push_back(v);
        }
    }

    fn push(&mut self, u: usize, idx: usize, s: usize, t: usize) -> bool {
        let v = self.graph.adj[u][idx].to;
        let cap = self.graph.adj[u][idx].cap;
        if cap == 0 || self.height[u] != self.height[v] + 1 {
            return false;
        }
        let amount = self.excess[u].min(cap);
        if amount == 0 {
            return false;
        }
        let rev = self.graph.adj[u][idx].rev;
        self.graph.adj[u][idx].cap -= amount;
        self.graph.adj[v][rev].cap += amount;
        self.excess[u] -= amount;
        self.excess[v] += amount;
        self.enqueue(v, s, t);
        true
    }

    fn relabel(&mut self, u: usize) {
        let mut min_height = usize::MAX;
        for e in &self.graph.adj[u] {
            if e.cap > 0 {
                min_height = min_height.min(self.height[e.to]);
            }
        }
        self.height[u] = if min_height == usize::MAX {
            usize::MAX / 4
        } else {
            min_height + 1
        };
    }

    fn discharge(&mut self, u: usize, s: usize, t: usize) {
        while self.excess[u] > 0 {
            if self.next[u] == self.graph.adj[u].len() {
                self.relabel(u);
                self.next[u] = 0;
                continue;
            }
            let idx = self.next[u];
            if !self.push(u, idx, s, t) {
                self.next[u] += 1;
            }
        }
    }
}

impl MaxFlow for PushRelabel {
    fn max_flow(&mut self, s: usize, t: usize) -> i64 {
        let n = self.graph.len();
        self.height[s] = n;

        for idx in 0..self.graph.adj[s].len() {
            let cap = self.graph.adj[s][idx].cap;
            if cap == 0 {
                continue;
            }
            let v = self.graph.adj[s][idx].to;
            let rev = self.graph.adj[s][idx].rev;
            self.graph.adj[s][idx].cap -= cap;
            self.graph.adj[v][rev].cap += cap;
            self.excess[v] += cap;
            self.enqueue(v, s, t);
        }

        while let Some(u) = self.active.pop_front() {
            self.queued[u] = false;
            self.discharge(u, s, t);
            self.enqueue(u, s, t);
        }

        self.excess[t]
    }

    fn graph(&self) -> &FlowGraph {
        &self.graph
    }
}

fn reconstruct_orientation(
    problem: &Problem,
    built: &BuiltNetwork,
    graph: &FlowGraph,
) -> Vec<(usize, usize)> {
    let mut oriented = Vec::with_capacity(problem.edges.len());

    for (k, &(u, v)) in problem.edges.iter().enumerate() {
        let e_node = built.edge_nodes[k];
        let mut head = None;
        for e in &graph.adj[e_node] {
            if e.to >= built.vertex_offset && e.to < built.vertex_offset + problem.n && e.cap == 0 {
                head = Some(e.to - built.vertex_offset);
                break;
            }
        }
        let head = head.expect("feasible max-flow must orient every edge");
        if head == u {
            oriented.push((v, u));
        } else {
            oriented.push((u, v));
        }
    }

    oriented
}

fn violating_vertices(problem: &Problem, built: &BuiltNetwork, graph: &FlowGraph) -> Vec<usize> {
    let seen = graph.reachable_from(built.source);
    let mut violators = Vec::new();
    for i in 0..problem.n {
        if seen[built.vertex_offset + i] {
            violators.push(i);
        }
    }
    violators
}

fn solve_rust(problem: &Problem, algorithm: RustAlgorithm) -> Solution {
    let built = build_network(problem);
    let m = problem.edges.len() as i64;

    let mut solver: Box<dyn MaxFlow> = match algorithm {
        RustAlgorithm::AugmentingPath => Box::new(EdmondsKarp::new(built.graph.clone())),
        RustAlgorithm::Leveling => Box::new(Dinic::new(built.graph.clone())),
        RustAlgorithm::PushRelabel => Box::new(PushRelabel::new(built.graph.clone())),
    };

    let flow = solver.max_flow(built.source, built.sink);
    let graph = solver.graph().clone();

    if flow == m {
        Solution {
            feasible: true,
            directed_edges: reconstruct_orientation(problem, &built, &graph),
            violators: Vec::new(),
        }
    } else {
        Solution {
            feasible: false,
            directed_edges: Vec::new(),
            violators: violating_vertices(problem, &built, &graph),
        }
    }
}

#[repr(C)]
pub struct RawProblem {
    pub n: usize,
    pub m: usize,
    pub bounds: *const i64,
    pub edges_u: *const usize,
    pub edges_v: *const usize,
}

#[repr(C)]
pub struct RawSolution {
    pub feasible: u8,
    pub directed_u: *mut usize,
    pub directed_v: *mut usize,
    pub directed_capacity: usize,
    pub directed_len: usize,
    pub violators: *mut usize,
    pub violators_capacity: usize,
    pub violators_len: usize,
}

impl RawProblem {
    fn from_problem(problem: &Problem) -> (Self, Vec<usize>, Vec<usize>) {
        let (mut eu, mut ev) = (
            Vec::with_capacity(problem.edges.len()),
            Vec::with_capacity(problem.edges.len()),
        );
        for &(u, v) in &problem.edges {
            eu.push(u);
            ev.push(v);
        }
        let raw = Self {
            n: problem.n,
            m: problem.edges.len(),
            bounds: problem.bounds.as_ptr(),
            edges_u: eu.as_ptr(),
            edges_v: ev.as_ptr(),
        };
        (raw, eu, ev)
    }
}

#[cfg(feature = "ffi-backend")]
fn solve_via_shared_library(
    backend: Engine,
    problem: &Problem,
    algorithm: RustAlgorithm,
) -> io::Result<Solution> {
    use std::path::PathBuf;

    use libloading::Library;

    // ABI expected from native backends (Idris2/K later):
    // int graph_orientation_solve(const go_raw_problem* p, int mode, go_raw_solution* out);
    type SolveFn = unsafe extern "C" fn(*const RawProblem, i32, *mut RawSolution) -> i32;

    let (lib_env, lib_default) = match backend {
        Engine::Idris2 => (
            "GRAPH_ORIENTATION_IDRIS2_LIB",
            "libgraph-orientation-idris2.so",
        ),
        Engine::K => ("GRAPH_ORIENTATION_K_LIB", "libgraph-orientation-k.so"),
        Engine::Futhark => (
            "GRAPH_ORIENTATION_FUTHARK_LIB",
            "libgraph-orientation-futhark.so",
        ),
        Engine::Zig => ("GRAPH_ORIENTATION_ZIG_LIB", "libgraph-orientation-zig.so"),
        Engine::Rust => panic!(
            "You are trying to use Rust as an FFI {:?}, maybe just use default runtime instead?",
            backend
        ),
    };

    let library_path: PathBuf = env::var_os(lib_env)
        .map(PathBuf::from)
        .unwrap_or_else(|| PathBuf::from(lib_default));

    let mode: i32 = match algorithm {
        RustAlgorithm::AugmentingPath => 0,
        RustAlgorithm::Leveling => 1,
        RustAlgorithm::PushRelabel => 2,
    };

    let (raw_problem, _eu, _ev) = RawProblem::from_problem(problem);

    let mut directed_u = vec![0usize; problem.edges.len()];
    let mut directed_v = vec![0usize; problem.edges.len()];
    let mut violators = vec![0usize; problem.n];

    let mut raw_solution = RawSolution {
        feasible: 0,
        directed_u: directed_u.as_mut_ptr(),
        directed_v: directed_v.as_mut_ptr(),
        directed_capacity: directed_u.len(),
        directed_len: 0,
        violators: violators.as_mut_ptr(),
        violators_capacity: violators.len(),
        violators_len: 0,
    };

    let _keep_alive = (_eu, _ev);

    unsafe {
        let lib = Library::new(&library_path).map_err(io::Error::other)?;
        let solve: libloading::Symbol<SolveFn> = lib
            .get(b"graph_orientation_solve\0")
            .map_err(io::Error::other)?;

        let rc = solve(
            &raw_problem as *const RawProblem,
            mode,
            &mut raw_solution as *mut RawSolution,
        );
        if rc != 0 {
            return Err(io::Error::other(format!(
                "backend returned error code {rc} ({})",
                library_path.display()
            )));
        }
    }

    if raw_solution.feasible != 0 {
        let len = raw_solution.directed_len.min(problem.edges.len());
        let directed_edges = directed_u.into_iter().zip(directed_v).take(len).collect();
        Ok(Solution {
            feasible: true,
            directed_edges,
            violators: Vec::new(),
        })
    } else {
        let len = raw_solution.violators_len.min(problem.n);
        let mut out = violators;
        out.truncate(len);
        out.sort_unstable();
        out.dedup();
        Ok(Solution {
            feasible: false,
            directed_edges: Vec::new(),
            violators: out,
        })
    }
}

#[cfg(feature = "futhark-backend")]
fn solve_via_futhark_bindgen(problem: &Problem, algorithm: RustAlgorithm) -> io::Result<Solution> {
    eprintln!("USING FUTHARK BINDGEN");
    let mode = match algorithm {
        RustAlgorithm::AugmentingPath => 0i32,
        RustAlgorithm::Leveling => 1i32,
        RustAlgorithm::PushRelabel => 2i32,
    };

    let n = i32::try_from(problem.n)
        .map_err(|_| io::Error::new(io::ErrorKind::InvalidInput, "n too large"))?;
    let m = i32::try_from(problem.edges.len())
        .map_err(|_| io::Error::new(io::ErrorKind::InvalidInput, "m too large"))?;

    let bounds_host: Vec<i64> = problem.bounds.clone();
    let mut eu_host = Vec::with_capacity(problem.edges.len());
    let mut ev_host = Vec::with_capacity(problem.edges.len());
    for &(u, v) in &problem.edges {
        eu_host.push(
            i32::try_from(u)
                .map_err(|_| io::Error::new(io::ErrorKind::InvalidInput, "u too large"))?,
        );
        ev_host.push(
            i32::try_from(v)
                .map_err(|_| io::Error::new(io::ErrorKind::InvalidInput, "v too large"))?,
        );
    }

    let ctx = graph_orientation_futhark::Context::new()
        .map_err(|e| io::Error::other(format!("futhark context init failed: {e}")))?;

    // This generator expects explicit shape argument: [len]
    let bounds =
        graph_orientation_futhark::ArrayI64D1::new(&ctx, [bounds_host.len() as i64], &bounds_host)
            .map_err(|e| io::Error::other(format!("bounds upload failed: {e}")))?;
    let eu = graph_orientation_futhark::ArrayI32D1::new(&ctx, [eu_host.len() as i64], &eu_host)
        .map_err(|e| io::Error::other(format!("eu upload failed: {e}")))?;
    let ev = graph_orientation_futhark::ArrayI32D1::new(&ctx, [ev_host.len() as i64], &ev_host)
        .map_err(|e| io::Error::other(format!("ev upload failed: {e}")))?;

    let (feasible, directed_u_arr, directed_v_arr, violators_arr) = ctx
        .graph_orientation_solve(mode, n, m, &bounds, &eu, &ev)
        .map_err(|e| io::Error::other(format!("futhark entry failed: {e}")))?;

    // This generator usually uses `values(&ctx)` (or `to_host(&ctx)` in some versions).
    let directed_u: Vec<i64> = directed_u_arr
        .get()
        .map_err(|e| io::Error::other(format!("download directed_u failed: {e}")))?;
    let directed_v: Vec<i64> = directed_v_arr
        .get()
        .map_err(|e| io::Error::other(format!("download directed_v failed: {e}")))?;
    let violators: Vec<i64> = violators_arr
        .get()
        .map_err(|e| io::Error::other(format!("download violators failed: {e}")))?;

    if feasible {
        if directed_u.len() != directed_v.len() {
            return Err(io::Error::other("directed_u/directed_v length mismatch"));
        }
        let directed_edges = directed_u
            .iter()
            .zip(directed_v.iter())
            .map(|(&u, &v)| {
                Ok((
                    usize::try_from(u).map_err(|_| io::Error::other("invalid directed_u index"))?,
                    usize::try_from(v).map_err(|_| io::Error::other("invalid directed_v index"))?,
                ))
            })
            .collect::<io::Result<Vec<_>>>()?;
        Ok(Solution {
            feasible: true,
            directed_edges,
            violators: Vec::new(),
        })
    } else {
        let mut vs = violators
            .into_iter()
            .map(|v| usize::try_from(v).map_err(|_| io::Error::other("invalid violator index")))
            .collect::<io::Result<Vec<_>>>()?;
        vs.sort_unstable();
        vs.dedup();
        Ok(Solution {
            feasible: false,
            directed_edges: Vec::new(),
            violators: vs,
        })
    }
}

#[cfg(not(feature = "ffi-backend"))]
fn solve_via_shared_library(_: &Problem, _algorithm: RustAlgorithm) -> io::Result<Solution> {
    Err(io::Error::new(
        io::ErrorKind::Unsupported,
        "external backends are disabled at build time",
    ))
}

pub fn solution_to_string(solution: &Solution) -> String {
    let mut output = String::new();
    if solution.feasible {
        for (u, v) in &solution.directed_edges {
            output.push_str(&u.to_string());
            output.push(' ');
            output.push_str(&v.to_string());
            output.push('\n');
        }
    } else {
        let mut violators = solution.violators.clone();
        violators.sort_unstable();
        violators.dedup();
        for v in violators {
            output.push_str(&v.to_string());
            output.push('\n');
        }
    }
    output
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Engine {
    Rust,
    Futhark,
    Idris2,
    K,
    Zig,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct SolveConfig {
    pub engine: Engine,
    pub algorithm: RustAlgorithm,
}

impl Default for SolveConfig {
    fn default() -> Self {
        Self {
            engine: Engine::Rust,
            algorithm: RustAlgorithm::Leveling,
        }
    }
}

fn algorithm_from_env() -> Result<RustAlgorithm, String> {
    let v = env::var("GRAPH_ORIENTATION_RUST_ALGORITHM")
        .unwrap_or_else(|_| "leveling".to_string())
        .to_ascii_lowercase();
    match v.as_str() {
        "augmenting" | "edmonds-karp" | "ek" => Ok(RustAlgorithm::AugmentingPath),
        "leveling" | "dinic" => Ok(RustAlgorithm::Leveling),
        "push-relabel" | "pushrelabel" | "relabel" => Ok(RustAlgorithm::PushRelabel),
        _ => Err(format!("unknown algorithm: {v}")),
    }
}

fn engine_from_env() -> Result<Engine, String> {
    let v = env::var("GRAPH_ORIENTATION_BACKEND")
        .unwrap_or_else(|_| "rust".to_string())
        .to_ascii_lowercase();
    match v.as_str() {
        "rust" => Ok(Engine::Rust),
        "futhark" => Ok(Engine::Futhark),
        "idris" | "idris2" => Ok(Engine::Idris2),
        "k" => Ok(Engine::K),
        "zig" => Ok(Engine::Zig),
        _ => Err(format!("unknown backend: {v}")),
    }
}

pub fn config_from_env() -> Result<SolveConfig, String> {
    Ok(SolveConfig {
        engine: engine_from_env()?,
        algorithm: algorithm_from_env()?,
    })
}

pub fn solve_with_config(problem: &Problem, cfg: SolveConfig) -> io::Result<Solution> {
    match cfg.engine {
        Engine::Rust => Ok(solve_rust(problem, cfg.algorithm)),
        Engine::Futhark => solve_via_futhark_bindgen(problem, cfg.algorithm),
        Engine::Idris2 | Engine::K | Engine::Zig => {
            solve_via_shared_library(cfg.engine, problem, cfg.algorithm)
        }
    }
}

pub fn solve_file_with_config(cfg: SolveConfig) -> io::Result<()> {
    let input = fs::read_to_string(INPUT_PATH)?;
    let problem =
        parse_problem(&input).map_err(|msg| io::Error::new(io::ErrorKind::InvalidData, msg))?;
    let solution = solve_with_config(&problem, cfg)?;
    fs::write(OUTPUT_PATH, solution_to_string(&solution))?;
    Ok(())
}
