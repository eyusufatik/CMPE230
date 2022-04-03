#ifndef VARS_H
#define VARS_H

#include "vector.h"

// Struct to hold vars.
static struct Variables {
    vector scalar_names;

    vector vector_names;
    vector vector_dimensions;

    vector matrix_names;
    vector matrix_dimensions;
} vars;

#endif