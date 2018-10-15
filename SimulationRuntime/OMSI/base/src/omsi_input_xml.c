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
#include <omsi_input_xml.h>


#define UNUSED(x) (void)(x)     /* ToDo: delete later */



/*
 * Reads input values from a xml file and allocates memory for osu_data struct.
 * Entry point for all other functions in this file
 */
omsi_status omsu_process_input_xml(omsi_t*                         osu_data,
                                   omsi_string                     filename,
                                   omsi_string                     fmuGUID,
                                   omsi_string                     instanceName,
                                   const omsi_callback_functions*  functions) {

    /* Variables */
    omsi_int done;
    omsi_int n_model_vars_and_params;
    omsi_string guid;
    omsi_char buf[BUFSIZ] = {0};
    omsi_status status;

    omc_ModelInput mi = {0};
    FILE* file = NULL;
    XML_Parser parser = NULL;

    status = omsi_ok;

    /* set global function pointer */
    global_callback = (omsi_callback_functions*) functions;
    global_instance_name = instanceName;

    /* Log function call */
    LOG_FILTER(global_callback->componentEnvironment, LOG_ALL,
        functions->logger(functions->componentEnvironment, instanceName, omsi_ok, logCategoriesNames[LOG_ALL],
        "fmi2Instantiate: Process XML file %s.", filename))

    /* open xml file */
    file = fopen(filename, "r");
    if(!file) {
        LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error,
            logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: Can not read input file %s.", filename))
        return omsi_error;
    }

    /* create the XML parser */
    parser = XML_ParserCreate("UTF-8");
    if(!parser) {
        fclose(file);
        LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error,
            logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: Out of memory."))
        return omsi_error;
    }
    /* set our user data */
    XML_SetUserData(parser, &mi);
    /* set the handlers for start/end of element. */
    XML_SetElementHandler(parser, startElement, endElement);

    /* read XML */
    do {
        omsi_unsigned_int len = fread(buf, 1, sizeof(buf), file);
        done = len < sizeof(buf);
        if(XML_STATUS_ERROR == XML_Parse(parser, buf, len, done)) {
            fclose(file);
            LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSERROR,
                functions->logger(functions->componentEnvironment, instanceName, omsi_error, logCategoriesNames[LOG_STATUSERROR],
                "fmi2Instantiate: failed to read the XML file %s: %s at line %lu.", filename,
                XML_ErrorString(XML_GetErrorCode(parser)), XML_GetCurrentLineNumber(parser)))
            XML_ParserFree(parser);
            return omsi_error;
        }
    } while(!done);

    fclose(file);
    XML_ParserFree(parser);

    /* check model GUID */
    guid = omsu_findHashStringStringNull(mi.md,"guid");
    if (NULL==guid) {
        LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, logCategoriesNames[LOG_STATUSERROR],
            "fmi2Instantiate: Model GUID %s is not set in model description %s.",
            fmuGUID, filename))
        return omsi_error;
    }
    else if (strcmp(fmuGUID, guid)) {
        LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSWARNING,
            functions->logger(functions->componentEnvironment, instanceName, omsi_warning, logCategoriesNames[LOG_STATUSWARNING],
            "fmi2Instantiate: Wrong GUID %s in file %s. Expected %s.",
            guid, filename, fmuGUID))
        status = omsi_warning;
    }

    /* process experiment data */
    osu_data->experiment = functions->allocateMemory(1, sizeof(omsi_experiment_t));
    if (!osu_data->experiment) {
        LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, logCategoriesNames[LOG_STATUSERROR],
            "fmi2Instantiate: Not enough memory to allocate osu_data->experiment."))
        return omsi_error;
    }

    /* read osu_data */
    omsu_read_value_real(omsu_findHashStringString(mi.de,"startTime"), &(osu_data->experiment->start_time), 0);
    omsu_read_value_real(omsu_findHashStringString(mi.de,"stopTime"), &(osu_data->experiment->stop_time), osu_data->experiment->start_time+1);
    omsu_read_value_real(omsu_findHashStringString(mi.de,"stepSize"), &(osu_data->experiment->step_size), (osu_data->experiment->stop_time - osu_data->experiment->start_time) / 500);
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfOutputVariables"), &(osu_data->experiment->num_outputs));
    omsu_read_value_real(omsu_findHashStringString(mi.de,"tolerance"), &(osu_data->experiment->tolerance), 1e-5);
    omsu_read_value_string(omsu_findHashStringString(mi.de,"solver"), (omsi_char**) &(osu_data->experiment->solver_name));

    /* process all model data */
    osu_data->model_data = functions->allocateMemory(1, sizeof(model_data_t));
    if (!osu_data->model_data) {
        LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, logCategoriesNames[LOG_STATUSERROR],
            "fmi2Instantiate: Not enough memory to allocate osu_data->model_data."))
        return omsi_error;
    }
    omsu_read_value_string(omsu_findHashStringStringNull(mi.md,"guid"), (omsi_char**) &(osu_data->model_data->modelGUID));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfContinuousStates"), &(osu_data->model_data->n_states));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfContinuousStates"), &(osu_data->model_data->n_derivatives));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfRealAlgebraicVariables"), &(osu_data->model_data->n_real_vars));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfIntegerAlgebraicVariables"), &(osu_data->model_data->n_int_vars));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfBooleanAlgebraicVariables"), &(osu_data->model_data->n_bool_vars));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfStringAlgebraicVariables"), &(osu_data->model_data->n_string_vars));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfRealParameters"), &(osu_data->model_data->n_real_parameters));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfIntegerParameters"), &(osu_data->model_data->n_int_parameters));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfBooleanParameters"), &(osu_data->model_data->n_bool_parameters));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfStringParameters"), &(osu_data->model_data->n_string_parameters));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfRealAlgebraicAliasVariables"), &(osu_data->model_data->n_real_aliases));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfIntegerAliasVariables"), &(osu_data->model_data->n_int_aliases));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfBooleanAliasVariables"), &(osu_data->model_data->n_bool_aliases));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfStringAliasVariables"), &(osu_data->model_data->n_string_aliases));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfEventIndicators"), &(osu_data->model_data->n_zerocrossings));       /* ToDo: Is numberOfTimeEvents also part of n_zerocrossings???? */
    osu_data->model_data->n_equations = -1; /* ToDo: numberOfEquations is not in XML */

    /* read model_vars_info */
    n_model_vars_and_params = osu_data->model_data->n_states + osu_data->model_data->n_derivatives
                            + osu_data->model_data->n_real_vars + osu_data->model_data->n_int_vars
                            + osu_data->model_data->n_bool_vars + osu_data->model_data->n_string_vars
                            + osu_data->model_data->n_real_parameters + osu_data->model_data->n_int_parameters
                            + osu_data->model_data->n_bool_parameters + osu_data->model_data->n_string_parameters
                            + osu_data->model_data->n_real_aliases + osu_data->model_data->n_int_aliases
                            + osu_data->model_data->n_bool_aliases + osu_data->model_data->n_string_aliases;

    osu_data->model_data->model_vars_info = (model_variable_info_t*) functions->allocateMemory(n_model_vars_and_params, sizeof(model_variable_info_t));
    if (!osu_data->model_data->model_vars_info) {
        LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSERROR,
            functions->logger(functions->componentEnvironment, instanceName, omsi_error, logCategoriesNames[LOG_STATUSERROR],
            "fmi2Instantiate: Not enough memory to allocate osu_data->model_data->model_vars_info."))
        return omsi_error;

    }

    /*read model_vars_info inner stuff */
    omsu_read_var_infos(osu_data->model_data, &mi);

    /* now all data from init_xml should be utilized */
    /* omsu_free_ModelInput(mi); */
    /*
     * ToDo: We get segmentation faults. Is it possible that the expat macros are not unique and
     * crazy stuff happens???
     */

    return status;
}


