{
  description = "graph orientation with Rust and FFI kernels";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    naersk.url = "github:nix-community/naersk";
    naersk.inputs.nixpkgs.follows = "nixpkgs";
    fenix.url = "github:nix-community/fenix";
    fenix.inputs.nixpkgs.follows = "nixpkgs";
    treefmt-nix.url = "github:numtide/treefmt-nix";
    idris2.url = "github:idris-lang/Idris2";
    idris2.inputs.nixpkgs.follows = "nixpkgs";
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs = inputs @ {
    self,
    nixpkgs,
    flake-utils,
    naersk,
    fenix,
    treefmt-nix,
    idris2,
    flake-parts,
    ...
  }:
    flake-parts.lib.mkFlake {inherit inputs;} {
      systems = flake-utils.lib.defaultSystems;
      imports = [];
      perSystem = {system, ...}: let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [fenix.overlays.default];
        };

        rustToolchain = fenix.packages.${system}.stable.withComponents [
          "rustc"
          "cargo"
          "rustfmt"
          "clippy"
          "rust-src"
        ];
        naersk-lib = pkgs.callPackage naersk {
          rustc = rustToolchain;
          cargo = rustToolchain;
        };

        treefmtEval = treefmt-nix.lib.evalModule pkgs ./treefmt.nix;

        graphSrc = ./.;

        # ----------- KERNEL BUILDS -----------
        kona = pkgs.fetchFromGitHub {
          owner = "kevinlawler";
          repo = "kona";
          rev = "master";
          sha256 = "sha256-eM5UlNzxzGATxOYygLM90K5EiZINyb/AKx7zgeFuE3g=";
        };

        kKernel = pkgs.stdenv.mkDerivation {
          name = "graph-orientation-k";
          src = ./kernels/K;
          nativeBuildInputs = [pkgs.gcc];
          buildInputs = [];
          unpackPhase = "true";
          installPhase = ''
            mkdir -p $out/lib
            cp ${./kernels/K/graph_orientation.k} graph_orientation.k
            gcc -fPIC -shared -o $out/lib/libgraph-orientation-k.so \
              kbridge.c ${kona}/build/c.o -I${kona}/c -lm
          '';
          outputs = ["out" "lib"];
        };

        idris2Kernel = pkgs.stdenv.mkDerivation {
          pname = "graph-orientation-idris2";
          version = "1.0";
          src = ./kernels/idris2;
          buildInputs = [
            idris2.packages.${system}.idris2
            pkgs.gcc
            pkgs.clang
          ];
          nativeBuildInputs = with pkgs; [idris2.packages.${system}.idris2 gcc clang];
          dontUnpack = true;
          installPhase = ''
            mkdir -p $out/lib
            cd $src
            idris2 --cg c GraphOrientation.idr -o graph_orientation_idris2
            gcc -fPIC -shared -o $out/lib/libgraph-orientation-idris2.so GraphOrientation.c bridge_idris2.c
          '';
        };

        zigKernel = pkgs.stdenv.mkDerivation {
          pname = "graph-orientation-zig";
          version = "1.0";
          src = ./kernels/zig;
          nativeBuildInputs = [pkgs.zig];
          installPhase = ''
            mkdir -p $out/lib
            zig build-lib graph_orientation.zig -dynamic -O ReleaseFast -fPIC \
              -femit-bin=$out/lib/libgraph-orientation-zig.so
          '';
        };

        rustPackage = naersk-lib.buildPackage {
          src = graphSrc;
          cargoBuildOptions = opts: opts ++ ["--features" "futhark-backend,ffi-backend"];
          nativeBuildInputs = [pkgs.futhark pkgs.clang pkgs.gcc pkgs.pkg-config];
          buildInputs = [pkgs.opencl-headers pkgs.ocl-icd];
        };

        linkBin = name: file:
          pkgs.runCommand name {} ''
            mkdir -p $out/bin
            ln -s ${rustPackage}/bin/${file} $out/bin/${name}
          '';
      in {
        packages = rec {
          default = pkgs.runCommand "graph-orientation" {nativeBuildInputs = [pkgs.makeWrapper];} ''
            mkdir -p $out/bin
            makeWrapper ${rustPackage}/bin/main $out/bin/graph-orientation \
              --set GRAPH_ORIENTATION_K_LIB ${kKernel}/lib/libgraph-orientation-k.so \
              --set GRAPH_ORIENTATION_IDRIS2_LIB ${idris2Kernel}/lib/libgraph-orientation-idris2.so \
              --set GRAPH_ORIENTATION_ZIG_LIB ${zigKernel}/lib/libgraph-orientation-zig.so \
              --set-default GRAPH_ORIENTATION_BACKEND rust
          '';
          graph-orientation = default;
          augmenting = linkBin "augmenting" "augmenting";
          leveling = linkBin "leveling" "leveling";
          "push-relabel" = linkBin "push-relabel" "push-relabel";
          rust = rustPackage;

          ffi-kernels = pkgs.symlinkJoin {
            name = "graph-orientation-kernels";
            paths = [
              kKernel
              idris2Kernel
              zigKernel
            ];
            postBuild = ''
              ln -s ${kKernel}/lib/libgraph-orientation-k.so $out/lib/
              ln -s ${idris2Kernel}/lib/libgraph-orientation-idris2.so $out/lib/
              ln -s ${zigKernel}/lib/libgraph-orientation-zig.so $out/lib/
            '';
          };
        };

        apps.default = flake-utils.lib.mkApp {drv = self.packages.${system}.default;};

        formatter = treefmtEval.config.build.wrapper;
        checks.formatting = treefmtEval.config.build.check self;

        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            rustToolchain
            futhark
            zig
            clang
            gcc
            idris2.packages.${system}.idris2
            kona
            pkg-config
            openssl
            treefmt
            nixfmt
          ];
          LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath [
            kKernel
            idris2Kernel
            zigKernel
          ];
        };
      };
    };
}
