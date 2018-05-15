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
               OpenModelica Simulation Inferface (OMSI) for C
"

import interface SimCodeTV;
import interface SimCodeBackendTV;
import CodegenUtil;
import CodegenFMU;


template generateSetupModelDataFunction(ModelInfo modelInfo, String modelNamePrefixStr)
"Generates C function to initialize OMSI model_data with data from xml file"
::=
  match modelInfo
    case MODELINFO(varInfo=VARINFO(__)) then
    <<
    /* initializes model data */
    omsi_data->model_data->n_States = <%varInfo.numStateVars%>;    /* number of states */
    omsi_data->model_data->n_derivatives = <%varInfo.numJacobians%>; // TODO: is this correct????    /* number of derivatives */
    omsi_data->model_data->n_real_vars = <%nVariablesReal(varInfo)%>;    /* number of real variables */
    omsi_data->model_data->n_int_vars = <%varInfo.numIntAlgVars%>;    /* number of integer varibales */
    omsi_data->model_data->n_bool_vars = <%varInfo.numBoolAlgVars%>;    /* number of boolean variables */
    omsi_data->model_data->n_string_vars = <%varInfo.numStringAlgVars%>;    /*number of string variables */
    omsi_data->model_data->n_real_parameters = <%varInfo.numParams%>;    /* number of real parameters */
    omsi_data->model_data->n_int_parameters = <%varInfo.numIntParams%>;    /* number of integer parameters */
    omsi_data->model_data->n_bool_parameters = <%varInfo.numBoolParams%>;    /* number of boolean parameters */
    omsi_data->model_data->n_zerocrossing = <%varInfo.numZeroCrossings%>;    /* number of zero crossings */

    <%CodegenUtil.symbolName(modelNamePrefixStr,"setupDataStruc")%>_setupModelVarsInfo(omsi_data->model_data->model_vars_info_t);
    <%CodegenUtil.symbolName(modelNamePrefixStr,"setupDataStruc")%>_setupEquationInfo(omsi_data->model_data->equation_info_t);
    >>
  end match
end generateSetupModelDataFunction;


template generateSetupExperimentData(SimCode simCode, String modelNamePrefixStr)
"Description"
::=
  match simCode
    case SIMCODE(modelInfo = MODELINFO(varInfo=VARINFO(__)), simulationSettingsOpt = SOME(s as SIMULATION_SETTINGS(__))) then
    <<
    /* initialize experiment data */
    omsi_data->experiment->start_time = <%s.startTime%>;
    omsi_data->experiment->stop_time = <%s.stopTime%>;
    omsi_data->experiment->step_size = <%s.stepSize%>;
    //omsi_data->experiment->num_outputs =  // TODO: complete
    omsi_data->experiment->tollerance = <%s.tolerance%>;
    omsi_data->experiment->solver_name = "<%s.method%>";
    >>
end generateSetupExperimentData;


template generateSetupSimulationDataFunction(SimCode simCode, String modelNamePrefixStr)
"Generates C function to initialize OMSI sim_data with data from xml file"
::=
  let scalarVariablesInitalization = ""

  match simCode
    case SIMCODE(modelInfo = MODELINFO(varInfo=VARINFO(__), vars = vars as SIMVARS(__))) then

    // set double *real_vars
    let _ = System.tmpTickResetIndex(0,1)
    let _ = vars.stateVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"real_vars") + "\n"
      <<>>
    )
    let _ = vars.derivativeVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"real_vars") + "\n"
      <<>>
    )
    let _ = vars.algVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"real_vars") + "\n"
      <<>>
    )
    let _ = vars.discreteAlgVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"real_vars") + "\n"
      <<>>
    )
    let _ = vars.realOptimizeConstraintsVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"real_vars") + "\n"
      <<>>
    )
    let _ = vars.realOptimizeFinalConstraintsVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"real_vars") + "\n"
      <<>>
    )
    let _ = vars.paramVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"real_vars") + "\n"
      <<>>
    )

    // set int *int_vars
    let _ = System.tmpTickResetIndex(0,1)
    let _ = vars.intAlgVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"int_vars") + "\n"
      <<>>
    )
    let _ = vars.intParamVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"int_vars") + "\n"
      <<>>
    )

    // set bool *bool_vars
    let _ = System.tmpTickResetIndex(0,1)
    let _ = vars.boolAlgVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"bool_vars") + "\n"
      <<>>
    )
    let _ = vars.boolParamVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"bool_vars") + "\n"
      <<>>
    )

    <<
    /* initialize simulation data */
    <%scalarVariablesInitalization%>
    >>
  end match
end generateSetupSimulationDataFunction;


template ScalarVariableOMSIC(SimVar simVar, String classType)
"Generates code for simulation data initialization"
::=
  let ci = System.tmpTickIndex(1)

  match simVar
    case SIMVAR(source = SOURCE(info = info)) then
    <<
    omsi_data->sim_data-><%classType%>[<%ci%>] = <%CodegenFMU.optInitValFMU(initialValue,"0.0")%>;    /* <%Util.escapeModelicaStringToCString(CodegenUtil.crefStrNoUnderscore(name))%> */
    >>
  end match
end ScalarVariableOMSIC;


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