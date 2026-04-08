use graph_orientation::{Engine, RustAlgorithm, SolveConfig};

fn parse_engine(s: &str) -> Result<Engine, String> {
    match s.to_ascii_lowercase().as_str() {
        "rust" => Ok(Engine::Rust),
        "futhark" => Ok(Engine::Futhark),
        "idris2" | "idris" => Ok(Engine::Idris2),
        "k" => Ok(Engine::K),
        "zig" => Ok(Engine::Zig),
        other => Err(format!("unknown engine: {other}")),
    }
}

fn parse_algorithm(s: &str) -> Result<RustAlgorithm, String> {
    match s.to_ascii_lowercase().as_str() {
        "augmenting" | "edmonds-karp" | "ek" => Ok(RustAlgorithm::AugmentingPath),
        "leveling" | "dinic" => Ok(RustAlgorithm::Leveling),
        "push-relabel" | "pushrelabel" | "relabel" => Ok(RustAlgorithm::PushRelabel),
        other => Err(format!("unknown algorithm: {other}")),
    }
}

fn parse_args() -> Result<SolveConfig, String> {
    let mut cfg = SolveConfig::default();
    let mut it = std::env::args().skip(1);

    while let Some(arg) = it.next() {
        match arg.as_str() {
            "--engine" => {
                let v = it.next().ok_or("--engine requires value")?;
                cfg.engine = parse_engine(&v)?;
            }
            "--algorithm" => {
                let v = it.next().ok_or("--algorithm requires value")?;
                cfg.algorithm = parse_algorithm(&v)?;
            }
            "--help" | "-h" => {
                println!(
                    "Usage: graph-orientation [--engine rust|futhark|idris2|k|zig] [--algorithm augmenting|leveling|push-relabel]"
                );
                std::process::exit(0);
            }
            other => return Err(format!("unknown argument: {other}")),
        }
    }

    Ok(cfg)
}

fn main() {
    let result = parse_args()
        .map_err(|e| std::io::Error::new(std::io::ErrorKind::InvalidInput, e))
        .and_then(graph_orientation::solve_file_with_config);

    if let Err(e) = result {
        eprintln!("{e}");
        std::process::exit(1);
    }
}
