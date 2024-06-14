// trigger optimisation from source file
#pragma GCC optimize("O3")
#pragma GCC optimize("inline")
#pragma GCC optimize("omit-frame-pointer")
#pragma GCC optimize("unroll-loops") //Optimization flags

#pragma GCC optimize("Ofast")

#pragma GCC option("arch=native","tune=native","no-zero-upper") //Enable AVX
#pragma GCC target("avx")  //Enable AVX
#pragma GCC target "bmi2"
#include <x86intrin.h> //AVX/SSE Extensions
