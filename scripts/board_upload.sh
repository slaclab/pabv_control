if [ "$#" -ne 2 ]; then
    echo "usage: board_upload.sh PORT SKETCH_DIR"
    exit 1
fi
echo Uploading to port: $1
file=arduino/$2/$2.hex
echo Sketch           : $2
echo HEX file         : ${file}
exec tools/bin/arduino-cli --config-file etc/arduino-cli.yaml upload -b arduino:avr:uno  -p  $1 -i ${file} arduino/$2
