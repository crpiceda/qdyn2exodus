## About
A simple c++ snippet (first commit) to convert output files from Qdyn (format "snapshots") into exodusii format (to be visualize and handle in Paraview, Nemesis etc ...).

#### 1. Installation
The snippet is written in c++ and it relies on basic Qt classes. So, you would need to have a QT installation locally. And, of course, you would need to have a local installation of the exodusii library.
I added a template Qt project file (*.pro and *.pri) where I link to libmesh library and retrieve the exodus mesh library format from it (-lmesh).
Caveat: there's no real need to install the full libmesh (relatively heavy) library package, but I use it quite a lot and it comes easy to me.

#### 4. test (WARNING: the qdyn snapshot file (though compressed) is really heavy and it can slow down the cloning)
There is a test example (which I took from the only 3D tutorial from the Qdyn website). It comes with a Paraview state file to create the movie (all inside the test/tut_04/gold directory).
Note: to run the test you would need first to untar the original qdyn snapshot file!
Caveat: all is hardcoded, so once compiled, any launch of the executable will run the test. Any user should modify the original source code to his/her needs.
These are just a couple of changes in the main file:
* "_input_folder_name" --> a string pointing to the mother directory of the test you want to run
* "_input_file_name"   --> a string pointing to the name of the qdyn snapshot you want to convert
Caveat: the original format from qdyn is not really handling! Therefore, I require the user to reprocess the original file to create a well formatted csv file like in the example provided (I make it directly in the original qdyn python interface)
* "_input_file_name_last" --> a string pointing to the name of the qdyn last snapshot you want to convert
* "_nx" --> (unsigned) int that provides the number of points along the x-fault axis
* "_nz" --> (unsigned) int that provides the number of points along the z-fault axis
* "_ntimes" --> (unsigned) int that provides the number of time steps to convert
Caveat: this last parameter I do not really need, and I only use it to output the percentage of the reading routine, so you can skip it by declaring off that console printing in the main file.

## Limitations
As said, this is a fast and furious implementation. A lot of things could and should be improved. I will try to implement the output inside the main source of Qdyn (time is an issue here though...).
Below the main assumptions/limitations/todos ...:
* So far the export only works for 3D problems (2D faults) --> I do personally not see any reason to have an export for 1D faults though
* It assume the same spatial rendering of the fault in the snapshots for each time steps as well as for the last one (debugging level - really easy)
* It works only for single faults, this is mainly because I do not have any example so far with multiple fault segments (debugging level - easy)

There might be some other points, which I cannnot recollect at the moment, but will add in case they show up in my mind.

My personal todo list to anyone interested:
* Convert the snippet into an instance of the output functionalities available in the utils in qdyn (easy)
* Add the exporting routine inside the main code (medium and time consuming)