/*
 * Compute corresponding index for alias variable.
 */
omsi_int omsu_find_alias_index(omsi_int alias_valueReference,
                               omsi_int n_variables) {
    /*
     *  Solution 1: read name of alias, search variables and parameters for that name,
     *  save index and id. Write in model_vars_info
     *  Drawback: Expensive search
     */

    /*
     * Solution 2: read modelDescription.xml which contains this information.
     * Drawback: parse two in stead of one xml-file
     */

    /*
     * ToDo: Solution 3: Edit generation of ..._init.xml and add aliasVariableValueReference
     */
    if (alias_valueReference <= n_variables) {
        return alias_valueReference;
    }
    else {
        return alias_valueReference-n_variables;
    }
}


/*
 * Reads variables info and attributes and writes in model_vars_info.
 * If one attribute is not found a default value is used.
 */
void omsu_read_var_info (omc_ScalarVariable*    v,
                         model_variable_info_t* model_var_info,
                         omsi_data_type         type,
                         omsi_unsigned_int*     variable_index,
                         omsi_int               number_of_prev_variables) {

    /* Variables */
    omsi_string aliasTmp;
    real_var_attribute_t * attribute_real;
    int_var_attribute_t * attribute_int;
    bool_var_attribute_t * attribute_bool;
    string_var_attribute_t * attribute_string;

    omsu_read_value_int(omsu_findHashStringString(v,"valueReference"), &model_var_info->id, 0);
    omsu_read_value_string(omsu_findHashStringString(v,"name"), (omsi_char**) &model_var_info->name);
    omsu_read_value_string(omsu_findHashStringStringEmpty(v,"description"), (omsi_char**) &model_var_info->comment);

    model_var_info->type_index.type = type;

    /* read attributes in dependence of variable_type */
    switch(type) {
        default:
        case OMSI_TYPE_UNKNOWN:
            LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSERROR,
                global_callback->logger(global_callback->componentEnvironment, global_instance_name,
                omsi_error, logCategoriesNames[LOG_STATUSERROR],
                "fmi2Instantiate: Unknown OMSI type for modelica attributes."))
        break;

        case OMSI_TYPE_REAL:
            attribute_real = (real_var_attribute_t *) global_callback->allocateMemory(1, sizeof(real_var_attribute_t));
            omsu_read_value_string(omsu_findHashStringStringEmpty(v,"unit"), (omsi_char**) &attribute_real->unit);
            omsu_read_value_string(omsu_findHashStringStringEmpty(v,"displayUnit"), (omsi_char**) &attribute_real->displayUnit);
            omsu_read_value_real(omsu_findHashStringString(v,"min"), &attribute_real->min, -OMSI_DBL_MAX);
            omsu_read_value_real(omsu_findHashStringString(v,"max"), &attribute_real->max, OMSI_DBL_MAX);
            omsu_read_value_bool(omsu_findHashStringString(v,"fixed"), &attribute_real->fixed);
            omsu_read_value_real(omsu_findHashStringString(v,"nominal"), &attribute_real->nominal, 1);
            omsu_read_value_real(omsu_findHashStringString(v,"start"), &attribute_real->start, 0);
            model_var_info->modelica_attributes = attribute_real;
        break;

        case OMSI_TYPE_INTEGER:
            attribute_int = (int_var_attribute_t *) global_callback->allocateMemory(1, sizeof(int_var_attribute_t));
            omsu_read_value_int(omsu_findHashStringString(v,"min"), &attribute_int->min, -OMSI_INT_MAX);
            omsu_read_value_int(omsu_findHashStringString(v,"max"), &attribute_int->min, OMSI_INT_MAX);
            omsu_read_value_bool(omsu_findHashStringString(v,"fixed"), &attribute_int->fixed);
            omsu_read_value_int(omsu_findHashStringString(v,"start"), &attribute_int->start, 0);
            model_var_info->modelica_attributes = attribute_int;
        break;

        case OMSI_TYPE_BOOLEAN:
            attribute_bool = (bool_var_attribute_t *) global_callback->allocateMemory(1, sizeof(bool_var_attribute_t));
            omsu_read_value_bool(omsu_findHashStringString(v,"fixed"), &attribute_bool->fixed);
            omsu_read_value_bool_default(omsu_findHashStringString(v,"start"), &attribute_bool->start, 0);
            model_var_info->modelica_attributes = attribute_bool;
        break;

        case OMSI_TYPE_STRING:
            attribute_string = (string_var_attribute_t *) global_callback->allocateMemory(1, sizeof(string_var_attribute_t));
            omsu_read_value_string(omsu_findHashStringStringEmpty(v,"start"), &attribute_string->start);
            model_var_info->modelica_attributes = attribute_string;
        break;
    }

    omsu_read_value_string(omsu_findHashStringStringNull(v,"alias"), (omsi_char**) &aliasTmp);
    if (0 == strcmp(aliasTmp,"noAlias")) {
        model_var_info->isAlias = omsi_false;
        model_var_info->negate = 1;
        model_var_info->aliasID = -1;
        model_var_info->type_index.index = *variable_index;
        (*variable_index)++;
    }
    else if (0 == strcmp(aliasTmp,"negatedAlias")){
        model_var_info->isAlias = omsi_true;
        model_var_info->negate = -1;
        /* ToDo: find alias id */
        omsu_read_value_int(omsu_findHashStringString(v,"aliasVariableId"), &model_var_info->aliasID, -1);
        model_var_info->type_index.index = omsu_find_alias_index(model_var_info->aliasID, number_of_prev_variables);
    }
    else {
        model_var_info->isAlias = omsi_true;
        model_var_info->negate = 1;
        /* ToDo: find alias id */
        omsu_read_value_int(omsu_findHashStringString(v,"aliasVariableId"), &model_var_info->aliasID, -1);
        model_var_info->type_index.index = omsu_find_alias_index(model_var_info->aliasID, number_of_prev_variables);
    }

    omsu_read_value_string(omsu_findHashStringStringEmpty(v,"fileName"), (omsi_char**) &model_var_info->info.filename);
    omsu_read_value_int(omsu_findHashStringString(v,"startLine"), &model_var_info->info.lineStart, 0);
    omsu_read_value_int(omsu_findHashStringString(v,"startColumn"), &model_var_info->info.colStart, 0);
    omsu_read_value_int(omsu_findHashStringString(v,"endLine"), &model_var_info->info.lineEnd, 0);
    omsu_read_value_int(omsu_findHashStringString(v,"endColumn"), &model_var_info->info.colEnd, 0);
    omsu_read_value_bool(omsu_findHashStringString(v,"fileWritable"), &model_var_info->info.fileWritable);
}

