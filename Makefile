cli=arduino-cli
libs=ambu_libraries
sketch_target=arduino:avr:uno
sketches=\
	ambu_control_superior \
	ambu_control_flow_cal \
	ambu_control_dual

targets=$(foreach t,$(sketches),$(t)/$(t).hex)



%.hex:
	$(cli) -b $(sketch_target) --libraries $(libs)  compile $(shell dirname $@)

all: 	$(targets)
	@echo $(targets) 

