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

#include "omsu/omsu_common.h"


/*
 * gets real number of struct OSU with value reference vr
 */
fmi2Real getReal (osu_t* OSU, const fmi2ValueReference vr) {
    fmi2Real output = (fmi2Real) OSU->osu_data->sim_data.model_vars_and_params->reals[vr];
    return output;
}

/*
 * sets real number of struct OSU for index reference vr with value
 */
fmi2Status setReal(osu_t* OSU, const fmi2ValueReference vr, const fmi2Real value) {
    OSU->osu_data->sim_data.model_vars_and_params->reals[vr] = (double) value;
    return fmi2OK;
}


/*
 * gets integer number of struct OSU with value reference vr
 */
fmi2Integer getInteger (osu_t* OSU, const fmi2ValueReference vr) {
    fmi2Integer output = (fmi2Integer) OSU->osu_data->sim_data.model_vars_and_params->ints[vr];
    return output;
}

/*
 * sets integer number of struct OSU for index reference vr with value
 */
fmi2Status setInteger(osu_t* OSU, const fmi2ValueReference vr, const fmi2Integer value) {
    OSU->osu_data->sim_data.model_vars_and_params->ints[vr] = (int) value;
    return fmi2OK;
}


/*
 * gets boolean variable of struct OSU with value reference vr
 */
fmi2Boolean getBoolean (osu_t* OSU, const fmi2ValueReference vr) {
    fmi2Boolean output = (fmi2Boolean) OSU->osu_data->sim_data.model_vars_and_params->bools[vr];
    return output;
}

/*
 * sets boolean variable of struct OSU for index reference vr with value
 */
fmi2Status setBoolean(osu_t* OSU, const fmi2ValueReference vr, const fmi2Boolean value) {
    OSU->osu_data->sim_data.model_vars_and_params->bools[vr] = (bool) value;
    return fmi2OK;
}


/*
 * gets stringe of struct OSU with value reference vr
 * ToDO: implement string in sim_data_t
 */
fmi2String getString (osu_t* OSU, const fmi2ValueReference vr) {
    return "ERROR";
}

/*
 * sets string of struct OSU for index reference vr with value
 *  * ToDO: implement string in sim_data_t
 */
fmi2Status setString(osu_t* OSU, const fmi2ValueReference vr, const fmi2String value) {
    return fmi2Error;
}




