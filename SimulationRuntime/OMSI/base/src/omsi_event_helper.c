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


/**
 * \brief Helper function for sample events
 *
 * Returns true at time instants `start + i*interval, (i=0, 1, ...)` if in
 * `modelEventMode` and false else.
 *
 * \param [in]  this_function
 * \param [in]  sample_id
 * \param [in]  model_state
 * \return      omsi_bool
 */
omsi_bool omsi_on_sample_event (omsi_function_t*    this_function,
                                omsi_unsigned_int   sample_id,
                                ModelState          model_state)
{
    /* Variables */
    omsi_real modulo_value;
    omsi_real time;
    omsi_sample* sample;
    omsi_bool is_on_sample;

    time = this_function->function_vars->time_value;
    sample = &this_function->sample_events[sample_id];
    is_on_sample = omsi_false;

    if (time>= sample->start_time) {
        modulo_value = (omsi_real)fmod(time+sample->start_time, sample->interval);
        if ((fabs(modulo_value-sample->interval) < 1e-8) || (modulo_value > -1e-8  && modulo_value < 1e-8)) {       /* ToDo: Some epsilon */
            is_on_sample = omsi_true;
        }
    }

    /* Return bool */
    if (model_state == modelEventMode ) {
        return is_on_sample;
    } else if (model_state == modelContinuousTimeMode) {
        return omsi_false;
    } else if (model_state == modelInitializationMode) {
        printf("Not implemented yet!"); fflush(stdout);
        return omsi_false;
    } else {
        return omsi_false;
    }
}


omsi_real omsi_next_sample(omsi_real    time,
                           omsi_sample* sample_event)
{
    /* Variables */
    omsi_real dist_prev_event;

    /* Compute next sample time */
    if (time < sample_event->start_time - 1e-8) {       /* ToDo: minus some epsilon */
        return sample_event->start_time;
    } else {
        dist_prev_event = fmod(time+sample_event->start_time, sample_event->interval);
        if (fabs(dist_prev_event-sample_event->interval) < 1e-8) {  /* nearly on an sample event */
            return time + sample_event->interval;
        } else {
            return time + sample_event->interval - dist_prev_event;
        }
    }
}


omsi_real omsi_compute_next_event_time (omsi_real           time,
                                        omsi_sample*        sample_events,
                                        omsi_unsigned_int   n_sample_events)
{
    /* Variables */
    omsi_real next_event_time;
    omsi_unsigned_int i;

    if (n_sample_events>0) {
        next_event_time = omsi_next_sample(time, &sample_events[0]);
    }
    else {
        next_event_time = -1;
    }

    for (i=1; i<n_sample_events; i++) {
        next_event_time = (omsi_real) fmin(omsi_next_sample(time, &sample_events[i]), next_event_time);
    }

    return next_event_time;
}
