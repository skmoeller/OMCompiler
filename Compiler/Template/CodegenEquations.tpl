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
import CodegenUtilSimulation;

template equationName(SimEqSystem eq, String modelNamePrefixStr)
 "Generates an equation.
  This template should not be used for a SES_RESIDUAL.
  Residual equations are handled differently."
::=
  let ix = CodegenUtilSimulation.equationIndex(eq)
  <<
  <%CodegenUtil.symbolName(modelNamePrefixStr,"eqFunction")%>_<%ix%>(data, threadData);

  >>
end equationName;

template generateEquationFiles(list<SimEqSystem> allEquations, String fileNamePrefix)
"
//let &eqFuncs += equationFuction(eqn, contextSimulationDiscrete, fileNamePrefix)
"
::=
  let eqFuncs = ""
  let eqCalls = ""
  let _ =  allEquations |> eqn => (
            let &eqCalls += equationName(eqn, fileNamePrefix)
            <<>>
            )
  <<
  <%eqFuncs%>
  
  int evalEquations(){

    <%eqCalls%>

  }
  >>
end generateEquationFiles;



annotation(__OpenModelica_Interface="backend");
end CodegenEquations;