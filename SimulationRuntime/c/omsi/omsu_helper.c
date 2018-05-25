/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF THE BSD NEW LICENSE OR THE
 * GPL VERSION 3 LICENSE OR THE OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs: http://www.openmodelica.org or
 * http://www.ida.liu.se/projects/OpenModelica, and in the OpenModelica
 * distribution. GNU version 3 is obtained from:
 * http://www.gnu.org/copyleft/gpl.html. The New BSD License is obtained from:
 * http://www.opensource.org/licenses/BSD-3-Clause.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, EXCEPT AS
 * EXPRESSLY SET FORTH IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE
 * CONDITIONS OF OSMC-PL.
 *
 */

#include "omsu_helper.h"

void storePreValues (DATA *data) {
    //TODO: implement for new data structure
}

int stateSelection(DATA *data, threadData_t *threadData, char reportError, int switchStates) {
    //TODO: implement for new data structure
}

void overwriteOldSimulationData(DATA *data) {
    //TODO: implement for new data structure
}


/*
 * Helper function for read_input_xml.
 * Reads simulation variables from xml file.
 */
void read_variables_xml(void* out, omc_ModelInput in, var_type attributeKind, nVariables){
	int i,j;

	j = start;
	for (i=0; i<nVariables; i++) {
		VAR_INFO *info = &out[j].info;






		j++;
	}
}

/*
 * Reads input values from a xml file.
 */
void read_input_xml(omsi_t* osu_data, char* filename) {

    /* Variables */
    omc_ModelInput mi = {0};
    FILE* file = NULL;

    /* open the xml file */
    file = fopen(filename, "r");
    if(!file) {
        throwStreamPrint(NULL, "In file omsu_helper.c in function read_input_xml: Error: can not read file %s as setup file to the generated simulation code.", filename);
    }

    /* create the XML parser */
    parser = XML_ParserCreate(NULL);
    if(!parser) {
        fclose(file);
        throwStreamPrint(NULL, "In file omsu_helper.c in function read_input_xml: Error: couldn't allocate memory for the XML parser!");
    }
    /* set our user data */
    XML_SetUserData(parser, &mi);
    /* set the handlers for start/end of element. */
    XML_SetElementHandler(parser, startElement, endElement);

    // deal with override???

    /* read all experiment values */
    read_value_real(findHashStringString(mi.de,"startTime"), &(osu_data->experiment->start_time), 0);
    read_value_real(findHashStringString(mi.de,"stopTime"), &(osu_data->experiment->stop_time), osu_data->experiment->start_time+1);
    read_value_real(findHashStringString(mi.de,"stepSize"), &(osu_data->experiment->step_size), (simulationInfo->stopTime - simulationInfo->startTime) / 500);
    // ToDo: num_outputs missing in xml file?
    read_value_real(findHashStringString(mi.de,"tolerance"), &(osu_data->experiment->tolerance), 1e-5);
    read_value_string(findHashStringString(mi.de,"solver"), &(osu_data->experiment->solver_name));

    /* read all model values */
    read_value_string(findHashStringStringNull(mi.md,"guid"), &(osu_data->model_data->modelGUID));
    read_value_long(findHashStringString(mi.md,"numberOfContinuousStates"), &(osu_data->model_data->n_states), 0);
    read_value_long(findHashStringString(mi.md,"numberOfContinuousStates"), &(osu_data->model_data->n_derivatives), 0);
    read_value_long(findHashStringString(mi.md,"numberOfRealAlgebraicVariables"), &(osu_data->model_data->n_real_vars), 0);
    read_value_long(findHashStringString(mi.md,"numberOfIntegerAlgebraicVariables"), &(osu_data->model_data->n_int_vars), 0);
    read_value_long(findHashStringString(mi.md,"numberOfBooleanAlgebraicVariables"), &(osu_data->model_data->n_bool_vars), 0);
    read_value_long(findHashStringString(mi.md,"numberOfStringAlgebraicVariables"), &(osu_data->model_data->n_string_vars), 0);
    read_value_long(findHashStringString(mi.md,"numberOfRealParameters"), &(osu_data->model_data->n_real_parameters), 0);
    read_value_long(findHashStringString(mi.md,"numberOfIntegerParameters"), &(osu_data->model_data->n_int_parameters), 0);
    read_value_long(findHashStringString(mi.md,"numberOfBooleanParameters"), &(osu_data->model_data->n_bool_parameters), 0);
    read_value_long(findHashStringString(mi.md,"numberOfStringParameters"), &(osu_data->model_data->n_string_parameters), 0);
    // ToDo: read n_zerocrossings, model_vars_info_t, equation_info_t, algebraic_system_t, n_algebraic_system

    /* read all simulation values */


    XML_ParserFree(parser);
}

int initializeNonlinearSystems(DATA *data, threadData_t *threadData) {
    //TODO: implement for new data structure
}

int initializeLinearSystems(DATA *data, threadData_t *threadData) {
    //TODO: implement for new data structure
}

int initializeMixedSystems(DATA *data, threadData_t *threadData) {
    //TODO: implement for new data structure
}

void initializeStateSetJacobians(DATA *data, threadData_t *threadData) {
    //TODO: implement for new data structure
}

void setZCtol(double relativeTol) {
    //ToDO: implement
}

void copyStartValuestoInitValues(DATA *data) {
    //ToDo: implement for new data structure
}

int initialization(DATA *data, threadData_t *threadData, const char* pInitMethod, const char* pInitFile, double initTime) {
    //ToDo: implement for new data structure
}

void initSample(DATA* data, threadData_t *threadData, double startTime, double stopTime) {
    //ToDo: implement for new data structure
}

void initDelay(DATA* data, double startTime) {
    //ToDo: implement for new data structure
}

double getNextSampleTimeFMU(DATA *data) {
    //ToDo: implement for new data structure
}

void setAllVarsToStart(DATA *data) {
    //ToDo: implement for new data structure
}

void setAllParamsToStart(DATA *data) {
    //ToDo: implement for new data structure
}

int freeNonlinearSystems(DATA *data, threadData_t *threadData) {
    //ToDo: implement for new data structure
}

int freeMixedSystems(DATA *data, threadData_t *threadData) {
    //ToDo: implement for new data structure
}

int freeLinearSystems(DATA *data, threadData_t *threadData) {
    //ToDo: implement for new data structure
}

void freeStateSetData(DATA *data) {
    //ToDo: implement for new data structure
}

void deInitializeDataStruc(DATA *data) {
    //ToDo: implement for new data structure
}

void updateRelationsPre(DATA *data) {
    //ToDo: implement for new data structure
}

modelica_boolean checkRelations(DATA *data) {
    //ToDo: implement for new data structure
}

void mmc_catch_dummy_fn (void) {
    //ToDo: delete
}

void omsic_model_setup_data (osu_t* OSU) {
    //ToDo: delete
}
