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
    def is_within_directory(directory, target):
        
        abs_directory = os.path.abspath(directory)
        abs_target = os.path.abspath(target)
    
        prefix = os.path.commonprefix([abs_directory, abs_target])
        
        return prefix == abs_directory
    
    def safe_extract(tar, path=".", members=None, *, numeric_owner=False):
    
        for member in tar.getmembers():
            member_path = os.path.join(path, member.name)
            if not is_within_directory(path, member_path):
                raise Exception("Attempted Path Traversal in Tar File")
    
        tar.extractall(path, members, numeric_owner=numeric_owner) 
        
    
    safe_extract(xz, ".")

os.rename(folder_name, "clang+llvm")
