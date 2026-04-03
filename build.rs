use std::{fs, path::Path};

fn main() {
    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-changed=kernels/futhark/graph_orientation_futhark.fut");

    futhark_bindgen::build(
        futhark_bindgen::Backend::C,
        "kernels/futhark/graph_orientation_futhark.fut",
        "graph_orientation_futhark.rs",
    );

    let out = std::env::var("OUT_DIR").unwrap();
    let p = Path::new(&out).join("graph_orientation_futhark.rs");
    let s = fs::read_to_string(&p).unwrap();
    let s = s.replace("extern \"C\" {", "unsafe extern \"C\" {");
    fs::write(&p, s).unwrap();
}
