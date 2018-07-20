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

 package CodegenOMSI_common
" file:        CodegenOMSIC.tpl
  package:     CodegenOMSIC
  description: Code generation using Susan templates for
               OpenModelica Simulation Inferface (OMSI) for C and C++
"

import interface SimCodeTV;
import interface SimCodeBackendTV;
import CodegenUtil;
import CodegenUtilSimulation;
import CodegenFMU;
import CodegenOMSIC_Equations;
import CodegenCFunctions;
import CodegenAdevs;


/* public */
template generateEquationsCode (SimCode simCode, String FileNamePrefix)
"Entrypoint to generate all Code for linear systems.
 Code is generated directly into files"
::=
  match simCode
  case SIMCODE(omsiData=omsiData as SOME(OMSI_DATA(simulation=simulation as OMSI_FUNCTION(__)))) then

    // generate file for algebraic systems in simulation problem
    let content = generateOmsiFunctionCode(simulation, FileNamePrefix)
    let () = textFile(content, FileNamePrefix+"_sim_equations.c")

    // generate file for initialization problem
    // ToDo: add
  <<>>
end generateEquationsCode;


template generateOmsiFunctionCode(OMSIFunction omsiFunction, String FileNamePrefix)
"Generates file for all equations, containing equation evaluations for all systems"
::=
  let &includes = buffer ""
  let &evaluationCode = buffer ""
  let &functionCall = buffer ""

  let _ = generateOmsiFunctionCode_inner(omsiFunction, FileNamePrefix, &includes, &evaluationCode, &functionCall, "")

  <<
  /* All Equations Code */
  <%includes%>

  #if defined(__cplusplus)
  extern "C" {
  #endif

  /* Evaluation functions for each equation */
  <%evaluationCode%>


  /* Equations evaluation */
  omsi_status <%FileNamePrefix%>_allEqns(omsi_function_t* simulation, omsi_values* model_vars_and_params){

    <%functionCall%>

    return omsi_ok;
  }

  #if defined(__cplusplus)
  }
  #endif
  <%\n%>
  >>
  /* leave a newline at the end of file to get rid of the warning */
end generateOmsiFunctionCode;


template generateOmsiFunctionCode_inner(OMSIFunction omsiFunction, String FileNamePrefix, Text &includes, Text &evaluationCode, Text &functionCall, Text &residualCall)
""
::=
  match omsiFunction
  case OMSI_FUNCTION(context=context as SimCodeFunction.OMSI_CONTEXT(__)) then
    let _ = (equations |> eqsystem hasindex i0 =>
      match eqsystem
      case SES_SIMPLE_ASSIGN(__) then
        let &evaluationCode += CodegenOMSIC_Equations.generateEquationFunction(eqsystem, FileNamePrefix, context) +"\n"
        let &functionCall += CodegenOMSIC_Equations.equationCall(eqsystem, FileNamePrefix, "simulation") +"\n"
        <<>>
      case SES_RESIDUAL(__) then
        let &evaluationCode += CodegenOMSIC_Equations.generateEquationFunction(eqsystem, FileNamePrefix, context) +"\n"
        let &residualCall += CodegenOMSIC_Equations.equationCall(eqsystem, FileNamePrefix, 'this_function, res[i++]') +"\n"      // ToDo: why is i0 always zero?
        <<>>
      case SES_ALGEBRAIC_SYSTEM(__) then
        let &includes += "#include \""+ FileNamePrefix + "_algSyst_" + index + ".h\"\n"
        let &functionCall += CodegenOMSIC_Equations.equationCall(eqsystem, FileNamePrefix, "simulation") +"\n"
        // write own file for each algebraic system
        let content = generateOmsiAlgSystemCode(eqsystem, FileNamePrefix)
        let () = textFile(content, FileNamePrefix+"_sim_algSyst_"+ index + ".c")
        <<>>
      else
        // NOT IMPLEMENTED YET
        // ToDo: add Error
        <<>>
      end match
    )

  <<>>
end generateOmsiFunctionCode_inner;


