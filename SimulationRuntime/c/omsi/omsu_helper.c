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

#define FMI2_DBL_MAX DBL_MAX
#define FMI2_INT_MAX INT_MAX

/* global function */
fmi2CallbackAllocateMemory global_allocateMemory = NULL;
fmi2CallbackFreeMemory global_freeMemory = NULL;


void storePreValues (DATA *data) {
    //TODO: implement for new data structure
}

omsi_int stateSelection(DATA *data, threadData_t *threadData, omsi_char reportError, omsi_int switchStates) {
    //TODO: implement for new data structure
    return -1;
}

void overwriteOldSimulationData(DATA *data) {
    //TODO: implement for new data structure
}



/*
 * frees memory for omsi_t struct and all its components
 */
void omsu_free_osu_data(omsi_t* omsi_data, const fmi2CallbackFreeMemory freeMemory) {

    omsi_int i, j=0;
    omsi_int n_model_vars_and_params;

    /* free memory for model data */
    freeMemory((omsi_char *)omsi_data->model_data.modelGUID);

    for (i=0; i<omsi_data->model_data.n_real_vars+omsi_data->model_data.n_real_parameters; i++, j++) {
        freeMemory (omsi_data->model_data.model_vars_info_t[j].name);
        freeMemory (omsi_data->model_data.model_vars_info_t[j].comment);
        real_var_attribute_t* attribute = omsi_data->model_data.model_vars_info_t[j].modelica_attributes;
        freeMemory (attribute->unit);
        freeMemory (attribute->displayUnit);
        freeMemory (omsi_data->model_data.model_vars_info_t[j].modelica_attributes);
        freeMemory(omsi_data->model_data.model_vars_info_t[j].info.filename);
    }
    n_model_vars_and_params = omsi_data->model_data.n_states + omsi_data->model_data.n_derivatives
                             + omsi_data->model_data.n_real_vars + omsi_data->model_data.n_int_vars
                             + omsi_data->model_data.n_bool_vars + omsi_data->model_data.n_string_vars
                             + omsi_data->model_data.n_real_parameters + omsi_data->model_data.n_int_parameters
                             + omsi_data->model_data.n_bool_parameters + omsi_data->model_data.n_string_parameters
                             + omsi_data->model_data.n_real_aliases + omsi_data->model_data.n_int_aliases
                             + omsi_data->model_data.n_bool_aliases + omsi_data->model_data.n_string_aliases;
    for (i=j; i<n_model_vars_and_params; i++, j++) {
        freeMemory (omsi_data->model_data.model_vars_info_t[j].name);
        freeMemory (omsi_data->model_data.model_vars_info_t[j].comment);
        freeMemory (omsi_data->model_data.model_vars_info_t[j].modelica_attributes);
        freeMemory(omsi_data->model_data.model_vars_info_t[j].info.filename);
    }
    freeMemory (omsi_data->model_data.model_vars_info_t);

//    for (i=0; i<omsi_data->model_data.n_equations; i++) {
//        freeMemory (omsi_data->model_data.equation_info_t[i].variables);
//    }
    freeMemory (omsi_data->model_data.equation_info_t);

    /* free memory for simulation data */
    // ToDo: free inner stuff of initialization
    freeMemory (omsi_data->sim_data.initialization);
    // ToDo: free inner stuff of initialization
    freeMemory (omsi_data->sim_data.simulation);

    freeMemory (omsi_data->sim_data.model_vars_and_params->reals);
    freeMemory (omsi_data->sim_data.model_vars_and_params->ints);
    freeMemory (omsi_data->sim_data.model_vars_and_params->bools);

    freeMemory (omsi_data->sim_data.zerocrossings_vars);
    freeMemory (omsi_data->sim_data.pre_zerocrossings_vars);

    /* free memory for experiment data */
    freeMemory ((omsi_char *)omsi_data->experiment->solver_name);        // type-cast to shut of warning when compiling
    freeMemory (omsi_data->experiment);
}


/*
 *
 */
static omsi_string omsu_findHashStringStringNull(hash_string_string *ht, omsi_string key) {
  hash_string_string *res;
  HASH_FIND_STR( ht, key, res );
  return res ? res->val : NULL;
}

static inline omsi_string omsu_findHashStringStringEmpty(hash_string_string *ht, omsi_string key)
{
  omsi_string res = omsu_findHashStringStringNull(ht,key);
  return res ? res : "";
}

static omsi_string omsu_findHashStringString(hash_string_string *ht, omsi_string key) {
  omsi_string res = omsu_findHashStringStringNull(ht,key);
  if (0==res) {
    hash_string_string *c, *tmp;
    HASH_ITER(hh, ht, c, tmp) {
      //fprintf(stderr, "HashMap contained: %s->%s\n", c->id, c->val);            // ToDo: Use logger everywhere
    }
    //throwStreamPrint(NULL, "Failed to lookup %s in hashmap %p", key, ht);     // ToDo: add Error message
  }
  return res;
}

/*
 *
 */
static void omsu_addHashLongVar(hash_long_var **ht, omsi_long key, omc_ScalarVariable *val) {
  hash_long_var *v = (hash_long_var*) global_allocateMemory(1, sizeof(hash_long_var));
  v->id=key;
  v->val=val;
  HASH_ADD_INT( *ht, id, v );
}

/*
 *
 */
static void omsu_addHashStringString(hash_string_string **ht, omsi_string key, omsi_string val) {
  hash_string_string *v = (hash_string_string*) global_allocateMemory(1, sizeof(hash_string_string));
  v->id=strdup(key);
  v->val=strdup(val);
  HASH_ADD_KEYPTR( hh, *ht, v->id, strlen(v->id), v );
}

