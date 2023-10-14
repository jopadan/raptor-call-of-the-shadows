#!/usr/bin/python

from subprocess import check_output as run
from subprocess import call
from sys import argv
import re

exes = argv[1:]
for exe in exes:
    print "Fixing %s" %exe
    out = run(["otool", "-L", exe]).split("\n")[1:]
    changes = []
    for lib in out:
        if not lib:
            continue
        lib = lib.split()[0]
        if not lib.startswith("/usr/local/opt/"):
            continue
        name = lib.split("/")[-1]
        changes += ["-change", lib, "@executable_path/../Frameworks/" + name]

    if changes:
        changes = ["install_name_tool"] + changes + [exe]
        call(changes)
