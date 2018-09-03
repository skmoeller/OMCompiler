/* Test omsu_Initialization.c
 *
 *  Use FMU-SDK
 *
 */

#include <omsi.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
/*#include <osi_fmi2_wrapper.h>*/
#include <fmi2Functions.h>

#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif

typedef struct {
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

FMU fmu; /* the fmu to simulate */

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
    va_list argp;

    /* replace C format strings */
    va_start(argp, message);
    vsprintf(msg, message, argp);
    va_end(argp);

    /* print the final message */
    if (!instanceName) instanceName = "?";
    if (!category) category = "?";
    printf("%s %s (%s): %s\n", fmi2StatusToString(status), instanceName, category, msg);
}


void test_instantiate (fmi2String   instanceName,
                       fmi2String   guid) {

    fmi2Component componente;
    fmi2CallbackFunctions callbacks = {my_fmuLogger, calloc, free, NULL, &fmu};
    fmi2Boolean visible = fmi2False;
    fmi2Boolean loggingOn = fmi2True;
    fmi2Char fmuResourceLocation[FILENAME_MAX];

    if(!GetCurrentDir(fmuResourceLocation, sizeof(fmuResourceLocation))) {
        return;
    }
    fmuResourceLocation[sizeof(fmuResourceLocation) - 1] = '\0';

    /* instantiate FMU */
    printf("Enter OMSI instantiate\n");
    fflush(stdout);
    componente = omsi_instantiate(instanceName, fmi2ModelExchange, guid, fmuResourceLocation, &callbacks, visible, loggingOn);
    if(!componente) {
        printf("FMI instantiation failed\n");
        fflush(stdout);
    }

    /* free data */
    printf("Free OMSI instance\n");
    fflush(stdout);
    omsi_free_instance(componente);

    printf("\nFinished test run!\n");
}


/*
 * Input: instanceName
 * Input: GUID
 *
 */
int main(int argc, char* argv[]) {

    /* test_instantiate(argv[1], argv[2]); */

    fmi2String instanceName = "SimpleModelLinear_1";
    fmi2String guid = "{e9e50f74-bbe4-4c28-8bd2-9894ad8c8c54}";

    test_instantiate(instanceName, guid);



    return 0;
}