/* reads integer value from a string */
static void omsu_read_value_int(omsi_string s, omsi_int* res, omsi_int default_value) {
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
static void omsu_read_value_uint(omsi_string s, omsi_unsigned_int* res) {
    if (s==NULL) {
        *res = 0;       // default value, if no string was found
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
static omc_ScalarVariable** omsu_findHashLongVar(hash_long_var *ht, omsi_long key) {
  hash_long_var *res;
  HASH_FIND_INT( ht, &key, res );
  if (0==res) {
    hash_long_var *c, *tmp;
    HASH_ITER(hh, ht, c, tmp) {
      fprintf(stderr, "HashMap contained: %ld->*map*\n", c->id);
    }
    // throwStreamPrint(NULL, "Failed to lookup %ld in hashmap %p", key, ht);       // ToDo: add Error message
  }
  return &res->val;
}


/* reads double value from a string */
static void omsu_read_value_real(omsi_string s, modelica_real* res, modelica_real default_value) {
    if (s== NULL || *s == '\0') {
        *res = default_value;
    } else if (0 == strcmp(s, "true")) {
        *res = 1.0;
    } else if (0 == strcmp(s, "false")) {
        *res = 0.0;
    } else {
        *res = atof(s);     // ToDo: use strtod()
    }
}


/* reads boolean value from a string */
static inline void omsu_read_value_bool(omsi_string s, omsi_bool* res) {
    *res = 0 == strcmp(s, "true");
}

static inline void omsu_read_value_bool_default (omsi_string s, omsi_bool* res, omsi_bool default_bool) {
    if (s == NULL || *s == '\0') {
        *res = default_bool;
    }
    *res = 0 == strcmp(s, "true");
}

/*
 *  Reads modelica_string value from a string.
 *  Allocates memory for string and copies string s into str.
 */
static void omsu_read_value_string(omsi_string s, omsi_string *str) {
    if(str == NULL) {
        //warningStreamPrint(LOG_SIMULATION, 0, "error read_value, no data allocated for storing string");    // ToDo: Use same log everywhere
        return;
    }
    *str = strdup(s);
    if (str == NULL) {
        //warningStreamPrint(LOG_SIMULATION, 0, "error read_value, out of memory");       // ToDo: Use same log everywhere
        return;
    }
}


static void XMLCALL startElement(void *userData, omsi_string name, omsi_string *attr) {
  omc_ModelInput* mi = (omc_ModelInput*)userData;
  mmc_sint_t i = 0;

  /* handle fmiModelDescription */
  if(!strcmp(name, "fmiModelDescription")) {
    for(i = 0; attr[i]; i += 2) {
      omsu_addHashStringString(&mi->md, attr[i], attr[i+1]);
    }
    return;
  }
  /* handle DefaultExperiment */
  if(!strcmp(name, "DefaultExperiment")) {
    for(i = 0; attr[i]; i += 2) {
      omsu_addHashStringString(&mi->de, attr[i], attr[i+1]);
    }
    return;
  }

  /* handle ScalarVariable */
  if(!strcmp(name, "ScalarVariable"))
  {
    omc_ScalarVariable *v = NULL;
    omsi_string ci, ct;
    omsi_int fail=0;
    mi->lastCI = -1;
    mi->lastCT = NULL;
    for(i = 0; attr[i]; i += 2) {
      omsu_addHashStringString(&v, attr[i], attr[i+1]);
    }
    /* fetch the class index/type  */
    ci = omsu_findHashStringString(v, "classIndex");
    ct = omsu_findHashStringString(v, "classType");
    /* transform to mmc_sint_t  */
    //mi->lastCI = atoi(ci);
    mi->lastCI = strtol(ci, NULL, 10);

    /* which one of the classifications?  */
    if (strlen(ct) == 4) {
      if (ct[0]=='r') {
        if (0 == strcmp(ct+1,"Sta")) {
          mi->lastCT = &mi->rSta;
        } else if (0 == strcmp(ct+1,"Der")) {
          mi->lastCT = &mi->rDer;
        } else if (0 == strcmp(ct+1,"Alg")) {
          mi->lastCT = &mi->rAlg;
        } else if (0 == strcmp(ct+1,"Par")) {
          mi->lastCT = &mi->rPar;
        } else if (0 == strcmp(ct+1,"Ali")) {
          mi->lastCT = &mi->rAli;
        } else if (0 == strcmp(ct+1,"Sen")) {
          mi->lastCT = &mi->rSen;
        } else {
          fail = 1;
        }
      } else if (ct[0]=='i') {
        if (0 == strcmp(ct+1,"Alg")) {
          mi->lastCT = &mi->iAlg;
        } else if (0 == strcmp(ct+1,"Par")) {
          mi->lastCT = &mi->iPar;
        } else if (0 == strcmp(ct+1,"Ali")) {
          mi->lastCT = &mi->iAli;
        } else {
          fail = 1;
        }
      } else if (ct[0]=='b') {
        if (0 == strcmp(ct+1,"Alg")) {
          mi->lastCT = &mi->bAlg;
        } else if (0 == strcmp(ct+1,"Par")) {
          mi->lastCT = &mi->bPar;
        } else if (0 == strcmp(ct+1,"Ali")) {
          mi->lastCT = &mi->bAli;
        } else {
          fail = 1;
        }
      } else if (ct[0]=='s') {
        if (0 == strcmp(ct+1,"Alg")) {
          mi->lastCT = &mi->sAlg;
        } else if (0 == strcmp(ct+1,"Par")) {
          mi->lastCT = &mi->sPar;
        } else if (0 == strcmp(ct+1,"Ali")) {
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
      //throwStreamPrint(NULL, "simulation_input_xml.c: error reading the xml file, found unknown class: %s  for variable: %s",ct,omsu_findHashStringString(v,"name"));     // ToDo: Add Error message
    }

    /* add the ScalarVariable map to the correct map! */
    omsu_addHashLongVar(mi->lastCT, mi->lastCI, v);

    return;
  }
  /* handle Real/Integer/Boolean/String */
  if(!strcmp(name, "Real") || !strcmp(name, "Integer") || !strcmp(name, "Boolean") || !strcmp(name, "String")) {
    /* add keys/value to the last variable */
    for(i = 0; attr[i]; i += 2) {
      /* add more key/value pairs to the last variable */
      omsu_addHashStringString(omsu_findHashLongVar(*mi->lastCT, mi->lastCI), attr[i], attr[i+1]);
    }
    omsu_addHashStringString(omsu_findHashLongVar(*mi->lastCT, mi->lastCI), "variableType", name);
    return;
  }
  /* anything else, we don't handle! */
}

static void XMLCALL endElement(void *userData, omsi_string name) {
  /* do nothing! */
}

/* deallocates memory for oms_ModelInput struct */
void omsu_free_ModelInput(omc_ModelInput mi, omsi_t* omsi_data, const fmi2CallbackFreeMemory freeMemory) {

    omsi_unsigned_int i;

    freeMemory((omsi_char *)mi.md->id);
    freeMemory((omsi_char *)mi.md->val);
    freeMemory(mi.md);

    freeMemory((omsi_char *)mi.de->id);
    freeMemory((omsi_char *)mi.de->val);
    freeMemory(mi.de);

    for (i=0; i<omsi_data->model_data.n_states ; i++) {
        freeMemory((omsi_char *)mi.rSta[i].val->id);
        freeMemory((omsi_char *)mi.rSta[i].val->val);
        freeMemory(mi.rSta[i].val);
    }
    freeMemory(mi.rSta);
    for (i=0; i<omsi_data->model_data.n_derivatives ; i++) {
        freeMemory((omsi_char *)mi.rDer[i].val->id);
        freeMemory((omsi_char *)mi.rDer[i].val->val);
        freeMemory(mi.rDer[i].val);
    }
    freeMemory(mi.rDer);

//    for (i=0; i<omsi_data->model_data.n_real_vars ; i++) {
//        freeMemory(mi.rAlg[i].val->id);
//        freeMemory(mi.rAlg[i].val->val);
//        freeMemory(mi.rAlg[i].val);
//    }
    // ToDo: getting segmentation fault when freeing inner memory
    // ToDo: free inner stuff also for integer, boolean and string
    freeMemory(mi.rAlg);
    freeMemory(mi.rPar);
    freeMemory(mi.rAli);
    freeMemory(mi.rSen);        // ToDo: What are sensitives?

    freeMemory(mi.iAlg);
    freeMemory(mi.iPar);
    freeMemory(mi.iAli);

    freeMemory(mi.bAlg);
    freeMemory(mi.bPar);
    freeMemory(mi.bAli);

    freeMemory(mi.sAlg);
    freeMemory(mi.sPar);
    freeMemory(mi.sAli);

    // ToDo: What to free for lastCT???

}

/*
 * Compute corresponding index for alias variable.
 */
omsi_int omsu_find_alias_index(omsi_int alias_valueReference, omsi_int n_variables) {
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
 * Reads variables info and attributes and writes in model_vars_info_t.
 * If one attribute is not found a default value is used.
 */
static void omsu_read_var_info (omc_ScalarVariable *v, model_variable_info_t* model_var_info, omsi_data_type type, size_t* variable_index, omsi_int number_of_prev_variables) {

    omsi_string aliasTmp;

    omsu_read_value_int(omsu_findHashStringString(v,"valueReference"), &model_var_info->id, 0);
    model_var_info->id -= 1000;
    omsu_read_value_string(omsu_findHashStringString(v,"name"), &model_var_info->name);
    omsu_read_value_string(omsu_findHashStringStringEmpty(v,"description"), &model_var_info->comment);

    model_var_info->type_index.type = type;

    real_var_attribute_t * attribute_real;
    int_var_attribute_t * attribute_int;
    bool_var_attribute_t * attribute_bool;
    string_var_attribute_t * attribute_string;

    /* read attributes in dependence of variable_type */
    switch(type) {
        default:
        case OMSI_TYPE_UNKNOWN:
            // ToDo: add error case
        break;

        case OMSI_TYPE_REAL:
            attribute_real = (real_var_attribute_t *) global_allocateMemory(1, sizeof(real_var_attribute_t));
            omsu_read_value_string(omsu_findHashStringStringEmpty(v,"unit"), &attribute_real->unit);
            omsu_read_value_string(omsu_findHashStringStringEmpty(v,"displayUnit"), &attribute_real->displayUnit);
            omsu_read_value_real(omsu_findHashStringString(v,"min"), &attribute_real->min, -FMI2_DBL_MAX);
            omsu_read_value_real(omsu_findHashStringString(v,"max"), &attribute_real->max, FMI2_DBL_MAX);
            omsu_read_value_bool(omsu_findHashStringString(v,"fixed"), &attribute_real->fixed);
            omsu_read_value_real(omsu_findHashStringString(v,"nominal"), &attribute_real->nominal, 1);
            omsu_read_value_real(omsu_findHashStringString(v,"start"), &attribute_real->start, 0);
            model_var_info->modelica_attributes = attribute_real;
        break;

        case OMSI_TYPE_INTEGER:
            attribute_int = (int_var_attribute_t *) global_allocateMemory(1, sizeof(int_var_attribute_t));
            omsu_read_value_int(omsu_findHashStringString(v,"min"), &attribute_int->min, -FMI2_INT_MAX);
            omsu_read_value_int(omsu_findHashStringString(v,"max"), &attribute_int->min, FMI2_INT_MAX);
            omsu_read_value_bool(omsu_findHashStringString(v,"fixed"), &attribute_int->fixed);
            omsu_read_value_int(omsu_findHashStringString(v,"start"), &attribute_int->start, 0);
            model_var_info->modelica_attributes = attribute_int;
        break;

        case OMSI_TYPE_BOOLEAN:
            attribute_bool = (bool_var_attribute_t *) global_allocateMemory(1, sizeof(bool_var_attribute_t));
            omsu_read_value_bool(omsu_findHashStringString(v,"fixed"), &attribute_bool->fixed);
            omsu_read_value_bool_default(omsu_findHashStringString(v,"start"), &attribute_bool->start, 0);
            model_var_info->modelica_attributes = attribute_bool;
        break;
        case OMSI_TYPE_STRING:
            attribute_string = (string_var_attribute_t *) global_allocateMemory(1, sizeof(string_var_attribute_t));
            omsu_read_value_string(omsu_findHashStringStringEmpty(v,"start"), &attribute_int->start);
            model_var_info->modelica_attributes = attribute_string;
        break;
    }

    omsu_read_value_string(omsu_findHashStringStringNull(v,"alias"), &aliasTmp);
    if (0 == strcmp(aliasTmp,"noAlias")) {
        model_var_info->isAlias = fmi2False;
        model_var_info->negate = 1;
        model_var_info->aliasID = -1;
        model_var_info->type_index.index = *variable_index;
        (*variable_index)++;
    }
    else if (0 == strcmp(aliasTmp,"negatedAlias")){
        model_var_info->isAlias = fmi2True;
        model_var_info->negate = -1;
        // ToDo: find alias id
        omsu_read_value_int(omsu_findHashStringString(v,"aliasVariableId"), &model_var_info->aliasID, -1);
        model_var_info->aliasID -= 1000;
        model_var_info->type_index.index = omsu_find_alias_index(model_var_info->aliasID, number_of_prev_variables);
    }
    else {
        model_var_info->isAlias = fmi2True;
        model_var_info->negate = 1;
        // ToDo: find alias id
        omsu_read_value_int(omsu_findHashStringString(v,"aliasVariableId"), &model_var_info->aliasID, -1);
        model_var_info->aliasID -= 1000;
        model_var_info->type_index.index = omsu_find_alias_index(model_var_info->aliasID, number_of_prev_variables);
    }

    omsu_read_value_string(omsu_findHashStringStringEmpty(v,"fileName"), &model_var_info->info.filename);
    omsu_read_value_int(omsu_findHashStringString(v,"startLine"), &model_var_info->info.lineStart, 0);
    omsu_read_value_int(omsu_findHashStringString(v,"startColumn"), &model_var_info->info.colStart, 0);
    omsu_read_value_int(omsu_findHashStringString(v,"endLine"), &model_var_info->info.lineEnd, 0);
    omsu_read_value_int(omsu_findHashStringString(v,"endColumn"), &model_var_info->info.colEnd, 0);
    omsu_read_value_bool(omsu_findHashStringString(v,"fileWritable"), &model_var_info->info.fileWritable);
}

/*
 * Fill model_vars_info_t for all states, derivatives, variables and parameters.
 * Allocates memory for strings.
 */
void omsu_read_var_infos(model_data_t* model_data, omc_ModelInput* mi) {

    size_t i, j=0;
    omsi_int variable_index = 0;
    omsi_int prev_variables;

    /* model vars info for states and derivatives */
    for (i=0; i<model_data->n_states; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->rSta ,i);
        omsu_read_var_info(v, &model_data->model_vars_info_t[j], OMSI_TYPE_REAL, &variable_index, -1);    // ToDo: Which type for states
    }
    for (i=0; i<model_data->n_states; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->rDer ,i);
        omsu_read_var_info(v, &model_data->model_vars_info_t[j], OMSI_TYPE_REAL, &variable_index, -1);    // ToDo: Which type for derivatives
    }

    /* model vars info for reals */
    for (i=0; i<model_data->n_real_vars; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->rAlg ,i);
        omsu_read_var_info(v, &model_data->model_vars_info_t[j], OMSI_TYPE_REAL, &variable_index, -1);
    }
    for (i=0; i<model_data->n_real_parameters; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->rPar ,i);
        omsu_read_var_info(v, &model_data->model_vars_info_t[j], OMSI_TYPE_REAL, &variable_index, -1);
    }
    for (i=0; i<model_data->n_real_aliases; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->rAli ,i);
        omsu_read_var_info(v, &model_data->model_vars_info_t[j], OMSI_TYPE_REAL, NULL, 0);
    }
    // ToDo: add sensitives?

    /* model vars info for intgers */
    variable_index = 0;
    prev_variables = model_data->n_real_vars+model_data->n_real_parameters;
    for (i=0; i<model_data->n_int_vars; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->iAlg ,i);
        omsu_read_var_info(v, &model_data->model_vars_info_t[j], OMSI_TYPE_INTEGER, &variable_index, -1);
    }
    for (i=0; i<model_data->n_int_parameters; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->iPar ,i);
        omsu_read_var_info(v, &model_data->model_vars_info_t[j], OMSI_TYPE_INTEGER, &variable_index, -1);
    }
    for (i=0; i<model_data->n_int_aliases; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->iAli ,i);
        omsu_read_var_info(v, &model_data->model_vars_info_t[j], OMSI_TYPE_INTEGER, NULL, prev_variables);
    }

    /* model vars info for booleans */
    variable_index = 0;
    prev_variables += model_data->n_int_vars+model_data->n_int_parameters;
    for (i=0; i<model_data->n_bool_vars; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->bAlg ,i);
        omsu_read_var_info(v, &model_data->model_vars_info_t[j], OMSI_TYPE_BOOLEAN, &variable_index, -1);
    }
    for (i=0; i<model_data->n_bool_parameters; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->bPar ,i);
        omsu_read_var_info(v, &model_data->model_vars_info_t[j], OMSI_TYPE_BOOLEAN, &variable_index, -1);
    }
    for (i=0; i<model_data->n_bool_aliases; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->bAli ,i);
        omsu_read_var_info(v, &model_data->model_vars_info_t[j], OMSI_TYPE_BOOLEAN, NULL, prev_variables);
    }

    /* model vars info for strings */
    variable_index = 0;
    prev_variables += model_data->n_bool_vars+model_data->n_bool_parameters;
    for (i=0; i<model_data->n_string_vars; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->sAlg ,i);
        omsu_read_var_info(v, &model_data->model_vars_info_t[j], OMSI_TYPE_STRING, &variable_index, -1);
    }
    for (i=0; i<model_data->n_string_parameters; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->sPar ,i);
        omsu_read_var_info(v, &model_data->model_vars_info_t[j], OMSI_TYPE_STRING, &variable_index, -1);
    }
    for (i=0; i<model_data->n_string_aliases; i++, j++) {
        omc_ScalarVariable *v = *omsu_findHashLongVar(mi->sAli ,i);
        omsu_read_var_info(v, &model_data->model_vars_info_t[j], OMSI_TYPE_STRING, NULL, prev_variables);
    }
}



