#include <iostream>
#include "DistributedMatrix.hpp"

#define IWANTCOLOR // comment this line to remove color
#ifdef IWANTCOLOR
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#else
#define ANSI_COLOR_RED     ""
#define ANSI_COLOR_GREEN   ""
#define ANSI_COLOR_YELLOW  ""
#define ANSI_COLOR_BLUE    ""
#define ANSI_COLOR_MAGENTA ""
#define ANSI_COLOR_CYAN    ""
#define ANSI_COLOR_RESET   ""
#endif

bool isapprox(const SubMatrix& x, const SubMatrix& y, double rtol=1e-8, double atol=1e-8);
void testisapprox(const SubMatrix &x, const SubMatrix &y, char const *s, int rank);
void PrintArray(int rank, DistributedMatrix &A);
std::ostream& operator<< (std::ostream& stream, const DistributedMatrix& a);
