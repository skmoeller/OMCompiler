/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2014, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

package CodegenEquations
" file:        CodegenEquations.tpl
  package:     CodegenEquations
  description: Code generation using Susan templates for 
               OpenModelica Simulation Inferface (OMSI) equation related templates
"



import interface SimCodeTV;
import interface SimCodeBackendTV;
import CodegenUtil;
import CodegenC;
import CodegenCFunctions;
import CodegenUtilSimulation.*;

template equationFunctionPrototypes(SimEqSystem eq, String modelNamePrefixStr)
 "Generates prototype for an equation function"
::=
  let ix = CodegenUtilSimulation.equationIndex(eq)
  <<
  void <%CodegenUtil.symbolName(modelNamePrefixStr,"eqFunction")%>_<%ix%>(Data_Struct_something* data, Data_Struct_something* threadData);<%\n%>
  >>
end equationFunctionPrototypes;


template equationFunction(SimEqSystem eq, Context context, String modelNamePrefixStr)
 "Generates C-function for an equation evaluation"
::=
  let ix = CodegenUtilSimulation.equationIndex(eq)
  let equationInfos = dumpEqs(fill(eq,1))

  let &varDecls = buffer ""
  let &auxFunction = buffer ""
  let equationCode = equationCStr(eq, context, &varDecls, &auxFunction) //sinnvollen context angeben

  <<
  /*
  <%equationInfos%>
  */
  void <%CodegenUtil.symbolName(modelNamePrefixStr,"eqFunction")%>_<%ix%>(sim_data_t* sim_data, double* writeData){
    const int equationIndexes[2] = {1,<%ix%>};
    <%varDecls%>
    <%auxFunction%>
    <%equationCode%>
  }
  >>
end equationFunction;


template equationCStr(SimEqSystem eq, Context context, Text &varDecls, Text &auxFunction)
 "Generates an equation that is just a simple assignment."
::=
match eq
case SES_SIMPLE_ASSIGN(__) then
  let &preExp = buffer ""
  let expPart = CodegenCFunctions.daeExp(exp, context, &preExp, &varDecls, &auxFunction)
  let crefStr = CodegenCFunctions.contextCref(cref, context, &auxFunction)
  <<
  <%crefStr%> = <%expPart%>;
  >>
case SES_LINEAR(lSystem=ls as LINEARSYSTEM(__)) then    //only for rectangular case
  let dimLinearSystem = ls.nUnknowns
  let crefStr = ""
  <<
  ERROR: LINEAR SYSTEM NOT IMPLEMENTED YET!

  int sucess;

  /* allocate memory for LAPACK */
  void *lapackData;
  allocateLapackData(<%dimLinearSystem%>, &lapackData);

  /* set lapackData  */
  setLapackData(lapackData, omsiData);

  /* solve linear equation using LAPACK dgesv */
  sucess = solveLapack(lapackData, omsiData, linearSystem);
  if !sucess {
    ERROR: ...
  }

  /* copy solution in sim_data */
  memcpy(<%crefStr%>, lapackData->A, sizeof(double)*<%dimLinearSystem%>);

  /* free memory */
  freeLapackData(&lapackData);
  >>
end equationCStr;


template equationCall(SimEqSystem eq, String modelNamePrefixStr)
 "Generates call function for evaluating functions"
::=
  let ix = ""
  <<
  <%CodegenUtil.symbolName(modelNamePrefixStr,"eqFunction")%>_<%ix%>(data);
  >>
end equationCall;


template generateEquationFiles(list<SimEqSystem> allEquations, String fileNamePrefix)
"Generates content of fileNamePrefix_eqns.c"
::=
  let eqFuncs = ""
  let _ = allEquations |> eqn => (
    let &eqFuncs += equationFunction(eqn, contextOMSI, fileNamePrefix) + "\n\n"
    <<>>
  )
  let eqCalls = ""
  let _ =  allEquations |> eqn => (
    let &eqCalls += equationCall(eqn, fileNamePrefix) + "\n"
    <<>>
  )

  <<
  #include "omsi.h"
  #include "<%fileNamePrefix%>_eqns.h"
  
  /* Equation functions */
  <%eqFuncs%>

  /* Equations evaluation */
  int evalEquations(sim_data_t sim_data){

    <%eqCalls%>

    return 0;
  }
  >>
end generateEquationFiles;


template generateEquationFilesHeader(list<SimEqSystem> allEquations, String fileNamePrefix)
"Generates content of header file fileNamePrefix_eqns.h"
::=
  let eqFuncsPrototypes = ""
            let _ = allEquations |> eqn => (
            let &eqFuncsPrototypes += equationFunctionPrototypes(eqn, fileNamePrefix)
            <<>>
            )

  <<
  #ifndef <%fileNamePrefix%>_eqns
  #define <%fileNamePrefix%>_eqns

  /* Equation functions prototypes */
  #if defined(__cplusplus)
  extern "C" {
  #endif

  int evalEquations(Blablabla);
  <%eqFuncsPrototypes%>

  #if defined(__cplusplus)
  }
  #endif

  #endif
  >>
end generateEquationFilesHeader;


annotation(__OpenModelica_Interface="backend");
end CodegenEquations;