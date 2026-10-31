// © Joseph Cameron - MIT License

#include "conformance.h"
#include "scratch_roots.h"

#include <jfc/storage/filesystem_store.h>

namespace jfc::storage::conformance {
    store_ptr_type make_store() { return make_filesystem_store(jfc_storage_test::next_scratch_root()); }

    std::string_view implementation() { return "filesystem"; }
}
