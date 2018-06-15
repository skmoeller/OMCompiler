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

#ifndef OMSU_INPUT_JSON_H
#define OMSU_INPUT_JSON_H

#include "omsi.h"
#include "omsu_common.h"

#include "util/omc_mmap.h"

/* typedef structs */
typedef struct {
  omsi_unsigned_int size;
  omsi_string data;
} omsi_mmap;



/* function prototypes */
omsi_status omsu_process_input_json(omsi_t* osu_data, omsi_string fileName, omsi_string fmuGUID, omsi_string instanceName, omsi_callback_functions* functions);
omsi_mmap omsi_mmap_open_read_unix(omsi_string fileName);
static inline omsi_string skipSpace(omsi_string str);
static inline omsi_string assertStringValue(omsi_string str, omsi_string value);
static inline omsi_string assertChar (omsi_string str, omsi_char c);
omsi_bool omsu_assertCharOrEnd (const char *str, char c);
static inline omsi_string assertNumber(omsi_string str, omsi_real expected);
static inline omsi_string skipObjectRest(omsi_string str, omsi_int first);
static omsi_string skipValue(omsi_string str);
static inline omsi_string skipFieldIfExist(omsi_string str,omsi_string name);
omsi_string readEquation(omsi_string str, equation_info_t* equation_info, omsi_unsigned_int expected_id);
omsi_string readEquations(omsi_string str, model_data_t* model_data);
static void readInfoJson(omsi_string str, model_data_t* model_data);










#endif
