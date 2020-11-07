#pragma once

#include "../predefs.h"

#include <exception>

namespace nr::base {

class CLApiException : public std::exception {
public:
    CLApiException(Status errorCode, const char* description);

    [[nodiscard]] const char* what() const noexcept override;

    [[nodiscard]] Status errorCode() const;

private:
    Status m_errorCode;
    const char* m_description;
};

}