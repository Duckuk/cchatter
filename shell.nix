{
  pkgs ?
    import (builtins.fetchTarball {
      url = "https://github.com/NixOS/nixpkgs/archive/c6245e83d836d0433170a16eb185cefe0572f8b8.tar.gz";
      sha256 = "sha256-G/WVghka6c4bAzMhTwT2vjLccg/awmHkdKSd2JrycLc=";
    }) {},
}:
pkgs.clangStdenv.mkDerivation {
  name = "networking-shell";
  nativeBuildInputs = with pkgs; [
    clang-tools
    clang
    lldb
    cmake
    ncurses6
    git
  ];
  buildInputs = [
  ];
}
