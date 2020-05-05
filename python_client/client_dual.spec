# -*- mode: python ; coding: utf-8 -*-
import sys
sys.path.append("arduino_installer")
import client_version
sys.setrecursionlimit(30000)
block_cipher = None


a = Analysis(['client_dual.py'],
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
          Tree('arduino/data/',prefix='arduino/data/'),
          [],
          name='client_dual_'+client_version.client_version,
          debug=False,
          bootloader_ignore_signals=False,
          strip=False,
          upx=True,
          upx_exclude=[],
          runtime_tmpdir=None,
          console=True )
