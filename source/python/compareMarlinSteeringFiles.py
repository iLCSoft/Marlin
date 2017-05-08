#!/bin/env python

import sys
from pprint import pprint
from copy import deepcopy

from xml.etree.ElementTree import ElementTree

def getTree( xmlFile ):
  """ parse file and return XML tree """
  et = ElementTree()
  et.parse( xmlFile )
  return et


def getProcessors( tree ):
  """ return a dict of all processors and their parameters """
  processors = dict()
  procElements = tree.findall('processor')
  for proc in procElements:
    procName = proc.attrib.get("name")
    paramDict = dict()
    parameters = proc.findall('parameter')
    for param in parameters:
      paramName = param.attrib.get('name')
      paramDict[paramName] = getValue( param , "<This Value did not Exist in this file or was empty>" )
    processors[procName] = paramDict

  groupElements = tree.findall('group')
  for group in groupElements:
    groupParam = dict()
    parameters = group.findall('parameter')
    for param in parameters:
      paramName = param.attrib.get('name')
      groupParam[paramName] = getValue( param , "<This Value did not Exist in this file or was empty>" )

    procElements = group.findall("processor")
    for proc in procElements:
      procName = proc.attrib.get("name")
      paramDict = deepcopy(groupParam)
      parameters = proc.findall('parameter')
      for param in parameters:
        paramName = param.attrib.get('name')
        paramDict[paramName] = getValue( param , "<This Value did not Exist in this file or was empty>" )
      processors[procName] = paramDict


  return processors


def getValue( element, default=None ):
  if element.get('value'):
    return element.get('value').strip()
  if element.text:
    return element.text.strip()
  return default


def getGlobalDict( globalElements ):
  """return dict of global parameters and values """
  pars = dict()
  for param in globalElements:
    name = param.get('name')
    value = getValue( param )
    pars[name] = value

  return pars

def compareGlobalParameters( tree1, tree2 ):
  """ compare the global parameters in the marlin steering files """

  par1 = getGlobalDict( tree1.findall('global/parameter') )
  par2 = getGlobalDict( tree2.findall('global/parameter') )
  parCopy1 = deepcopy(par1)
  parCopy2 = deepcopy(par2)

  for name1, value1 in par1.iteritems():
    if name1 in par2 and value1 == par2.get(name1, "<Unknown Parameter>"):
      ## remove parameters that are equal
      del parCopy1[name1]
      del parCopy2[name1]

  print "\n++++ GlobalParameters that exist in the first file that are different or non-existent in the second file"
  pprint( parCopy1 )
  print "\n++++ GlobalParameters that exist in the second file that are different or non-existent in the first file"
  pprint( parCopy2 )


def compareExecutingProcessors( tree1, tree2 ):
  """ compare the list of processors to execute, order matters """
  exec1 = tree1.findall('execute/processor')
  exec2 = tree2.findall('execute/processor')
  if len(exec1) != len(exec2):
    print "Mismatch in number of executing processors!", len(exec1), "vs", len(exec2)

  for index, proc1 in enumerate( exec1 ):
    if index >= len(exec2):
      print "More processors in first file than in second"
      break
    proc2 = exec2[index]
    name1 = proc1.get('name')
    name2 = proc2.get('name')
    if name1 != name2:
      print "Difference in executing processors", name1, "does not match processor", name2
      print "Check the order of processor execution!"
      break

def compareExecutingGroups( tree1, tree2 ):
  """ compare the list of groups to execute, order matters """
  exec1 = tree1.findall('execute/group')
  exec2 = tree2.findall('execute/group')
  if len(exec1) != len(exec2):
    print "Mismatch in number of executing groups!", len(exec1), "vs", len(exec2)

  for index, grp1 in enumerate( exec1 ):
    if index >= len(exec2):
      print "More groups in first file than in second"
      break
    grp2 = exec2[index]
    name1 = grp1.get('name')
    name2 = grp2.get('name')
    if name1 != name2:
      print "Difference in executing groups", name1, "does not match group", name2
      print "Check the order of group execution!"
      break

def __keepOnlySelected( processors, selectedProcessors ):
  """ keep only those processors we want to compare """
  if not selectedProcessors:
    return

  for processor in processors.keys():
    keep = False
    for procString in selectedProcessors:
      if procString.lower() in processor.lower():
        keep=True
        break

    if not keep:
      del processors[processor]

def compareTrees( tree1, tree2, selectedProcessors=None ):
  """compare the content of the two xml trees, as these are marlin steering
  files we only look for processors, get their parameters and then compare the
  value of all the processor parameters

  """

  processors1 = getProcessors( tree1 )
  processors2 = getProcessors( tree2 )

  __keepOnlySelected( processors1, selectedProcessors )
  __keepOnlySelected( processors2, selectedProcessors )

  ### we need a copy so we can remove all parameters/processors that are equal
  procCopy1 = deepcopy(processors1)
  procCopy2 = deepcopy(processors2)

  for proc1, parameters1 in processors1.iteritems():
    if proc1 in processors2:
      parameters2 = processors2[ proc1 ]

      for param1, value1 in parameters1.iteritems():
        if param1 in parameters2:
          value2 = parameters2[param1].strip()
          if  value2 == value1.strip():
            del procCopy1[proc1][param1]
            del procCopy2[proc1][param1]

    ## remove empty parameter dicts
    if not procCopy1.get(proc1):
      del procCopy1[proc1]
    #default is true to not trying to delete non-existent entry
    if not procCopy2.get(proc1, True):
      del procCopy2[proc1]

  print "\n++++ Entries that exist in the first file that are different or non-existent in the second file"
  pprint( procCopy1 )
  print "\n++++ Entries that exist in the second file that are different or non-existent in the first file"
  pprint( procCopy2 )


def run():
  args = sys.argv
  if len(args) < 3:
    print "incorrect number of input files, need two marlin steering files as argument and optionally select processors to compare"
    print "compareMarlinSteeringFiles.py file1.xml file2.xml [processor1] [...]"
    exit(1)

  try:
    tree1 = getTree( args[1] )
    tree2 = getTree( args[2] )
    selectedProcessors = args[3:]
  except Exception as ex:
    print "Exception when getting trees: %r " % ex
    exit(1)

  print "\n++++ Start comparing"
  compareExecutingProcessors( tree1, tree2 )
  print ""
  compareExecutingGroups( tree1, tree2 )
  print ""
  compareGlobalParameters( tree1, tree2 )
  print ""
  compareTrees( tree1, tree2, selectedProcessors)
  print "\n++++ Done comparing"


if __name__ == "__main__":
  run()
