# -*- mode: python -*-

block_cipher = None


a = Analysis(['..\\Base.py', '..\\confCompute.py', '..\\resultsOutput.py'],
             pathex=['C:\\Users\\Overlord\\Desktop\\xray-tracing\\release'],
             binaries=None,
             datas=None,
             hiddenimports=[],
             hookspath=[],
             runtime_hooks=[],
             excludes=[],
             win_no_prefer_redirects=False,
             win_private_assemblies=False,
             cipher=block_cipher)
a.datas += [('gtkrc', r'C:\Python27\Lib\site-packages\gtk-2.0\runtime\share\themes\MS-Windows\gtk-2.0\gtkrc', 'DATA')]
a.binaries += [(r'lib\gtk-2.0\2.10.0\engines\libwimp.dll', r'C:\Python27\Lib\site-packages\gtk-2.0\runtime\lib\gtk-2.0\2.10.0\engines\libwimp.dll', 'BINARY') ]
             
pyz = PYZ(a.pure, a.zipped_data,
             cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          exclude_binaries=True,
          name='Ray-tracing',
          debug=False,
          strip=False,
          upx=True,
          console=True )
coll = COLLECT(exe,
               a.binaries,
               a.zipfiles,
               a.datas,
               strip=False,
               upx=True,
               name='Ray-tracing')
