#include "MemoryView.h"

namespace nr::base {

MemoryView::MemoryView(cl_mem memoryObj)
    : m_memory(memoryObj)
{
}

cl_mem MemoryView::rawHandle() { return m_memory; }

}
