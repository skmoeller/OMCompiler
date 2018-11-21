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

import CodegenOMSI_common;
import CodegenUtil;
import CodegenUtilSimulation;
import CodegenFMU;


template generateOMSIC(SimCode simCode)
"Generates main entry point for omsi wrapper functions for FMI"
::=
  let &functionPrototypes = buffer ""
  let &includes = buffer ""

  match simCode
    case SIMCODE(modelInfo=MODELINFO(functions=functions, varInfo=varInfo as VARINFO(__))) then
    let modelNamePrefixStr = CodegenUtilSimulation.modelNamePrefix(simCode)

    let &functionPrototypes += <<void initialize_start_function (omsi_template_callback_functions_t* callback);<%\n%>>>

    // generate header file
    let &includes += <<#include "<%fileNamePrefix%>_sim_eqns.h"<%\n%>>>
    let &includes += <<#include "<%fileNamePrefix%>_init_eqns.h"<%\n%>>>
    let headerFileName = fileNamePrefix+"_omsic"
    let headerFileContent = CodegenOMSI_common.generateCodeHeader(fileNamePrefix, &includes, headerFileName, &functionPrototypes)
    let () = textFile(headerFileContent, headerFileName+".h")

    // generate content of C file
    <<
    <%CodegenOMSI_common.insertCopyrightOpenModelica()%>

    #include "<%fileNamePrefix%>_omsic.h"

    /* Simulation code for <%CodegenUtil.dotPath(modelInfo.name)%> generated by the OpenModelica Compiler <%getVersionNr()%>. */
    /* Translated model <%fileNamePrefix%> to OMSIC */

    /* Set function pointers for initialization in global struct. */
    void initialize_start_function (omsi_template_callback_functions_t* callback) {

      callback->initialize_initialization_problem = <%fileNamePrefix%>_instantiate_init_eqns_OMSIFunc;
      callback->initialize_simulation_problem = <%fileNamePrefix%>_instantiate_sim_eqns_OMSIFunc;

      callback->isSet = omsi_true;
    }

    <%\n%>
    >>
    /* leave new line at end of file */
end generateOMSIC;


template createMakefile(SimCode simCode, String target, String FileNamePrefix, String makeflieName)
 "Generates contents of the makefile for simulation case."
