#include <omsi.h>
#include <omsi_callbacks.h>
#include <omsi_global.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif


/* Prototypes */
extern omsi_int omsu_process_input_xml(omsi_t*                          osu_data,
                                       omsi_char*                       filename,
                                       omsi_string                      fmuGUID,
                                       omsi_string                      instanceName,
                                       const omsi_callback_functions*   functions);


#define MAX_MSG_SIZE 1000
void my_fmuLogger(const void *componentEnvironment, omsi_string instanceName, omsi_status status,
               omsi_string category, omsi_string message, ...) {
    omsi_char msg[MAX_MSG_SIZE];
    va_list argp;

    /* replace C format strings */
    va_start(argp, message);
    vsprintf(msg, message, argp);
    va_end(argp);


    if (!instanceName) instanceName = "?";
    if (!category) category = "?";
    printf("%s %s (%s): %s\n", "status", instanceName, category, msg);
}

omsi_int main(omsi_int argc, omsi_char* argv[]) {

    omsi_char fmuResourceLocation[FILENAME_MAX];
    if(!GetCurrentDir(fmuResourceLocation, sizeof(fmuResourceLocation))) {
        return -1;
    }
    fmuResourceLocation[sizeof(fmuResourceLocation) - 1] = '\0';

    omsi_t* omsi_data;
    omsi_char* initXMLFilename;
    omsi_string instanceName = "SimpleModelLinear_1";
    omsi_string guid = "{e9e50f74-bbe4-4c28-8bd2-9894ad8c8c54}";

    const omsi_callback_functions callbacks = {my_fmuLogger, calloc, free, NULL, NULL};
    omsi_bool visible = omsi_true;
    omsi_bool loggingOn = omsi_false;

    omsi_data = callbacks.allocateMemory(1, sizeof(omsi_t));
    if (!omsi_data) {
        printf("ERROR: No memory\n");
    }
    initXMLFilename = callbacks.allocateMemory(20 + strlen(instanceName) + strlen(fmuResourceLocation), sizeof(omsi_char));
    sprintf(initXMLFilename, "%s/%s_init.xml", fmuResourceLocation, instanceName);

    global_callback = &callbacks;

    omsu_process_input_xml(omsi_data, initXMLFilename, guid, instanceName, &callbacks);

    /* print results */
    omsu_print_omsi_t (omsi_data, "");

    printf("Finished XML test successfully!\n");
    return 0;
}
