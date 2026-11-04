// Includes the contest template as a library for stress tests: the template
// ships its own main(), which is renamed so the test can define the real one.
// Usage (first line of every stress test):
//   #include "tests/stress/template_lib.h"
#pragma once
#define main nb_template_main
#include "template/template.cpp"
#undef main