::=

  let FMUVersion="2.0"

  let &InitAlgSystemFiles = buffer""
  let &InitDerMatFiles = buffer""

  let &SimAlgSystemFiles = buffer""
  let &SimDerMatFiles = buffer""

  let _ = (match simCode
    case SIMCODE(omsiData=SOME(OMSI_DATA(simulation=simulation as OMSI_FUNCTION(__), initialization=initialization as OMSI_FUNCTION(__)))) then
      let _ = ( initialization.equations |> eq =>
        match eq
        case system as SES_ALGEBRAIC_SYSTEM(__) then
          let &InitAlgSystemFiles += " " + FileNamePrefix + "_init_eqns_algSyst_" + system.algSysIndex + ".c"
          let &InitDerMatFiles += " " + FileNamePrefix + "_init_eqns_derMat_" + system.algSysIndex + ".c"
          <<>>
        end match
      )
      let _ = ( simulation.equations |> eq =>
        match eq
        case system as SES_ALGEBRAIC_SYSTEM(__) then
          let &SimAlgSystemFiles += " " + FileNamePrefix + "_sim_eqns_algSyst_" + system.algSysIndex + ".c"
          let &SimDerMatFiles += " " + FileNamePrefix + "_sim_eqns_derMat_" + system.algSysIndex + ".c"
          <<>>
        end match
      )
      <<>>
  )

  match simCode
  case SIMCODE(modelInfo=MODELINFO(__), makefileParams=MAKEFILE_PARAMS(__), simulationSettingsOpt = sopt) then
    let includedir = '<%fileNamePrefix%>.fmutmp/sources/include/'
    let mkdir = match makefileParams.platform case "win32" case "win64" then '"mkdir.exe"' else 'mkdir'
    let OMLibs = match makefileParams.platform case "win32" case "win64" then 'lib' case "linux64" then 'lib/x86_64-linux-gnu' else 'lib'
    let lapackDirWin = match makefileParams.platform case "win32" case "win64" then '-L$(OMDEV)/tools/msys/mingw32/bin/libopenblas.dll -L$(OMDEV)/tools/msys/mingw64/bin/libopenblas.dll' else ''

    <<
    # Makefile generated by OpenModelica
    OMHOME=<%makefileParams.omhome%>
    OMLIB=<%makefileParams.omhome%>/<%OMLibs%>

    CC=<%makefileParams.ccompiler%>
    CFLAGS= -fPIC -Wall -Wextra -ansi -pedantic -g
    CXX=<%makefileParams.cxxcompiler%>
    LD=$(CC) -shared

    RUNTIMEPATH=.

    # Files
    MAINFILE=<%fileNamePrefix%>_omsic.c
    MAINOBJ=<%fileNamePrefix%>_omsic.o
    INIT_FILES=<%fileNamePrefix%>_init_eqns.c $(INIT_ALGLOOP_FILES) $(INIT_DERMAT_FILES)
    INIT_ALGLOOP_FILES=<%&InitAlgSystemFiles%>
    INIT_DERMAT_FILES=<%&InitDerMatFiles%>
    SIM_FILES=<%fileNamePrefix%>_sim_eqns.c $(SIM_ALGLOOP_FILES) $(SIM_DERMAT_FILES)
    SIM_ALGLOOP_FILES=<%&SimAlgSystemFiles%>
    SIM_DERMAT_FILES=<%&SimDerMatFiles%>
    CFILES=$(MAINFILE) $(INIT_FILES) $(SIM_FILES)
    OFILES=$(CFILES:.c=.o)
    HFILES=$(CFILES:.c=.h)

    GENERATEDFILES=$(MAINFILE) <%fileNamePrefix%>_FMU.makefile # ...

    # Includes
    INCLUDE_DIR_OMSI=$(OMHOME)/include/omc/omsi
    INCLUDE_DIR_OMSI_BASE=$(OMHOME)/include/omc/omsi/base
    INCLUDE_DIR_OMSI_SOLVER=$(OMHOME)/include/omc/omsi/solver
    INCLUDE_DIR_OMSI_FMI2=$(OMHOME)/include/omc/omsi/fmi2
    INCLUDE_DIR_OMSIC=$(OMHOME)/include/omc/omsic
    INCLUDE_DIR_OMSIC_FMI2=$(OMHOME)/include/omc/omsic/fmi2

    # Libraries
    EXPAT_LIBDIR=-L$(OMHOME)/../OMCompiler/3rdParty/FMIL/build/ExpatEx
    EXPAT_LIB=-lexpat

    LAPACK_LIBDIR=<%lapackDirWin%>
    LAPACK_LIB=-lopenblas

    OMSU_STATIC_LIB=-Wl,--whole-archive -lOMSISolver_static -lOMSIBase_static -lOMSU_static -Wl,--no-whole-archive
    OMSU_STATIC_LIBDIR=-L$(OMLIB)/omc/omsi
    LIBS = $(OMSU_STATIC_LIB) -Wl,-Bdynamic $(EXPAT_LIB) $(LAPACK_LIB)
    LIBSDIR= $(OMSU_STATIC_LIBDIR) $(EXPAT_LIBDIR) $(LAPACK_LIBDIR)

    .PHONY: clean

    all: <%fileNamePrefix%>_FMU

    <%fileNamePrefix%>_FMU : compile
    <%\t%>cd <%fileNamePrefix%>.fmutmp; \
    <%\t%>zip<%makefileParams.exeext%> -r ../<%fileNamePrefix%>.fmu *;\
    <%\t%>cd ..;\
    <%\t%>rm -rf <%fileNamePrefix%>.fmutmp


    copyFiles: makeStructure
    <%\t%>cp -a $(OMHOME)/include/omc/omsi/* <%includedir%>
    <%\t%>cp -a $(OMHOME)/include/omc/omsic/* <%includedir%>
    <%\t%>cp -a $(OMLIB)/omc/omsi/libOMSIBase_static.* <%fileNamePrefix%>.fmutmp/sources/libs
    <%\t%>cp -a $(OMLIB)/omc/omsi/libOMSU_static.* <%fileNamePrefix%>.fmutmp/sources/libs
    <%\t%>cp -a $(OMLIB)/omc/omsi/libOMSISolver_static.* <%fileNamePrefix%>.fmutmp/sources/libs
    <%\t%>cp -f $(OMHOME)/../OMCompiler/3rdParty/FMIL/build/ExpatEx/libexpat.a <%fileNamePrefix%>.fmutmp/sources/libs

    <%\t%>cp -a modelDescription.xml <%fileNamePrefix%>.fmutmp/
    <%\t%>cp -a $(CFILES) <%fileNamePrefix%>.fmutmp/sources/
    <%\t%>cp -a $(HFILES) <%fileNamePrefix%>.fmutmp/sources/include
    <%\t%>cp -a <%fileNamePrefix%>_info.json <%fileNamePrefix%>.fmutmp/resources/
    <%\t%>cp -a <%fileNamePrefix%>_init.xml <%fileNamePrefix%>.fmutmp/resources/

    makeStructure:
    <%\t%><%mkdir%> -p <%fileNamePrefix%>.fmutmp/sources <%fileNamePrefix%>.fmutmp/sources/include <%fileNamePrefix%>.fmutmp/sources/libs
    <%\t%><%mkdir%> -p <%fileNamePrefix%>.fmutmp/resources
    <%\t%><%mkdir%> -p <%fileNamePrefix%>.fmutmp/binaries/<%makefileParams.platform%>

    compile: $(OFILES) copyFiles
    <%\t%>$(LD) -o <%fileNamePrefix%><%makefileParams.dllext%> $(OFILES) $(LIBSDIR) $(LIBS)
    <%\t%>cp -a <%fileNamePrefix%><%makefileParams.dllext%> <%fileNamePrefix%>.fmutmp/binaries/<%makefileParams.platform%>/

    %.o : %.c copyFiles
    <%\t%>$(CC) $(CFLAGS) -I$(INCLUDE_DIR_OMSI)  -I$(INCLUDE_DIR_OMSI_BASE) -I$(INCLUDE_DIR_OMSI_SOLVER) -I$(INCLUDE_DIR_OMSI_FMI2) -I$(INCLUDE_DIR_OMSIC) -I$(INCLUDE_DIR_OMSIC_FMI2) -c $<

    clean:
    <%\t%>rm -f *.o
    <%\t%>rm -Rf helloWorld.fmutmp

    distclean:
    <%\t%>rm -f -R <%fileNamePrefix%>.fmutmp
    >>
end createMakefile;


template createMakefileIn(SimCode simCode, String target, String FileNamePrefix, String makeflieName)
 "Generates contents of makefile.in for simulation case."
::=

  let FMUVersion="2.0"

  let &InitAlgSystemFiles = buffer""
  let &InitDerMatFiles = buffer""

  let &SimAlgSystemFiles = buffer""
  let &SimDerMatFiles = buffer""

  let _ = (match simCode
    case SIMCODE(omsiData=SOME(OMSI_DATA(simulation=simulation as OMSI_FUNCTION(__), initialization=initialization as OMSI_FUNCTION(__)))) then
      let _ = ( initialization.equations |> eq =>
        match eq
        case system as SES_ALGEBRAIC_SYSTEM(__) then
          let &InitAlgSystemFiles += " " + FileNamePrefix + "_init_eqns_algSyst_" + system.algSysIndex + ".c"
          let &InitDerMatFiles += " " + FileNamePrefix + "_init_eqns_derMat_" + system.algSysIndex + ".c"
          <<>>
        end match
      )
      let _ = ( simulation.equations |> eq =>
        match eq
        case system as SES_ALGEBRAIC_SYSTEM(__) then
          let &SimAlgSystemFiles += " " + FileNamePrefix + "_sim_eqns_algSyst_" + system.algSysIndex + ".c"
          let &SimDerMatFiles += " " + FileNamePrefix + "_sim_eqns_derMat_" + system.algSysIndex + ".c"
          <<>>
        end match
      )
      <<>>
  )

  let common = ( match simCode
    case SIMCODE(modelInfo=MODELINFO(__), makefileParams=MAKEFILE_PARAMS(__), simulationSettingsOpt = sopt) then
        <<
        MAINFILE=<%fileNamePrefix%>_omsic.c
        MAINOBJ=<%fileNamePrefix%>_omsic.o
        INIT_FILES=<%fileNamePrefix%>_init_eqns.c $(INIT_ALGLOOP_FILES) $(INIT_DERMAT_FILES)
        INIT_ALGLOOP_FILES=<%&InitAlgSystemFiles%>
        INIT_DERMAT_FILES=<%&InitDerMatFiles%>
        SIM_FILES=<%fileNamePrefix%>_sim_eqns.c $(SIM_ALGLOOP_FILES) $(SIM_DERMAT_FILES)
        SIM_ALGLOOP_FILES=<%&SimAlgSystemFiles%>
        SIM_DERMAT_FILES=<%&SimDerMatFiles%>
        CFILES= $(INIT_FILES) $(SIM_FILES)
        OFILES=$(CFILES:.c=.o)
        GENERATEDFILES=$(MAINFILE) <%fileNamePrefix%>_FMU.makefile # ...

        # FIXME: before you push into master...
        RUNTIMEDIR=<%makefileParams.omhome%>/include/omc/omsic/
        OMC_MINIMAL_RUNTIME=1
        OMC_FMI_RUNTIME=1
        # include $(RUNTIMEDIR)/Makefile.objs
        ifneq ($(NEED_RUNTIME),)
        RUNTIMEFILES=$(FMI_ME_OBJS:%=$(RUNTIMEDIR)/%.o)
        endif
        >>
  )


  match CodegenUtil.getGeneralTarget(target)
    case "msvc" then
      match simCode
        case SIMCODE(modelInfo=MODELINFO(__), makefileParams=MAKEFILE_PARAMS(__), simulationSettingsOpt = sopt) then
        let dirExtra = if modelInfo.directory then '/LIBPATH:"<%modelInfo.directory%>"' //else ""
        let libsStr = (makefileParams.libs |> lib => lib ;separator=" ")
        let libsPos1 = if not dirExtra then libsStr //else ""
        let libsPos2 = if dirExtra then libsStr // else ""
        let fmudirname = '<%fileNamePrefix%>.fmutmp'
        let extraCflags = match sopt case SOME(s as SIMULATION_SETTINGS(__)) then
          '<%match s.method
             case "inline-euler" then "-D_OMC_INLINE_EULER"
             case "inline-rungekutta" then "-D_OMC_INLINE_RK"%>'
        let compilecmds = CodegenFMU.getPlatformString2(CodegenUtilSimulation.modelNamePrefix(simCode), makefileParams.platform, fileNamePrefix, fmuTargetName, dirExtra, libsPos1, libsPos2, makefileParams.omhome, FMUVersion)
        let mkdir = match makefileParams.platform case "win32" case "win64" then '"mkdir.exe"' else 'mkdir'
        <<
        # Makefile generated by OpenModelica

        # Simulations use -O3 by default
        SIM_OR_DYNLOAD_OPT_LEVEL=
        MODELICAUSERCFLAGS=
        CXX=cl
        EXEEXT=.exe
        DLLEXT=.dll
        FMUEXT=.fmu
        PLATWIN32 = win32

        # /Od - Optimization disabled
        # /EHa enable C++ EH (w/ SEH exceptions)
        # /fp:except - consider floating-point exceptions when generating code
        # /arch:SSE2 - enable use of instructions available with SSE2 enabled CPUs
        # /I - Include Directories
        # /DNOMINMAX - Define NOMINMAX (does what it says)
        # /TP - Use C++ Compiler
        CFLAGS=/MP /Od /ZI /EHa /fp:except /I"<%makefileParams.omhome%>/include/omc/c" /I"<%makefileParams.omhome%>/include/omc/msvc/" <%if isFMIVersion20(FMUVersion) then '/I"<%makefileParams.omhome%>/include/omc/c/fmi2"' else '/I"<%makefileParams.omhome%>/include/omc/c/fmi1"'%> /I. /DNOMINMAX /TP /DNO_INTERACTIVE_DEPENDENCY  <% if Flags.isSet(Flags.FMU_EXPERIMENTAL) then '/DFMU_EXPERIMENTAL'%>

        # /ZI enable Edit and Continue debug info
        CDFLAGS=/ZI

        # /MD - link with MSVCRT.LIB
        # /link - [linker options and libraries]
        # /LIBPATH: - Directories where libs can be found
        LDFLAGS=/MD /link /dll /debug /pdb:"<%fileNamePrefix%>.pdb" /LIBPATH:"<%makefileParams.omhome%>/lib/<%getTriple()%>/omc/msvc/" /LIBPATH:"<%makefileParams.omhome%>/lib/<%getTriple()%>/omc/msvc/release/" <%dirExtra%> <%libsPos1%> <%libsPos2%> f2c.lib initialization.lib libexpat.lib math-support.lib meta.lib results.lib simulation.lib solver.lib sundials_kinsol.lib sundials_nvecserial.lib util.lib lapack_win32_MT.lib lis.lib  gc-lib.lib user32.lib pthreadVC2.lib wsock32.lib cminpack.lib umfpack.lib amd.lib

        # /MDd link with MSVCRTD.LIB debug lib
        # lib names should not be appended with a d just switch to lib/omc/msvc/debug


        <%common%>

        <%fileNamePrefix%>$(FMUEXT): <%fileNamePrefix%>$(DLLEXT) modelDescription.xml
            if not exist <%fmudirname%>\binaries\$(PLATWIN32) <%mkdir%> <%fmudirname%>\binaries\$(PLATWIN32)
            if not exist <%fmudirname%>\sources <%mkdir%> <%fmudirname%>\sources

            copy <%fileNamePrefix%>.dll <%fmudirname%>\binaries\$(PLATWIN32)
            copy <%fileNamePrefix%>.lib <%fmudirname%>\binaries\$(PLATWIN32)
            copy <%fileNamePrefix%>.pdb <%fmudirname%>\binaries\$(PLATWIN32)
            copy <%fileNamePrefix%>.c <%fmudirname%>\sources\<%fileNamePrefix%>.c
            copy <%fileNamePrefix%>_model.h <%fmudirname%>\sources\<%fileNamePrefix%>_model.h
            copy <%fileNamePrefix%>_FMU.c <%fmudirname%>\sources\<%fileNamePrefix%>_FMU.c
            copy <%fileNamePrefix%>_info.c <%fmudirname%>\sources\<%fileNamePrefix%>_info.c
            copy <%fileNamePrefix%>_init_fmu.c <%fmudirname%>\sources\<%fileNamePrefix%>_init_fmu.c
            copy <%fileNamePrefix%>_functions.c <%fmudirname%>\sources\<%fileNamePrefix%>_functions.c
            copy <%fileNamePrefix%>_functions.h <%fmudirname%>\sources\<%fileNamePrefix%>_functions.h
            copy <%fileNamePrefix%>_records.c <%fmudirname%>\sources\<%fileNamePrefix%>_records.c
            copy modelDescription.xml <%fmudirname%>\modelDescription.xml
            copy <%stringReplace(makefileParams.omhome,"/","\\")%>\bin\SUNDIALS_KINSOL.DLL <%fmudirname%>\binaries\$(PLATWIN32)
            copy <%stringReplace(makefileParams.omhome,"/","\\")%>\bin\SUNDIALS_NVECSERIAL.DLL <%fmudirname%>\binaries\$(PLATWIN32)
            copy <%stringReplace(makefileParams.omhome,"/","\\")%>\bin\LAPACK_WIN32_MT.DLL <%fmudirname%>\binaries\$(PLATWIN32)
            copy <%stringReplace(makefileParams.omhome,"/","\\")%>\bin\pthreadVC2.dll <%fmudirname%>\binaries\$(PLATWIN32)
            cd <%fmudirname%>
            "zip.exe" -r ../<%fmuTargetName%>.fmu *
            cd ..
            rm -rf <%fmudirname%>

        <%fileNamePrefix%>$(DLLEXT): $(MAINOBJ) $(CFILES)
            $(CXX) /Fe<%fileNamePrefix%>$(DLLEXT) $(MAINFILE) <%fileNamePrefix%>_FMU.c $(CFILES) $(CFLAGS) $(LDFLAGS)
        >>
      end match

    case "gcc" then
      match simCode
        case SIMCODE(modelInfo=MODELINFO(varInfo=varInfo as VARINFO(__)), delayedExps=DELAYED_EXPRESSIONS(maxDelayedIndex=maxDelayedIndex), makefileParams=MAKEFILE_PARAMS(__), simulationSettingsOpt = sopt) then
        let dirExtra = if modelInfo.directory then '-L"<%modelInfo.directory%>"' //else ""
        let libsStr = (makefileParams.libs |> lib => lib ;separator=" ")
        let libsPos1 = if not dirExtra then libsStr //else ""
        let libsPos2 = if dirExtra then libsStr // else ""
        let extraCflags = match sopt case SOME(s as SIMULATION_SETTINGS(__)) then
          '<%match s.method
             case "inline-euler" then "-D_OMC_INLINE_EULER"
             case "inline-rungekutta" then "-D_OMC_INLINE_RK"%>'
        let compilecmds = CodegenFMU.getPlatformString2(CodegenUtilSimulation.modelNamePrefix(simCode), makefileParams.platform, fileNamePrefix, fmuTargetName, dirExtra, libsPos1, libsPos2, makefileParams.omhome, FMUVersion)
        let platformstr = makefileParams.platform
        <<
        # Makefile generated by OpenModelica
        CC=@CC@
        AR=@AR@
        CFLAGS=@CFLAGS@
        LD=$(CC) -shared
        LDFLAGS=@LDFLAGS@ @LIBS@
        DLLEXT=@DLLEXT@
        NEED_RUNTIME=@NEED_RUNTIME@
        NEED_DGESV=@NEED_DGESV@
        FMIPLATFORM=@FMIPLATFORM@
        # Note: Simulation of the fmu with dymola does not work with -finline-small-functions (enabled by most optimization levels)
        CPPFLAGS=@CPPFLAGS@
        OMC_NUM_LINEAR_SYSTEMS=<%varInfo.numLinearSystems%>
        OMC_NUM_NONLINEAR_SYSTEMS=<%varInfo.numNonLinearSystems%>
        OMC_NUM_MIXED_SYSTEMS=<%varInfo.numMixedSystems%>
        OMC_NDELAY_EXPRESSIONS=<%maxDelayedIndex%>
        OMC_NVAR_STRING=<%varInfo.numStringAlgVars%>

        override CPPFLAGS += -Iinclude/ -Iinclude/fmi<%if isFMIVersion20(FMUVersion) then "2" else "1"%> -I. <%makefileParams.includes ; separator=" "%> <% if Flags.isSet(Flags.FMU_EXPERIMENTAL) then '-DFMU_EXPERIMENTAL'%>  -DOMC_MODEL_PREFIX=<%CodegenUtilSimulation.modelNamePrefix(simCode)%> -DOMC_NUM_MIXED_SYSTEMS=<%varInfo.numMixedSystems%> -DOMC_NUM_LINEAR_SYSTEMS=<%varInfo.numLinearSystems%> -DOMC_NUM_NONLINEAR_SYSTEMS=<%varInfo.numNonLinearSystems%> -DOMC_NDELAY_EXPRESSIONS=<%maxDelayedIndex%> -DOMC_NVAR_STRING=<%varInfo.numStringAlgVars%>

        <%common%>

        PHONY: <%fileNamePrefix%>_FMU
        <%compilecmds%>
        >>
      end match

    else //target is nor msvc or gcc
      CodegenUtil.error(sourceInfo(), 'target <%target%> is not handled!')

end createMakefileIn;


annotation(__OpenModelica_Interface="backend");
end CodegenOMSIC;