/*
 * Reads input values from a xml file and allocates memory for osu_data struct.
 */
omsi_int omsu_process_input_xml(omsi_t* osu_data, omsi_char* filename, fmi2String fmuGUID, fmi2String instanceName, const fmi2CallbackFunctions* functions) {

    /* set global function pointer */
    global_allocateMemory = functions->allocateMemory;
    global_freeMemory = functions->freeMemory;

    /* Variables */
    omsi_int done;
    omsi_int n_model_vars_and_params;
    omsi_string guid;
    omsi_char buf[BUFSIZ] = {0};

    omc_ModelInput mi = {0};
    FILE* file = NULL;
    XML_Parser parser = NULL;

    /* open xml file */
    file = fopen(filename, "r");
    if(!file) {
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
                        "fmi2Instantiate: Can not read input file %s.", filename);
        return -1;
    }

    /* create the XML parser */
    parser = XML_ParserCreate("UTF-8");
    if(!parser) {
        fclose(file);
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
                                "fmi2Instantiate: Out of memory.");
        return -1;
    }
    /* set our user data */
    XML_SetUserData(parser, &mi);
    /* set the handlers for start/end of element. */
    XML_SetElementHandler(parser, startElement, endElement);

    /* read XML */
    do {
        size_t len = fread(buf, 1, sizeof(buf), file);
        done = len < sizeof(buf);
        if(XML_STATUS_ERROR == XML_Parse(parser, buf, len, done)) {
            fclose(file);
            functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
                "fmi2Instantiate: failed to read the XML file %s: %s at line %lu.", filename,
                XML_ErrorString(XML_GetErrorCode(parser)),
                XML_GetCurrentLineNumber(parser));
            XML_ParserFree(parser);
            return -1;
        }
    } while(!done);

    fclose(file);
    XML_ParserFree(parser);

    // check model GUID
    guid = omsu_findHashStringStringNull(mi.md,"guid");
    if (NULL==guid) {
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
            "fmi2Instantiate: Model GUID %s is not set in model description %s.",
            fmuGUID, filename);
        return -1;
    }
    else if (strcmp(fmuGUID, guid)) {
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
            "fmi2Instantiate: Wrong GUID %s in file %s. Expected %s.",
            guid, filename, fmuGUID);
        return -1;
    }

    /* process experiment data */
    osu_data->experiment = functions->allocateMemory(1, sizeof(omsi_experiment_t));
    if (!osu_data->experiment) {
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
            "fmi2Instantiate: Not enough memory.");
        return -1;
    }

    omsu_read_value_real(omsu_findHashStringString(mi.de,"startTime"), &(osu_data->experiment->start_time), 0);
    omsu_read_value_real(omsu_findHashStringString(mi.de,"stopTime"), &(osu_data->experiment->stop_time), osu_data->experiment->start_time+1);
    omsu_read_value_real(omsu_findHashStringString(mi.de,"stepSize"), &(osu_data->experiment->step_size), (osu_data->experiment->stop_time - osu_data->experiment->start_time) / 500);
    omsu_read_value_uint(omsu_findHashStringString(mi.de,"numberOfOutputVariables"), &(osu_data->experiment->num_outputs));
    omsu_read_value_real(omsu_findHashStringString(mi.de,"tolerance"), &(osu_data->experiment->tolerance), 1e-5);
    omsu_read_value_string(omsu_findHashStringString(mi.de,"solver"), &(osu_data->experiment->solver_name));

    /* process all model data */
    omsu_read_value_string(omsu_findHashStringStringNull(mi.md,"guid"), &(osu_data->model_data.modelGUID));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfContinuousStates"), &(osu_data->model_data.n_states));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfContinuousStates"), &(osu_data->model_data.n_derivatives));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfRealAlgebraicVariables"), &(osu_data->model_data.n_real_vars));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfIntegerAlgebraicVariables"), &(osu_data->model_data.n_int_vars));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfBooleanAlgebraicVariables"), &(osu_data->model_data.n_bool_vars));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfStringAlgebraicVariables"), &(osu_data->model_data.n_string_vars));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfRealParameters"), &(osu_data->model_data.n_real_parameters));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfIntegerParameters"), &(osu_data->model_data.n_int_parameters));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfBooleanParameters"), &(osu_data->model_data.n_bool_parameters));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfStringParameters"), &(osu_data->model_data.n_string_parameters));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfRealAlgebraicAliasVariables"), &(osu_data->model_data.n_real_aliases));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfIntegerAliasVariables"), &(osu_data->model_data.n_int_aliases));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfBooleanAliasVariables"), &(osu_data->model_data.n_bool_aliases));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfStringAliasVariables"), &(osu_data->model_data.n_string_aliases));
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfEventIndicators"), &(osu_data->model_data.n_zerocrossings));       // ToDo: Is numberOfTimeEvents also part of n_zerocrossings????
    omsu_read_value_uint(omsu_findHashStringString(mi.md,"numberOfEquations"), &(osu_data->model_data.n_equations));      // ToDo: Is numberOfEquations in XML???

    // read model_vars_info_t
    n_model_vars_and_params = osu_data->model_data.n_states + osu_data->model_data.n_derivatives
                            + osu_data->model_data.n_real_vars + osu_data->model_data.n_int_vars
                            + osu_data->model_data.n_bool_vars + osu_data->model_data.n_string_vars
                            + osu_data->model_data.n_real_parameters + osu_data->model_data.n_int_parameters
                            + osu_data->model_data.n_bool_parameters + osu_data->model_data.n_string_parameters
                            + osu_data->model_data.n_real_aliases + osu_data->model_data.n_int_aliases
                            + osu_data->model_data.n_bool_aliases + osu_data->model_data.n_string_aliases;
    osu_data->model_data.model_vars_info_t = (model_variable_info_t*) functions->allocateMemory(n_model_vars_and_params, sizeof(model_variable_info_t));
    if (!osu_data->model_data.model_vars_info_t) {
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
            "fmi2Instantiate: Not enough memory.");
        return -1;
    }
    //read model_vars_info_t inner stuff
    omsu_read_var_infos(&osu_data->model_data, &mi);

    /* now all data from init_xml should be utilized */
    omsu_free_ModelInput(mi, osu_data, functions->freeMemory);


    // ToDo: read equation_info_t from JSON file
    osu_data->model_data.equation_info_t = (equation_info_t*) functions->allocateMemory(osu_data->model_data.n_equations, sizeof(equation_info_t));
    if (!osu_data->model_data.equation_info_t) {
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
            "fmi2Instantiate: Not enough memory.");
        return -1;
    }
    //omsu_process_info_json();


    /* allocate memory for sim_data_t */
    if (omsu_allocate_sim_data(osu_data, functions->allocateMemory)) {
        functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error",
            "fmi2Instantiate: Not enough memory.");
        return -1;
    }
    // ToDo: where do we get sim_data_t->inputs_real_index and so on?

    // ToDo: add error case

    return 0;
}

