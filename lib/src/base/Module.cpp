#include <general/predefs.h>

#include <base/Module.h>
#include <utils/converters.h>

#include <numeric>
#include <iostream>

namespace nr
{

const Module::Macro           Module::DEBUG               = Module::Macro("_DEBUG");
const Module::CLVersion       Module::CL_VERSION_22       = Module::CLVersion("2.2");
const Module::CLVersion       Module::CL_VERSION_21       = Module::CLVersion("2.1");
const Module::CLVersion       Module::CL_VERSION_20       = Module::CLVersion("2.0");
const Module::CLVersion       Module::CL_VERSION_12       = Module::CLVersion("1.2");
const Module::CLVersion       Module::CL_VERSION_11       = Module::CLVersion("1.1");
const Module::Option          Module::WARNINGS_ARE_ERRORS = Module::Option("-Werror");
const Module::RenderDimension Module::_3D                 = Module::RenderDimension(3);
const Module::RenderDimension Module::_4D                 = Module::RenderDimension(4);
const Module::RenderDimension Module::_5D                 = Module::RenderDimension(5);

}
