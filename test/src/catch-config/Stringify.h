#pragma once

#include <catch2/catch.hpp>
#include <sstream>

#include <base/Exceptions.h>
#include <base/Module.h>

#include "../util/Prettify.h"

CATCH_TRANSLATE_EXCEPTION(const nr::base::CLApiException& ex) {
    std::stringstream ret;
    ret << ex.what() << " [" << util::stringifyCLStatus(ex.errorCode()) << " = " << ex.errorCode() << "]";
    return ret.str();
}

CATCH_TRANSLATE_EXCEPTION(const nr::base::BuildFailedException& ex) {
    std::stringstream ret;

    ret << ex.what() << ":" << std::endl;
    for (size_t i = 0; i < ex.allBuildLogs().size(); ++i) {
        ret << "log from device " << i << ":" << std::endl;
        ret << ex.allBuildLogs()[i] << std::endl;
        ret << std::string(60, '=') << std::endl;
    }

    return ret.str();
}