/*
 * Allocates memory for sim_data_t struct and all its components.
 * Gets called from omsu_process_input_xml().
 */
omsi_int omsu_allocate_sim_data(omsi_t* omsi_data, const fmi2CallbackAllocateMemory allocateMemory) {

    omsi_int n_model_vars_and_params;

    omsi_data->sim_data.initialization = (equation_system_t*) allocateMemory(1, sizeof(equation_system_t));
    if (!omsi_data->sim_data.initialization) {
        return -1;      // Error: Out of memory
    }
    // ToDo: add stuff, e.g. allocate memory for all parts of struct initialization

    omsi_data->sim_data.simulation = (equation_system_t*) allocateMemory(1, sizeof(equation_system_t));
    if (!omsi_data->sim_data.initialization) {
        return -1;      // Error: Out of memory
    }
    // ToDo: add more stuff

    n_model_vars_and_params = omsi_data->model_data.n_real_vars + omsi_data->model_data.n_int_vars + omsi_data->model_data.n_bool_vars + omsi_data->model_data.n_string_vars
                              + omsi_data->model_data.n_real_parameters + omsi_data->model_data.n_int_parameters + omsi_data->model_data.n_bool_parameters + omsi_data->model_data.n_string_parameters;
    omsi_data->sim_data.model_vars_and_params = (omsi_values *) allocateMemory(n_model_vars_and_params, sizeof(omsi_values));
    if (!omsi_data->sim_data.model_vars_and_params) {
        return -1;      // Error: Out of memory
    }
    omsi_data->sim_data.model_vars_and_params->reals = (omsi_real *) allocateMemory(omsi_data->model_data.n_real_vars + omsi_data->model_data.n_real_parameters, sizeof(omsi_real));
    omsi_data->sim_data.model_vars_and_params->ints = (omsi_int*) allocateMemory(omsi_data->model_data.n_int_vars + omsi_data->model_data.n_int_parameters, sizeof(omsi_int));
    omsi_data->sim_data.model_vars_and_params->bools = (omsi_bool*) allocateMemory(omsi_data->model_data.n_bool_vars + omsi_data->model_data.n_bool_parameters, sizeof(omsi_bool));
    if (!omsi_data->sim_data.model_vars_and_params->reals || !omsi_data->sim_data.model_vars_and_params->ints || !omsi_data->sim_data.model_vars_and_params->bools ) {
        return -1;      // Error: Out of memory
    }

    // ToDo: allocate memory for some pre-values

    omsi_data->sim_data.zerocrossings_vars = (omsi_bool *) allocateMemory(omsi_data->model_data.n_zerocrossings, sizeof(omsi_bool));
    omsi_data->sim_data.pre_zerocrossings_vars = (omsi_bool *) allocateMemory(omsi_data->model_data.n_zerocrossings, sizeof(omsi_bool));

    // ToDo: Add error cases

    return 0;
}

