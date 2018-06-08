/* Test omsu_Initialization.c
 *
 *  Use FMU-SDK
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
//#include "../fmi2/osi_fmi2_wrapper.h"
#include "../fmi2/fmi2Functions.h"

typedef struct {
//    ModelDescription* modelDescription;
//
//    HMODULE dllHandle; // fmu.dll handle
    /***************************************************
    Common Functions
    ****************************************************/
    fmi2GetTypesPlatformTYPE         *getTypesPlatform;
    fmi2GetVersionTYPE               *getVersion;
    fmi2SetDebugLoggingTYPE          *setDebugLogging;
    fmi2InstantiateTYPE              *instantiate;
    fmi2FreeInstanceTYPE             *freeInstance;
    fmi2SetupExperimentTYPE          *setupExperiment;
    fmi2EnterInitializationModeTYPE  *enterInitializationMode;
    fmi2ExitInitializationModeTYPE   *exitInitializationMode;
    fmi2TerminateTYPE                *terminate;
    fmi2ResetTYPE                    *reset;
    fmi2GetRealTYPE                  *getReal;
    fmi2GetIntegerTYPE               *getInteger;
    fmi2GetBooleanTYPE               *getBoolean;
    fmi2GetStringTYPE                *getString;
    fmi2SetRealTYPE                  *setReal;
    fmi2SetIntegerTYPE               *setInteger;
    fmi2SetBooleanTYPE               *setBoolean;
    fmi2SetStringTYPE                *setString;
    fmi2GetFMUstateTYPE              *getFMUstate;
    fmi2SetFMUstateTYPE              *setFMUstate;
    fmi2FreeFMUstateTYPE             *freeFMUstate;
    fmi2SerializedFMUstateSizeTYPE   *serializedFMUstateSize;
    fmi2SerializeFMUstateTYPE        *serializeFMUstate;
    fmi2DeSerializeFMUstateTYPE      *deSerializeFMUstate;
    fmi2GetDirectionalDerivativeTYPE *getDirectionalDerivative;
    /***************************************************
    Functions for FMI2 for Co-Simulation
    ****************************************************/
    fmi2SetRealInputDerivativesTYPE  *setRealInputDerivatives;
    fmi2GetRealOutputDerivativesTYPE *getRealOutputDerivatives;
    fmi2DoStepTYPE                   *doStep;
    fmi2CancelStepTYPE               *cancelStep;
    fmi2GetStatusTYPE                *getStatus;
    fmi2GetRealStatusTYPE            *getRealStatus;
    fmi2GetIntegerStatusTYPE         *getIntegerStatus;
    fmi2GetBooleanStatusTYPE         *getBooleanStatus;
    fmi2GetStringStatusTYPE          *getStringStatus;
    /***************************************************
    Functions for FMI2 for Model Exchange
    ****************************************************/
    fmi2EnterEventModeTYPE                *enterEventMode;
    fmi2NewDiscreteStatesTYPE             *newDiscreteStates;
    fmi2EnterContinuousTimeModeTYPE       *enterContinuousTimeMode;
    fmi2CompletedIntegratorStepTYPE       *completedIntegratorStep;
    fmi2SetTimeTYPE                       *setTime;
    fmi2SetContinuousStatesTYPE           *setContinuousStates;
    fmi2GetDerivativesTYPE                *getDerivatives;
    fmi2GetEventIndicatorsTYPE            *getEventIndicators;
    fmi2GetContinuousStatesTYPE           *getContinuousStates;
    fmi2GetNominalsOfContinuousStatesTYPE *getNominalsOfContinuousStates;
} FMU;

FMU fmu; // the fmu to simulate

static const char* fmi2StatusToString(fmi2Status status){
    switch (status){
        case fmi2OK:      return "ok";
        case fmi2Warning: return "warning";
        case fmi2Discard: return "discard";
        case fmi2Error:   return "error";
        case fmi2Fatal:   return "fatal";
    #ifdef FMI_COSIMULATION
        case fmi2Pending: return "fmi2Pending";
    #endif
        default:         return "?";
    }
}

#define MAX_MSG_SIZE 1000
void my_fmuLogger(void *componentEnvironment, fmi2String instanceName, fmi2Status status,
               fmi2String category, fmi2String message, ...) {
    char msg[MAX_MSG_SIZE];
//    char* copy;
    va_list argp;

    // replace C format strings
    va_start(argp, message);
    vsprintf(msg, message, argp);
    va_end(argp);

    // replace e.g. ## and #r12#
//    copy = strdup(msg);
//    replaceRefsInMessage(copy, msg, MAX_MSG_SIZE, &fmu);
//    free(copy);

    // print the final message
    if (!instanceName) instanceName = "?";
    if (!category) category = "?";
    printf("%s %s (%s): %s\n", fmi2StatusToString(status), instanceName, category, msg);
}


void test_1 () {

    fmi2Component c;

    fmi2CallbackFunctions callbacks = {my_fmuLogger, calloc, free, NULL, &fmu};
    const char *instanceName = "SimpleModelLinear_1";
    const char* guid = "{e9e50f74-bbe4-4c28-8bd2-9894ad8c8c54}";
    //char *fmuResourceLocation = "d:\\workspace\\OpenModelica\\OMCompiler\\SimulationRuntime\\c\\omsi\\testsuite";
    char *fmuResourceLocation = "/home/wbraun/workspace/OpenModelica/OMCompiler/SimulationRuntime/c/omsi/testsuite";
    fmi2Boolean visible = fmi2False;
    fmi2Boolean loggingOn = fmi2True;



    // import xml
    c = fmi2Instantiate(instanceName, fmi2ModelExchange, guid, fmuResourceLocation, &callbacks, visible, loggingOn);
    // print data struct

    // free data
    fmi2FreeInstance(c);

    printf("\nFinished test run!\n");
}


int main(int argc, char* argv[]) {

    test_1();



    return 0;
}
