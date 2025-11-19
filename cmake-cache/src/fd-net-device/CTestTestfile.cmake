# CMake generated Testfile for 
# Source directory: /home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/src/fd-net-device
# Build directory: /home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/cmake-cache/src/fd-net-device
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ctest-raw-sock-creator "ns3.41-raw-sock-creator-default")
set_tests_properties(ctest-raw-sock-creator PROPERTIES  WORKING_DIRECTORY "/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/build/src/fd-net-device/" _BACKTRACE_TRIPLES "/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/build-support/custom-modules/ns3-executables.cmake;47;add_test;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/build-support/custom-modules/ns3-executables.cmake;122;set_runtime_outputdirectory;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/src/fd-net-device/CMakeLists.txt;156;build_exec;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/src/fd-net-device/CMakeLists.txt;0;")
add_test(ctest-tap-device-creator "ns3.41-tap-device-creator-default")
set_tests_properties(ctest-tap-device-creator PROPERTIES  WORKING_DIRECTORY "/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/build/src/fd-net-device/" _BACKTRACE_TRIPLES "/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/build-support/custom-modules/ns3-executables.cmake;47;add_test;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/build-support/custom-modules/ns3-executables.cmake;122;set_runtime_outputdirectory;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/src/fd-net-device/CMakeLists.txt;194;build_exec;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/src/fd-net-device/CMakeLists.txt;0;")
subdirs("examples")
