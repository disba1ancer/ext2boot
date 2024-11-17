#ifndef EXT2DRIVER_H
#define EXT2DRIVER_H

#include "types.h"
#include "Device.h"

namespace e2bt {

class Ext2Driver
{
public:
    static constexpr auto Magic = 0xEF53;
    enum class State : U16 {
        Valid = 1,
        Error,
    };
    enum class Error : U16 {
        Continue = 1,
        ReadOnly,
        Panic,
    };
    enum class CompatFeatures : U32 {
        DirPrealloc = 1,
        ImagicINodes,
        HasJournal = 4,
        ExtendedAttributes = 8,
        ResizedINodes = 16,
        IndexedDirectories = 32
    };
    enum class IncompatFeatures : U32 {
        Compression = 1,
        FileType,
        Recover = 4,
        JournalDevice = 8,
        MetaBg = 16
    };
    enum class ReadOnlyFeatures : U32 {
        SparseSuper = 1,
        LargeFile,
        BTreeDirectory = 4
    };
    enum class ComprAlgos : U32 {
        LZV1 = 1,
        LZRW3A,
        GZIP = 4,
        BZIP2 = 8,
        LZO = 16
    };

    Ext2Driver(const wchar_t* name, std::ptrdiff_t defSectSize = 512);
    void DeserializeSuperblock(Byte* buf, std::ptrdiff_t size);
    auto GetBlockSize() -> I32;
    void ReadBlocks(Byte* buf, U32 blockNum, I32 count = 1);
    void WriteBlocks(U32 blockNum, const Byte* buf, I32 count = 1);

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
    U32 creatorOS;
    U32 revision;
    U16 defResUId;
    U16 defResGId;
    U32 firstINode;
    U16 iNodeSize;
    U16 superBlockGroup;
    CompatFeatures compatibleFeatures;
    IncompatFeatures incompatibleFeatures;
    ReadOnlyFeatures readOnlyFeatures;
    UUID uuid;
    char volumeName[16];
    char lastMountPath[64];
    ComprAlgos complAlgos;
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

} // namespace e2bt

#endif // EXT2DRIVER_H
