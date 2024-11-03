import os

compiler: str = 'gcc'

files: list[str] = ['']

flags: list[str] = ['Werror', 'Wall', 'pthread', 'O3']

target_name: str = 'bot.bin'

if __name__ == '__main__':
    cmd = compiler
    for file in files:
        cmd += file + ' '
    
    for flag in flags:
        cmd += '-' + flag + ' '
    
    cmd += f'-d {target_name}'

    os.system(cmd)
