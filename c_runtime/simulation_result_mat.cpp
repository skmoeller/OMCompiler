/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2010, Linköpings University,
 * Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF THIS OSMC PUBLIC
 * LICENSE (OSMC-PL). ANY USE, REPRODUCTION OR DISTRIBUTION OF
 * THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE OF THE OSMC
 * PUBLIC LICENSE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from Linköpings University, either from the above address,
 * from the URL: http://www.ida.liu.se/projects/OpenModelica
 * and in the OpenModelica distribution.
 *
 * This program is distributed  WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS
 * OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

/*

 */

#include "simulation_result_mat.h"
#include <cstring>
#include <cstdlib>


simulation_result_mat::simulation_result_mat(const char* filename, 
					     double tstart, double tstop)
  : simulation_result(filename,numpoints),fp(),data2HdrPos(-1),ntimepoints(0)
{
  const char *timeValName = "Time";
  const char *timeValDesc = "Simulation time [s]";
  const char Aclass[] = "A1 bt. ir1 na  Tj  re  ac  nt  so   r   y   ";
  
  const int rank = 9;
  int dims[rank] = { 1, globalData->nStates, globalData->nStates, 
		     globalData->nAlgebraic, globalData->intVariables.nAlgebraic,
		     globalData->boolVariables.nAlgebraic, 
		     globalData->nParameters, globalData->intVariables.nParameters,
		     globalData->boolVariables.nParameters };
  const char **names[rank] = { &timeValName,
			       globalData->statesNames,
			       globalData->stateDerivativesNames,
			       globalData->algebraicsNames,
			       globalData->int_alg_names,
			       globalData->bool_alg_names,
			       globalData->parametersNames,
			       globalData->int_param_names,
			       globalData->bool_param_names,
                             };
  const char **descriptions[rank] = { &timeValDesc, 
				      globalData->statesComments,
				      globalData->stateDerivativesComments,
				      globalData->algebraicsComments,
				      globalData->int_alg_comments,
				      globalData->bool_alg_comments,
				      globalData->parametersComments,
				      globalData->int_param_comments,
				      globalData->bool_param_comments 
                                    };
  const int nVars = globalData->nStates*2+globalData->nAlgebraic
    +globalData->intVariables.nAlgebraic +globalData->boolVariables.nAlgebraic;
  const int nParams = globalData->nParameters+globalData->intVariables.nParameters
    +globalData->boolVariables.nParameters;

  char *stringMatrix = NULL;
  int rows, cols;
  double *doubleMatrix = NULL;
  
  try {
    // open file
    fp.open(filename, std::ofstream::binary|std::ofstream::trunc);
    if (!fp) throw SimulationResultFileOpenException();
    
    // write `AClass' matrix
    writeMatVer4Matrix("Aclass", 4, 11, Aclass, true); 
    
    // flatten variables' names
    flattenStrBuf(rank, dims, names, stringMatrix, rows, cols, true);
    // write `name' matrix
    writeMatVer4Matrix("name", rows, cols, stringMatrix, true);
    delete[] stringMatrix; stringMatrix = NULL;
    
    // flatten variables' comments
    flattenStrBuf(rank, dims, descriptions, stringMatrix, rows, cols, false);
    // write `description' matrix
    writeMatVer4Matrix("description", rows, cols, stringMatrix, true);
    delete[] stringMatrix; stringMatrix = NULL;

    // generate dataInfo table
    generateDataInfo(doubleMatrix, rows, cols, globalData, nVars, nParams);
    // write `dataInfo' matrix
    writeMatVer4Matrix("dataInfo", cols, rows, doubleMatrix, false);
    delete[] doubleMatrix; doubleMatrix = NULL;
    

    // generate `data_1' matrix (with parameter data)
    generateData_1(doubleMatrix, rows, cols, globalData, tstart, tstop);
    //  write `data_1' matrix
    writeMatVer4Matrix("data_1", cols, rows, doubleMatrix, false);
    delete[] doubleMatrix; doubleMatrix = NULL;
    
    data2HdrPos = fp.tellp();
    // write `data_2' header
    writeMatVer4MatrixHeader("data_2", nVars+1, 0, false);

    fp.flush();

  } catch(...) {
    fp.close();
    delete[] stringMatrix;
    delete[] doubleMatrix;
    throw;
  }
}
simulation_result_mat::~simulation_result_mat()
{
  int nVars = 1+globalData->nStates*2+globalData->nAlgebraic
    +globalData->intVariables.nAlgebraic +globalData->boolVariables.nAlgebraic;
  // this is a bad programming practice - closing file in destructor,
  // where a proper error reporting can't be done
  if (fp) {
    try {
      fp.seekp(data2HdrPos);
      writeMatVer4MatrixHeader("data_2", nVars, ntimepoints, false);
      fp.close();
    } catch (...) {
      // just ignore, we are in destructor
    }
  }
}

