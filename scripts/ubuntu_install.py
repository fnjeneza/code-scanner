#!/usr/bin/env python

import urllib.request
import tarfile
import os

print("*"*80)
print(
        "This script will download clang+llvm from the clang's official site.\n"
        "The download may take a while"
        )
print("*"*80)

folder_name = "clang+llvm-5.0.0-linux-x86_64-ubuntu16.04"
url = "http://releases.llvm.org/5.0.0/{}.tar.xz".format(folder_name)
filename, _ = urllib.request.urlretrieve(url)

with tarfile.open(filename) as xz:
    xz.extractall('.')

os.rename(folder_name, "clang+llvm")
