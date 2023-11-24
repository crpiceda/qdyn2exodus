## About
A simple c++ snippet (first commit) to convert output file format from Qdyn (snaposhots) into an exodus format

#### 1. Installation
It is written based on Qt classes so you would need to have an QT installation locally. And, of course, you would need to have a local installation of the exodus library.
There'S an example of a Qt project file (*.pro and *.pri) where I link to libmesh and retrieve the exodus mesh library format from it. Caveat: tehre's no real need to install the full libmesh (relatively heavy) library packages, but I used them quite a lot and it comes easy to me.


*Note: the "devel" branch of GOLEM is the "up-to-date" branch which is updated only if all tests are passing.*

#### 4. test
There is a test example (which I took from the only 3D tutorial from the Qdyn website). It is hardcoded, so once compiled, any launch of the executable will run it. Any user should modify the original source code to his/her needs. These are just a couple of changes in the main file:
* "_input_folder_name" --> a string poiting to the mother directory of the test you want to run
* "_input_file_name"   --> a string poiting to the name of the qdyn snapshot you want to convert. Caveat: the original format from qdyn is not easily handling! Therefore, I require to reprocess the original file to create a well formatted csv file like in the example provided(I make it directly in the original qdyn python interface)
* "_input_file_name_last" --> a string poiting to the name of the qdyn last snapshot you want to convert
* "_nx" --> (unsigned) int that provides the number of points along the x-fault axis
* "_nz" --> (unsigned) int that provides the number of points along the z-fault axis
* "_ntimes" --> (unsigned) int that provides the number of time steps to convert. Caveat: this I do not really need it, only to nicely output the percentage of the reading routine, so you can skip it by declaring off that console printing in the main file.

## Limitations
As said, this is a fast and furious implemantation. A lot of things could and should be improved, and I will certainly try to implement the output inside the main source of Qdyn (time is an issue here though...).
Below the main assumptions/limitations/todos ...:
* So far the export only works for 3D problems (2D faults) --> I do personally not see any reason to have an export for 1D faults though.
* It assume the same spatial rendering of the fault in the snapshots for each time steps as well as for the last one (debugging level - really easy)
* It works only for a single fault, this is mainly because I do not have any example so far with multiple fault segments (debugging level - easy)

There might be some other points, which I cannnot recollect at the moment, but will add in case they show up in my mind.
