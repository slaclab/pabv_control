#!/usr/bin/env python
import os
import sys
sys.path.insert(0,"python_client")
import git_version

os.execvp("python",['python','python_client/client.py'])