/*
 * Fill model_vars_info for all states, derivatives, variables and parameters.
 * Allocates memory for strings.
 */
void omsu_read_var_infos(model_data_t*      model_data,
                         omc_ModelInput*    mi) {

    /* Log function call */
    LOG_FILTER(global_callback->componentEnvironment, LOG_ALL,
        global_callback->logger(global_callback->componentEnvironment, global_instance_name,
        omsi_ok, logCategoriesNames[LOG_ALL], "fmi2Instantiate: Read variable informations from XML file."))


    /* Variables */
    omsi_unsigned_int i, j=0;
    omsi_unsigned_int variable_index = 0;
    omsi_int prev_variables;

    /* model vars info for states and derivatives */
    for (i=0; i<model_data->n_states; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->rSta ,i);
        omsu_read_var_info(v, &model_data->model_vars_info[j], OMSI_TYPE_REAL, &variable_index, -1);
    }
    for (i=0; i<model_data->n_states; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->rDer ,i);
        omsu_read_var_info(v, &model_data->model_vars_info[j], OMSI_TYPE_REAL, &variable_index, -1);
    }

    /* model vars info for reals */
    for (i=0; i<model_data->n_real_vars; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->rAlg ,i);
        omsu_read_var_info(v, &model_data->model_vars_info[j], OMSI_TYPE_REAL, &variable_index, -1);
    }
    for (i=0; i<model_data->n_real_parameters; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->rPar ,i);
        omsu_read_var_info(v, &model_data->model_vars_info[j], OMSI_TYPE_REAL, &variable_index, -1);
    }
    for (i=0; i<model_data->n_real_aliases; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->rAli ,i);
        omsu_read_var_info(v, &model_data->model_vars_info[j], OMSI_TYPE_REAL, NULL, 0);
    }
    /* ToDo: add sensitives? */

    /* model vars info for intgers */
    variable_index = 0;
    prev_variables = model_data->n_real_vars+model_data->n_real_parameters;
    for (i=0; i<model_data->n_int_vars; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->iAlg ,i);
        omsu_read_var_info(v, &model_data->model_vars_info[j], OMSI_TYPE_INTEGER, &variable_index, -1);
    }
    for (i=0; i<model_data->n_int_parameters; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->iPar ,i);
        omsu_read_var_info(v, &model_data->model_vars_info[j], OMSI_TYPE_INTEGER, &variable_index, -1);
    }
    for (i=0; i<model_data->n_int_aliases; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->iAli ,i);
        omsu_read_var_info(v, &model_data->model_vars_info[j], OMSI_TYPE_INTEGER, NULL, prev_variables);
    }

    /* model vars info for booleans */
    variable_index = 0;
    prev_variables += model_data->n_int_vars+model_data->n_int_parameters;
    for (i=0; i<model_data->n_bool_vars; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->bAlg ,i);
        omsu_read_var_info(v, &model_data->model_vars_info[j], OMSI_TYPE_BOOLEAN, &variable_index, -1);
    }
    for (i=0; i<model_data->n_bool_parameters; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->bPar ,i);
        omsu_read_var_info(v, &model_data->model_vars_info[j], OMSI_TYPE_BOOLEAN, &variable_index, -1);
    }
    for (i=0; i<model_data->n_bool_aliases; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->bAli ,i);
        omsu_read_var_info(v, &model_data->model_vars_info[j], OMSI_TYPE_BOOLEAN, NULL, prev_variables);
    }

    /* model vars info for strings */
    variable_index = 0;
    prev_variables += model_data->n_bool_vars+model_data->n_bool_parameters;
    for (i=0; i<model_data->n_string_vars; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->sAlg ,i);
        omsu_read_var_info(v, &model_data->model_vars_info[j], OMSI_TYPE_STRING, &variable_index, -1);
    }
    for (i=0; i<model_data->n_string_parameters; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->sPar ,i);
        omsu_read_var_info(v, &model_data->model_vars_info[j], OMSI_TYPE_STRING, &variable_index, -1);
    }
    for (i=0; i<model_data->n_string_aliases; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->sAli ,i);
        omsu_read_var_info(v, &model_data->model_vars_info[j], OMSI_TYPE_STRING, NULL, prev_variables);
    }
}


