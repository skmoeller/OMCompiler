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
  let equationCode = equationCStr(eq, context, &varDecls, &auxFunction)

  let funcName = (match eq
    case SES_RESIDUAL(__) then
      "resFunction"
    case SES_ALGEBRAIC_SYSTEM(__) then
      "algSystFunction"
    else
      "eqFunction"
  )

  let funcArguments = (match eq
    case SES_RESIDUAL(__) then
      "omsi_function_t* this_function, omsi_real* res"
    else
      "omsi_function_t* this_function"
  )


  <<
  /*
  <%equationInfos%>
  */
  void <%CodegenUtil.symbolName(modelNamePrefixStr,funcName)%>_<%ix%>(<%funcArguments%>){
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
  let &preExp = buffer ""

  match eq
  case SES_SIMPLE_ASSIGN(__) then
    let expPart = CodegenCFunctions.daeExp(exp, context, &preExp, &varDecls, &auxFunction)
    let crefStr = CodegenCFunctions.contextCref(cref, context, &auxFunction)
    <<
    <%crefStr%> = <%expPart%>;
    >>
  case SES_ALGEBRAIC_SYSTEM(__) then

    <<
    /* Add stuff here*/
    >>
  case SES_RESIDUAL(__) then
    let expPart = CodegenCFunctions.daeExp(exp, context, &preExp, &varDecls, &auxFunction)
    <<
    *res = <%expPart%>;
    >>
  else
    <<
    NOT IMPLEMENTED YET
    >>

end equationCStr;


template equationCall(SimEqSystem eq, String modelNamePrefixStr, String input)
 "Generates call function for evaluating functions"
::=
  let ix = CodegenUtilSimulation.equationIndex(eq)
  match eq
  case SES_SIMPLE_ASSIGN(__) then
    <<
    <%CodegenUtil.symbolName(modelNamePrefixStr,"eqFunction")%>_<%ix%>(<%input%>);
    >>
  case SES_RESIDUAL(__) then
    <<
    <%CodegenUtil.symbolName(modelNamePrefixStr,"resFunction")%>_<%ix%>(<%input%>);
    >>
  case SES_ALGEBRAIC_SYSTEM(__) then
    <<
    <%CodegenUtil.symbolName(modelNamePrefixStr,"algSystFunction")%>_<%ix%>(<%input%>);
    >>
  else
    // ToDo: generate Warning
    <<
    //equationCall not implemented yet
    >>
end equationCall;


template generateMatrixInitialization(Option<JacobianMatrix> matrix)
"generates code for matrix initialization and evaluation"
::=
  let columnsString = ""

  match matrix
  case SOME(m as JAC_MATRIX(__)) then
    let _ = m.columns |> col => (
      let columnsString = generateMatrixColumn(col)
      <<>>
    )

  <<

  <%columnsString%>
  
  >>
end generateMatrixInitialization;


template generateMatrixColumn(JacobianColumn column)
""
::=
  match column
  case JAC_COLUMN(__) then

  <<

  >>
end generateMatrixColumn;


annotation(__OpenModelica_Interface="backend");
end CodegenOMSIC_Equations;