const std = @import("std");

pub const RawProblem = extern struct {
    n: usize,
    m: usize,
    bounds: [*]const i64,
    edges_u: [*]const usize,
    edges_v: [*]const usize,
};

pub const RawSolution = extern struct {
    feasible: u8,
    directed_u: [*]usize,
    directed_v: [*]usize,
    directed_capacity: usize,
    directed_len: usize,
    violators: [*]usize,
    violators_capacity: usize,
    violators_len: usize,
};

const Edge = struct {
    to: usize,
    rev: usize,
    cap: i64,
};

const FlowGraph = struct {
    adj: []std.ArrayListUnmanaged(Edge),
    allocator: std.mem.Allocator,

    fn init(allocator: std.mem.Allocator, n: usize) !FlowGraph {
        const adj = try allocator.alloc(std.ArrayListUnmanaged(Edge), n);
        for (adj) |*lst| lst.* = .{};
        return .{ .adj = adj, .allocator = allocator };
    }

    fn deinit(self: *FlowGraph) void {
        for (self.adj) |*lst| lst.deinit(self.allocator);
        self.allocator.free(self.adj);
    }

    fn len(self: *const FlowGraph) usize {
        return self.adj.len;
    }

    fn addEdge(self: *FlowGraph, u: usize, v: usize, cap: i64) !void {
        const u_rev = self.adj[u].items.len;
        const v_rev = self.adj[v].items.len;
        try self.adj[u].append(self.allocator, .{ .to = v, .rev = v_rev, .cap = cap });
        try self.adj[v].append(self.allocator, .{ .to = u, .rev = u_rev, .cap = 0 });
    }
};

const BuiltNetwork = struct {
    graph: FlowGraph,
    source: usize,
    sink: usize,
    vertex_offset: usize,
    edge_nodes: []usize,
    allocator: std.mem.Allocator,

    fn deinit(self: *BuiltNetwork) void {
        self.graph.deinit();
        self.allocator.free(self.edge_nodes);
    }
};

fn validProblemPointers(p: *const RawProblem, out: *RawSolution) bool {
    _ = out;
    return p.bounds != null and p.edges_u != null and p.edges_v != null;
}

fn buildNetwork(allocator: std.mem.Allocator, p: *const RawProblem) !BuiltNetwork {
    const m = p.m;
    const source: usize = 0;
    const edge_offset: usize = 1;
    const vertex_offset: usize = 1 + m;
    const sink: usize = 1 + m + p.n;
    var g = try FlowGraph.init(allocator, sink + 1);
    const edge_nodes = try allocator.alloc(usize, m);

    var k: usize = 0;
    while (k < m) : (k += 1) {
        const u = p.edges_u[k];
        const v = p.edges_v[k];
        const e_node = edge_offset + k;
        edge_nodes[k] = e_node;
        try g.addEdge(source, e_node, 1);
        try g.addEdge(e_node, vertex_offset + u, 1);
        try g.addEdge(e_node, vertex_offset + v, 1);
    }

    var i: usize = 0;
    while (i < p.n) : (i += 1) {
        try g.addEdge(vertex_offset + i, sink, p.bounds[i]);
    }

    return .{
        .graph = g,
        .source = source,
        .sink = sink,
        .vertex_offset = vertex_offset,
        .edge_nodes = edge_nodes,
        .allocator = allocator,
    };
}

fn reachableFrom(allocator: std.mem.Allocator, g: *const FlowGraph, s: usize) ![]bool {
    const n = g.adj.len;
    const seen = try allocator.alloc(bool, n);
    @memset(seen, false);
    var q = std.ArrayListUnmanaged(usize){};
    defer q.deinit(allocator);

    seen[s] = true;
    try q.append(allocator, s);
    var head: usize = 0;
    while (head < q.items.len) : (head += 1) {
        const u = q.items[head];
        for (g.adj[u].items) |e| {
            if (e.cap > 0 and !seen[e.to]) {
                seen[e.to] = true;
                try q.append(allocator, e.to);
            }
        }
    }
    return seen;
}