/*
 *
 */
omsi_string omsu_findHashStringStringNull(hash_string_string*   ht,
                                          omsi_string           key) {

    hash_string_string *res;
    HASH_FIND_STR( ht, key, res );
    return res ? res->val : NULL;
}


omsi_string omsu_findHashStringStringEmpty(hash_string_string*  ht,
                                           omsi_string          key) {

  omsi_string res = omsu_findHashStringStringNull(ht,key);
  return res ? res : "";
}


omsi_string omsu_findHashStringString(hash_string_string*   ht,
                                      omsi_string           key) {

  omsi_string res = omsu_findHashStringStringNull(ht,key);
  if (0==res) {
      hash_string_string *c, *tmp;
      HASH_ITER(hh, ht, c, tmp) {
          /* ToDo: To much noise */
          /* LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSWARNING,
              global_callback->logger(global_callback->componentEnvironment, global_instance_name,
              omsi_warning, logCategoriesNames[LOG_STATUSWARNING], "HashMap contained: %s->%s\n", c->id, c->val)) */
      }
      /* ToDo: Log error for non optional keys */
      /*LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSWARNING,
          global_callback->logger(global_callback->componentEnvironment, global_instance_name,
          omsi_warning, logCategoriesNames[LOG_STATUSWARNING], "fmi2Instantiate: Failed to lookup string %s in hashmap %p", key, ht))*/
  }
  return res;
}

