
First set the correct OpenModelica path in OMSIC/makefile:
  PATH_TO_OPENMODELICA = /path/to/OpenModelica

Needed libraries:
  expat library (on Windows: sould be in path with OMDev)
                 on Unix: make it accessible from path or add it manualy in
                          OMSIC/src/omsu/makefile in rule libomsu)

To build and test the omsu library run:
  $ make all


To build OpenModelica Simulation Unit library do:
  $ make -C src/omsu libomsu

To test the omsu library do:
  $ make -C testsuite test

If you want to test different models add modelDescription.xml,<modelName>_init.xml
and <modelName>_info.json to folder testsuite. Get the GUID from modelDescription.xml
and write it manualy in testsuite/makefile
  # set FMI GUID
  # if other XML or JSON are tested you have to manualy set the GUID here to
  # matching GUID from modelDescription.xml
  TEST_GUID = {e9e50f74-bbe4-4c28-8bd2-9894ad8c8c54}
