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

    /* generate file for algebraic systems in simulation problem */
    let content = generateOmsiFunctionCode(simulation, FileNamePrefix)
    let () = textFile(content, FileNamePrefix+"_sim_equations.c")

    /* generate file for initialization problem */
    /* ToDo: add */
  <<>>
end generateEquationsCode;


template generateOmsiFunctionCode(OMSIFunction omsiFunction, String FileNamePrefix)
"Generates file for all equations, containing equation evaluations for all systems"
::=
  let &includes = buffer ""
  let &evaluationCode = buffer ""
  let &functionCall = buffer ""
  let &functionPrototypes = buffer ""

  let initializationCode = generateInitalizationOMSIFunction(omsiFunction, "allEqns", FileNamePrefix, &functionPrototypes, &includes)
  let _ = generateOmsiFunctionCode_inner(omsiFunction, FileNamePrefix, "simulation", &includes, &evaluationCode, &functionCall, "", &functionPrototypes)

  // generate header file
  let &functionPrototypes +=<<omsi_status <%FileNamePrefix%>_allEqns(omsi_function_t* simulation, omsi_values* model_vars_and_params, void* data);<%\n%>>>
  let headerFileName = FileNamePrefix+"_sim_equations"
  let headerFileContent = generateCodeHeader(FileNamePrefix, &includes, headerFileName, &functionPrototypes)

  let () = textFile(headerFileContent, headerFileName+".h")

  <<
  <%insertCopyrightOpenModelica()%>

  /* All Equations Code */
  #include "<%headerFileName%>.h"

  #if defined(__cplusplus)
  extern "C" {
  #endif

  /* Instantiation of omsi_function_t simulation */
  <%initializationCode%>

  /* Evaluation functions for each equation */
  <%evaluationCode%>


  /* Equations evaluation */
  omsi_status <%FileNamePrefix%>_allEqns(omsi_function_t* simulation, omsi_values* model_vars_and_params, void* data){

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


template generateOmsiFunctionCode_inner(OMSIFunction omsiFunction, String FileNamePrefix, String funcCallArgName, Text &includes, Text &evaluationCode, Text &functionCall, Text &residualCall, Text &functionPrototypes)
""
::=
  match omsiFunction
  case OMSI_FUNCTION(context=context as SimCodeFunction.OMSI_CONTEXT(__)) then
    let _ = (equations |> eqsystem hasindex i0 =>
      match eqsystem
      case SES_SIMPLE_ASSIGN(__) then
        let &evaluationCode += CodegenOMSIC_Equations.generateEquationFunction(eqsystem, FileNamePrefix, context, &functionPrototypes) +"\n"
        let &functionCall += CodegenOMSIC_Equations.equationCall(eqsystem, FileNamePrefix, '<%funcCallArgName%>, model_vars_and_params') +"\n"
        <<>>
      case SES_RESIDUAL(__) then
        let &evaluationCode += CodegenOMSIC_Equations.generateEquationFunction(eqsystem, FileNamePrefix, context, &functionPrototypes) +"\n"
        let &residualCall += CodegenOMSIC_Equations.equationCall(eqsystem, FileNamePrefix, '<%funcCallArgName%>, model_vars_and_params, &res[i++]') +"\n"
        <<>>
      case algSystem as SES_ALGEBRAIC_SYSTEM(__) then
        let &includes += <<#include "<%FileNamePrefix%>_sim_algSyst_<%algSysIndex%>.h"<%\n%>>>
        let &functionCall += CodegenOMSIC_Equations.equationCall(eqsystem, FileNamePrefix, '&<%funcCallArgName%>->algebraic_system_t[<%algSysIndex%>], model_vars_and_params, simulation->function_vars') +"\n"
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
"Returns Code for an algebraic system and creates associated derMat file and header files."
::=
  let &includes = buffer ""
  let &evaluationCode = buffer ""
  let &functionCall = buffer ""
  let &residualCall = buffer ""
  let &derivativeMatrix = buffer ""
  let &functionPrototypes = buffer ""

  match equationSystem
  case algSystem as SES_ALGEBRAIC_SYSTEM(matrix = matrix as SOME(DERIVATIVE_MATRIX(__))) then
    let &includes += <<#include <solver_lapack.h><%\n%>>>

    let initlaizationFunction = generateInitalizationAlgSystem(equationSystem, FileNamePrefix, &functionPrototypes, &includes)
    let _ = generateOmsiFunctionCode_inner(residual, FileNamePrefix, "this_function", &includes, &evaluationCode, &functionCall, &residualCall, &functionPrototypes)
    let matrixString = CodegenOMSIC_Equations.generateMatrixInitialization(matrix)
    let equationInfos = CodegenUtilSimulation.dumpEqs(fill(equationSystem,1))

    // generate jacobian matrix files
    let &includes += <<#include "<%FileNamePrefix%>_sim_derMat_<%algSystem.algSysIndex%>.h"<%\n%>>>
    let _ = generateDerivativeFile(matrix, FileNamePrefix, algSystem.algSysIndex)

    // generate algebraic system header file
    let &functionPrototypes +=
      <<
      omsi_status <%FileNamePrefix%>_resFunction_<%algSystem.algSysIndex%> (omsi_function_t* this_function, const omsi_values* model_vars_and_params, omsi_real* res);
      omsi_status <%FileNamePrefix%>_algSystFunction_<%algSystem.algSysIndex%>(omsi_algebraic_system_t* this_alg_system, const omsi_values* model_vars_and_params, omsi_values* out_function_vars);
      >>
    let headerFileName = FileNamePrefix+"_sim_algSyst_"+algSystem.algSysIndex
    let headerFileContent = generateCodeHeader(FileNamePrefix, &includes, headerFileName, &functionPrototypes)
    let () = textFile(headerFileContent, headerFileName+".h")

  <<
  <%insertCopyrightOpenModelica()%>

  /* Algebraic system code */
  #include "<%headerFileName%>.h"

  #if defined(__cplusplus)
  extern "C" {
  #endif

  /* Instantiation and initialization */
  <%initlaizationFunction%>

  <%matrixString%>

  /* Evaluation functions for <%FileNamePrefix%>_resFunction_<%algSystem.algSysIndex%> */
  <%evaluationCode%>

  omsi_status <%FileNamePrefix%>_resFunction_<%algSystem.algSysIndex%> (omsi_function_t* this_function, const omsi_values* model_vars_and_params, omsi_real* res) {
    omsi_unsigned_int i=0;
    <%functionCall%>
    <%residualCall%>

    return omsi_ok;
  }

  /* Algebraic system evaluation */
  /*
  <%equationInfos%>
  */
  omsi_status <%FileNamePrefix%>_algSystFunction_<%algSystem.algSysIndex%>(omsi_algebraic_system_t* this_alg_system,
                            const omsi_values* model_vars_and_params,
                            omsi_values* out_function_vars){

    /* call API function something */
    solveLapack(this_alg_system, model_vars_and_params, NULL);

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


template generateCodeHeader(String FileNamePrefix, Text &includes, String headerName, Text &functionPrototypes)
"Generates Header for omsi algebraic system C files"
::=
  <<
  <%insertCopyrightOpenModelica()%>

  #if !defined(<%headerName%>_H)
  #define <%headerName%>_H

  #include <omsi.h>
  #include <omsic.h>
  #include <omsi_callbacks.h>
  #include <omsi_global.h>

  #include <stdlib.h>
  #include <math.h>

  <%includes%>

  #if defined(__cplusplus)
  extern "C" {
  #endif

  /* function prototypes */
  <%functionPrototypes%>

  #if defined(__cplusplus)
  }
  #endif

  #endif
  <%\n%>
  >>
  /* leave a newline at the end of file to get rid of the warning */
end generateCodeHeader;


template generateDerivativeFile (Option<DerivativeMatrix> matrix, String FileNamePrefix, String index)
"Generates code for derivative matrix C and header files and creates those files."
::=
  let &includes = buffer ""
  let &initalizationCodeCol = buffer ""
  let &functionPrototypes = buffer ""

  let initalizationCode = (match matrix
    case SOME(derMat as DERIVATIVE_MATRIX(__)) then
    let initalizationCodeCol = (derMat.columns |> column =>
      <<
      <%generateInitalizationOMSIFunction(column, 'derivativeMatFunc_<%index%>', FileNamePrefix, &functionPrototypes, &includes)%>
      >>
    ;separator="\n\n")
    <<
    <%initalizationCodeCol%>
    >>
  )

  let body = CodegenOMSIC_Equations.generateDerivativeMatrix(matrix, FileNamePrefix, index, &functionPrototypes)

  let headerFileName = FileNamePrefix+"_sim_derMat_"+index

  let content =
    <<
    <%insertCopyrightOpenModelica()%>

    /* derivative matrix code for algebraic system <%index%>*/
    #include "<%headerFileName%>.h"

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

  // generate c and header files
  let () = textFile(content, FileNamePrefix+"_sim_derMat_"+index+".c")
//  let &functionPrototypes +=

  let headerFileContent = generateCodeHeader(FileNamePrefix, &includes, headerFileName, &functionPrototypes)
  let () = textFile(headerFileContent, headerFileName+".h")

  <<>>
end generateDerivativeFile;


template generateInitalizationAlgSystem (SimEqSystem equationSystem, String FileNamePrefix, Text &functionPrototypes, Text &includes)
""
::=
  match equationSystem
  case SES_ALGEBRAIC_SYSTEM(residual=residual as OMSI_FUNCTION(__)) then

    let &functionPrototypes += <<omsi_status <%FileNamePrefix%>_initializeAlgSystem_<%algSysIndex%>(omsi_algebraic_system_t* algSystem, omsi_values* function_vars);<%\n%>>>

    let zeroCrossingIndices = (zeroCrossingConditions |> cond =>
      '<%cond%>'
    ;separator=", ")
    <<
    /* function initialize omsi_algebraic_system_t struct */
    omsi_status <%FileNamePrefix%>_initializeAlgSystem_<%algSysIndex%>(omsi_algebraic_system_t* algSystem, omsi_values* function_vars) {
      algSystem->n_iteration_vars = <%listLength(residual.outputVars)%>;

      algSystem->n_conditions = <%listLength(zeroCrossingConditions)%>;
      <% if listLength(zeroCrossingConditions) then
      'algSystem->zerocrossing_indices[listLength(zeroCrossingConditions)] = {<%zeroCrossingIndices%>};'
      else
      'algSystem->zerocrossing_indices = NULL;'
       %>

      algSystem->isLinear = <% if linearSystem then 'omsi_true' else 'omsi_false'%>;

      /* Instantiate and initialize omsi_function_t jacobian */
      algSystem->jacobian = omsu_instantiate_omsi_function (function_vars);
      if (!algSystem->jacobian) {
        return omsi_error;
      }
      if (<%FileNamePrefix%>_initialize_derivativeMatFunc_<%algSysIndex%>_OMSIFunc(algSystem->jacobian) == omsi_error){
        return omsi_error;
      }

      /* Instantiate and initialize omsi_function_t function */
      algSystem->functions = omsu_instantiate_omsi_function (function_vars);
      if (!algSystem->functions) {
        return omsi_error;
      }
      if (<%FileNamePrefix%>_initialize_resFunction_<%algSysIndex%>_OMSIFunc(algSystem->functions) == omsi_error){
        return omsi_error;
      }

      /* ToDo: put into init functions */
      algSystem->functions->evaluate = <%FileNamePrefix%>_resFunction_<%algSysIndex%>;
      algSystem->jacobian->evaluate = <%FileNamePrefix%>_derivativeMatFunc_<%algSysIndex%>;

      algSystem->solver_data = NULL;

      return omsi_ok;
    }

    <%generateInitalizationOMSIFunction (residual, 'resFunction_<%algSysIndex%>', FileNamePrefix, &functionPrototypes, &includes)%>
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
    <%omsiFuncName%>[<%i0%>].type = <%stringType%>;
    <%omsiFuncName%>[<%i0%>].index = <%stringIndex%>;    /* <%stringName%> <%stringVarKind%>*/
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


template generateInitalizationOMSIFunction (OMSIFunction omsiFunction, String functionName, String FileNamePrefix, Text &functionPrototypes, Text &includes)
"Generates code for omsi_function_t initialization"
::=
  match omsiFunction
  case func as OMSI_FUNCTION(__) then
    let &functionPrototypes += "omsi_status " + FileNamePrefix + "_initialize_" + functionName + "_OMSIFunc (omsi_function_t* omsi_function);\n"

    let evaluationTarget = FileNamePrefix+"_"+functionName
    let algSystemInit = generateAlgebraicSystemInstantiation (FileNamePrefix, nAlgebraicSystems, equations)

    let &includes +=
      <<
      #include <omsu_helper.h>
      <%if nAlgebraicSystems then <<
        #include <solver_api.h>
        >>
      %>
      #include <omsi_input_sim_data.h><%\n%>
      >>

    <<
    omsi_status <%FileNamePrefix%>_initialize_<%functionName%>_OMSIFunc (omsi_function_t* omsi_function) {

      filtered_base_logger(global_logCategories, log_all, omsi_ok,
          "fmi2Instantiate: Initialize omsi_function <%functionName%>.");

      omsi_function->n_algebraic_system = <%nAlgebraicSystems%>;

      omsi_function->n_input_vars = <%listLength(inputVars)%>;
      omsi_function->n_inner_vars = <%listLength(innerVars)%>;
      omsi_function->n_output_vars = <%listLength(outputVars)%>;

      <%if nAlgebraicSystems then
        <<
        /* Initialize algebraic system */
        omsi_function->algebraic_system_t = omsu_initialize_alg_system_array(<%nAlgebraicSystems%>);
        if (!omsi_function->algebraic_system_t) {
          filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                        "fmi2Instantiate: Not enough memory.");
          return omsi_error;
        }
        <%algSystemInit%>

        >>
      %>

      if (instantiate_input_inner_output_indices (omsi_function, <%listLength(inputVars)%>, <%listLength(outputVars)%>) == omsi_error) {
        return omsi_error;
      }

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


template generateAlgebraicSystemInstantiation (String FileNamePrefix, Integer nAlgebraicSystems, list<SimEqSystem> equations)
"Generates code for instantiation of array of omsi_algebraic_system_t"
::=
  let initialization =  (equations |> equation hasindex i0 =>
    match equation
      case SES_ALGEBRAIC_SYSTEM(__) then
      <<
      <%FileNamePrefix%>_initializeAlgSystem_<%algSysIndex%>(&(omsi_function->algebraic_system_t[<%i0%>]), omsi_function->function_vars);
      if (!&omsi_function->algebraic_system_t[<%i0%>]) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
            "fmi2Instantiate: Function <%FileNamePrefix%>_initializeAlgSystem_<%algSysIndex%> failed.");
        return omsi_error;
      }
      >>
  ;separator="\n")

  <<
  <%initialization%>
  >>
end generateAlgebraicSystemInstantiation;


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