/*
 *
 */
void omsu_addHashLongVar(hash_long_var**        ht,
                         omsi_long              key,
                         omc_ScalarVariable*    val) {

    hash_long_var *v = (hash_long_var*) global_callback->allocateMemory(1, sizeof(hash_long_var));      /* ToDo: where is this memory freed? */
    v->id=key;
    v->val=val;
    HASH_ADD_INT( *ht, id, v );
}


/*
 *
 */
void omsu_addHashStringString(hash_string_string**  ht,
                              omsi_string           key,
                              omsi_string           val) {

    hash_string_string *v = (hash_string_string*) global_callback->allocateMemory(1, sizeof(hash_string_string));
    v->id=strdup(key);
    v->val=strdup(val);
    HASH_ADD_KEYPTR( hh, *ht, v->id, strlen(v->id), v );
}


/* reads integer value from a string */
void omsu_read_value_int(omsi_string    s,
                         omsi_int*      res,
                         omsi_int       default_value) {

    if (s==NULL || *s == '\0') {
        *res = default_value;
    } else if (0 == strcmp(s, "true")) {
        *res = 1;
    } else if (0 == strcmp(s, "false")) {
        *res = 0;
    } else {
        *res = strtol(s, (omsi_char **) NULL, 10);
    }
}


/* reads integer value from a string */
void omsu_read_value_uint(omsi_string           s,
                          omsi_unsigned_int*    res) {
    if (s==NULL) {
        *res = 0;       /*default value, if no string was found */
        return;
    }
    if (0 == strcmp(s, "true")) {
        *res = 1;
    } else if (0 == strcmp(s, "false")) {
        *res = 0;
    } else {
        *res = strtol(s, (omsi_char **) NULL, 10);
    }
}


