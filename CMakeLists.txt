cmake_minimum_required(VERSION 3.26)
project(data_fusion_db)

set(CMAKE_CXX_STANDARD 14)

add_executable(data_fusion_db
        main.cpp
        SequentialFile.h
        ISAMSparseIndex.h
        ExtendibleHashing.h
        Compiler.h
)