fn maxFlowEdmondsKarp(g: *FlowGraph, s: usize, t: usize, allocator: std.mem.Allocator) !i64 {
    const n = g.len();
    var total: i64 = 0;
    const parent_v = try allocator.alloc(usize, n);
    defer allocator.free(parent_v);
    const parent_e = try allocator.alloc(usize, n);
    defer allocator.free(parent_e);
    const seen = try allocator.alloc(bool, n);
    defer allocator.free(seen);

    while (true) {
        @memset(seen, false);
        var q = std.ArrayListUnmanaged(usize){};
        defer q.deinit(allocator);
        var head: usize = 0;

        seen[s] = true;
        try q.append(allocator, s);

        while (head < q.items.len and !seen[t]) : (head += 1) {
            const u = q.items[head];
            for (g.adj[u].items, 0..) |e, idx| {
                if (e.cap > 0 and !seen[e.to]) {
                    seen[e.to] = true;
                    parent_v[e.to] = u;
                    parent_e[e.to] = idx;
                    try q.append(allocator, e.to);
                    if (e.to == t) break;
                }
            }
        }

        if (!seen[t]) break;

        var delta: i64 = std.math.maxInt(i64);
        var v = t;
        while (v != s) {
            const u = parent_v[v];
            const ei = parent_e[v];
            const c = g.adj[u].items[ei].cap;
            if (c < delta) delta = c;
            v = u;
        }

        v = t;
        while (v != s) {
            const u = parent_v[v];
            const ei = parent_e[v];
            const rev = g.adj[u].items[ei].rev;
            g.adj[u].items[ei].cap -= delta;
            g.adj[v].items[rev].cap += delta;
            v = u;
        }

        total += delta;
    }

    return total;
}

fn dinicBfs(g: *const FlowGraph, s: usize, t: usize, level: []i32, allocator: std.mem.Allocator) !bool {
    @memset(level, -1);
    var q = std.ArrayListUnmanaged(usize){};
    defer q.deinit(allocator);
    var head: usize = 0;

    level[s] = 0;
    try q.append(allocator, s);

    while (head < q.items.len) : (head += 1) {
        const u = q.items[head];
        for (g.adj[u].items) |e| {
            if (e.cap > 0 and level[e.to] < 0) {
                level[e.to] = level[u] + 1;
                try q.append(allocator, e.to);
            }
        }
    }
    return level[t] >= 0;
}

fn dinicDfs(g: *FlowGraph, v: usize, t: usize, pushed: i64, level: []i32, it: []usize) i64 {
    if (v == t) return pushed;
    while (it[v] < g.adj[v].items.len) {
        const i = it[v];
        const to = g.adj[v].items[i].to;
        const cap = g.adj[v].items[i].cap;
        if (cap > 0 and level[v] + 1 == level[to]) {
            const tr = dinicDfs(g, to, t, @min(pushed, cap), level, it);
            if (tr > 0) {
                const rev = g.adj[v].items[i].rev;
                g.adj[v].items[i].cap -= tr;
                g.adj[to].items[rev].cap += tr;
                return tr;
            }
        }
        it[v] += 1;
    }
    return 0;
}

fn maxFlowDinic(g: *FlowGraph, s: usize, t: usize, allocator: std.mem.Allocator) !i64 {
    const n = g.len();
    const level = try allocator.alloc(i32, n);
    defer allocator.free(level);
    const it = try allocator.alloc(usize, n);
    defer allocator.free(it);

    var flow: i64 = 0;
    const inf = std.math.maxInt(i64) / 4;

    while (try dinicBfs(g, s, t, level, allocator)) {
        @memset(it, 0);
        while (true) {
            const pushed = dinicDfs(g, s, t, inf, level, it);
            if (pushed == 0) break;
            flow += pushed;
        }
    }

    return flow;
}

fn enqueue(allocator: std.mem.Allocator, v: usize, s: usize, t: usize, excess: []i64, queued: []bool, active: *std.ArrayListUnmanaged(usize)) !void {
    if (v != s and v != t and excess[v] > 0 and !queued[v]) {
        queued[v] = true;
        try active.append(allocator, v);
    }
}

fn pushPR(allocator: std.mem.Allocator, g: *FlowGraph, u: usize, idx: usize, s: usize, t: usize, height: []usize, excess: []i64, queued: []bool, active: *std.ArrayListUnmanaged(usize)) !bool {
    const v = g.adj[u].items[idx].to;
    const cap = g.adj[u].items[idx].cap;
    if (cap == 0 or height[u] != height[v] + 1) return false;
    const amount = @min(excess[u], cap);
    if (amount == 0) return false;
    const rev = g.adj[u].items[idx].rev;
    g.adj[u].items[idx].cap -= amount;
    g.adj[v].items[rev].cap += amount;
    excess[u] -= amount;
    excess[v] += amount;
    try enqueue(allocator, v, s, t, excess, queued, active);
    return true;
}

fn relabelPR(g: *FlowGraph, u: usize, height: []usize) void {
    var min_h: usize = std.math.maxInt(usize);
    for (g.adj[u].items) |e| {
        if (e.cap > 0 and height[e.to] < min_h) min_h = height[e.to];
    }
    height[u] = if (min_h == std.math.maxInt(usize)) std.math.maxInt(usize) / 4 else min_h + 1;
}

