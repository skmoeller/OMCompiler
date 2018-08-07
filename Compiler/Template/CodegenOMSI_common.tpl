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

import CodegenOMSIC_Equations;
import CodegenUtil;
import CodegenUtilSimulation;


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
  let initializationCode = generateInitalizationOMSIFunction(omsiFunction, "simulation", FileNamePrefix)

  <<
  /* All Equations Code */
  <%includes%>

  #if defined(__cplusplus)
  extern "C" {
  #endif

  /* Instantiation of omsi_function_t simulation */
  <%initializationCode%>

  /* Evaluation functions for each equation */
  <%evaluationCode%>


  /* Equations evaluation */
  omsi_status <%FileNamePrefix%>_allEqns(omsi_function_t* simulation){

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
        let &functionCall += CodegenOMSIC_Equations.equationCall(eqsystem, FileNamePrefix, "simulation, model_vars_and_params") +"\n"
        <<>>
      case SES_RESIDUAL(__) then
        let &evaluationCode += CodegenOMSIC_Equations.generateEquationFunction(eqsystem, FileNamePrefix, context) +"\n"
        let &residualCall += CodegenOMSIC_Equations.equationCall(eqsystem, FileNamePrefix, 'this_function, res[i++]') +"\n"
        <<>>
      case algSystem as SES_ALGEBRAIC_SYSTEM(__) then
        let &includes += "#include \""+ FileNamePrefix + "_algSyst_" + index + ".h\"\n"
        let &functionCall += CodegenOMSIC_Equations.equationCall(eqsystem, FileNamePrefix, "simulation, model_vars_and_params") +"\n"
        // write own file for each algebraic system
        let content = generateOmsiAlgSystemCode(eqsystem, FileNamePrefix)
        let () = textFile(content, FileNamePrefix+"_sim_algSyst_"+ algSystem.algSysIndex + ".c")
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

    let derivativeMatrix = generateDerivativeFile(matrix, FileNamePrefix, algSystem.index)
    let () = textFile(derivativeMatrix, FileNamePrefix+"_sim_derMat_"+algSystem.algSysIndex+".c")

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


template generateDerivativeFile (Option<DerivativeMatrix> matrix, String modelName, String index)
"generates c code for derivative matrix files"
::=
  let includes = ""
  let body = CodegenOMSIC_Equations.generateDerivativeMatrix(matrix, modelName, index)
  let &initalizationCodeCol = buffer ""

  let initalizationCode = (match matrix
    case SOME(derMat as DERIVATIVE_MATRIX(__)) then
    let initalizationCodeCol = (derMat.columns |> column =>
      <<
      <%generateInitalizationOMSIFunction(column, 'derivativeMatFunc<%index%>', modelName)%>
      >>
    ;separator="\n\n")
    <<
    <%initalizationCodeCol%>
    >>
  )

  <<
  /* derivative matrix code for algebraic system <%index%>*/
  <%includes%>

  #if defined(__cplusplus)
  extern "C" {
  #endif

  /* Instantiation and initalization */
  <%initalizationCode%>

  /* derivative matrix evaluation */
  <%body%>

  #if defined(__cplusplus)
  }
  #endif
  <%\n%>
  >>
  /* leave a newline at the end of file to get rid of the warning */
end generateDerivativeFile;


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

      algSystem->n_conditions = <%listLength(zeroCrossingConditions)%>;
      <% if listLength(zeroCrossingConditions) then
      'algSystem->zerocrossing_indices[listLength(zeroCrossingConditions)] = {<%zeroCrossingIndices%>};'
      else
      'algSystem->zerocrossing_indices = NULL;'
       %>

      algSystem->isLinear = <% if linearSystem then 'omsi_true' else 'omsi_false'%>;

      /* initliazie omsi_function_t jacobian*/
      algSystem->functions = (omsi_function_t*) omsi_callback_functions->omsi_callback_allocate_memory(1, sizeOf(omsi_function_t));
      if (!algSystem->functions) {
        /* ToDo: Log error */
        return omsi_error;
      }
      if (!problem2_initialize_resFunc<%index%>_OMSIFunc(algSystem->functions)){
        return omsi_error;
      }
      /* initliazile omsi_function_t function */

      /* ToDo: put into init functions */
      algSystem->functions->evaluate = <%FileNamePrefix%>_resFunction_<%index%>;
      algSystem->jacobian->evaluate = <%FileNamePrefix%>_derivativeMatFunc_<%index%>;

      algSystem->solverData = NULL;

      return omsi_ok;
    }

    <%generateInitalizationOMSIFunction (residual, 'resFunc<%index%>', FileNamePrefix)%>
    >>
end generateInitalizationAlgSystem;



template generateOmsiIndexTypeInitialization (list<SimVar> variables, String StrucPrefix, String targetName, String omsiFuncName)
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

    let &stringName = buffer""
    let &stringIndex = buffer ""
    let &stringVarKind = buffer ""
    let _ = (match variable
      case var as SIMVAR(varKind=JAC_VAR(__))
      case var as SIMVAR(varKind=JAC_DIFF_VAR(__))
      case var as SIMVAR(varKind=SEED_VAR(__)) then
      let &stringName += '<%CodegenUtil.escapeCComments(CodegenUtil.crefStrNoUnderscore(var.name))%>'
      let &stringIndex += var.index
      let &stringVarKind += CodegenUtil.variabilityString(var.varKind)
      <<>>
      case var as SIMVAR(__) then
      let &stringName += '<%CodegenUtil.escapeCComments(CodegenUtil.crefStrNoUnderscore(var.name))%>'
      let &stringIndex += getValueReference(var, getSimCode(), false)
      let &stringVarKind += CodegenUtil.variabilityString(var.varKind)
      <<>>
    )

    <<
    <%omsiFuncName%>[<%i0%>]->type = <%stringType%>;
    <%omsiFuncName%>[<%i0%>]->index = <%stringIndex%>;    /* <%stringName%> <%stringVarKind%>*/
    >>
  ;separator="\n")

  if listLength(variables) then
    <<
    /* maps to <%targetName%> */
    <%stringBuffer%>
    >>
  else
    <<>>
