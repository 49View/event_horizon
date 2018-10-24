make_script_exec() {
chmod +x $1.sh
}

make_script_exec "build-machine"
make_script_exec "gitbuild"
make_script_exec "gitmakebuild"
make_script_exec "install_boost"
make_script_exec "install_embree"
make_script_exec "install_mkl"
make_script_exec "install_cmake"
make_script_exec "install_git-lfs"
