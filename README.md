# Installation
### Getting the Code and dependencies
1. Install Anaconda3 (64-bit) for Windows 10 from here:
   https://www.anaconda.com/products/individual
and open a Anaconda Prompt
2. `conda install -y -c conda-forge git pyinstaller pyserial pyqt`
3. `git clone https://github.com/slaclab/pabv_control`
4. `cd pabv_control`  
5. We now use submodules for various libraries, run this to pull them  
   `git submodule update --init --recursive`
##### Downloading arduin-cli
5. `bootstrap.py`
##### Compiling all sketches
6. `board_compile.py`

### Tested platforms
1.    Windows 10 64-bit
2.    Windows 10 32-bit
3.    Linux 64-bit
4.    MacOSX 64-bit

- Excutables built on Windows 10 32-bit may run on Windows 10 64-bit. Windows 7 or 8 support would require a native build.

### Packing the python client for distribution
- simply run the packaging scripts
- `pkg_gui.py`

### Wrapper scripts for arduino CLI

##### Show connected boards
- `board_list.py`
- this will create a standalone executable that can be copied to a USB stick

= example filename: `dist\client_py_Windows_64bit_v0.0.2-50-g3790d7b.exe`

Example output for an UNO

`Port Type              Board Name  FQBN            Core`

`COM3 Serial Port (USB) Arduino Uno arduino:avr:uno arduino:avr`

##### Upload compiled sketch
- `board_upload.py COM3 ambu_control_flow_cal`

Example output:

`Uploading sketch ambu_control_flow_cal to port COM3`

### Getting Running
- In Anaconda Prompt  
  - `cd pabv_control/python_client`  
  - `python client.py`  

# Arduino Wireing
test

# Useage