fn dischargePR(allocator: std.mem.Allocator, g: *FlowGraph, u: usize, s: usize, t: usize, height: []usize, excess: []i64, next: []usize, queued: []bool, active: *std.ArrayListUnmanaged(usize)) !void {
    while (excess[u] > 0) {
        if (next[u] == g.adj[u].items.len) {
            relabelPR(g, u, height);
            next[u] = 0;
            continue;
        }
        const idx = next[u];
        if (!(try pushPR(allocator, g, u, idx, s, t, height, excess, queued, active))) {
            next[u] += 1;
        }
    }
}

fn maxFlowPushRelabel(g: *FlowGraph, s: usize, t: usize, allocator: std.mem.Allocator) !i64 {
    const n = g.len();
    const height = try allocator.alloc(usize, n);
    defer allocator.free(height);
    const excess = try allocator.alloc(i64, n);
    defer allocator.free(excess);
    const next = try allocator.alloc(usize, n);
    defer allocator.free(next);
    const queued = try allocator.alloc(bool, n);
    defer allocator.free(queued);

    @memset(height, 0);
    @memset(excess, 0);
    @memset(next, 0);
    @memset(queued, false);

    var active = std.ArrayListUnmanaged(usize){};
    defer active.deinit(allocator);
    var head: usize = 0;

    height[s] = n;

    var idx: usize = 0;
    while (idx < g.adj[s].items.len) : (idx += 1) {
        const cap = g.adj[s].items[idx].cap;
        if (cap == 0) continue;
        const v = g.adj[s].items[idx].to;
        const rev = g.adj[s].items[idx].rev;
        g.adj[s].items[idx].cap -= cap;
        g.adj[v].items[rev].cap += cap;
        excess[v] += cap;
        try enqueue(allocator, v, s, t, excess, queued, &active);
    }

    while (head < active.items.len) : (head += 1) {
        const u = active.items[head];
        queued[u] = false;
        try dischargePR(allocator, g, u, s, t, height, excess, next, queued, &active);
        try enqueue(allocator, u, s, t, excess, queued, &active);
    }

    return excess[t];
}

fn reconstructOrientation(p: *const RawProblem, built: *const BuiltNetwork, out: *RawSolution) !void {
    if (out.directed_capacity < p.m) return error.OutTooSmall;
    var k: usize = 0;
    while (k < p.m) : (k += 1) {
        const u = p.edges_u[k];
        const v = p.edges_v[k];
        const e_node = built.edge_nodes[k];
        var head_opt: ?usize = null;

        for (built.graph.adj[e_node].items) |e| {
            if (e.to >= built.vertex_offset and e.to < built.vertex_offset + p.n and e.cap == 0) {
                head_opt = e.to - built.vertex_offset;
                break;
            }
        }

        if (head_opt == null) return error.BadFlowDecode;
        const head = head_opt.?;

        if (head == u) {
            out.directed_u[k] = v;
            out.directed_v[k] = u;
        } else {
            out.directed_u[k] = u;
            out.directed_v[k] = v;
        }
    }
    out.directed_len = p.m;
    out.violators_len = 0;
}

fn fillViolators(allocator: std.mem.Allocator, p: *const RawProblem, built: *const BuiltNetwork, out: *RawSolution) !void {
    if (out.violators_capacity < p.n) return error.OutTooSmall;
    const seen = try reachableFrom(allocator, &built.graph, built.source);
    defer allocator.free(seen);

    var w: usize = 0;
    var i: usize = 0;
    while (i < p.n) : (i += 1) {
        if (seen[built.vertex_offset + i]) {
            out.violators[w] = i;
            w += 1;
        }
    }
    out.violators_len = w;
    out.directed_len = 0;
}

pub export fn graph_orientation_solve(p_opt: ?*const RawProblem, mode: i32, out_opt: ?*RawSolution) i32 {
    const p = p_opt orelse return 1;
    const out = out_opt orelse return 1;

    var i: usize = 0;
    while (i < p.m) : (i += 1) {
        if (p.edges_u[i] >= p.n or p.edges_v[i] >= p.n) return 4;
    }

    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var built = buildNetwork(allocator, p) catch return 2;
    defer built.deinit();

    const flow = switch (mode) {
        0 => maxFlowEdmondsKarp(&built.graph, built.source, built.sink, allocator) catch return 2,
        1 => maxFlowDinic(&built.graph, built.source, built.sink, allocator) catch return 2,
        2 => maxFlowPushRelabel(&built.graph, built.source, built.sink, allocator) catch return 2,
        else => return 5,
    };

    const feasible = flow == @as(i64, @intCast(p.m));
    out.feasible = if (feasible) 1 else 0;

    if (feasible) {
        reconstructOrientation(p, &built, out) catch return 3;
    } else {
        fillViolators(allocator, p, &built, out) catch return 3;
    }

    return 0;
}
