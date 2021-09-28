#include "Util.h"

namespace chase {

bool eventHappens(double percentage, int num)
{
    bool eventHappens = num <= int(percentage * 100);
    return eventHappens;
}

}