

Marlin (**M**odular **A**nalysis and **R**econstruction for the **LIN**ear collider) is a simple modular application framework for analysis and reconstruction code based on LCIO.

# Overview

The main purpose of Marlin is to facilitate the modular development of reconstruction and analysis code based on LCIO. As a lot of different groups are involved it should be simple and straight forward to have distributed development of modules and combine existing modules as needed in a larger application.
The base class for a Marlin module is called `marlin::Processor`. It defines a set of callbacks that the user can implement in their subclasses. A steering file mechanism allows to activate the needed processors. These are then called for every event using the LCEvent as container for input and output data in terms of LCCollections:

![Overview](https://github.com/iLCSoft/Marlin/raw/master/doc/marlin_overview.gif)

The processors are compiled in shared libraries (i.e libMyProcessor.so) and loaded at runtime using the `marlin::PluginManager`. The libraries to use when running Marlin are exported in the environment variable `MARLIN_DLL`:

```shell
$ export MARLIN_DLL=./lib/libMyProcessor.so
```

or with multiple libraries (split by `:`):


```shell
$ export MARLIN_DLL=./lib/libMyProcessor.so:./libOtherProcessor.so
```

# Marlin usage

## The `Marlin` executable

The executable provided by the software is called `Marlin`. The current help command gives the following output:

```shell
$ Marlin -h
Parsing command line ...
 Usage: Marlin [OPTION] [FILE]...
   runs a Marlin application

 Running the application with a given steering file:
   Marlin steer.xml   

   Marlin [-h/-?]             	 print this help information
   Marlin -x [steer.xml]      	 print an example steering file to output file file (default: marlin_steer.xml)

 Example:
 To create a new default steering file from any Marlin application, run
     Marlin -x  mysteer.xml
 Dynamic command line options may be specified in order to overwrite individual steering file parameters, e.g.:
     Marlin --datasource.LCIOInputFiles="input1.slcio input2.slcio" --geometry.CompactFile=mydetector.xml
            --MyLCIOOutputProcessor.LCIOWriteMode=WRITE_APPEND --MyLCIOOutputProcessor.LCIOOutputFile=out.slcio steer.xml

     NOTE: Dynamic options do NOT work together with Marlin options (-x, -f) nor with the MarlinGUI
```

The XML steering file list all configuration parameters of the different Marlin components:

- processors
- geometry
- data source
- global parameters

The `<execute>` section of the steering file lists the processors to run sequentially in the application.

For more information see [XML parser documentation](xml-parser.md).

## Dumping plugins

It is sometimes very useful, before running a `Marlin` application, to dump the registered plugins. The executable `MarlinDumpPlugins` list all registered plugins in the Marlin framework and from all libraries listed in the `MARLIN_DLL` variable.

Without `MARLIN_DLL` (possible output):

```shell
$ MarlinDumpPlugins
[ MESSAGE "PluginManager"] ------------------------------------
[ MESSAGE "PluginManager"]  ** Marlin plugin manager dump **
[ MESSAGE "PluginManager"]  Processor plugins:
[ MESSAGE "PluginManager"]  - CPUCrunching
[ MESSAGE "PluginManager"]  - EventSelector
[ MESSAGE "PluginManager"]  - LCIOOutputProcessor
[ MESSAGE "PluginManager"]  - MemoryMonitor
[ MESSAGE "PluginManager"]  - SimpleFastMCProcessor
[ MESSAGE "PluginManager"]  - Statusmonitor
[ MESSAGE "PluginManager"]  - TestProcessor
[ MESSAGE "PluginManager"]  GeometryPlugin plugins:
[ MESSAGE "PluginManager"]  - DD4hepGeometry
[ MESSAGE "PluginManager"]  - EmptyGeometry
[ MESSAGE "PluginManager"]  - GearGeometry
[ MESSAGE "PluginManager"]  DataSource plugins:
[ MESSAGE "PluginManager"]  - LCIO
[ MESSAGE "PluginManager"]  - StdHep
[ MESSAGE "PluginManager"] ----------------------------------
```

Using `MARLIN_DLL` variable:

```shell
# Contains a processor called 'MyProcessor'
$ export MARLIN_DLL=./lib/libMyProcessor.so
$ MarlinDumpPlugins
[ MESSAGE "PluginManager"] ------------------------------------
[ MESSAGE "PluginManager"]  ** Marlin plugin manager dump **
[ MESSAGE "PluginManager"]  Processor plugins:
[ MESSAGE "PluginManager"]  - CPUCrunching
[ MESSAGE "PluginManager"]  - EventSelector
[ MESSAGE "PluginManager"]  - LCIOOutputProcessor
[ MESSAGE "PluginManager"]  - MemoryMonitor
[ MESSAGE "PluginManager"]  - SimpleFastMCProcessor
[ MESSAGE "PluginManager"]  - Statusmonitor
[ MESSAGE "PluginManager"]  - TestProcessor
[ MESSAGE "PluginManager"]  - MyProcessor
[ MESSAGE "PluginManager"]  GeometryPlugin plugins:
[ MESSAGE "PluginManager"]  - DD4hepGeometry
[ MESSAGE "PluginManager"]  - EmptyGeometry
[ MESSAGE "PluginManager"]  - GearGeometry
[ MESSAGE "PluginManager"]  DataSource plugins:
[ MESSAGE "PluginManager"]  - LCIO
[ MESSAGE "PluginManager"]  - StdHep
[ MESSAGE "PluginManager"] ----------------------------------
```

## Getting a template steering file

As shown in the `Marlin` help command above, you can dump a steering file containing all parameters of all registered processors in Marlin and the processors listed in `MARLIN_DLL`:

```shell
$ Marlin -x steer.xml
```
