#include <exception>
#include <iostream>
#include <fstream>
#include <memory>
#include <ext2fs/ext2fs.h>

template <class T, void(*close_func)(T&)>
struct HanldeWrapper {
    T handle = {};
    operator T&()
    {
        return handle;
    }
    auto operator &() -> T*
    {
        return &handle;
    }
    ~HanldeWrapper()
    {
        if (handle) {
            close_func(handle);
        }
    }
};

using E2FS = HanldeWrapper<ext2_filsys, +[](ext2_filsys& fs) { ext2fs_close(fs); }>;
using E2File = HanldeWrapper<ext2_file_t, +[](ext2_file_t& fs) { ext2fs_file_close(fs); }>;

void throw_if_err(errcode_t err)
{
    if (err) {
        throw std::runtime_error(error_message(err));
    }
}

int main(int argc, char* argv[])
try {
    if (argc < 2) {
        return -1;
    }
    std::ifstream in{argv[2], std::ios::binary};
    auto inrdbuf = in.rdbuf();
    E2FS fileSystem;
    throw_if_err(ext2fs_open(argv[1], EXT2_FLAG_RW, 0, 0, default_io_manager, &fileSystem));
    {
        E2File file;
        throw_if_err(ext2fs_file_open(fileSystem, EXT2_BOOT_LOADER_INO, EXT2_FILE_WRITE | EXT2_FILE_CREATE, &file));
        std::unique_ptr<unsigned char[]> buf{new unsigned char[0x10000]};
        while (!in.fail()) {
            in.read((char*)buf.get(), 0x10000);
            auto* beg = buf.get();
            unsigned toWrite = in.gcount();
            while (toWrite) {
                unsigned written;
                throw_if_err(ext2fs_file_write(file, beg, toWrite, &written));
                beg += written;
                toWrite -= written;
            }
        }
    }
    return 0;
} catch (std::exception& e) {
    std::cerr << e.what() << "\n";
    return -1;
}
