#include "GameConstants.h"

static Constants constants_;

Constants& Constants::get()
{
    return constants_;
}
