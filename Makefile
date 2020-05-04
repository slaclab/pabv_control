cli=tools/bin/arduino-cli
cli_opts=--config-file etc/arduino-cli.yaml
libs=ambu_libraries
sketch_target=arduino:avr:uno
sketches=\
	ambu_control_superior \
	ambu_control_flow_cal \
	ambu_control_dual

targets=$(foreach t,$(sketches),$(t)/$(t).hex)


default: all

%.hex:
	$(cli) $(cli_opts) -b $(sketch_target) --libraries $(libs)  compile $(shell dirname $@)

distro:
	pyinstaller -y  python_client/client_dual.spec
	@echo distributable excutable: dist/client_dual/client_dual
clean.distro:
	rm -rf build dist

all: 	$(targets)
