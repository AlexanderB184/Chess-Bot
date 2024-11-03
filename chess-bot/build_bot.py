import os, sys


compiler: str = "gcc"

source_dir: "str" = "src"
chess_lib_src_dir: "str" = "chess-lib/src"

obj_dir: "list[str]" = "obj"

build_dir: "list[str]" = "build"

lib_dir: "list[str]" = "lib"


flags: "list[str]" = ["-Werror", "-Wall", "-pthread", "-O3", "-DDEBUG"]

target_name: str = "chess.lib"

if __name__ == "__main__":
    path_from_cwd = os.path.join(*sys.argv[0].split("/")[:-1])

    flag_string = " ".join(flags)

    src_full_path = os.path.join(path_from_cwd, source_dir)
    obj_full_path = os.path.join(path_from_cwd, obj_dir)
    lib_full_path = lib_dir
    target = os.path.join(build_dir, target_name)

    files = [
        os.path.join(src_full_path, file)
        for file in os.listdir(src_full_path)
        if file[-2:] == ".c"
    ]

    chess_lib_files = [
        os.path.join(chess_lib_src_dir, file)
        for file in os.listdir(chess_lib_src_dir)
        if file[-2:] == ".c"
    ]

    cmd = " ".join([compiler, *chess_lib_files, *files, *flags, '-o', target])
    print(f"executing: {cmd}")

    os.system(cmd)
