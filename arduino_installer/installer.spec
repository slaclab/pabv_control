# -*- mode: python ; coding: utf-8 -*-
import sys
import platform
sys.path.append("python_client")
import client_version
target=platform.system()+"_"+platform.architecture()[0]
sys.setrecursionlimit(30000)
block_cipher = None


a = Analysis(['installer.py'],
             pathex=['.'],
             binaries=[],
             datas=[
                ("../etc/arduino-cli.yaml","etc/"),
                ("../tools/bin/*","tools/bin/"),
                ("../arduino/ambu_control_dual/*","ambu_control_dual/")
             ],
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
          name='installer_'+target+"_"+client_version.client_version,
          debug=False,
          bootloader_ignore_signals=False,
          strip=False,
          upx=True,
          upx_exclude=[],
          runtime_tmpdir=None,
          console=True )
