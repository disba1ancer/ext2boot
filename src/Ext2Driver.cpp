#include "Ext2Driver.h"
#include "byte_parser.h"
#include "Ext2INode.h"

namespace e2bt {

Ext2Driver::Ext2Driver(const wchar_t* name, std::ptrdiff_t defSectSize) :
    dev(name, defSectSize)
{
    if (dev.GetDiskSize() < 2048) {
        throw std::runtime_error("Drive is too small");
    }
    auto sectSize = dev.GetSectSize();
    auto ssmall = sectSize < 2048;
    auto bufSize = ssmall ? 1024 : sectSize;
    auto buf = std::make_unique<Byte[]>(bufSize);
    dev.Read(buf.get(), 1024 * ssmall, bufSize);
    DeserializeSuperblock(buf.get(), bufSize);
    if (magic != Magic) {
        throw std::runtime_error("Ext2 filesystem not found");
    }
    if (revision == OldRevision) {
        firstINode = LegacyFirstINode;
        iNodeSize = LegacyINodeSize;
        superBlockGroup = 0;
        compatibleFeatures = {};
        incompatibleFeatures = {};
        readOnlyFeatures = {};
        uuid = {};
        comprAlgos = {};
        preallocBlocks = 0;
        preallocDirBlocks = 0;
        journalUUID = {};
        journalINode = 0;
        journalDevice = 0;
        lastOrphan = 0;
        std::memset(hashSeed, 0, sizeof(hashSeed));
        hashVersion = 0;
        defMountOpts = 0;
        firstMetaBg = 0;
    }
}

void Ext2Driver::DeserializeSuperblock(Byte* buf, std::ptrdiff_t size)
{
    byte_parser parser({buf, buf + size});
    char volumeName[16];
    char lastMountPath[64];
    parser
        .get(iNodeCount).get(blockCount).get(reservedBlocks)
        .get(freeBlocks).get(freeINodes).get(firstDataBlock)
        .get(logBlockSize).get(logFragSize).get(blocksPerGroup)
        .get(fragmentsPerGroup).get(iNodesPerGroup).get(mountTime)
        .get(writeTime).get(timesMounted).get(maxMounts)
        .get(magic).get(state).get(error)
        .get(minorRevision).get(checkTime).get(checkInterval)
        .get(creatorOS).get(revision).get(reservedAllocUId)
        .get(reservedAllocGId).get(firstINode).get(iNodeSize)
        .get(superBlockGroup).get(compatibleFeatures).get(incompatibleFeatures)
        .get(readOnlyFeatures).get(uuid).get(volumeName)
        .get(lastMountPath).get(comprAlgos).get(preallocBlocks)
        .get(preallocDirBlocks).get(journalUUID).get(journalINode)
        .get(journalDevice).get(lastOrphan)
        .get(hashSeed[0]).get(hashSeed[1]).get(hashSeed[2]).get(hashSeed[3])
        .get(hashVersion).get(defMountOpts).get(firstMetaBg)
    ;
    this->volumeName = volumeName;
    this->lastMountPath = lastMountPath;
}

auto Ext2Driver::GetBlockSize() const -> I32
{
    return 1024 << logBlockSize;
}

void Ext2Driver::ReadBlocks(Byte* buf, U32 blockNum, I32 count)
{
    U64 blockSize = GetBlockSize();
    dev.Read(buf, blockNum * blockSize, count * blockSize);
}

void Ext2Driver::WriteBlocks(U32 blockNum, const Byte* buf, I32 count)
{
    U64 blockSize = GetBlockSize();
    dev.Write(blockNum * blockSize, buf, count * blockSize);
}

auto Ext2Driver::GetBlockGroupDescriptor(U32 bgdNum) -> BlockGroupDescriptor
{
    U32 descriptorsPerBlock = GetBlockSize() / BGDSize;
    U32 blockNum = firstDataBlock + 1 + bgdNum / descriptorsPerBlock;
    bgdNum = bgdNum % descriptorsPerBlock;
    auto buf = std::make_unique<Byte[]>(GetBlockSize());
    ReadBlocks(buf.get(), blockNum);
    auto ptr = buf.get() + bgdNum * BGDSize;
    byte_parser parser({ptr, BGDSize});
    BlockGroupDescriptor desc;
    parser
        .get(desc.blockBitmap)
        .get(desc.iNodeBitmap)
        .get(desc.iNodeTable)
        .get(desc.freeBlocks)
        .get(desc.freeINodes)
        .get(desc.directoryBlocks)
    ;
    return desc;
}

} // namespace e2bt
