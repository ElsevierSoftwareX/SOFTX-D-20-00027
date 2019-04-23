/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Sebastian Wagner
 *
 * TraCurate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TraCurate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with TraCurate.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef EXAMPLES
#define EXAMPLES

/*!
 * Some examples to test/demonstrate some of TraCurates functions
 */

#define EXAMPLE_XML_FILE   "/home/sebastian/arbeit/dev/data_old/14-04-10-Pos-13-after-well_6"
#define EXAMPLE_BIG_FILE   "/home/sebastian/arbeit/dev/data/old/big-example-data.h5"
#define EXAMPLE_SMALL_FILE "/home/sebastian/arbeit/dev/data/old/smaller-example-data.h5"
#define EXAMPLE_WRITE_HDF5 "/home/sebastian/arbeit/dev/write.h5"
#define EXAMPLE_SMALL_HDF5 "/home/sebastian/arbeit/dev/smaller2_written_annotations3.h5"
#define EXAMPLE_WRITE_DIR  "/home/sebastian/arbeit/dev/write_test"
#define EXAMPLE_PROFILING  "/home/sebastian/arbeit/dev/profiling-dataset/profiling.h5"

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
void exampleSaveAllCombinations() __attribute__((noreturn));
void exampleLoadMultipleTimes() __attribute__((noreturn));
void exampleTestMemoryLeak() __attribute__((noreturn));
void exampleTestFloodFill() __attribute__((noreturn));

#endif // EXAMPLES

