# -*- mode: python ; coding: utf-8 -*-
import sys
import platform
import subprocess
git_tag = subprocess.check_output(["git", "describe","--tags"]).strip()
git_status=subprocess.check_output(["git", "status","--porcelain"]).strip()

if(len(git_status)!=0): git_status="-dirty"
else: git_status=""

git_tag=str("%s%s"%(repr(git_tag)[2:-1],git_status))
print('######## Tag:',git_tag)
#import client_version
target=platform.system()+"_"+platform.architecture()[0]
sys.setrecursionlimit(30000)
block_cipher = None


a = Analysis(['client.py'],
             pathex=['python_client'],
             binaries=[],
             datas=[],
             hiddenimports=['pkg_resources.py2_warn'],
             hookspath=[],
             runtime_hooks=[],
             excludes=[],
             win_no_prefer_redirects=False,
             win_private_assemblies=False,
             cipher=block_cipher,
             noarchive=False)
pyz = PYZ(a.pure, a.zipped_data,
             cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          a.binaries,
          a.zipfiles,
          a.datas,
           [],
          name="client_%s_%s" %(target,git_tag),
          debug=False,
          bootloader_ignore_signals=False,
          strip=False,
          upx=True,
          upx_exclude=[],
          runtime_tmpdir=None,
          console=True )
