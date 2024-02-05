#!/usr/bin/env python3

import subprocess
import os
import sys 
import argparse

   
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--target-os', help='Target os')
    parser.add_argument('--target-arch', help='Target architecture')
    parser.add_argument('--output', help='Where the result binary should be moved')
    parser.add_argument('--input', help='The build folder')
    parser.add_argument('--result', help="The name of the produced binary")
    options = parser.parse_args()

    with open(f'{options.output}/log.txt', 'w') as out:
        os.chdir(options.input)
        build_command = f"GOOS={options.target_os} GOARCH={options.target_arch} GOFLAGS=-buildvcs=false go build -o {options.result} -v"
        subprocess.call(build_command, stdout=out, stderr=out, shell=True)
        subprocess.call(["mv", options.result, options.output])


if __name__ == "__main__":
    sys.exit(main())

