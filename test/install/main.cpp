// © Joseph Cameron - MIT License

#include <jfc/storage/filesystem_store.h>
#include <jfc/storage/store.h>

#if __has_include(<jfc/storage/impl_user_directories.h>) || __has_include(<jfc/storage/impl_flush.h>)
#error "a private header was installed"
#endif

#if __has_include(<jfc/storage/user_directories.h>)
#include <jfc/storage/user_directories.h>
#endif

#include <cstddef>
#include <filesystem>

int main() {
    const auto root = std::filesystem::temp_directory_path() / "jfc-storage_filesystem-consumer";

    std::filesystem::remove_all(root);

    auto store = jfc::storage::make_filesystem_store(root);

    const std::byte data[] = {std::byte{1}, std::byte{2}};

    store->save_file("a/b", data);

    const auto loaded = store->load_file("a/b");

    std::filesystem::remove_all(root);

    if (!loaded || loaded->size() != 2) return 1;

#if __has_include(<jfc/storage/user_directories.h>)
    if (jfc::storage::user_data_directory("consumer").filename() != "consumer") return 1;
#endif

    return 0;
}
