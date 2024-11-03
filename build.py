import os, sys

# c compiler
compiler: str = "gcc"

# choose which modules to include, will automatically compile all .c files in [dir]/src
src_dirs: "list[str]" = ['chess-lib', 'chess-gui'] 

# c files not with the src folder of a module that should be included
loose_c_files: "list[str]" = ['tests/botvbot.c'] 

# gcc compiler flags + custom flags (-DDEBUG enables some debug messages in chess-lib functions)
flags: "list[str]" = ["-Werror", "-Wall", "-pthread", "-O3", "-DDEBUG"]

# output directory
build_dir: "list[str]" = "build" 

#output name
target_name: str = "botvbot.exe"

if __name__ == "__main__":

    path_from_cwd = (
        os.path.join(*sys.argv[0].split("/")[:-1]) if "/" in sys.argv[0] else ""
    )


    target = os.path.join(build_dir, target_name)
    flag_string = " ".join(flags)
    # compile each .c file in /src into .o

    files = [*loose_c_files]
    for src_dir in src_dirs:
        src_full_path = os.path.join(path_from_cwd, src_dir, 'src')
        files = [*files, *(os.path.join(src_full_path, file) for file in os.listdir(src_full_path) if file[-2:] == ".c")]
        print(files)
    cmd = " ".join([compiler, *files, *flags, "-o", target])
    print(f"executing: {cmd}")
    os.system(cmd)