template generateOmsiAlgSystemCode (SimEqSystem equationSystem, String FileNamePrefix)
""
::=
  let &includes = buffer ""
  let &evaluationCode = buffer ""
  let &functionCall = buffer ""
  let &residualCall = buffer ""
  let &derivativeMatrix = buffer ""

  match equationSystem
  case algSystem as SES_ALGEBRAIC_SYSTEM(matrix = matrix as SOME(DERIVATIVE_MATRIX(__))) then
    let _ = generateOmsiFunctionCode_inner(residual, FileNamePrefix, &includes, &evaluationCode, &functionCall, &residualCall)
    let initlaizationFunction = generateInitalizationAlgSystem(equationSystem, FileNamePrefix)
    let matrixString = CodegenOMSIC_Equations.generateMatrixInitialization(matrix)
    let equationInfos = CodegenUtilSimulation.dumpEqs(fill(equationSystem,1))

    let derivativeMatrix = CodegenOMSIC_Equations.generateDerivativeFile(matrix, FileNamePrefix, algSystem.index)
    let () = textFile(derivativeMatrix, FileNamePrefix+"_sim_derMat_"+algSystem.index+".c")

  <<
  /* Algebraic system code */
  <%includes%>

  #if defined(__cplusplus)
  extern "C" {
  #endif

  /* Instantiation and initalization*/
  <%initlaizationFunction%>

  <%matrixString%>

  /* Evaluation functions for <%FileNamePrefix%>_resFunction_<%index%> */
  <%evaluationCode%>

  void <%FileNamePrefix%>_resFunction_<%index%> (omsi_function* this_function, omsi_real* res) {
    omsi_unsigned_int i=0;
    <%residualCall%>
  }

  /* Algebraic system evaluation */
  /*
  <%equationInfos%>
  */
  omsi_status <%FileNamePrefix%>_algSystFunction_<%index%>(omsi_function_t* simulation, omsi_values* model_vars_and_params){

    /* evaluate assignments */
    <%functionCall%>

    /* call API function something */

      /* ToDo: Add crazy stuff here */

    return omsi_ok;
  }

  #if defined(__cplusplus)
  }
  #endif
  <%\n%>
  >>
  /* leave a newline at the end of file to get rid of the warning */
end generateOmsiAlgSystemCode;


template generateInitalizationAlgSystem (SimEqSystem equationSystem, String FileNamePrefix)
""
::=
  match equationSystem
  case SES_ALGEBRAIC_SYSTEM(residual=residual as OMSI_FUNCTION(__)) then

    let zeroCrossingIndices = (zeroCrossingConditions |> cond =>
      '<%cond%>'
    ;separator=", ")
    <<
    /* function initialize omsi_algebraic_system_t struct */
    omsi_status <%FileNamePrefix%>_initializeAlgSystem_<%index%>(omsi_algebraic_system_t* algSystem) {
      algSystem->n_iteration_vars = <%listLength(residual.outputVars)%>;

      <%generateOmsiIndexTypeInitialization(residual.outputVars, "algSystem->iteration_vars_indices")%>

      algSystem->n_conditions = <%listLength(zeroCrossingConditions)%>;
      <% if listLength(zeroCrossingConditions) then 'algSystem->zerocrossing_indices[listLength(zeroCrossingConditions)] = {<%zeroCrossingIndices%>};' %>

      algSystem->isLinear = <% if linearSystem then 'omsi_true' else 'omsi_false'%>;

      algSystem->functions->evaluate = <%FileNamePrefix%>_resFunction_<%index%>;
      algSystem->jacobian->evaluate = <%FileNamePrefix%>_derivativeMatFunc_<%index%>;

      algSystem->solverData = NULL;

      return omsi_ok;
    }
    >>
end generateInitalizationAlgSystem;



template generateOmsiIndexTypeInitialization (list<SimVar> variables, String StrucPrefix)
"Generates code for instantiation and initialization of struct omsi_index_type "
::=
  let stringBuffer = (variables |> variable hasindex i0 =>
    let stringType = (match variable
      case SIMVAR(type_=type_ as T_REAL(__)) then
        "OMSI_TYPE_REAL"
      case SIMVAR(type_=type_ as T_INTEGER(__)) then
        "OMSI_TYPE_INTEGER"
      case SIMVAR(type_=type_ as T_BOOL(__)) then
        "OMSI_TYPE_BOOLEAN"
      case SIMVAR(type_=type_ as T_STRING(__)) then
        "OMSI_TYPE_STRING"
      else
        "OMSI_TYPE_UNKNOWN"
    )

    let stringIndex = ""
    let stringName = ""
    let targetName = ""     // ToDo: fill targetName

    let _ = (match variable
      case var as SIMVAR(__) then
        let stringIndex = index
        let stringName = CodegenUtil.escapeCComments(CodegenUtil.crefStrNoUnderscore(var.name))
        <<>>
    )


    <<
    omsi_index_pointer[<%i0%>]->type = <%stringType%>;
    omsi_index_pointer[<%i0%>]->index = <%stringIndex%>;    /* maps <%stringName%> to <%targetName%> */
    >>
  ;separator="\n")

  <<
  omsi_index_type* omsi_index_pointer;
  omsi_index_pointer = omsi_callback_functions->omsi_callback_allocate_memory(<%listLength(variables)%>, sizeof(omsi_index_type));
  if (!omsi_index_pointer) {
    /* ToDo: Log error */
    return omsi_error;
  }
  <%stringBuffer%>
  <%StrucPrefix%> = omsi_index_pointer;
  >>
end generateOmsiIndexTypeInitialization;




template insertCopyrightOpenModelica()
::=
  <<
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
  >>
end insertCopyrightOpenModelica;


annotation(__OpenModelica_Interface="backend");
end CodegenOMSI_common;