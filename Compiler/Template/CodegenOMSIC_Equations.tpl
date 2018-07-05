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

package CodegenOMSIC_Equations
" file:        CodegenOMSIC_Equations.tpl
  package:     CodegenOMSIC_Equations
  description: Code generation using Susan templates for 
               OpenModelica Simulation Inferface (OMSI) equation related templates
"



import interface SimCodeTV;
import interface SimCodeBackendTV;
import CodegenUtil;
import CodegenC;
import CodegenCFunctions;
import CodegenUtilSimulation;

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
  let equationInfos = CodegenUtilSimulation.dumpEqs(fill(eq,1))

  let &varDecls = buffer ""
  let &auxFunction = buffer ""
  let equationCode = equationCStr(eq, context, &varDecls, &auxFunction) //ToDo: choose right context

  <<
  /*
  <%equationInfos%>
  */
  void <%CodegenUtil.symbolName(modelNamePrefixStr,"eqFunction")%>_<%ix%>(omsi_function_t* this_function){
    const omsi_int equationIndexes[2] = {1,<%ix%>};
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
    /* Linear equation system */
    // ToDo: Log something

    omsi_status status;

    /* solve equation system */
    status = solveLapack(this_function, global_callback_functions);
    if (status != omsi_ok) {
      /* ToDo: error case */
      printf("Solving linear system %i failed at time=%.15g.\n", equationIndexes[1], this_function->function_vars->time_value);
    }


    >>
  case SES_RESIDUAL(__) then
    <<
    SES_RESIDUAL NOT IMPLEMENTED YET
    >>
  else
    <<
    NOT IMPLEMENTED YET
    >>

end equationCStr;


template equationCall(SimEqSystem eq, String modelNamePrefixStr)
 "Generates call function for evaluating functions"
::=
  let ix = CodegenUtilSimulation.equationIndex(eq)
  <<
  <%CodegenUtil.symbolName(modelNamePrefixStr,"eqFunction")%>_<%ix%>(simulation);
  >>
end equationCall;


template generateEquationFiles (SimCode simCode, String fileNamePrefix, String name)
"Generates equations for initialization and simulation"
::=
  match simCode
  case SIMCODE(omsiData=SOME(OMSI_DATA( simulation = simulation as OMSI_FUNCTION(__) ))) then
    <<
    <%generateEquationFile(simulation.equations, fileNamePrefix, name)%>
    >>
    /*
  case SIMCODE(omsiData=SOME(OMSI_DATA( initialization = initialization as OMSI_FUNCTION(__) ))) then
    <<
    >>
    */

end generateEquationFiles;


template generateEquationFile(list<SimEqSystem> equations, String fileNamePrefix, String name)
"Description"
::=
  let eqFuncs = ""
  let _ = equations |> eqn => (
    let &eqFuncs += equationFunction(eqn, contextOMSI, fileNamePrefix) + "\n\n"
    <<>>
  )
  let eqCalls = ""
  let _ =  equations |> eqn => (
    let &eqCalls += equationCall(eqn, fileNamePrefix) + "\n"
    <<>>
  )

  let instantiateCall = ""//instantiateOmsiFunction(equations, fileNamePrefix, name)
  <<
  #include "omsi.h"
  
  /* Equation functions */
  <%eqFuncs%>

  /* Equations evaluation */
  omsi_status <%fileNamePrefix%>_<%name%>(omsi_function_t* simulation, omsi_values* model_vars_and_params){

    <%eqCalls%>

    return omsi_ok;
  }
  
  <%instantiateCall%>
  >>

end generateEquationFile;


/*
template instantiateOmsiFunction(list<OMSIFunction> omsiFunction, String fileNamePrefix, String name)
"Bla"
::=

  let counter = ""
  let algInitCall = ""
  let instAlgSystemCalls =  omsiFunction |> eqn => (match eqn
    case eq as SES_LINEAR(__)
    case eq as SES_NONLINEAR(__) then
      let algInitCall = generateAlgSystemFile(eq, fileNamePrefix, name) // Jeweils eigenes File erzeugen, Aufruf zrückgeben
      <<algebraic_system[i++] =  <%algInitCall%>>>
    else
    <<>>
  )

  <<
  omsi_status <%fileNamePrefix%>_instantiate_omsi_function_<%name%>(omsi_function_t* function_instance) {

    function_instance->evaluate = <%fileNamePrefix%>_<%name%>;

    omsi_algebraic_system_t * algebraic_system = malloc(<%omsiFunction.nAlgebraicSystems%>, sizeof(omsi_algebraic_system_t)) ;
    int i = 0;
    // alloc all algebraic systems

  }

  >>
end instantiateOmsiFunction;
*/

template generateAlgSystemFile(list<SimEqSystem> equations, String fileNamePrefix, String name)
"Description"
::=

  << <%fileNamePrefix%>_instantiate_omsi_algSystem_<%name%>( 
  
  >>
end generateAlgSystemFile;




annotation(__OpenModelica_Interface="backend");
end CodegenOMSIC_Equations;