void omsu_print_debug_helper (model_data_t* model_data, size_t start_index, size_t size, size_t indent) {

    omsi_string oneIndent = "| ";
    omsi_string allIndent = (omsi_string) global_allocateMemory(100, sizeof(omsi_char));


    size_t i;
    for (i=0; i<indent; i++) {
        strcat(allIndent, oneIndent);
    }

    real_var_attribute_t* attribute_real;
    real_var_attribute_t* attribute_integer;
    real_var_attribute_t* attribute_bool;
    real_var_attribute_t* attribute_string;
    for (i=0; i<size; i++) {
        printf("%sid:\t\t\t%i\n", allIndent, model_data->model_vars_info_t[start_index+i].id);
        printf("%sname:\t\t\t%s\n", allIndent, model_data->model_vars_info_t[start_index+i].name);
        printf("%scomment:\t\t\t%s\n", allIndent, model_data->model_vars_info_t[start_index+i].comment);
        printf("%svariable type:\t\t%i\n", allIndent, (omsi_int)model_data->model_vars_info_t[start_index+i].type_index.type);
        printf("%svariable index:\t\t%i\n", allIndent, model_data->model_vars_info_t[start_index+i].type_index.index);
        printf("%sattribute:\n", allIndent);

        switch (model_data->model_vars_info_t[start_index+i].type_index.type) {
        case OMSI_TYPE_REAL:
            attribute_real = model_data->model_vars_info_t[start_index+i].modelica_attributes;
            printf("| %sunit:\t\t\t%s\n", allIndent, attribute_real->unit);
            printf("| %sdisplayUnit:\t\t%s\n", allIndent, attribute_real->displayUnit);
            if (attribute_real->min <= -FMI2_DBL_MAX) {
                printf("| %smin:\t\t\t-infinity\n", allIndent);
            }
            else {
                printf("| %smin:\t\t\t%f\n", allIndent, attribute_real->min);
            }
            if (attribute_real->max >= FMI2_DBL_MAX) {
                printf("| %smax:\t\t\tininity\n", allIndent);
            }
            else {
                printf("| %smax:\t\t\t%f\n", allIndent, attribute_real->max);
            }
            printf("| %sfixed:\t\t\t%s\n", allIndent, attribute_real->fixed ? "true" : "false");
            printf("| %snominal:\t\t%f\n", allIndent, attribute_real->nominal);
            printf("| %sstart:\t\t\t%f\n", allIndent, attribute_real->start);
            break;
        case OMSI_TYPE_INTEGER:
            attribute_integer = model_data->model_vars_info_t[start_index+i].modelica_attributes;
            if (attribute_integer->min <= -FMI2_INT_MAX) {
                printf("| %smin:\t\t\t-infinity\n", allIndent);
            }
            else {
                printf("| %smin:\t\t\t%i\n", allIndent, attribute_integer->min);
            }
            if (attribute_integer->max >= FMI2_INT_MAX) {
                printf("| %smax:\t\t\tininity\n", allIndent);
            }
            else {
                printf("| %smax:\t\t\t%i\n", allIndent, attribute_integer->max);
            }
            printf("| %sfixed:\t\t\t%s\n", allIndent, attribute_integer->fixed ? "true" : "false");
            printf("| %sstart:\t\t\t%i\n", allIndent, attribute_integer->start);
            break;
        case OMSI_TYPE_BOOLEAN:
            attribute_bool = model_data->model_vars_info_t[start_index+i].modelica_attributes;
            printf("| %sfixed:\t\t\t%s\n", allIndent, attribute_integer->fixed ? "true" : "false");
            printf("| %sstart:\t\t\t%s\n", allIndent, attribute_integer->start ? "true" : "false");
            break;
        case OMSI_TYPE_STRING:
            attribute_string = model_data->model_vars_info_t[start_index+i].modelica_attributes;
            printf("| %sstart:\t\t\t%s\n", allIndent, attribute_string->start);
        }

        printf("%salias:\t\t\t%s\n",  allIndent, model_data->model_vars_info_t[start_index+i].isAlias ? "true" : "false");
        printf("%snegate:\t\t\t%i\n",  allIndent, model_data->model_vars_info_t[start_index+i].negate);
        printf("%saliasID:\t\t\t%i\n",  allIndent, model_data->model_vars_info_t[start_index+i].aliasID);

        printf("%sfile info:\n", allIndent);
        printf("| %sfilename:\t\t%s\n", allIndent, model_data->model_vars_info_t[start_index+i].info.filename);
        printf("| %slineStart:\t\t%i\n", allIndent, model_data->model_vars_info_t[start_index+i].info.lineStart);
        printf("| %scolStart:\t\t%i\n", allIndent, model_data->model_vars_info_t[start_index+i].info.colStart);
        printf("| %slineEnd:\t\t%i\n", allIndent, model_data->model_vars_info_t[start_index+i].info.lineEnd);
        printf("| %scolEnd:\t\t\t%i\n", allIndent, model_data->model_vars_info_t[start_index+i].info.colEnd);
        printf("| %sfileWritable:\t\t%s\n", allIndent, model_data->model_vars_info_t[start_index+i].info.fileWritable ? "true" : "false");
        printf("%s\n", allIndent);
    }

    // free memory
    global_freeMemory(allIndent);

}
/*
 * debug function for development
 * ToDo: make prettier or delete when finished
 */
