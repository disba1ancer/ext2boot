#include <swal/win_headers.h>
#include <shellapi.h>
#include <exception>
#include <iostream>
#include "Ext2Driver.h"
#include "Ext2INode.h"

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
    Ext2Driver fs(argv[1]);
    Ext2INode iNode(fs, 5);

    return 0;
} catch (std::exception& e) {
    std::cerr << e.what() << "\n";
    return -1;
}