void simulation_result_mat::emit()
{
  double datPoint;

  // that does not belong here
  storeExtrapolationData();

  // this is done wrong -- a buffering should be used
  // although ofstream does have some buffering, but it is not enough and 
  // not for this purpose
  fp.write((char*)&globalData->timeValue,sizeof(double));
  fp.write((char*)globalData->states,sizeof(double)*globalData->nStates);
  fp.write((char*)globalData->statesDerivatives,sizeof(double)*globalData->nStates);
  fp.write((char*)globalData->algebraics,sizeof(double)*globalData->nAlgebraic);
  for(fortran_integer i = 0; i < globalData->intVariables.nAlgebraic; ++i) {
    datPoint = (double)globalData->intVariables.algebraics[i];
    fp.write((char*)&datPoint,sizeof(double));
  }
  for(fortran_integer i = 0; i < globalData->boolVariables.nAlgebraic; ++i) {
    datPoint = (double)globalData->boolVariables.algebraics[i];
    fp.write((char*)&datPoint,sizeof(double));
  }

  if (!fp) throw SimulationResultBaseException();
  ++ntimepoints;
}

// from an array of string creates flatten 'char*'-array suitable to be 
// stored as MAT-file matrix
static inline void fixDerInName(char *str, size_t len)
{
  char* dot;
  if (len < 6) return;
  
  // check if name start with "der(" and includes at least one dot
  while (strncmp(str,"der(",4) == 0 && (dot = strrchr(str,'.')) != NULL) {
    size_t pos = (size_t)(dot-str)+1;
    // move prefix to the begining of string :"der(a.b.c.d)" -> "a.b.c.b.c.d)"
    for(size_t i = 4; i < pos; ++i)
      str[i-4] = str[i];
    // move "der(" to the end of prefix
    // "a.b.c.b.c.d)" -> "a.b.c.der(d)"
    strncpy(&str[pos-4],"der(",4);
  }
}

long simulation_result_mat::flattenStrBuf(int rank, const int *dims, 
					  const char ** const src[],
					  char* &dest, int& longest, int& nstrings,
					  bool fixNames)
{
  int i,j;
  int len;
  nstrings = 0;
  longest = 0; // the longest-string length

  // calculate required size
  for (i = 0; i < rank; ++i) {
    // add number of all strings
    nstrings += dims[i];
    // get length of longest string
    for(j = 0; j < dims[i]; ++j) {
      len = strlen(src[i][j]);
      if (len > longest) longest = len;
    }
  }

  // allocate memory
  dest = new char[longest*nstrings+1];
  if (!dest) throw SimulationResultMallocException();
  // copy data
  char *ptr = dest;
  for (i = 0; i < rank; ++i)
    for (j = 0; j < dims[i]; ++j) {
      strncpy(ptr,src[i][j],longest);
      if (fixNames) fixDerInName(ptr,strlen(src[i][j]));
      ptr += longest;
    }
  // return the size of the `dest' buffer
  return (longest*nstrings);
}

// writes MAT-file matrix header to file
void simulation_result_mat::writeMatVer4MatrixHeader(const char *name,
						     int rows, int cols,
						     bool is_text)
{
  typedef struct {
    long type;
    long mrows;
    long ncols;
    long imagf;
    long namelen;
  } MHeader_t;
  const int endian_test = 1;
  MHeader_t hdr;
  
  // create matrix header structure
  hdr.type = 1000*((*(char*)&endian_test) == 0) + (is_text? 51 : 0);
  hdr.mrows = rows;
  hdr.ncols = cols;
  hdr.imagf = 0;
  hdr.namelen = strlen(name)+1;
  // write header to file
  fp.write((char*)&hdr, sizeof(MHeader_t));
  if (!fp) throw SimulationResultFileCloseException();
  fp.write(name, sizeof(char)*hdr.namelen);
  if (!fp) throw SimulationResultFileCloseException();
}

