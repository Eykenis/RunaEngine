#include "../IndexBufferModule.h"
#include <cstdint>

class IndexBufferGL : public IndexBufferModule {
    public:
    virtual int Init(void* data, uint32_t count, uint32_t stride_size, uint32_t index);
    virtual uint32_t getIndex();
    virtual ~IndexBufferGL() { }
    private:
    uint32_t mEBO;
};