/* ToDo: comment me  */
omc_ScalarVariable** omsu_findHashLongVar(hash_long_var*    ht,
                                          omsi_long         key) {

    hash_long_var *res;
    HASH_FIND_INT( ht, &key, res );
    if (0==res) {
        hash_long_var *c, *tmp;
        HASH_ITER(hh, ht, c, tmp) {
            /* ToDo: to much noise */
            /* LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSWARNING,
                global_callback->logger(global_callback->componentEnvironment, global_instance_name,
                omsi_warning, logCategoriesNames[LOG_STATUSWARNING], "HashMap contained: %ld->*map*\n", c->id)) */
        }
    LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSWARNING,
        global_callback->logger(global_callback->componentEnvironment, global_instance_name,
        omsi_warning, logCategoriesNames[LOG_STATUSWARNING], "fmi2Instantiate: Failed to lookup long %s in hashmap %p", key, ht))
    }
    return &res->val;
}


/* reads double value from a string */
void omsu_read_value_real(omsi_string   s,
                          omsi_real*    res,
                          omsi_real     default_value) {

    if (s== NULL || *s == '\0') {
        *res = default_value;
    } else if (0 == strcmp(s, "true")) {
        *res = 1.0;
    } else if (0 == strcmp(s, "false")) {
        *res = 0.0;
    } else {
        *res = atof(s);     /* ToDo: use strtod() */
    }
}


/* reads boolean value from a string */
void omsu_read_value_bool(omsi_string   s,
                          omsi_bool*    res) {

    *res = 0 == strcmp(s, "true");
}


void omsu_read_value_bool_default (omsi_string  s,
                                   omsi_bool*   res,
                                   omsi_bool    default_bool) {

    if (s == NULL || *s == '\0') {
        *res = default_bool;

    }
    else{
        *res = 0 == strcmp(s, "true");
    }
}


