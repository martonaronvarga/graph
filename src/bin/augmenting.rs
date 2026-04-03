fn main() -> std::io::Result<()> {
    graph_orientation::solve_file_with_config(graph_orientation::SolveConfig {
        engine: graph_orientation::Engine::Rust,
        algorithm: graph_orientation::RustAlgorithm::AugmentingPath,
    })
}
