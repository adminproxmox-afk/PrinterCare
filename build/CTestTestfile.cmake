# CMake generated Testfile for 
# Source directory: E:/app/PrinterCare
# Build directory: E:/app/PrinterCare/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[PrinterCareSmoke]=] "powershell" "-ExecutionPolicy" "Bypass" "-File" "E:/app/PrinterCare/scripts/run-dev.ps1" "-Mode" "smoke" "-BuildDir" "E:/app/PrinterCare/build")
  set_tests_properties([=[PrinterCareSmoke]=] PROPERTIES  _BACKTRACE_TRIPLES "E:/app/PrinterCare/CMakeLists.txt;102;add_test;E:/app/PrinterCare/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test([=[PrinterCareSmoke]=] "powershell" "-ExecutionPolicy" "Bypass" "-File" "E:/app/PrinterCare/scripts/run-dev.ps1" "-Mode" "smoke" "-BuildDir" "E:/app/PrinterCare/build")
  set_tests_properties([=[PrinterCareSmoke]=] PROPERTIES  _BACKTRACE_TRIPLES "E:/app/PrinterCare/CMakeLists.txt;102;add_test;E:/app/PrinterCare/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test([=[PrinterCareSmoke]=] "powershell" "-ExecutionPolicy" "Bypass" "-File" "E:/app/PrinterCare/scripts/run-dev.ps1" "-Mode" "smoke" "-BuildDir" "E:/app/PrinterCare/build")
  set_tests_properties([=[PrinterCareSmoke]=] PROPERTIES  _BACKTRACE_TRIPLES "E:/app/PrinterCare/CMakeLists.txt;102;add_test;E:/app/PrinterCare/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test([=[PrinterCareSmoke]=] "powershell" "-ExecutionPolicy" "Bypass" "-File" "E:/app/PrinterCare/scripts/run-dev.ps1" "-Mode" "smoke" "-BuildDir" "E:/app/PrinterCare/build")
  set_tests_properties([=[PrinterCareSmoke]=] PROPERTIES  _BACKTRACE_TRIPLES "E:/app/PrinterCare/CMakeLists.txt;102;add_test;E:/app/PrinterCare/CMakeLists.txt;0;")
else()
  add_test([=[PrinterCareSmoke]=] NOT_AVAILABLE)
endif()
