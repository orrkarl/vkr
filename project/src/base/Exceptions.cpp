#include "Exceptions.h"

namespace nr::base {

CLApiException::CLApiException(Status errorCode, const char* description)
    : m_errorCode(errorCode)
    , m_description(description)
{
}

const char* nr::base::CLApiException::what() const noexcept
{
    return m_description;
}

Status CLApiException::errorCode() const
{
    return m_errorCode;
}

}
