import subprocess
import os

def format_c_h_files():
    try:
        # Find all .c and .h files and format them using clang-format
        result = subprocess.run(
            'find . -name "*.c" -o -name "*.h" | xargs clang-format -i',
            shell=True,
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        print("ClangFormat applied successfully.")
    except subprocess.CalledProcessError as e:
        print("Error while running ClangFormat:")
        print(e.stderr.decode())
    except Exception as e:
        print(f"Unexpected error: {e}")

if __name__ == "__main__":
    format_c_h_files()