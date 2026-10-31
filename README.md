# jfc-storage_filesystem
The [jfc-storage](https://github.com/jfcameron/jfc-storage) store interface, implemented over the host's filesystem, and the directories each platform expects a program to keep its user's files in.

API documentation: https://jfcameron.github.io/jfc-storage_filesystem/

## Getting Started
Install cmake 3.21 or greater, and a c++20 compatible compiler
```
# initialize the tree
git submodule update --init --recursive

# generate the script to build the library and its tests on linux using gcc
cmake --preset linux-gcc

# build the library and tests
cmake --build --preset linux-gcc

# run the tests
ctest --preset linux-gcc
```
The `linux-clang-fuzz` preset builds the fuzzer instead. See `CMakePresets.json` for the full list of supported platforms and compilers. See `.github/workflows/ci.yml` to see exactly how CI runners build it on different platforms.

To use it from another project, add it with `add_subdirectory` and link `jfcstorage_filesystem` for the store and `jfcstorage_user_directories` for the directories, which Android does not have. The installed package is `jfc-storage_filesystem`, targets `jfc::storage_filesystem` and `jfc::storage_user_directories`. A project that has already added jfc-storage gets one copy of it.

## Overview
### `make_filesystem_store`
A store whose files are files under a directory. A save writes a temporary file beside the one it replaces and renames it into place, so a reader sees the old file or the new one and never part of either, and two stores saving the same file at once, in the same process or another, leave one of the two whole. With `flush_to_disk`, a change returns only once it is on the disk, so a power cut cannot undo it. With `restrict_to_owner`, the store's directories and the files it saves can only be read by the user who saved them, which suits a login token. This does nothing on Windows, where a user's application data is already their own.

No path leaves the root, but a link already in it is followed wherever it leads. A link to a directory is listed as a directory, unless it leads back to the directory it is in or one above it, which a copy would otherwise follow until the path was too long. A file in the directory that the store would refuse to name, one put there by another program, is not listed.

### User directories
`user_data_directory` and `user_config_directory` give the directories a platform expects a program to keep its user's data and configuration in, which a program makes a filesystem store at. The program's name is checked as a path is, so it cannot climb out of them.

| system | data | configuration |
| --- | --- | --- |
| Linux and BSD | `$XDG_DATA_HOME/<name>`, by default `~/.local/share/<name>` | `$XDG_CONFIG_HOME/<name>`, by default `~/.config/<name>` |
| macOS 10.15+ | `~/Library/Application Support/<name>` | `~/Library/Preferences/<name>` |
| iOS 13+ | the app container's `Library/Application Support/<name>` | the app container's `Library/Preferences/<name>` |
| Windows | `%APPDATA%\<name>`, which roams with the user | `%LOCALAPPDATA%\<name>`, which stays with the machine |
| Emscripten | `/persistent/data/<name>` | `/persistent/config/<name>` |
| Android | none | none |

Android has no user directories: an app's directory is its `Context`'s `getFilesDir()`, which only Java can ask for, so a program makes a filesystem store at the path its Java side passes down.

### Threads
A filesystem store keeps nothing of its own, so it can be used from any number of threads. Two filesystem stores on the same directory, in one process or two, are as safe together as one is, though when both save the same file it is the last rename that is kept.
