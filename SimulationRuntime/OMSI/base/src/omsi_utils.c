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

#include <omsi_utils.h>

#define DEBUG_FILTER_FLUSH omsi_true
#define DEBUG_FLUSH if (DEBUG_FILTER_FLUSH) fflush(stdout);


#define OVERFLOW_PROTECTED omsi_false

/* Filter function for logger */
void filtered_base_logger(omsi_bool*            logCategories,      /* Array of categories, that should be logged, can be NULL */
                          log_categories        category,           /* Category of this log call */
                          omsi_status           status,             /* Status for logger */
                          omsi_string           message,            /* Message for logger */
                          ...) {                                    /* Optional arguments in message */

    va_list args;
    va_start(args, message);

#if OVERFLOW_PROTECTED
    omsi_int size;
    omsi_char* buffer;
#else
    omsi_char buffer[BUFSIZ];
#endif

#if OVERFLOW_PROTECTED
    size = vsnprintf(NULL, 0, message, args);
    buffer = (omsi_char*) global_callback->allocateMemory(size+1, sizeof(omsi_char));
#endif

    vsprintf(buffer, message, args);

    if(isCategoryLogged(logCategories, category)) {
        global_callback->logger(global_callback->componentEnvironment,
                                global_instance_name,
                                status,
                                log_categories_names[category],
                                buffer);
        DEBUG_FLUSH
    }

    /* free stuff */
#if OVERFLOW_PROTECTED
    global_callback->freeMemory(buffer);
#endif
    va_end(args);

}


/*
 * Returns true, if categoryIndex should be logged or OSU was not set.
 */
omsi_bool isCategoryLogged(omsi_bool*       logCategories,
                           log_categories   categoryIndex) {

    if (logCategories==NULL) {
        return omsi_true;
    }

    if (categoryIndex < NUMBER_OF_CATEGORIES && (logCategories[categoryIndex] || logCategories[log_all])) {
        return omsi_true;
    }
    return omsi_false;
}

/*
 * ============================================================================
 * Section for allocating and deallocating stuff
 * ============================================================================
 */

/*
 * frees memory for omsi_t struct and all its components
 */
void omsu_free_osu_data(omsi_t* omsi_data) {

    if (omsi_data==NULL) {
        return;
    }

    /* free memory for model data */
    omsu_free_model_data(omsi_data->model_data);

    /* free memory for simulation data */
    omsu_free_sim_data(omsi_data->sim_data);

    /* free memory for experiment data */
    if (omsi_data->experiment != NULL) {
        global_callback->freeMemory((omsi_char *)omsi_data->experiment->solver_name);        /* type-cast to shut of warning when compiling */
        global_callback->freeMemory(omsi_data->experiment);
    }

}


/*
 * Frees memory for model_data_t structure and all its components.
 */
void omsu_free_model_data (model_data_t* model_data) {

    /* Variables */
    omsi_unsigned_int i, j;
    omsi_unsigned_int size;
    real_var_attribute_t* attribute;

    if (model_data==NULL) {
        return;
    }

    global_callback->freeMemory((omsi_char *)model_data->modelGUID);

    size = model_data->n_states + model_data->n_derivatives
         + model_data->n_real_vars+model_data->n_real_parameters
         + model_data->n_real_aliases;
    for (i=0, j=0; i<size; i++, j++) {
        global_callback->freeMemory ((omsi_char *)model_data->model_vars_info[j].name);
        global_callback->freeMemory ((omsi_char *)model_data->model_vars_info[j].comment);
        attribute = model_data->model_vars_info[j].modelica_attributes;
        global_callback->freeMemory ((omsi_char *)attribute->unit);
        global_callback->freeMemory ((omsi_char *)attribute->displayUnit);
        global_callback->freeMemory (attribute);
        /*global_callback->freeMemory(model_data.model_vars_info[j].info.filename);     // ToDo: something is wrong here */
    }
    size += model_data->n_int_vars + model_data->n_int_parameters + model_data->n_int_aliases
            + model_data->n_bool_vars + model_data->n_bool_parameters + model_data->n_bool_aliases
            + model_data->n_string_vars + model_data->n_string_parameters + model_data->n_string_aliases;
    for (i=j; i<size; i++, j++) {
        global_callback->freeMemory ((omsi_char *)model_data->model_vars_info[j].name);
        global_callback->freeMemory ((omsi_char *)model_data->model_vars_info[j].comment);
        global_callback->freeMemory (model_data->model_vars_info[j].modelica_attributes);
        /*global_callback->freeMemory(model_data.model_vars_info[j].info.filename);     // ToDo: something is wrong here */
    }
    global_callback->freeMemory (model_data->model_vars_info);

    /*for (i=0; i<omsi_data->model_data.n_equations; i++) {
            global_callback->freeMemory (omsi_data->model_data.equation_info[i].variables);
        }
        //global_callback->freeMemory (omsi_data->model_data.equation_info);     // ToDo: something's wrong here
    */

    global_callback->freeMemory (model_data);
}


/*
 * Frees memory for sim_data_t structure and all its components.
 */
void omsu_free_sim_data (sim_data_t* sim_data) {

    if (sim_data==NULL) {
        return;
    }

    omsu_free_omsi_function (sim_data->initialization);
    omsu_free_omsi_function (sim_data->simulation);

    omsu_free_omsi_values(sim_data->model_vars_and_params);
    omsu_free_omsi_values(sim_data->pre_vars);

    /* ToDo: free pre_vars_mapping */

    global_callback->freeMemory(sim_data->zerocrossings_vars);      /* ToDo: free inner stuff */
    global_callback->freeMemory(sim_data->pre_zerocrossings_vars);

    global_callback->freeMemory(sim_data);
}


/*
 * frees memory for omsi_function struct and all its components
 */
void omsu_free_omsi_function(omsi_function_t* omsi_function) {

    if (omsi_function==NULL) {
        return;
    }

    global_callback->freeMemory(omsi_function);
}


/*
 * frees memory for omsi_values struct and all its components
 */
void omsu_free_omsi_values(omsi_values* values) {

    if (values==NULL) {
        return;
    }

    global_callback->freeMemory(values->reals);
    global_callback->freeMemory(values->ints);
    global_callback->freeMemory(values->bools);

    global_callback->freeMemory(values);
}

/*
 * Returns true if vr is out of range of end and emits error message.
 * Otherwise it returns false.
 */
omsi_bool omsi_vr_out_of_range(omsi_t*               omsu,
                               omsi_string          function_name,
                               omsi_unsigned_int    vr,
                               omsi_int             end) {

    if ((omsi_int)vr >= end) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
            "%s: Illegal value reference %u.", function_name, vr);

         return omsi_true;
    }
    return omsi_false;
}
