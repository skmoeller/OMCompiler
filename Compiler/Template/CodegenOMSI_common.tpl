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
import CodegenUtilSimulation;
import CodegenFMU;
import CodegenOMSIC_Equations;
import CodegenCFunctions;
import CodegenAdevs;


template generateSetupSimulationDataFunction(SimCode simCode, String modelNamePrefixStr)
"Generates C function to initialize OMSI sim_data"
::=
  let scalarVariablesInitalization = ""

  match simCode
    case SIMCODE(modelInfo = MODELINFO(varInfo=VARINFO(__), vars = vars as SIMVARS(__))) then

    // set double *real_vars
    let _ = System.tmpTickResetIndex(0,1)
    let _ = vars.stateVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"model_vars_and_params->reals") + "\n"
      <<>>
    )
    let _ = vars.derivativeVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"model_vars_and_params->reals") + "\n"
      <<>>
    )
    let _ = vars.algVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"model_vars_and_params->reals") + "\n"
      <<>>
    )
    let _ = vars.discreteAlgVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"model_vars_and_params->reals") + "\n"
      <<>>
    )
    let _ = vars.realOptimizeConstraintsVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"model_vars_and_params->reals") + "\n"
      <<>>
    )
    let _ = vars.realOptimizeFinalConstraintsVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"model_vars_and_params->reals") + "\n"
      <<>>
    )
    let _ = vars.paramVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"model_vars_and_params->reals") + "\n"
      <<>>
    )

    // set int *int_vars
    let _ = System.tmpTickResetIndex(0,1)
    let _ = vars.intAlgVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"model_vars_and_params->ints") + "\n"
      <<>>
    )
    let _ = vars.intParamVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"model_vars_and_params->ints") + "\n"
      <<>>
    )

    // set bool *bool_vars
    let _ = System.tmpTickResetIndex(0,1)
    let _ = vars.boolAlgVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"model_vars_and_params->bools") + "\n"
      <<>>
    )
    let _ = vars.boolParamVars |> var => (
      let &scalarVariablesInitalization += ScalarVariableOMSIC(var,"model_vars_and_params->bools") + "\n"
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


template simulationFile_lsyOMSI(SimCode simCode)
"Linear Systems"
::=
  match simCode
    case simCode as SIMCODE(modelInfo=MODELINFO(varInfo=varInfo as VARINFO(__),linearSystems=linearSystems)) then

    let functionSetupLinearSysems = functionSetupLinearSystemsOMSI(linearSystems, CodegenUtilSimulation.modelNamePrefix(simCode))


    <<
    /* Linear Systems */
    #include "<%simCode.fileNamePrefix%>_blablabla.h"

    #if defined(__cplusplus)
    extern "C" {
    #endif

    <%functionSetupLinearSysems%>

    <% if intGt(varInfo.numLinearSystems,0) then functionInitialLinearSystemsOMSI(linearSystems, CodegenUtilSimulation.modelNamePrefix(simCode))%>

    #if defined(__cplusplus)
    }
    #endif
    <%\n%>
    >>
    /* adrpo: leave a newline at the end of file to get rid of the warning */
  end match
end simulationFile_lsyOMSI;


template functionSetupLinearSystemsOMSI(list<SimEqSystem> linearSystems, String modelNamePrefix)
  "Generates functions in simulation file."
::=
  let linearbody = functionSetupLinearSystemsTempOMSI(linearSystems, modelNamePrefix)
  <<
  /* linear systems */
  <%linearbody%>
  >>
end functionSetupLinearSystemsOMSI;


template functionSetupLinearSystemsTempOMSI(list<SimEqSystem> linearSystems, String modelNamePrefix)
  "Generates functions in simulation file."
::=
  (linearSystems |> eqn => (match eqn
     case eq as SES_MIXED(__) then functionSetupLinearSystemsTempOMSI(fill(eq.cont,1), modelNamePrefix)
     // no dynamic tearing
     case eq as SES_LINEAR(lSystem=ls as LINEARSYSTEM(__), alternativeTearing=NONE()) then
     match ls.jacobianMatrix
       case SOME(__) then
         let eqFuncs = ""
         let _ = ls.residual |> eq => (
           let &eqFuncs += CodegenOMSIC_Equations.equationFunction(eq, contextOMSI, modelNamePrefix) + "\n\n"
           <<>>
         )

         <<
         <%eqFuncs%>
         >>
       else
         <<>>
     end match

     // dynamic tearing
     case eq as SES_LINEAR(lSystem=ls as LINEARSYSTEM(__), alternativeTearing = SOME(at as LINEARSYSTEM(__))) then
     match ls.jacobianMatrix
       case SOME(__) then
         <<>>
       else
         <<>>
     end match
   )
   ;separator="\n\n")
end functionSetupLinearSystemsTempOMSI;


template functionInitialLinearSystemsOMSI(list<SimEqSystem> linearSystems, String modelNamePrefix)
  "Generates function to initialize omsi_algebraic_system_t for linear systems"
::=
  let &tempeqns = buffer ""
  let &tempeqns += (linearSystems |> eq => match eq case eq as SES_LINEAR(alternativeTearing = SOME(__)) then 'int <%CodegenUtil.symbolName(modelNamePrefix,"eqFunction")%>_<%CodegenUtilSimulation.equationIndex(eq)%>(DATA*);' ; separator = "\n")
  let &globalConstraintsFunctions = buffer ""
  let linearbody = functionInitialLinearSystemsTempOMSI(linearSystems, modelNamePrefix, &globalConstraintsFunctions)

  <<
  /* ToDo: Some description */
  void <%CodegenUtil.symbolName(modelNamePrefix,"initialLinearSystem")%> (omsi_function_t* initialization) {
    /* linear systems */
    <%linearbody%>
  }
  >>
end functionInitialLinearSystemsOMSI;


template functionInitialLinearSystemsTempOMSI(list<SimEqSystem> linearSystems, String modelNamePrefix, Text &globalConstraintsFunctions)
  "Helper function for functionInitialLinearSystemOMSI"
::=
  (linearSystems |> eqn => (match eqn
    case eq as SES_MIXED(__) then functionInitialLinearSystemsTempOMSI(fill(eq.cont,1), modelNamePrefix, "")
    // no dynamic tearing
    case eq as SES_LINEAR(lSystem=ls as LINEARSYSTEM(__), alternativeTearing=NONE()) then
      match ls.jacobianMatrix
        case NONE() then
          <<
          NOT IMPLEMENTED YET
          >>
        case SOME(__) then
          <<
          START IMPLEMENTATION
          >>
        else
          ///CodegenAdevs.error(sourceInfo(), ' No jacobian create for linear system <%ls.index%>.')
          <<>>
      end match
    // dynamic tearing
    case eq as SES_LINEAR(lSystem=ls as LINEARSYSTEM(__), alternativeTearing = SOME(at as LINEARSYSTEM(__))) then
      match ls.jacobianMatrix
        case NONE() then
          <<
          NOT IMPLEMENTED YET
          >>
        case SOME(__) then
          <<
          NOT IMPLEMENTED YET
          >>
        else
          ///CodegenAdevs.error(sourceInfo(), ' No jacobian create for linear system <%ls.index%> or <%at.index%>.')
          <<>>
      end match
    )
    ;separator="\n\n")
end functionInitialLinearSystemsTempOMSI;



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