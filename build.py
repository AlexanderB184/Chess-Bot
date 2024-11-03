import os, sys, json

opt_flag = ['-O3']
warn_flag = ['-Wall', '-Werror']
thread_flag = ['-pthread']
debug_flag = ['-DDEBUG']

def path_to_this() -> str:
    return os.path.join(*sys.argv[0].split("/")[:-1]) if "/" in sys.argv[0] else ""

def list_c_files(dir: str) -> 'list[str]':
    return [os.path.join(dir, file) for file in os.listdir(dir) if file[-2:] == ".c"]

def compile(compiler, files, flags, target):
    cmd = " ".join([compiler, *files, *flags, "-o", target])
    print(f"executing: {cmd}")
    os.system(cmd)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"usage: python3 {sys.argv[0]} [build scheme name]")
        print(f"do build.py --help for a list of options")
        exit(0)

    if sys.argv[1] == "--help":
        with open('build.json') as file:
            schemes = json.load(file)
        for scheme in schemes:
            print(scheme)
        exit(0)

    
    with open('build.json') as file:
        scheme = json.load(file)[sys.argv[1]]
    compiler = scheme["compiler"]
    files = scheme["files"]
    modules = scheme["modules"]
    flags = scheme["flags"]
    builddir = scheme["builddir"]
    output = os.path.join(builddir, scheme["output"])
    for module in modules:
        files = files + list_c_files(os.path.join(module, 'src'))
    compile(compiler, files, flags, output)
