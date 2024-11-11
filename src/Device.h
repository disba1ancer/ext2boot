#ifndef DEVICE_H
#define DEVICE_H

#include <swal/handle.h>
#include <winioctl.h>

namespace e2bt {

struct Device
{
    using SM = swal::ShareMode;
    using CM = swal::CreateMode;
    Device(const wchar_t* name, std::ptrdiff_t defSectSize = 512);
    auto GetSectSize() const -> std::ptrdiff_t;
    auto GetDiskSize() const -> std::ptrdiff_t;
    void Read(std::uint64_t pos, void* buf, std::ptrdiff_t size) const;
    void Write(std::uint64_t pos, const void* buf, std::ptrdiff_t size) const;
private:
    void QueryInfo();
    swal::File device;
    std::ptrdiff_t sectSize;
    std::uint64_t diskSize;
};

} // namespace e2bt

#endif // DEVICE_H