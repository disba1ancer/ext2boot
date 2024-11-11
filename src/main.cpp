#include <format>
#include <swal/win_headers.h>
#include <shellapi.h>
#include <exception>
#include <iostream>
#include "Device.h"

using namespace e2bt;

struct ArgvFree
{
    void operator()(void* mem)
    {
        ::LocalFree(mem);
    }
};

int main(int, char*[])
try {
    int argc;
    std::unique_ptr<wchar_t*[], ArgvFree> argv(::CommandLineToArgvW(::GetCommandLine(), &argc));
    using SM = swal::ShareMode;
    using CM = swal::CreateMode;
    Device drive(argv[1]);
    if (drive.GetDiskSize() < 2048) {
        return 1;
    }
    auto sectSize = drive.GetSectSize();
    auto ssmall = sectSize < 2048;
    auto bufsSize = ssmall ? 1024 : sectSize;
    auto buf = std::make_unique<unsigned char[]>(bufsSize);
    drive.Read(1024 * ssmall, buf.get(), bufsSize);
    std::uint16_t magic;
    memcpy(&magic, buf.get() + 56, 2);
    if (magic != 0xEF53) {
        std::cerr << std::format("Invalid magic: {0}\n", magic);
        return 2;
    }
    std::cout << std::format("Magic is: {0}", magic) << std::endl;
    return 0;
} catch (std::exception& e) {
    std::cerr << e.what() << "\n";
    return -1;
}
