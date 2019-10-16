#pragma once

struct TempAllocator
{
    static constexpr unsigned PAGE_SIZE = 4096;  // TODO(ted): Query this.

    using byte = char;

    byte*    buffer   = nullptr;
    unsigned index    = 0;
    unsigned capacity = 0;

    TempAllocator() = default;

    explicit TempAllocator(unsigned pages) : index(0), capacity(pages * PAGE_SIZE)
    {
        buffer = static_cast<byte*>(malloc(capacity));
    }
};

[[gnu::warn_unused_result]]
bool HasRoom(const TempAllocator& allocator, const unsigned bytes)
{
    return allocator.index + bytes < allocator.capacity;
}

void Clear(TempAllocator& allocator) noexcept
{
    allocator.index = 0;
}

[[gnu::warn_unused_result]]
char* Allocate(TempAllocator& allocator, const unsigned bytes)
{
    const unsigned index_to_object = allocator.index;
    allocator.index += bytes;

    return &allocator.buffer[index_to_object];
}

[[gnu::warn_unused_result]]
TempAllocator Partition(TempAllocator& allocator, const unsigned bytes)
{
    const u32 start = allocator.capacity - bytes;
    allocator.capacity = start;

    TempAllocator partitioned_allocator {};
    partitioned_allocator.buffer   = allocator.buffer;
    partitioned_allocator.capacity = bytes;
    partitioned_allocator.index    = start;

    return partitioned_allocator;
}