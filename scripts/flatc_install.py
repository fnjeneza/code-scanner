#!/usr/bin/env python

import tempfile
import os
import sys
import shutil
import subprocess

temp = tempfile.mkdtemp()
current_dir = os.getcwd()
if len(sys.argv) < 2:
    print("Error: missing path to flatbuffers")
    sys.exit()

flatbuffers = sys.argv[1]
if not os.path.exists(flatbuffers):
    print("{} does not exist".format(flatbuffers))
    sys.exit()

flatbuffers = os.path.abspath(flatbuffers)

os.chdir(temp)
cmd = "cmake -G Ninja {} -DCMAKE_INSTALL_PREFIX={}".format(flatbuffers,
        current_dir)
exitcode = subprocess.call(cmd.split())

if exitcode != 0:
    print("Generation failed")

cmd = "ninja"
exitcode = subprocess.call(cmd.split())
if exitcode != 0:
    print("Compilation failed")

cmd = "ninja install"
exitcode = subprocess.call(cmd.split())
if exitcode != 0:
    print("Installation failed")

# remove the temp folder
shutil.rmtree(temp)