void omsu_print_debug (osu_t* OSU) {
    size_t lastIndex, size;
    omsi_int n_vars_and_params;

    printf("\n========== omsu_print_debug start ==========\n");
    /* print omsi_data */
    printf("struct omsi_t:\n");
    // print model data
    printf("| struct model_data_t:\n");
    printf("| | modelGUID:\t\t\t%s\n", OSU->osu_data->model_data.modelGUID);
    printf("| | n_states:\t\t\t%d\n", OSU->osu_data->model_data.n_states);
    printf("| | n_derivatives:\t\t%d\n", OSU->osu_data->model_data.n_derivatives);
    printf("| | n_real_vars:\t\t%d\n", OSU->osu_data->model_data.n_real_vars);
    printf("| | n_int_vars:\t\t\t%d\n", OSU->osu_data->model_data.n_int_vars);
    printf("| | n_bool_vars:\t\t%d\n", OSU->osu_data->model_data.n_bool_vars);
    printf("| | n_string_vars:\t\t%d\n", OSU->osu_data->model_data.n_string_vars);
    printf("| | n_real_parameters:\t\t%d\n", OSU->osu_data->model_data.n_real_parameters);
    printf("| | n_int_parameters:\t\t%d\n", OSU->osu_data->model_data.n_int_parameters);
    printf("| | n_bool_parameters:\t\t%d\n", OSU->osu_data->model_data.n_bool_parameters);
    printf("| | n_string_parameters:\t%d\n", OSU->osu_data->model_data.n_string_parameters);
    printf("| | n_real_aliases:\t\t%d\n", OSU->osu_data->model_data.n_real_aliases);
    printf("| | n_int_aliases:\t\t%d\n", OSU->osu_data->model_data.n_int_aliases);
    printf("| | n_bool_aliases:\t\t%d\n", OSU->osu_data->model_data.n_bool_aliases);
    printf("| | n_string_aliases:\t\t%d\n", OSU->osu_data->model_data.n_string_aliases);
    printf("| | n_zerocrossings:\t\t%d\n", OSU->osu_data->model_data.n_zerocrossings);
    printf("| | n_equations:\t\t%d\n", OSU->osu_data->model_data.n_equations);

    printf("| | model_vars_info_t:\n");
    lastIndex = 0;
    size = OSU->osu_data->model_data.n_states + OSU->osu_data->model_data.n_derivatives
           + OSU->osu_data->model_data.n_real_vars + OSU->osu_data->model_data.n_real_parameters
           + OSU->osu_data->model_data.n_real_aliases;
    omsu_print_debug_helper(&(OSU->osu_data->model_data), lastIndex, size, 3);

    lastIndex += size;
    size = OSU->osu_data->model_data.n_int_vars + OSU->osu_data->model_data.n_int_parameters
           + OSU->osu_data->model_data.n_int_aliases;
    omsu_print_debug_helper(&(OSU->osu_data->model_data), lastIndex, size, 3);

    lastIndex += size;
    size = OSU->osu_data->model_data.n_bool_vars + OSU->osu_data->model_data.n_bool_parameters
           + OSU->osu_data->model_data.n_bool_aliases;
    omsu_print_debug_helper(&(OSU->osu_data->model_data), lastIndex, size, 3);

    lastIndex += size;
    size = OSU->osu_data->model_data.n_string_vars + OSU->osu_data->model_data.n_string_parameters
           + OSU->osu_data->model_data.n_string_aliases;
    omsu_print_debug_helper(&(OSU->osu_data->model_data), lastIndex, size, 3);

    printf("| | equation_info_t:\n");
    for(size_t i=0; i<OSU->osu_data->model_data.n_equations; i++) {
        // print equation_info
    }

    // print sim_data


    // print experiment_data
    printf("|\n");
    printf("| struct omsi_experiment_t:\n");
    printf("| | start_time:\t\t\t%f\n", OSU->osu_data->experiment->start_time);
    printf("| | stop_time:\t\t\t%f\n", OSU->osu_data->experiment->stop_time);
    printf("| | step_size:\t\t\t%f\n", OSU->osu_data->experiment->step_size);
    printf("| | num_outputss:\t\t%u\n", OSU->osu_data->experiment->num_outputs);
    printf("| | tolerance:\t\t\t%1.e\n", OSU->osu_data->experiment->tolerance);
    printf("| | solver_name:\t\t%s\n", OSU->osu_data->experiment->solver_name);

    printf("\n==========  omsu_print_debug end  ==========\n\n");
}

