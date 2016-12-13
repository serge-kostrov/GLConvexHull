#include "base.h"
#include "constants.h"


double drand(double min, double max)
{
    return (max - min)*(static_cast<double>(::rand()) / RAND_MAX) + min;
}
