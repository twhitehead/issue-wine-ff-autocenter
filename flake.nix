{
  description = "Wine issue 8605";

  outputs = inputs@{ flake-parts, nixpkgs, systems, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      imports = [ ];
      systems = import systems;
      perSystem = { config, self', inputs', pkgs, system, ... }: {
        packages = {
          test = pkgs.pkgsCross.mingw32.callPackage (
            { stdenv, cmake }:
            stdenv.mkDerivation {
              name = "ffdefault";
              version = "1.0.0";
              src = ./.;
              nativeBuildInputs = [ cmake ];
            }
          ) { };
        };
      };
    };
}
