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

package CodegenOMSIC
" file:        CodegenOMSIC.tpl
  package:     CodegenOMSIC
  description: Code generation using Susan templates for 
               OpenModelica Simulation Inferface (OMSI) for C
"



import interface SimCodeTV;
import interface SimCodeBackendTV;
import CodegenEquations;
import CodegenFMU2;

template translateModel(SimCode simCode, String FMUVersion, String FMUType)
" main entry point for OMSIC code generator"
::=
  match simCode
  case SIMCODE(__) then
  let guid = getUUIDStr()
  let () = textFile( createMakefile(), '<%fileNamePrefix%>_FMU.makefile')
  let () = textFile( CodegenFMU2.fmiModelDescription(simCode, guid, FMUType), 'modelDescription.xml')
  let () = textFile( generateOMSIC(simCode), '<%fileNamePrefix%>_omsic.c')
  let () = textFile( CodegenEquations.generateEquationFiles(allEquations, fileNamePrefix), '<%fileNamePrefix%>_eqns.c')
  let () = textFile( CodegenEquations.generateEquationFilesHeader(allEquations, fileNamePrefix), '<%fileNamePrefix%>_eqns.h')
 <<>>
end translateModel;

template generateOMSIC(SimCode simCode)
::=
  match simCode
  case SIMCODE(__) then
<<
// translated model <%fileNamePrefix%> to OMSIC <%\n%>
>>
end generateOMSIC;


template createMakefile()
::=
<<
all:
>>
end createMakefile;


annotation(__OpenModelica_Interface="backend");
end CodegenOMSIC;