void simulation_result_mat::writeMatVer4Matrix(const char *name, 
					       int rows, int cols, 
					       const void *data, bool is_text)
{
  writeMatVer4MatrixHeader(name,rows,cols,is_text);

  // write data
  fp.write((const char*)data, (is_text ? sizeof(char) : sizeof(double))*rows*cols);
  if (!fp) throw SimulationResultFileCloseException();
}


void simulation_result_mat::generateDataInfo(double* &dataInfo, 
					     int& rows, int& cols,
					     const sim_DATA *mdl_data,
					     int nVars, int nParams)
{
  //size_t nVars = mdl_data->nStates*2+mdl_data->nAlgebraic;
  //rows = 1+nVars+mdl_data->nParameters+mdl_data->nVarsAliases;
  size_t ccol = 0; // current column - index offset

  // assign rows & cols
  rows = 1+ nVars + nParams;
  cols = 4;
  
  dataInfo = new double[rows*cols];
  if (dataInfo == NULL) throw SimulationResultMallocException();
  // time variable
  dataInfo[0] = 0.0;
  dataInfo[1] = 1.0;
  dataInfo[2] = 0.0;
  dataInfo[3] = -1.0;
  ccol += 4;
  // continuous and discrete variables
  for(size_t i = 0; i < (size_t)nVars; ++i) {
    // row 1 - which table
    dataInfo[ccol+4*i] = 2.0;
    // row 2 - index of var in table (variable 'Time' have index 1)
    dataInfo[ccol+4*i+1] = i+2.0;
    // row 3 - linear interpolation == 0
    dataInfo[ccol+4*i+2] = 0.0;
    // row 4 - not defined outside of the defined time range == -1
    dataInfo[ccol+4*i+3] = -1.0;
  }
  ccol += nVars*4;
  // parameters and constants
  for(size_t i = 0; i < (size_t)nParams; ++i) {
    // col 1 - which table
    dataInfo[ccol+4*i] = 1.0;
    // col 2 - index of var in the table (first parameter has index 2)
    dataInfo[ccol+4*i+1] = i+2.0;
    // col 3 (== 0 <- interpolation doesn't matter here)
    dataInfo[ccol+4*i+2] = 0.0;
    // col 4 - keep first/last value outside of time range
    dataInfo[ccol+4*i+3] = 0.0;

  }
  //ccol += mdl_data->nParameters*4;
}

void simulation_result_mat::generateData_1(double* &data_1, 
					   int& rows, int& cols,
					   const sim_DATA *mdl_data,
					   double tstart, double tstop)
{
  int offset;
  // calculate number of rows and columns
  rows = 2;
  cols = 1+mdl_data->nParameters+mdl_data->intVariables.nParameters
    +mdl_data->boolVariables.nParameters;
  // allocate data buffer
  data_1 = new double[rows*cols];
  if (data_1 == NULL) 
    throw SimulationResultMallocException();
  data_1[0] = tstart; // start time
  data_1[cols] = tstop; // stop time
  offset = 1;
  // double variables
  for(fortran_integer i = 0; i < mdl_data->nParameters; ++i) {
      data_1[offset+i] = mdl_data->parameters[i];
      data_1[offset+i+cols] = mdl_data->parameters[i];
  }
  offset += mdl_data->nParameters;
  // integer variables
  for(fortran_integer i = 0; i < mdl_data->intVariables.nParameters; ++i) {
    data_1[offset+i] = (double)mdl_data->intVariables.parameters[i];
    data_1[offset+i+cols] = (double)mdl_data->intVariables.parameters[i];
  }
  offset += mdl_data->intVariables.nParameters;
  // bool variables
  for(fortran_integer i = 0; i < mdl_data->boolVariables.nParameters; ++i) {
    data_1[offset+i] = (double)mdl_data->boolVariables.parameters[i];
    data_1[offset+i+cols] = (double)mdl_data->boolVariables.parameters[i];
  }
	 
}
