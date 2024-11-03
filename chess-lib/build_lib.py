import os, sys


compiler: str = "gcc"

source_dir: "str" = "src"

obj_dir: "list[str]" = "obj"

build_dir: "list[str]" = "build"


flags: "list[str]" = ["-c", "-Werror", "-Wall", "-O3", "-DDEBUG"]

target_name: str = "chess.lib"

if __name__ == "__main__":
    path_from_cwd = "/".join(sys.argv[0].split("/")[:-1])

    flag_string = " ".join(flags)

    src_full_path = os.path.join(path_from_cwd, source_dir)
    obj_full_path = os.path.join(path_from_cwd, obj_dir)

    # compile each .c file in /src into .o
    for file in os.listdir(src_full_path):
        if file[-2:] == ".c":
            cmd = " ".join([
                compiler,
                os.path.join(src_full_path, file),
                flag_string,
                "-o",
                os.path.join(obj_full_path, file[:-2] + ".o")
            ])
            print(f"executing: {cmd}")
            os.system(cmd)

    cmd = f"ar rcs {build_dir}/{target_name} {obj_full_path}/*.o"
    print(f"executing: {cmd}")

    os.system(cmd)
