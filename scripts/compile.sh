CLI=tools/bin/arduino-cli
CLI_OPTS="--config-file etc/arduino-cli.yaml"
board=arduino:avr:uno
arduino_libs=arduino/libraries
git_state=$((${CONDA_PREFIX}/Library/bin/git status --porcelain | /usr/bin/grep -q . ) && echo "-dirty" || echo "")

git_tag=${git_tag_raw}
echo "const char *version_string=\""${git_tag}${git_state}"\";" >arduino/libraries/ambu_common/version.h
echo "client_version=\""$Pgit_tag${Pgit_state}"\"" >python_client/client_version.py
echo "client_version=\""${git_tag}${git_state}"\"" >arduino_installer/client_version.py
arduino_sketches="ambu_control_superior ambu_control_flow_cal ambu_control_dual"

for x in ${arduino_sketches}
do
${CLI} $CLI_OPTS --libraries ${arduino_libs}  compile  -b ${board} -o arduino/${x}/${x}.hex arduino/${x}
done
