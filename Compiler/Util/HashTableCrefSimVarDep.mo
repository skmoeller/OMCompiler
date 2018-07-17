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

encapsulated package HashTableCrefSimVarDep

/* Below is the instance specific code. For each hashtable the user must define:

Key        - The key used to uniquely define elements in a hashtable
Value      - Tuple of SimCodeVar.SimVar and dependecy information for SimVar
hashFunc   - A function that maps a key to a positive integer.
keyEqual   - A comparison function between two keys, returns true if equal.
*/

/* HashTable instance specific code */

// public inport
import BaseHashTable;
import DAE;
import SimCodeVar;
import Error.addInternalError;

// protected import
protected
import ComponentReference;

public
type Key = DAE.ComponentRef;
type Value = tuple<SimCodeVar.SimVar, Integer>;

type HashTableCrefFunctionsType = tuple<FuncHashCref,FuncCrefEqual,FuncCrefStr,FuncExpStr>;
type HashTable = tuple<
  array<list<tuple<Key,Integer>>>,
  tuple<Integer,Integer,array<Option<tuple<Key,Value>>>>,
  Integer,
  HashTableCrefFunctionsType
>;

partial function FuncHashCref
  input Key cr;
  input Integer mod;
  output Integer res;
end FuncHashCref;

partial function FuncCrefEqual
  input Key cr1;
  input Key cr2;
  output Boolean res;
end FuncCrefEqual;

partial function FuncCrefStr
  input Key cr;
  output String res;
end FuncCrefStr;

partial function FuncExpStr
  input Value exp;
  output String res;
end FuncExpStr;

function emptyHashTable
"
  Returns an empty HashTable.
  Using the default bucketsize..
"
  output HashTable hashTable;
algorithm
  hashTable := emptyHashTableSized(BaseHashTable.defaultBucketSize);
end emptyHashTable;

function emptyHashTableSized
"Returns an empty HashTable.
 Using the bucketsize size."
  input Integer size;
  output HashTable hashTable;
algorithm
  hashTable := BaseHashTable.emptyHashTableWork(size,
               (ComponentReference.hashComponentRefMod,
                ComponentReference.crefEqual,
                ComponentReference.printComponentRefStr, opaqueStr));
end emptyHashTableSized;

function opaqueStr
  input Value value;
  output String str;
algorithm
  _ := match (value)
    local
      SimCodeVar.SimVar var;
      Integer mappedIndex;
    case ((var, mappedIndex))
      algorithm
        str := "#SimVar(index="+String(var.index)+",name="+ComponentReference.printComponentRefStr(var.name)+")#"
             + "#Integer=" + String(mappedIndex)+")#";
      then ();
    end match;
end opaqueStr;


public function addSimVarDepToHashTable
"adds (SimVar,integer) to hash table inHT and returns extended hash table"
  input   Value     valueIn;
  input   HashTable inHT;
  output  HashTable outHT;
protected
  SimCodeVar.SimVar simVarIn;
  Integer integerIn;

algorithm
  (simVarIn, integerIn) := match (valueIn)
    local
      SimCodeVar.SimVar simVar;
      Integer integer;

    case (simVar as SimCodeVar.SIMVAR(__), integer as _ )
      then(simVar, integer);
  end match;

  outHT :=
  matchcontinue (simVarIn, integerIn, inHT)
    local
      DAE.ComponentRef cr, acr;
      SimCodeVar.SimVar sv;
      Integer mappedIndexOut;

    case (sv as SimCodeVar.SIMVAR(name = cr, arrayCref = NONE()), _, _)
      equation
        outHT = BaseHashTable.add((cr, (sv, integerIn)), inHT);
      then outHT;
        // add the whole array crefs to the hashtable, too
    case (sv as SimCodeVar.SIMVAR(name = cr, arrayCref = SOME(acr)), _, _)
      equation
        outHT = BaseHashTable.add((acr, (sv, integerIn)), inHT);
        outHT = BaseHashTable.add((cr, (sv, integerIn)), outHT);
      then outHT;
    else
      equation
        Error.addInternalError("function addSimVarToHashTable failed", sourceInfo());
      then
        fail();
  end matchcontinue;
end addSimVarDepToHashTable;


annotation(__OpenModelica_Interface="backend");
end HashTableCrefSimVarDep;