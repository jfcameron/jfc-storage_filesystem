// © Joseph Cameron - MIT License

#ifndef JFC_STORAGE_TEST_SCRATCH_ROOTS_H
#define JFC_STORAGE_TEST_SCRATCH_ROOTS_H

#include "test_include.h"

#include <deque>
#include <filesystem>
#include <string>

namespace jfc_storage_test {
    //! a new scratch directory, removed when the test program ends
    [[nodiscard]] inline const std::filesystem::path &next_scratch_root() {
        static std::deque<scratch_directory> roots;

        return roots.emplace_back("conformance_" + std::to_string(roots.size())).path();
    }
}

#endif
