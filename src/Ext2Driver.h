#ifndef EXT2DRIVER_H
#define EXT2DRIVER_H

#include "types.h"
#include "Device.h"
#include "enum_bit.h"

namespace e2bt {

class Ext2INode;

class Ext2Driver
{
public:
    enum class State : U16 {
        Valid = 1,
        Error,
    };
    enum class Error : U16 {
        Continue = 1,
        ReadOnly,
        Panic,
    };
    enum class CreatorOS : U32 {
        Linux,
        Hurd,
        Masix,
        FreeBSD,
        Lites,
    };

    enum class CompatFeatures : U32 {
        DirPrealloc = 1,
        ImagicINodes,
        HasJournal = 4,
        ExtendedAttributes = 8,
        ResizedINodes = 16,
        IndexedDirectories = 32,
        Supported = ResizedINodes
    };
    enum class IncompatFeatures : U32 {
        Compression = 1,
        FileTypeInDir,
        Recover = 4,
        JournalDevice = 8,
        MetaBg = 16,
        Supported = 0
    };
    enum class ReadOnlyFeatures : U32 {
        SparseSuper = 1,
        LargeFile,
        BTreeDirectory = 4,
        Supported = SparseSuper | LargeFile
    };
    enum class ComprAlgos : U32 {
        LZV1 = 1,
        LZRW3A,
        GZIP = 4,
        BZIP2 = 8,
        LZO = 16
    };
    enum Constants {
        Magic = 0xEF53,
        BGDSize = 32,
        OldRevision = 0,
        LegacyINodeSize = 128,
        LegacyFirstINode = 11,
    };

    struct BlockGroupDescriptor {
        U32 blockBitmap;
        U32 iNodeBitmap;
        U32 iNodeTable;
        U16 freeBlocks;
        U16 freeINodes;
        U16 directoryBlocks;
    };

    friend class Ext2INode;

    Ext2Driver(const wchar_t* name, std::ptrdiff_t defSectSize = 512);
    void DeserializeSuperblock(Byte* buf, std::ptrdiff_t size);
    auto GetBlockSize() const -> I32;
    void ReadBlocks(Byte* buf, U32 blockNum, I32 count = 1);
    void WriteBlocks(U32 blockNum, const Byte* buf, I32 count = 1);
    auto GetBlockGroupDescriptor(U32 ) -> BlockGroupDescriptor;
private:
    Device dev;

    U32 iNodeCount;
    U32 blockCount;
    U32 reservedBlocks;
    U32 freeBlocks;
    U32 freeINodes;
    U32 firstDataBlock;
    U32 logBlockSize;
    I32 logFragSize;
    U32 blocksPerGroup;
    U32 fragmentsPerGroup;
    U32 iNodesPerGroup;
    I32 mountTime;
    I32 writeTime;
    U16 timesMounted;
    U16 maxMounts;
    U16 magic;
    State state;
    Error error;
    U16 minorRevision;
    I32 checkTime;
    I32 checkInterval;
    CreatorOS creatorOS;
    U32 revision;
    U16 reservedAllocUId;
    U16 reservedAllocGId;
    U32 firstINode;
    U16 iNodeSize;
    U16 superBlockGroup;
    CompatFeatures compatibleFeatures;
    IncompatFeatures incompatibleFeatures;
    ReadOnlyFeatures readOnlyFeatures;
    UUID uuid;
    std::string volumeName;
    std::string lastMountPath;
    ComprAlgos comprAlgos;
    U8 preallocBlocks;
    U8 preallocDirBlocks;
    UUID journalUUID;
    U32 journalINode;
    U32 journalDevice;
    U32 lastOrphan;
    U32 hashSeed[4];
    U8 hashVersion;
    U32 defMountOpts;
    U32 firstMetaBg;
};

template <>
struct enum_bit<Ext2Driver::CompatFeatures> : std::true_type {};

template <>
struct enum_bit<Ext2Driver::IncompatFeatures> : std::true_type {};

template <>
struct enum_bit<Ext2Driver::ReadOnlyFeatures> : std::true_type {};

using namespace enum_bit_ops;

} // namespace e2bt

#endif // EXT2DRIVER_H
