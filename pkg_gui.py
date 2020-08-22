#!/usr/bin/env python

# -*- coding: utf-8 -*-
import re
import sys
sys.path.insert(0,"python_client")
import git_version

from PyInstaller.__main__ import run

import os
pyi_opts="--clean -y --onedir"

workdir = os.getcwd()
spec = os.path.join(workdir, 'python_client/client.spec')

devdir = os.getcwd()
distdir = os.path.join(devdir, 'dist')
builddir = os.path.join(devdir, 'build')

# call pyinstaller directly
args=['--distpath', distdir, '--workpath', builddir, spec]
args=args+pyi_opts.split(" ")
sys.argv+=args

if __name__ == '__main__':
    sys.argv[0] = re.sub(r'(-script\.pyw?|\.exe)?$', '', sys.argv[0])
    run()
