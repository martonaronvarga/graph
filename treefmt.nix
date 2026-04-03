{pkgs, ...}: {
  projectRootFile = "flake.nix";

  settings.global.excludes = [
    "target"
    "result"
    ".direnv"
  ];

  programs = {
    alejandra.enable = true;
    rustfmt.enable = true;
  };
}
