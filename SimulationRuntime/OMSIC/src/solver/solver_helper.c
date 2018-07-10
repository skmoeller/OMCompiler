/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2014, Open Source Modelica Consortium (OSMC),
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



#include "solver/solver_helper.h"


/* forward global functions */
omsi_callback_allocate_memory   global_allocateMemory;
omsi_callback_free_memory       global_freeMemory;



/*
 * Sets vars to targetValue for each place specified in targetPlaces.
 * numTargetPlaces tells length of array targetPlaces.
 */
omsi_status omsu_set_omsi_value (omsi_values*       vars,
                                 omsi_index_type**  targetPlaces,       /* input: pointer to array of omsi_index_type */
                                 omsi_unsigned_int  numTargetPlaces,
                                 void*              targetValue)        /* input of type omsi_int* or omsi_real* */
{
    omsi_unsigned_int i;
    omsi_unsigned_int mappedIndex;

    for(i=0; i<numTargetPlaces; i++) {
        targetPlaces[i]->index = mappedIndex;

        switch (targetPlaces[i]->type) {
        case OMSI_TYPE_REAL:
            vars->reals[mappedIndex] = targetValue;
            break;
        case OMSI_TYPE_INTEGER:
            vars->ints[mappedIndex] = targetValue;
            break;
        default:
            //ToDo: Add error case
            return omsi_error;
        }
    }
    return omsi_ok;
}


/*
 * Sets vars to targetValues for places specified in targetPlaces.
 * numTargetPlaces tells length of array targetPlaces respectively length of
 * array targetValues.
 */
omsi_status omsu_set_omsi_values (omsi_values*      vars,
                                  omsi_index_type** targetPlaces,   /* input: pointer to array of omsi_index_type */
                                  omsi_values*      targetValues) {

    omsi_unsigned_int i;
    omsi_unsigned_int j1 = 0, j2 = 0;
    omsi_unsigned_int mappedIndex;
    omsi_unsigned_int numTargetPlaces;

    numTargetPlaces = targetValues->n_reals + targetValues->n_ints
                      + targetValues->n_bools;

    for(i=0; i<numTargetPlaces; i++) {
        targetPlaces[i]->index = mappedIndex;

        switch (targetPlaces[i]->type) {
        case OMSI_TYPE_REAL:
            vars->reals[mappedIndex] = targetValues->reals[j1];
            j1++;
            break;
        case OMSI_TYPE_INTEGER:
            vars->ints[mappedIndex] = targetValues->ints[j2];
            j2++;
            break;
        default:
            //ToDo: Add error case
            return omsi_error;
        }
    }
    return omsi_ok;
}

/*
 * Copies in array saveTarget specified values from vars and saves them in
 * returned savedVars.
 */
omsi_values* save_omsi_values (const omsi_values*   vars,
                               omsi_index_type**    saveTarget,
                               omsi_unsigned_int    n_reals,
                               omsi_unsigned_int    n_ints,
                               omsi_unsigned_int    n_bools) {

    omsi_values* savedVars;
    omsi_unsigned_int size, length;
    omsi_unsigned_int i;
    omsi_unsigned_int j1=0, j2=0, j3=0;

    /* allocate memory */
    size = n_reals*sizeof(omsi_real) + n_ints*sizeof(omsi_int)
           + n_bools*sizeof(omsi_bool);
    savedVars = (omsi_values*) global_allocateMemory(1, size);
    if (!savedVars) {
        return NULL;
    }

    /* set length of reals, ints and bools */
    savedVars->n_reals = n_reals;
    savedVars->n_ints = n_ints;
    savedVars->n_bools = n_bools;

    /* copy data */
    length = savedVars->n_reals + savedVars->n_ints + savedVars->n_bools;
    for (i=0; i<length; i++) {
        switch (saveTarget[i]->type) {
        case OMSI_TYPE_REAL:
            savedVars->reals[j1] = vars->reals[saveTarget[i]->index];
            j1++;
            break;
        case OMSI_TYPE_INTEGER:
            savedVars->ints[j2] = vars->ints[saveTarget[i]->index];
            j2++;
            break;
        case OMSI_TYPE_BOOLEAN:
            savedVars->bools[j3] = vars->bools[saveTarget[i]->index];
            j3++;
            break;
        default:
            //ToDo: Add error case
            global_freeMemory(savedVars);
            return NULL;
        }
    }

    return savedVars;
}


omsi_status omsi_get_rhs_eqSystem (void* ) {


    return omsi_ok;
}
















