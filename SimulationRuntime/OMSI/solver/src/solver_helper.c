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

#include <solver_helper.h>


/*
 * Sets first numTargetPlaces variables in vars to tragetValue.
 *
 * Sets vars to targetValue for each place specified in targetPlaces.
 * numTargetPlaces tells length of array targetPlaces.
 */
omsi_status omsu_set_omsi_value (omsi_values*       vars,
                                 omsi_index_type**  targetPlaces,       /* input: pointer to array of omsi_index_type */
                                 omsi_unsigned_int  numTargetPlaces,
                                 omsi_real          targetValue)        /* input of omsi_real*, gets typecasted for integers */
{
    omsi_unsigned_int i;
    omsi_unsigned_int j_real = 0, j_int = 0;

    omsi_status status = omsi_ok;

    for(i=0; i<numTargetPlaces; i++) {
        switch (targetPlaces[i]->type) {
        case OMSI_TYPE_REAL:
            vars->reals[j_real] = targetValue;
            j_real++;
            break;
        case OMSI_TYPE_INTEGER:
            vars->ints[j_int] = (omsi_int)targetValue;
            j_int++;
            if (abs(vars->ints[j_int]-targetValue)>1e-16) {
                /* ToDo: Add warning: Set omsi_value of type omsi_int to non integer value. Result was typecastet. */
                status = omsi_warning;
            }
            break;
        default:
            /* ToDo: Add error case */
            return omsi_error;
        }
    }
    return status;
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
    savedVars = (omsi_values*) global_callback->allocateMemory(1, size);
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
            /* ToDo: Add error case */
            global_callback->freeMemory(savedVars);
            return NULL;
        }
    }

    return savedVars;
}


/*
 * Reads derivative matrix informations from generated functions and copies them
 * into returned matrix in row-major order or column-major order.
 * Only for quadratic matrices.
 *
 * Use rowMajorOrder = true for e.g.
 * Use rowMajorOrder = false for e.g. LAPACK solver
 */
omsi_status omsi_get_derivative_matrix (omsi_algebraic_system_t*    algebraicSystem,
                                        omsi_bool                   rowMajorOrder,
                                        omsi_real*                  matrix) {

    omsi_unsigned_int dim = algebraicSystem->jacobian->n_output_vars;
    omsi_unsigned_int dim1, dim2;
    omsi_unsigned_int i_row, i_column;

    omsi_real* tmpColumnVector = (omsi_real*) global_callback->allocateMemory(dim, sizeof(omsi_real));
    omsi_real* seedVector = (omsi_real*) global_callback->allocateMemory(dim, sizeof(omsi_real));            /*seed vector, initialized with zeros */

    if (!tmpColumnVector || !seedVector) {
        /* ToDo: add error case out of memory */
        return omsi_error;
    }

    if (rowMajorOrder) {
        dim1 = dim;         /* element (i,j) of dim times dim matrix is */
        dim2 = 0;           /* matrix[i*dim+j] */
    }
    else {
        dim1 = 0;           /* element (i,j) of dim times dim matrix is */
        dim2 = dim;         /* matrix[i+j*dim] */
    }

    for (i_column=0; i_column<dim; i_column++) {
        seedVector[i_column] = 1;       /* set seed vector for current row */
        /* evaluate derivative matrix column wise
        algebraicSystem->jacobian->evaluate(algebraicSystem->jacobian,
                                            seedVector,
                                            tmpColumnVector);*/
        for (i_row=0; i_row<dim; i_row++) {
            matrix[i_row*dim1+i_column*dim2] = tmpColumnVector[i_row];
        }
        seedVector[i_column] = 0;
    }

    /* free memory */
    global_callback->freeMemory(tmpColumnVector);
    global_callback->freeMemory(seedVector);

    return omsi_ok;
}








