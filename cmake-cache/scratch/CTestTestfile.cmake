# CMake generated Testfile for 
# Source directory: /home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/scratch
# Build directory: /home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/cmake-cache/scratch
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ctest-scratch_meshwifi "ns3.41-meshwifi-default")
set_tests_properties(ctest-scratch_meshwifi PROPERTIES  WORKING_DIRECTORY "/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/build/scratch/" _BACKTRACE_TRIPLES "/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/build-support/custom-modules/ns3-executables.cmake;47;add_test;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/build-support/custom-modules/ns3-executables.cmake;122;set_runtime_outputdirectory;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/scratch/CMakeLists.txt;67;build_exec;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/scratch/CMakeLists.txt;79;create_scratch;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/scratch/CMakeLists.txt;0;")
add_test(ctest-scratch_scratch-simulator "ns3.41-scratch-simulator-default")
set_tests_properties(ctest-scratch_scratch-simulator PROPERTIES  WORKING_DIRECTORY "/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/build/scratch/" _BACKTRACE_TRIPLES "/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/build-support/custom-modules/ns3-executables.cmake;47;add_test;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/build-support/custom-modules/ns3-executables.cmake;122;set_runtime_outputdirectory;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/scratch/CMakeLists.txt;67;build_exec;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/scratch/CMakeLists.txt;79;create_scratch;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/scratch/CMakeLists.txt;0;")
add_test(ctest-scratch_subdir_scratch-subdir "ns3.41-scratch-subdir-default")
set_tests_properties(ctest-scratch_subdir_scratch-subdir PROPERTIES  WORKING_DIRECTORY "/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/build/scratch/subdir/" _BACKTRACE_TRIPLES "/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/build-support/custom-modules/ns3-executables.cmake;47;add_test;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/build-support/custom-modules/ns3-executables.cmake;122;set_runtime_outputdirectory;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/scratch/CMakeLists.txt;67;build_exec;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/scratch/CMakeLists.txt;109;create_scratch;/home/pham-dinh-chien/ns-allinone-3.41/ns-3.41/scratch/CMakeLists.txt;0;")
subdirs("nested-subdir")
