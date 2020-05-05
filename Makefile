cli=tools/bin/arduino-cli
cli_opts=--config-file etc/arduino-cli.yaml
sketch_target=arduino:avr:uno
pyinstaller=pyinstaller
pyi_opts=--clean -y

git_state=$(shell (git status --porcelain | grep -q .) && echo "-dirty" || echo "")
git_tag_raw=$(shell git describe --tags)
ifeq ($(findstring v,$(git_tag_raw)),v)
git_tag=$(git_tag_raw)
else
git_tag=unknown
endif

$(shell echo "const char *version_string=\""$(git_tag)$(git_state)"\";" >arduino/ambu_libraries/ambu_common/version.h) 
$(shell echo "client_version=\""$(git_tag)$(git_state)"\"" >python_client/client_version.py)
$(shell echo "client_version=\""$(git_tag)$(git_state)"\"" >arduino_installer/client_version.py)
arduino_sketches= \
	ambu_control_superior \
	ambu_control_flow_cal \
	ambu_control_dual

arduino_libs=arduino/ambu_libraries

arduino_dirs=$(addprefix arduino/,$(arduino_sketches))
arduino_clean=$(addsuffix .clean,$(arduino_sketches))

path_list = $(subst $(eval) ,:,$(wildcard $1))
VPATH=$(call path_list,$(arduino_dirs))


default: arduino


define make-arduino-target
$1: $1.hex 
$1.clean:
	rm -f arduino/$1/*.{hex,elf}
endef

$(foreach element, $(arduino_sketches), $(eval $(call make-arduino-target,$(element))))

arduino: $(arduino_sketches)




%.hex: %.ino
	$(cli) $(cli_opts) -b $(sketch_target) --libraries $(arduino_libs)  compile $(shell dirname $<) -o  $(shell dirname $<)/$(shell basename $< .ino)


distro:
	pyinstaller $(pyi_opts) python_client/client_dual.spec
	@echo distributable excutable: dist/client_dual
	pyinstaller $(pyi_opts) arduino_installer/installer.spec
	@echo distributable excutable: dist/installer	

distro.clean:
	rm -rf build dist

clean: $(arduino_clean)


