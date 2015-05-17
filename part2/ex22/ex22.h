#ifndef _exe_h
#define _exe_h

// in the context of data, extern means that we are declaring
// a variable to the compiler which is instantiated elsewhere.
//
// It performs two slightly different functions: in the file where
// the variable is actually instantiated, having the extern declaration
// tells the compiler to make this available for linking when making
// object code.
//
// When it's included in a file that does not instantiate the variable,
// the extern declarationt tells the compiler to assume the variable
// exists when creating the object code; it will get linked in later.
extern int THE_SIZE;

int get_age();
void set_age(int age);

double update_ratio(double ratio);

void print_size();

#endif
