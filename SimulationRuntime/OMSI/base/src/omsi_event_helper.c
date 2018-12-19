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

/** \file omsi_event_helper.c
 */

#include <omsi_global.h>

#include <omsi_event_helper.h>


/**
 * \brief
 *
 * Returns preValue of zeroCrossing in `modelContinuousTimeMode` and value of
 * the zeroCrossing in `modelEventMode` and sets `pre_zerocrossing_vars` in
 * `modelInitializationMode`.
 *
 * @param this_function
 * @param new_zero_crossing
 * @param index
 * @param is_event_mode
 * @return
 */
omsi_bool omsi_function_zero_crossings (omsi_function_t*    this_function,
                                        omsi_bool           new_zero_crossing,
                                        omsi_unsigned_int   index,
                                        ModelState          model_state)
{
    if (this_function->zerocrossings_vars == NULL
            || this_function->pre_zerocrossings_vars == NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Evaluate: in omsi_function_zero_crossings: No memory for zero crossings allocated.");
        return new_zero_crossing;
    }
    /* Update zerocrossing variable */
    if (new_zero_crossing) {
        this_function->zerocrossings_vars[index] = 1;
    } else {
        this_function->zerocrossings_vars[index] = -1;
    }


    /* Return bool */
    if (model_state == modelEventMode ) {
        return this_function->zerocrossings_vars[index]>0;
    } else if (model_state == modelContinuousTimeMode) {
        return this_function->pre_zerocrossings_vars[index]>0;
    } else if (model_state == modelInitializationMode) {
        this_function->pre_zerocrossings_vars[index] = this_function->zerocrossings_vars[index];
        return this_function->zerocrossings_vars[index]>0;
    } else {
        return this_function->zerocrossings_vars[index]>0;
    }
}
