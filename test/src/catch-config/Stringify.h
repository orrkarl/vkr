#pragma once

#include <base/Exceptions.h>
#include <catch2/catch.hpp>
#include <sstream>

#include "../util/Prettify.h"

CATCH_TRANSLATE_EXCEPTION(const nr::base::CLApiException& ex) {
    std::stringstream ret;
    ret << ex.what() << " [" << util::stringifyCLStatus(ex.errorCode()) << " = " << ex.errorCode() << "]";
    return ret.str();
}
