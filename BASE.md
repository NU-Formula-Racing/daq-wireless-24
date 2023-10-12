# daq-firmware-base-24

This repository is the base repo, which daq projects should fork from as needed. The base is configured to work with ESP32-based projects, which may not be the case for your project.

## Important Notes
* For the **Logger, Wireless, and Dashboard Projects**, you will not be using the ESP32 enviornment configured here. Instead, you should go to ```platformio.ini``` and uncomment the teensy enviornment.
* In formula, we use a branching naming convention ```<initials>/<branch-name>```. So if I were to make a branch called ```analysis-backend```, it should be called ```ebs/analysis-backend```. Notice how we use kebab case, which is the standard in git related things.
* Try to resync your fork with this base as often as you can. There might be new resources that pop up here!

## Resources for Firmware
#### General
* [Arduino Framework Documentation](https://www.arduino.cc/reference/en/)
* [How to use Git and Github](https://product.hubspot.com/blog/git-and-github-tutorial-for-beginners)
* [How to write Good Pull Requests](https://developers.google.com/blockly/guides/contribute/get-started/write_a_good_pr)
* [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

#### Formula Specific
* [CAN Example](https://github.com/NU-Formula-Racing/CAN_Interface_Demo/blob/main/src/main.cpp)
* [Project Example (temp-board-23)](https://github.com/NU-Formula-Racing/daq-firmware-23/blob/jm/daqTemp/src/main.cpp)