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

/** \file omsi_solver.h
 * Solver application interface to solve linear and non-linear equation systems
 * in shape of
 *   A*x = b or F(x) = 0,
 * with square Matrix A of dimension n or F:R^n --> R^n.
 *
 */


/** \defgroup SOLVER OMSI Solver Library
 *
 * Long description of group.
 */


/** @addtogroup SOLVER  OMSI Solver Library
  * \ingroup OMSI
  *  @{ */

#ifndef SOLVER_API_H
#define SOLVER_API_H


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Type definitions of variables
 */
#ifdef _OMSI_H

typedef omsi_unsigned_int   solver_unsigned_int;
typedef omsi_real           solver_real;
typedef omsi_int            solver_int;
typedef omsi_long           solver_long;
typedef omsi_bool           solver_bool;
#define solver_true         omsi_true
#define solver_false        omsi_false
#ifndef true
#define true omsi_true
#endif
#ifndef false
#define false omsi_false
#endif
typedef omsi_char           solver_char;
typedef omsi_string         solver_string;
#else
typedef unsigned int        solver_unsigned_int;
typedef double              solver_real;
typedef int                 solver_int;
typedef long                solver_long;
typedef int                 solver_bool;
#define solver_true  1
#define solver_false 0
typedef char                solver_char;
typedef const solver_char*  solver_string;
#endif


/** \file solver_api.h
 */


/* callback functions */
typedef void*   (*solver_callback_allocate_memory)  (solver_unsigned_int, solver_unsigned_int);
typedef void    (*solver_callback_free_memory)      (void*);

typedef void    (*solver_set_matrix_A)              (solver_unsigned_int, solver_unsigned_int, solver_real*);
typedef void    (*solver_set_F_func)                (void);


/* global functions */
solver_callback_allocate_memory solver_allocateMemory;
solver_callback_free_memory solver_freeMemory;

typedef enum {
    solver_lapack,
    solver_newton,
    solver_extern,
    solver_unregistered
}solver_name;


typedef enum {
    solver_uninitialized,
    solver_initialized,
    solver_ready,
    solver_error
}solver_state;



/** \brief General solver structure.
 *
 *  Containing informations and function callbacks for a solver instance.
 *  Solver specific data is stored in `specific_data` in it's own data formats.
 */
typedef struct solver_data {
    solver_name         name;
    int                 linear;     /* 1 if linear, 0 if non-linear */

    solver_state        state;

    solver_unsigned_int dim_n;
    void*               specific_data;

    solver_set_matrix_A set_A_element;
    solver_set_matrix_A get_A_element;
    /*solver_set_vector_b set_b_element;
    solver_get_vector_b get_b_element;*/

    solver_set_F_func   set_F_func;
} solver_data;




#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif

/** @} */
