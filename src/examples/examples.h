#ifndef EXAMPLES
#define EXAMPLES

#define EXAMPLE_XML_FILE   "/home/sebastian/arbeit/dev/data_old/14-04-10-Pos-13-after-well_6"
#define EXAMPLE_BIG_FILE   "/home/sebastian/arbeit/dev/data/old/big-example-data.h5"
#define EXAMPLE_SMALL_FILE "/home/sebastian/arbeit/dev/data/old/smaller-example-data.h5"
#define EXAMPLE_WRITE_HDF5 "/home/sebastian/arbeit/dev/write.h5"

#define CURRENT_EXAMPLE EXAMPLE_SMALL_FILE

void exampleAddAnnotation();
void exampleAddTrackDivision();
void exampleIdProvider();
void exampleLoadProjectHDF5 ();
void exampleLoadProjectXML();
void exampleObject();
void exampleWriteAllImages();
void exampleWriteHDF5();
void exampleWriteOutlineToGnuplot ();
void exampleWriteProjectStdout();
void exampleSignalSlot();

#endif // EXAMPLES

