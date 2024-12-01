#ifndef EXT2INODE_H
#define EXT2INODE_H

#include "types.h"
#include "enum_bit.h"

namespace e2bt {

class Ext2Driver;

class Ext2INode
{
public:
    enum class Mode : U32 {
        Deleted,
        OX,
        OW,
        OR = 4,
        GX = 8,
        GW = 16,
        GR = 32,
        UX = 64,
        UW = 128,
        UR = 256,
        VTX = 512,
        GId = 1024,
        UId = 2048,
        FIFO = 0x1000,
        CharacterDev = 0x2000,
        Directory = 0x4000,
        BlockDev = 0x6000,
        File = 0x8000,
        Symlink = 0xA000,
        Socket = 0xC000,
    };

    Ext2INode(Ext2Driver& drv, U32 iNum);
    void Deserialize(Byte* ptr);
    bool IsAlive();
    void Init(Mode mode, U32 uid, U32 gid);
private:
    Ext2Driver& drv;
    U32 iNode;

    Mode mode;
    U32 uId;
    U64 size;
    I32 accessTime;
    I32 createTime;
    I32 modifyTime;
    I32 deleteTime;
    U32 gId;
    U16 linksCount;
    U32 size512;
    U32 flags;
    U32 osd1;
    U32 block[15];
    U32 generation;
    U32 fileACL;
    U32 dirACL;
    U32 i_faddr;
    U32 author;
};

template <>
struct enum_bit<Ext2INode::Mode> : std::true_type {};

using namespace enum_bit_ops;

} // namespace e2bt

#endif // EXT2INODE_H
