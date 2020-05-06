# Installation
### Getting the Code and dependencies
1. Install Anaconda and open a Anaconda Prompt
2. `conda install -y -c conda-forge git pyinstaller make pyserial`
3. `git clone https://github.com/slaclab/pabv_control`
4. `cd pabv_control`
##### Downloading arduin-cli
5. `bash ./bootstrap.sh`
##### gnumake commands should be run in a bash shell
6.  `bash`
##### on some Anaconda3 installations it might be necessary to fix the PATH for bash once

    First check if you Anaconda3 installation needs to be fixed by running in bash
   
    `which git`
   
If git is found the next step can be skipped

7.  `echo 'export PATH=$CONDA_PREFIX/Library/bin:$PATH' >> ~/.bashrc`

    After modifying .bashrc, exit bash ny typing exit, enter bash again
      
##### Building arduino targets
8. `gnumake`
##### Building client stand-alone executable
9.  `gnumake distro`
##### Building the arduino software installer
10.  `gnumake installer`

### Test platforms
1.    Windows 10 64-bit
2.    Windows 10 32-bit
3.    Linux 64-bit

- Excutables built on Windows 10 32-bit may run on Windows 10 64-bit. Windows 7 or 8 support would require a native build.

### Wrapper scripts for arduino CLI

##### Show connected boards
- `scripts/board_list.sh`

Example output for an UNO

`Port Type              Board Name  FQBN            Core`

`COM3 Serial Port (USB) Arduino Uno arduino:avr:uno arduino:avr`

##### Upload compiled sketch
- `scripts/board_upload.sh COM3 arduino/ambu_control_dual`

Example output:

`Uploading to port: COM3`

`Sketch : arduino/ambu_control_dual`

`HEX file : arduino/ambu_control_dual/ambu_control_dual.hex`

### Getting Running
- In Anaconda Prompt  
  - `cd pabv_control/python_client`  
  - `python client_dual.py`  

# Arduino Wireing
test

# Useage