omsi_int initializeNonlinearSystems(DATA *data, threadData_t *threadData) {
    //TODO: implement for new data structure
    return -1;
}

omsi_int initializeLinearSystems(DATA *data, threadData_t *threadData) {
    //TODO: implement for new data structure
    return -1;
}

omsi_int initializeMixedSystems(DATA *data, threadData_t *threadData) {
    //TODO: implement for new data structure
    return -1;
}

void initializeStateSetJacobians(DATA *data, threadData_t *threadData) {
    //TODO: implement for new data structure
}

void setZCtol(omsi_real relativeTol) {
    //ToDO: implement
}

void copyStartValuestoInitValues(DATA *data) {
    //ToDo: implement for new data structure
}

omsi_int initialization(DATA *data, threadData_t *threadData, omsi_string pInitMethod, omsi_string pInitFile, omsi_real initTime) {
    //ToDo: implement for new data structure
    return -1;
}

void initSample(DATA* data, threadData_t *threadData, omsi_real startTime, omsi_real stopTime) {
    //ToDo: implement for new data structure
}

void initDelay(DATA* data, omsi_real startTime) {
    //ToDo: implement for new data structure
}

omsi_real getNextSampleTimeFMU(DATA *data) {
    //ToDo: implement for new data structure
    return -1;
}

void setAllVarsToStart(DATA *data) {
    //ToDo: implement for new data structure
}

void setAllParamsToStart(DATA *data) {
    //ToDo: implement for new data structure
}

omsi_int freeNonlinearSystems(DATA *data, threadData_t *threadData) {
    //ToDo: implement for new data structure
    return -1;
}

omsi_int freeMixedSystems(DATA *data, threadData_t *threadData) {
    //ToDo: implement for new data structure
    return -1;
}

omsi_int freeLinearSystems(DATA *data, threadData_t *threadData) {
    //ToDo: implement for new data structure
    return -1;
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
    return fmi2False;
}

void mmc_catch_dummy_fn (void) {
    //ToDo: delete
}

/*
void omsic_model_setup_data (osu_t* OSU) {
    //ToDo: delete
}
*/

extern void omsu_initialization(omsi_t* osu_data) {
    //ToDo: delete
}

extern void setDefaultStartValues(osu_t* OSU) {
    //ToDo: delete
}
