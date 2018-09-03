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

/*
 * ToDo: Description
 */

#include <omsu_common.h>

#define UNUSED(x) (void)(x)     // ToDo: delete later


/*
 * gets real number of struct OSU with value reference vr
 */
omsi_real getReal (osu_t* OSU, const omsi_unsigned_int vr) {
    omsi_real output = (omsi_real) OSU->osu_data->sim_data->model_vars_and_params->reals[vr];
    return output;
}


/*
 * sets real number of struct OSU for index reference vr with value
 */
omsi_status setReal(osu_t* OSU, const omsi_unsigned_int vr, const omsi_real value) {
    OSU->osu_data->sim_data->model_vars_and_params->reals[vr] = (omsi_real) value;
    return omsi_ok;
}


/*
 * gets integer number of struct OSU with value reference vr
 */
omsi_int getInteger (osu_t* OSU, const omsi_unsigned_int vr) {
    omsi_int output = (omsi_int) OSU->osu_data->sim_data->model_vars_and_params->ints[vr];
    return output;
}

/*
 * sets integer number of struct OSU for index reference vr with value
 */
omsi_status setInteger(osu_t* OSU, const omsi_unsigned_int vr, const omsi_int value) {
    OSU->osu_data->sim_data->model_vars_and_params->ints[vr] = (omsi_int) value;
    return omsi_ok;
}


/*
 * gets boolean variable of struct OSU with value reference vr
 */
omsi_bool getBoolean (osu_t* OSU, const omsi_unsigned_int vr) {
    omsi_bool output = (omsi_bool) OSU->osu_data->sim_data->model_vars_and_params->bools[vr];
    return output;
}

/*
 * sets boolean variable of struct OSU for index reference vr with value
 */
omsi_status setBoolean(osu_t* OSU, const omsi_unsigned_int vr, const omsi_bool value) {
    OSU->osu_data->sim_data->model_vars_and_params->bools[vr] = (omsi_bool) value;
    return omsi_ok;
}


/*
 * gets stringe of struct OSU with value reference vr
 * ToDO: implement string in sim_data_t
 */
omsi_string getString (osu_t* OSU, const omsi_unsigned_int vr) {
    UNUSED(OSU); UNUSED(vr);
    return "ERROR";
}

/*
 * sets string of struct OSU for index reference vr with value
 *  * ToDO: implement string in sim_data_t
 */
omsi_status setString(osu_t* OSU, const omsi_unsigned_int vr, const omsi_string value) {
    UNUSED(OSU); UNUSED(vr); UNUSED(value);
    return omsi_error;
}




