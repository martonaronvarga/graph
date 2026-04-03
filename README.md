# Graph Orientation with Degree Bounds

This toy project orients an undirected graph so that every vertex `v` has indegree at most `g[v]`. If no feasible orientation exists, the program emits a violating vertex set.

The Rust crate exposes a single file-based interface:

- read `input.txt`
- write `output.txt`

The runtime is deterministic. The default solver path is Rust, but the binary can dispatch to native backend libraries through FFI.

## Binaries

- `main`: backend-dispatching entry point
- `augmenting`: Rust Edmonds–Karp
- `leveling`: Rust Dinic
- `push-relabel`: Rust Push–Relabel

## Backend selection

Run:

`cargo run --features futhark-backend|idris2-backend|k-backend --bin main -- --engine futhark|rust|idris2 --algorithm augmenting|leveling|push-relabel`

## Build

```bash
nix build
# or
cargo build --feature futhark-backend|idris2-backend|k-backend
```

## Notes

The Rust side uses futhark-bindgen for Futhark, and a raw C ABI for other backend interoperability. That keeps the interface direct and avoids subprocess orchestration.
