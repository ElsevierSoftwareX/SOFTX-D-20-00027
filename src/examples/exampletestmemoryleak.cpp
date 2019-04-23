/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2017, 2016 Sebastian Wagner
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
#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

#include "examples.h"
#include "project.h"
#include "io/importhdf5.h"

void process_mem_usage(double& vm_usage, double& resident_set);
void process_mem_usage(double& vm_usage, double& resident_set)
{
#ifndef _WIN32
   using std::ios_base;
   using std::ifstream;
   using std::string;

   vm_usage     = 0.0;
   resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat",ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   //
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   //
   unsigned long vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   stat_stream.close();

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = vsize / 1024.0;
   resident_set = rss * page_size_kb;
#endif
}

void exampleTestMemoryLeak() {
    using namespace TraCurate;
    double vm, rss;
    process_mem_usage(vm, rss);
    std::cerr << "VM: " << vm << " RSS: " << rss << std::endl;

    {
        ImportHDF5 ih;
        std::shared_ptr<Project> proj;

        for (int i = 0; i < 10; i++) {
            std::cerr << "Run " << (i+1) << std::endl;
            proj = ih.load(EXAMPLE_SMALL_HDF5);
            process_mem_usage(vm, rss);
            std::cerr << "VM: " << vm << " RSS: " << rss << std::endl;
            proj.reset();
        }
    }
    exit(0);
}
