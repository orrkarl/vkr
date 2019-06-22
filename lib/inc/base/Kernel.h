#pragma once

#include "../general/predefs.h"

#include "Wrapper.h"

namespace nr
{

class Module;

/**
 * A wrapper for kernel
 **/
class NR_SHARED Kernel : public Wrapper<cl_kernel>
{
public:
    Kernel();

    explicit Kernel(const cl_kernel& kernel, const NRbool retain = false);

    Kernel(Module module, const string name, cl_status* err = nullptr);

    Kernel(const Kernel& other);

    Kernel(Kernel&& other);

    Kernel& operator=(const Kernel& other);

    Kernel& operator=(Kernel&& other);

    operator cl_kernel() const;

    template<typename T>
    typename std::enable_if<!std::is_pointer<T>::value && !std::is_base_of<Wrapper<cl_mem>, T>::value, cl_status>::type setArg(const NRuint index, const T& value)
    {
        return clSetKernelArg(object, index, sizeof(T), &value);
    }

    template<typename T>
    typename std::enable_if<!std::is_pointer<T>::value && std::is_base_of<Wrapper<cl_mem>, T>::value, cl_status>::type setArg(const NRuint index, const T& value)
    {
        return clSetKernelArg(object, index, sizeof(T), &(value.get()));
    }
    
    template<typename T>
    cl_status setArg(const NRuint index, const NRuint size, const T value)
    {
        return clSetKernelArg(object, index, size, &value);
    }
};

}
