// Scratch validation for Handles and Registry Phase
#include <cassert>
#include <cstdio>
#include "renderer/core/HandleRegistry.hpp"

namespace {
    struct DummyBuffer {
        int bytes = 0;
        explicit DummyBuffer(int b) : bytes(b) {}
    };

    using DummyTag      = yumi::rhi::BufferTag;
    using DummyRegistry = yumi::rhi::HandleRegistry<DummyTag, DummyBuffer>;
}

int main() {
    DummyRegistry reg;

    // Three creates -> Three distinct handles
    auto a = reg.create(64);
    auto b = reg.create(128);
    auto c = reg.create(256);

    assert(a != b && b != c && a.is_valid());
    assert(reg.live_count() == 3);

    // Get Resolves
    assert(reg.get(b) != nullptr && reg.get(b)->bytes == 128);

    // Destroy middle one
    reg.destroy(b);
    assert(!reg.is_valid(b) && reg.get(b) == nullptr && reg.live_count() == 2);

    // Create REUSES Slot 1 with a Bumped Generation
    auto d = reg.create(999);
    assert(d.index() == b.index() && d.generation() == b.generation() + 1);

    // Stale Handle must be rejected everywhere
    assert(!reg.is_valid(b) && reg.get(b) == nullptr);

    // Double destroy -> logged, safe no-op (no crash, no gen bump)
    reg.destroy(b);
    assert(d.generation() == b.generation() + 1 && reg.live_count() == 3);

    std::puts("ALL HANDLE TESTS PASSED");
    return 0;
}