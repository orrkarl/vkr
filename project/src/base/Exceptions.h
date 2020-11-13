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

#define CL_TYPE_CREATE_EXCEPTION(ExceptionClassName, PrettyTypeName)                                         \
    class ExceptionClassName : public CLApiException {                                                       \
    public:                                                                                                  \
        explicit ExceptionClassName(Status status)                                                           \
            : CLApiException(status, "could not create" PrettyTypeName)                                      \
        {                                                                                                    \
        }                                                                                                    \
                                                                                                             \
        ExceptionClassName(Status status, const char* description)                                           \
            : CLApiException(status, description)                                                            \
        {                                                                                                    \
        }                                                                                                    \
    };

}