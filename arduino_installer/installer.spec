# -*- mode: python ; coding: utf-8 -*-
import os
extra_data=list()
for root, dirs, files in os.walk("../arduino"):
    for file in files:
        filename=os.path.join(root, file)
        dirname=os.path.relpath(os.path.dirname(filename),"..")+'/'
        extra_data.append((filename,dirname))

extra_data.append(("../etc/arduino-cli.yaml","etc/"))
extra_data.append(("../tools/bin/*","tools/bin/"))
extra_data.append(("../ambu_control_dual/*","ambu_control_dual/"))
toc=Tree('arduino/data/',prefix='arduino/data/')
block_cipher = None


a = Analysis(['installer.py'],
             pathex=['.'],
             binaries=[],
             datas=extra_data,
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
          toc,
          [],
          name='installer',
          debug=False,
          bootloader_ignore_signals=False,
          strip=False,
          upx=True,
          upx_exclude=[],
          runtime_tmpdir=None,
          console=True )
