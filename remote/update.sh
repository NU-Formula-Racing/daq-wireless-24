cd .pio/libdeps/teensy40
echo "Updating Teensy 4.0 libraries"
echo "Updating daqser"
cd daq-serializer-24
git stash
git pull -f
cd ..
echo "Updating wircom"
cd daq-wireless-com
git stash
git pull -f