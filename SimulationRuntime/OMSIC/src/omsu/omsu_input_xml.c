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

#include"omsu/omsu_input_xml.h"

/* forward global functions */
omsi_callback_allocate_memory global_allocateMemory;
omsi_callback_free_memory global_freeMemory;


/*
 *
 */
omsi_string omsu_findHashStringStringNull(hash_string_string *ht, omsi_string key) {
  hash_string_string *res;
  HASH_FIND_STR( ht, key, res );
  return res ? res->val : NULL;
}

omsi_string omsu_findHashStringStringEmpty(hash_string_string *ht, omsi_string key)
{
  omsi_string res = omsu_findHashStringStringNull(ht,key);
  return res ? res : "";
}

omsi_string omsu_findHashStringString(hash_string_string *ht, omsi_string key) {
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
void omsu_addHashLongVar(hash_long_var **ht, omsi_long key, omc_ScalarVariable *val) {
  hash_long_var *v = (hash_long_var*) global_allocateMemory(1, sizeof(hash_long_var));      // ToDo: where is this memory freed?
  v->id=key;
  v->val=val;
  HASH_ADD_INT( *ht, id, v );
}

/*
 *
 */
void omsu_addHashStringString(hash_string_string **ht, omsi_string key, omsi_string val) {
  hash_string_string *v = (hash_string_string*) global_allocateMemory(1, sizeof(hash_string_string));
  v->id=strdup(key);
  v->val=strdup(val);
  HASH_ADD_KEYPTR( hh, *ht, v->id, strlen(v->id), v );
}

/* reads integer value from a string */
void omsu_read_value_int(omsi_string s, omsi_int* res, omsi_int default_value) {
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
void omsu_read_value_uint(omsi_string s, omsi_unsigned_int* res) {
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
omc_ScalarVariable** omsu_findHashLongVar(hash_long_var *ht, omsi_long key) {
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
void omsu_read_value_real(omsi_string s, omsi_real* res, omsi_real default_value) {
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
void omsu_read_value_bool(omsi_string s, omsi_bool* res) {
    *res = 0 == strcmp(s, "true");
}

void omsu_read_value_bool_default (omsi_string s, omsi_bool* res, omsi_bool default_bool) {
    if (s == NULL || *s == '\0') {
        *res = default_bool;
    }
    *res = 0 == strcmp(s, "true");
}

/*
 *  Reads modelica_string value from a string.
 *  Allocates memory for string and copies string s into str.
 */
void omsu_read_value_string(omsi_string s, omsi_string *str) {
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


void XMLCALL startElement(void *userData, omsi_string name, omsi_string *attr) {
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
    /* transform to omsi_long  */
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

void XMLCALL endElement(void *userData, omsi_string name) {
  /* do nothing! */
}

/* deallocates memory for oms_ModelInput struct */
void omsu_free_ModelInput(omc_ModelInput mi, omsi_t* omsi_data, const omsi_callback_free_memory freeMemory) {

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
