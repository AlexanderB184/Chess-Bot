import os
import subprocess
import glob
import sys
import shutil

src_folders : 'list[str]' = ['chess-lib', 'chess-bot']

c_flags : 'list[str]' = ['-Wall', '-Werror', '-std=c11', '-DNDEBUG', '-D_POSIX_C_SOURCE=199309L', '-fPIC']
optimiser_flags : 'list[str]' = ['-O3']
debug_flags : 'list[str]' = ['-g']
profiler_flags : 'list[str]' = ['-pd']

compiler : str = 'gcc'

cmds : 'list[str]' = ['build', 'save-as-version', 'help']

def find_c_files(folders):
    c_files = []
    for folder in folders:
        c_files.extend(glob.glob(f"{folder}/src/*.c"))
    return c_files

def get_next_version(base_path):
    versions = [int(name.split('-')[1]) for name in os.listdir(base_path) if name.startswith('version-')]
    return max(versions, default=0) + 1

def copy_src_folders(src_folders, dest_folder):
    for folder in src_folders:
        shutil.copytree(folder, os.path.join(dest_folder, folder))

def update_contenders_file(version):
    contenders_file = 'chess-bot-tournament/contenders.h'
    new_entry = f"\"{version}\""
    if os.path.exists(contenders_file):
        with open(contenders_file, 'r') as file:
            lines = file.readlines()

        if lines:
            start = lines[0].index('{') + 1
            end = lines[0].index('}')
            current_contenders = lines[0][start:end].strip()
            if current_contenders:
                new_contenders = f"{current_contenders}, {new_entry}"
            else:
                new_contenders = new_entry
            lines[0] = f'const char* contenders[] = {{{new_contenders}}};\n'

        with open(contenders_file, 'w') as file:
            print(f"Writing to file: '{contenders_file}' 'const char* contenders[] = {{{new_contenders}}};\"'")

            file.writelines(lines)
    else:
        print(f"Creating file: {contenders_file}")
        with open(contenders_file, 'w') as file:
            print(f"Writing to file: '{contenders_file}' 'const char* contenders[] = {{{new_entry}}};\"'")
            file.write(f'const char* contenders[] = {{{new_entry}}};\n')

if __name__ == "__main__":

    if len(sys.argv) < 2:
        print("usage: build.py [cmd] (args))")
        exit(0)

    cmd = sys.argv[1]
    

    if cmd not in cmds:
        print(f"unknown command: {cmd}")
        exit(1)

    if cmd == "help":
        if len(sys.argv) == 2:
            print("cmds: build save-as-version help")
            print("for help on a specific command do help [cmd]")
            exit(0)
        if sys.argv[2] == "build":
            print("usage: build.py build [main_file_path] [output_path] (additional flags)")
            exit(0)
        if sys.argv[2] == "save-as-version":
            print("usage: build.py save-as-version (additional flags)")
            exit(0)
        print(f"unknown command: {sys.argv[2]}")
        exit(0)

    if cmd == "build":
        if len(sys.argv) < 4:
            print("usage: build.py build [main_file_path] [output_path] (additional flags)")
            exit(0)
        target_file = sys.argv[2]
        output_path = sys.argv[3]

        c_files = find_c_files(src_folders)

        all_files = ' '.join(c_files + [target_file])

        additional_flags = sys.argv[4:] if len(sys.argv) > 4 else []

        all_flags = ' '.join(c_flags + optimiser_flags + additional_flags)

        compile_cmd = f"{compiler} {all_files} -o {output_path} {all_flags}"

        print(f"running command: {compile_cmd}")

        result = subprocess.run(compile_cmd, shell=True)

        if result.returncode != 0:
            print("compilation failed")
            exit(1)
        else:
            print(f"Compilation succeeded, binary created: {output_path}")
            
    if cmd == "save-as-version":
        tournament_folder = "chess-bot-tournament"

        next_version = get_next_version(tournament_folder)
        version_folder = os.path.join(tournament_folder, f"version-{next_version}")

        print("Making folder {version_folder}")
        os.makedirs(version_folder, exist_ok=True)

        print("Copying folders {src_folders} into {version_folder}")
        copy_src_folders(src_folders, version_folder)

        

        version_src_folders = [os.path.join(version_folder, folder) for folder in src_folders]
        c_files = find_c_files(version_src_folders)

        all_files = ' '.join(c_files)

        additional_flags = sys.argv[2:] if len(sys.argv) > 2 else []
        
        all_flags = c_flags + optimiser_flags + additional_flags + ['-shared', '-fPIC']

        output_so = os.path.join(version_folder, f"version-{next_version}.so")
        compile_cmd = f"gcc {' '.join(all_flags)} {all_files} -o {output_so}"
        
        print(f"Running command: {compile_cmd}")
        
        result = subprocess.run(compile_cmd, shell=True)
        
        if result.returncode != 0:
            print("Compilation failed")
            exit(1)
        else:
            print(f"Compilation succeeded, shared object created: {output_so}")

        
        update_contenders_file(f"version-{next_version}")
        
        