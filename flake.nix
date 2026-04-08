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

        # kKernel = pkgs.stdenv.mkDerivation {
        #   name = "graph-orientation-k";
        #   src = ./kernels/K;
        #   nativeBuildInputs = [pkgs.gcc];
        #   buildInputs = [];
        #   unpackPhase = "true";
        #   buildPhase = ''
        #     runHook preBuild
        #     cp ${./kernels/K/graph_orientation.k} graph_orientation.k
        #     cp ${./kernels/K/bridge_k.c} kbridge.c
        #     gcc -fPIC -c kbridge.c -I${kona}/c -o kbridge.o
        #     gcc -shared -o libgraph-orientation-k.so kbridge.o ${kona}/build/c.o -lm
        #     runHook postBuild
        #   '';
        #   installPhase = ''
        #     runHook preInstall
        #     mkdir -p $out/lib
        #     cp -f libgraph-orientation-k.so $out/lib/
        #     runHook postInstall
        #   '';
        #   outputs = ["out" "lib"];
        # };

        fs = pkgs.lib.fileset;
        sourceFiles = fs.unions [
          ./kernels/graph_orientation_ffi.h
          ./kernels/idris2/graph_orientation.ipkg
          ./kernels/idris2/idris_bridge.h
          ./kernels/idris2/idris_bridge.c
          ./kernels/idris2/src
        ];
        idris2Kernel = pkgs.stdenv.mkDerivation {
          pname = "graph-orientation-idris2";
          version = "1.0";
          src = fs.toSource {
            root = ./kernels;
            fileset = sourceFiles;
          };
          buildInputs = [
            idris2.packages.${system}.idris2
            pkgs.gcc
            pkgs.gmp
            pkgs.chez
            pkgs.clang
          ];
          nativeBuildInputs = with pkgs; [idris2.packages.${system}.idris2 gcc clang chez];
          buildPhase = ''
            runHook preBuild
            cd idris2

            export NIX_CFLAGS_COMPILE="$NIX_CFLAGS_COMPILE -fPIC -include idris_bridge.h"
            export IDRIS2_CC=gcc
            export IDRIS2_SRC=${idris2}

            idris2 --build graph_orientation.ipkg

            for f in build/exec/*.o; do
              objcopy --strip-debug --redefine-sym main=idris_main "$f"
            done

            gcc -c idris_bridge.c -o idris_bridge.o

            find build/exec -name "*.o" > o_files.list

            echo "Compiling Idris 2 RTS (Runtime System)..."
            if [ -z "$IDRIS2_SRC" ]; then
                echo "Error: IDRIS2_SRC environment variable is not set."
                echo "Make sure you added IDRIS2_SRC=${idris2} to your flake.nix!"
                exit 1
            fi

            REFC_SUPPORT="$IDRIS2_SRC/support/refc"
            C_SUPPORT="$IDRIS2_SRC/support/c"

            # Compile both the refc specific files AND the common C support files
            for c_file in "$REFC_SUPPORT"/*.c "$C_SUPPORT"/*.c; do
                echo "  -> Compiling $(basename "$c_file")"
                # We compile the raw C files with -fPIC for our shared library
                gcc -c -fPIC "$c_file" -I"$REFC_SUPPORT" -I"$C_SUPPORT" -o "rts_$(basename "$c_file" .c).o"
                echo "rts_$(basename "$c_file" .c).o" >> o_files.list
            done

            echo "Linking shared library..."
            gcc -shared -o libgraph-orientation-idris2.so \
                idris_bridge.o \
                $(cat o_files.list) \
                -L "$IDRIS2_SRC/libs" \
                -lgmp -lm

            runHook postBuild
          '';
          installPhase = ''
            runHook preInstall
            mkdir -p $out/lib
            cp -f libgraph-orientation-idris2.so $out/lib/
            runHook postInstall
          '';
        };

        zigKernel = pkgs.stdenv.mkDerivation {
          pname = "graph-orientation-zig";
          version = "1.0";
          src = ./kernels/zig;
          nativeBuildInputs = [pkgs.zig];
          dontUseZigBuild = true;
          dontUseZigInstall = true;

          buildPhase = ''
            runHook preBuild

            zig build-lib ${./kernels/zig/graph_orientation_zig.zig} \
              -dynamic -O ReleaseFast -fPIC \
              -femit-bin=libgraph-orientation-zig.so

            runHook postBuild
          '';
          installPhase = ''
            runHook preInstall

            mkdir -p $out/lib

            cp libgraph-orientation-zig.so $out/lib/

            runHook postInstall
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
          default = pkgs.stdenv.mkDerivation {
            pname = "graph-orientation";
            version = "1.0";
            dontUnpack = true;
            nativeBuildInputs = [pkgs.makeWrapper];
            installPhase = ''
              mkdir -p $out/bin
              # futhark is linked from within rust
              makeWrapper ${rustPackage}/bin/main $out/bin/graph-orientation \
                --set GRAPH_ORIENTATION_IDRIS2_LIB ${idris2Kernel}/lib/libgraph-orientation-idris2.so \
                --set GRAPH_ORIENTATION_ZIG_LIB ${zigKernel}/lib/libgraph-orientation-zig.so \
                --set-default GRAPH_ORIENTATION_BACKEND rust
              # --set GRAPH_ORIENTATION_K_LIB ''${kKernel}/lib/libgraph-orientation-k.so \
            '';
          };
          graph-orientation = default;
          augmenting = linkBin "augmenting" "augmenting";
          leveling = linkBin "leveling" "leveling";
          "push-relabel" = linkBin "push-relabel" "push-relabel";
          rust = rustPackage;

          ffi-kernels = pkgs.symlinkJoin {
            pname = "graph-orientation-kernels";
            paths = [
              # kKernel
              idris2Kernel
              zigKernel
            ];
            postBuild = ''
              # ln -s ''${kKernel}/lib/libgraph-orientation-k.so $out/lib/
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
            gmp
            idris2.packages.${system}.idris2
            kona
            pkg-config
            openssl
            treefmt
            nixfmt
            (pkgs.python3.withPackages (python-pkgs:
              with python-pkgs; [
                numpy
                matplotlib
              ]))
          ];
          IDRIS2_SRC = "${idris2}";
          LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath [
            # kKernel
            idris2Kernel
            zigKernel
            rustToolchain
          ];
        };
      };
    };
}
