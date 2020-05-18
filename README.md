# Programming by Contract
Programming by Contract for C++ and C

## Author

* **Jeff Hurchalla**

## License

This project is licensed under the MIT License - see the
[LICENSE.TXT](LICENSE.TXT) file for details

## TODO

Document options HPBC_WRAP_STDLIB_ASSERT, TEST_HURCHALLA_TOP_LEVEL, and
TEST_HURCHALLA_LIBS

## Running CMake

To run cmake for this project, open a command prompt in your desired build
directory, and assuming <src_dir> is the name of the directory on your system
that contains these program_by_contract sources, type:
   cmake <src_dir>
To build with one or more of the options, use -D<option>=ON for each desired
option. For example if you wish to enable the TEST_HURCHALLA_LIBS option, type:
   cmake -DTEST_HURCHALLA_LIBS=ON <src_dir>
