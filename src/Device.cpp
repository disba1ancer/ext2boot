#include "Device.h"

namespace e2bt {

Device::Device(const wchar_t* name, ptrdiff_t defSectSize) :
    device(name, FILE_READ_DATA, SM::Read | SM::Write, CM::OpenExisting, 0),
    sectSize(defSectSize)
{
    QueryInfo();
}

auto Device::GetSectSize() const -> ptrdiff_t
{
    return sectSize;
}

auto Device::GetDiskSize() const -> ptrdiff_t
{
    return diskSize;
}

void Device::Read(uint64_t pos, void* buf, ptrdiff_t size) const
{
    OVERLAPPED ovl = {};
    ovl.Offset = pos;
    ovl.OffsetHigh = pos >> 32;
    DWORD bRead;
    device.Read(buf, size, bRead, ovl);
}

void Device::Write(uint64_t pos, const void* buf, ptrdiff_t size) const
{
    OVERLAPPED ovl = {};
    ovl.Offset = pos;
    ovl.OffsetHigh = pos >> 32;
    DWORD bRead;
    device.Write(buf, size, bRead, ovl);
}

void Device::QueryInfo()
try {
    DWORD bytesReturned;
    DISK_GEOMETRY_EX g;
    device.DeviceIoControl(IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, nullptr, 0, &g, sizeof(g), bytesReturned);
    sectSize = g.Geometry.BytesPerSector;
    diskSize = g.DiskSize.QuadPart;
} catch (std::system_error& e) {
    auto& c = e.code();
    if (c.category() == swal::win32_category::instance()) {
        LARGE_INTEGER size;
        swal::winapi_call(::GetFileSizeEx(device, &size));
        diskSize = size.QuadPart;
    }
    throw;
}

} // namespace e2bt
