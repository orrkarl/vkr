#pragma once

#include "../predefs.h"

#include <exception>

namespace nr::base {

class CLApiException : public std::exception {
public:
    CLApiException(Status errorCode, const char* description);

    const char* what() const noexcept override;

    Status errorCode() const;

private:
    Status m_errorCode;
    const char* m_description;
};

#define CL_TYPE_CREATE_EXCEPTION(TypeName, ParentType)                                                       \
    class TypeName##CreateException : public ParentType {                                                    \
    public:                                                                                                  \
        explicit TypeName##CreateException(Status status)                                                    \
            : ParentType(status, "could not create " #TypeName) {                                            \
        }                                                                                                    \
                                                                                                             \
        TypeName##CreateException(Status status, const char* description)                                    \
            : ParentType(status, description) {                                                              \
        }                                                                                                    \
    }

}