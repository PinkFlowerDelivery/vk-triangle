{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs =
    { self, nixpkgs, ... }:
    let
      system = "x86_64-linux";
    in
    {
      devShells.${system}.default =
        let
          pkgs = import nixpkgs { inherit system; };
        in
        pkgs.mkShell {
          packages = with pkgs; [
            vulkan-tools
            pkg-config
          ];
          buildInputs = with pkgs; [
            vulkan-headers
            vulkan-loader
            vulkan-validation-layers
            glfw
          ];

          shellHook = ''
            export CMAKE_PREFIX_PATH="${pkgs.glfw}:${pkgs.vulkan-loader}:$CMAKE_PREFIX_PATH"

            export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:${
              pkgs.lib.makeLibraryPath [
                pkgs.vulkan-loader
                pkgs.glfw
              ]
            }"

            export VK_LAYER_PATH="${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d"          '';
        };
    };
}