/*
 *  Reads modelica_string value from a string.
 *  Allocates memory for string and copies string s into str.
 */
void omsu_read_value_string(omsi_string s,
                            omsi_char** str) {

    if(str == NULL) {
        LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSERROR,
            global_callback->logger(global_callback->componentEnvironment, global_instance_name,
            omsi_ok, logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: In function omsu_read_value_string: Memory for string not allocated."))
        return;
    }

    *str = strdup(s);
    if (str == NULL) {
        LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSERROR,
            global_callback->logger(global_callback->componentEnvironment, global_instance_name,
            omsi_ok, logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: Out of memory."))
        return;
    }
}


void XMLCALL startElement(void*         userData,
                          omsi_string   name,
                          omsi_string*  attr) {

    omc_ModelInput* mi = (omc_ModelInput*) userData;
    omsi_long i = 0;

    /* handle fmiModelDescription */
    if (!strcmp(name, "fmiModelDescription")) {
        for (i = 0; attr[i]; i += 2) {
            omsu_addHashStringString(&mi->md, attr[i], attr[i + 1]);
        }
        return;
    }
    /* handle DefaultExperiment */
    if (!strcmp(name, "DefaultExperiment")) {
        for (i = 0; attr[i]; i += 2) {
            omsu_addHashStringString(&mi->de, attr[i], attr[i + 1]);
        }
        return;
    }

    /* handle ScalarVariable */
    if (!strcmp(name, "ScalarVariable")) {
        omc_ScalarVariable *v = NULL;
        omsi_string ci, ct;
        omsi_int fail = 0;
        mi->lastCI = -1;
        mi->lastCT = NULL;
        for (i = 0; attr[i]; i += 2) {
            omsu_addHashStringString(&v, attr[i], attr[i + 1]);
        }
        /* fetch the class index/type  */
        ci = omsu_findHashStringString(v, "classIndex");
        ct = omsu_findHashStringString(v, "classType");
        /* transform to omsi_long  */
        mi->lastCI = strtol(ci, NULL, 10);

        /* which one of the classifications?  */
        if (strlen(ct) == 4) {
            if (ct[0] == 'r') {
                if (0 == strcmp(ct + 1, "Sta")) {
                    mi->lastCT = &mi->rSta;
                } else if (0 == strcmp(ct + 1, "Der")) {
                    mi->lastCT = &mi->rDer;
                } else if (0 == strcmp(ct + 1, "Alg")) {
                    mi->lastCT = &mi->rAlg;
                } else if (0 == strcmp(ct + 1, "Par")) {
                    mi->lastCT = &mi->rPar;
                } else if (0 == strcmp(ct + 1, "Ali")) {
                    mi->lastCT = &mi->rAli;
                } else if (0 == strcmp(ct + 1, "Sen")) {
                    mi->lastCT = &mi->rSen;
                } else {
                    fail = 1;
                }
            } else if (ct[0] == 'i') {
                if (0 == strcmp(ct + 1, "Alg")) {
                    mi->lastCT = &mi->iAlg;
                } else if (0 == strcmp(ct + 1, "Par")) {
                    mi->lastCT = &mi->iPar;
                } else if (0 == strcmp(ct + 1, "Ali")) {
                    mi->lastCT = &mi->iAli;
                } else {
                    fail = 1;
                }
            } else if (ct[0] == 'b') {
                if (0 == strcmp(ct + 1, "Alg")) {
                    mi->lastCT = &mi->bAlg;
                } else if (0 == strcmp(ct + 1, "Par")) {
                    mi->lastCT = &mi->bPar;
                } else if (0 == strcmp(ct + 1, "Ali")) {
                    mi->lastCT = &mi->bAli;
                } else {
                    fail = 1;
                }
            } else if (ct[0] == 's') {
                if (0 == strcmp(ct + 1, "Alg")) {
                    mi->lastCT = &mi->sAlg;
                } else if (0 == strcmp(ct + 1, "Par")) {
                    mi->lastCT = &mi->sPar;
                } else if (0 == strcmp(ct + 1, "Ali")) {
                    mi->lastCT = &mi->sAli;
                } else {
                    fail = 1;
                }
            } else {
                fail = 1;
            }
        } else {
            fail = 1;
        }

    if (fail) {
        LOG_FILTER(global_callback->componentEnvironment, LOG_STATUSERROR,
            global_callback->logger(global_callback->componentEnvironment, global_instance_name,
            omsi_ok, logCategoriesNames[LOG_STATUSERROR], "fmi2Instantiate: Found unknown class: %s  for variable: %s while reading XML.",ct,omsu_findHashStringString(v,"name")))
    }

        /* add the ScalarVariable map to the correct map! */
        omsu_addHashLongVar(mi->lastCT, mi->lastCI, v);

        return;
    }
    /* handle Real/Integer/Boolean/String */
    if (!strcmp(name, "Real") || !strcmp(name, "Integer")
            || !strcmp(name, "Boolean") || !strcmp(name, "String")) {
        /* add keys/value to the last variable */
        for (i = 0; attr[i]; i += 2) {
            /* add more key/value pairs to the last variable */
            omsu_addHashStringString(
                    omsu_findHashLongVar(*mi->lastCT, mi->lastCI), attr[i],
                    attr[i + 1]);
        }
        omsu_addHashStringString(omsu_findHashLongVar(*mi->lastCT, mi->lastCI),
                "variableType", name);
        return;
    }
    /* anything else, we don't handle! */
    /* ToDo: Error mesage or warning??? */
}


void XMLCALL endElement(void*       userData,
                        omsi_string name) {

    /* do nothing! */
    UNUSED(userData); UNUSED(name);
}


/* deallocates memory for omc_ModelInput struct */
/* ToDo: is full of bugs */
void omsu_free_ModelInput(omc_ModelInput mi) {

    free_hash_string_string(mi.md);
    free_hash_string_string(mi.de);

    free_hash_long_var(mi.rSta);
    free_hash_long_var(mi.rDer);
    free_hash_long_var(mi.rAlg);
    free_hash_long_var(mi.rPar);
    free_hash_long_var(mi.rAli);
    free_hash_long_var(mi.rSen);

    free_hash_long_var(mi.iAlg);
    free_hash_long_var(mi.iPar);
    free_hash_long_var(mi.iAli);

    free_hash_long_var(mi.bAlg);
    free_hash_long_var(mi.bPar);
    free_hash_long_var(mi.bAli);

    free_hash_long_var(mi.sAlg);
    free_hash_long_var(mi.sPar);
    free_hash_long_var(mi.sAli);

}


/* delete all items from hash and frees memory for hash table hash_string_string*/
void free_hash_string_string (hash_string_string* data) {

    hash_string_string* current, *tmp;

    HASH_ITER(hh, data, current, tmp) {
        HASH_DEL(data, current);             /* delete; current advances to next */
        global_callback->freeMemory((omsi_char *)current->id);
        global_callback->freeMemory((omsi_char *)current->val);
        free(current);
    }
}


/* delete all items from hash and frees memory for hash table hash_long_var*/
void free_hash_long_var (hash_long_var* data) {

    hash_long_var* current, *tmp;

    HASH_ITER(hh, data, current, tmp) {
        HASH_DEL(data, current);             /* delete; current advances to next */
        global_callback->freeMemory((omsi_char *)current->val);
        free(current);            /* optional- if you want to free  */
    }
}


/* delete all items from hash and frees memory for hash table hash_long_var*/
void free_hash_string_long (hash_string_long* data) {

    hash_string_long* current, *tmp;

    HASH_ITER(hh, data, current, tmp) {
        HASH_DEL(data, current);             /* delete; current advances to next */
        global_callback->freeMemory((omsi_char *)current->id);
        free(current);            /* optional- if you want to free  */
    }
}