end generateOmsiIndexTypeInitialization;


template generateInitalizationOMSIFunction (OMSIFunction omsiFunction, String functionName, String FileNamePrefix)
"Generates code for omsi_function_t initialization"
::=
  match omsiFunction
  case func as OMSI_FUNCTION(__) then
    let evaluationTarget = FileNamePrefix+"_allEqns"

    <<
    omsi_status <%FileNamePrefix%>_initialize_<%functionName%>_OMSIFunc (omsi_function_t* omsi_function) {
      omsi_function->n_algebraic_system = <%nAlgebraicSystems%>

      omsi_function->n_input_vars = <%listLength(inputVars)%>;
      omsi_function->n_inner_vars = <%listLength(innerVars)%>;
      omsi_function->n_output_vars = <%listLength(outputVars)%>;

      /* Allocate memory */
      omsi_function->algebraic_system_t = (omsi_algebraic_system_t*) omsi_callback_functions->omsi_callback_allocate_memory(<%nAlgebraicSystems%>, sizeOf(omsi_algebraic_system_t));
      omsi_function->function_vars = (omsi_values*) omsi_callback_functions->omsi_callback_allocate_memory(<%nAllVars%>, sizeOf(omsi_values));
      if (!omsi_function->algebraic_system_t || !omsi_function->function_vars) {
        /* ToDo: Log error */
        return omsi_error;
      }

      <%if listLength(inputVars) then
        <<
        omsi_function->input_vars_indices = (omsi_index_type*) omsi_callback_functions->omsi_callback_allocate_memory(<%listLength(inputVars)%>, sizeOf(omsi_index_type));
        if (!omsi_function->input_vars_indices) {
          /* ToDo: Log error */
          return omsi_error;
        }
        >>
      else
        'omsi_function->input_vars_indices = NULL;'
      %>
      <%if listLength(outputVars) then
        <<omsi_function->output_vars_indices = (omsi_index_type*) omsi_callback_functions->omsi_callback_allocate_memory(<%listLength(outputVars)%>, sizeOf(omsi_index_type));
        if (!omsi_function->output_vars_indices) {
          /* ToDo: Log error */
          return omsi_error;
        }
        >>
      %>

      /* fill omsi_index_type indices */
      <%generateOmsiIndexTypeInitialization(inputVars, "omsi_function->input_vars_indices", "sim_data->model_vars_and_params", "omsi_function->input_vars_indices")%>
      <%if listLength(inputVars) then "\n"%>
      <%generateOmsiIndexTypeInitialization(outputVars, "omsi_function->output_vars_indices", "sim_data->model_vars_and_params", "omsi_function->output_vars_indices")%>

      /* set pointer for evaluation function */
      omsi_function->evaluate = <%evaluationTarget%>;

      return omsi_ok;
    }
    >>
end generateInitalizationOMSIFunction;


template insertCopyrightOpenModelica()
"Insert copyright notive as c comment.
 Should be used in every generated c or cpp file."
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