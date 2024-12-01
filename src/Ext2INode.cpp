#include "Ext2INode.h"

#include "Ext2Driver.h"
#include "byte_parser.h"
#include <ctime>

namespace e2bt {

Ext2INode::Ext2INode(Ext2Driver& drv, U32 iNum) :
    drv(drv),
    iNode(iNum)
{
    if (iNum == 0) {
        throw std::runtime_error("Zero INode can not be opened");
    }
    auto descNum = (iNum - 1) / drv.iNodesPerGroup;
    iNum = (iNum - 1) % drv.iNodesPerGroup;
    auto desc = drv.GetBlockGroupDescriptor(descNum);
    auto iNodesPerBlock = drv.GetBlockSize() / drv.iNodeSize;
    auto blockNum = desc.iNodeTable + iNum / iNodesPerBlock;
    auto buf = std::make_unique<Byte[]>(drv.GetBlockSize());
    drv.ReadBlocks(buf.get(), blockNum);
    iNum = iNum % iNodesPerBlock;
    auto ptr = buf.get() + iNum * drv.iNodeSize;
    Deserialize(ptr);
}

void Ext2INode::Deserialize(Byte* ptr)
{
    byte_parser parser({ptr, drv.iNodeSize});
    mode = static_cast<Mode>(parser.get<U16>());
    uId = parser.get<U16>();
    size = parser.get<U32>();
    parser
        .get(accessTime)
        .get(createTime)
        .get(modifyTime)
        .get(deleteTime)
    ;
    gId = parser.get<U16>();
    parser
        .get(linksCount)
        .get(size512)
        .get(flags)
        .get(osd1)
        .get(block)
        .get(generation)
        .get(fileACL)
    ;
    using ROF = Ext2Driver::ReadOnlyFeatures;
    if (
        drv.revision > 0 &&
        static_cast<bool>(drv.readOnlyFeatures & ROF::LargeFile) &&
        static_cast<bool>(mode & Mode::File))
    {
        size |= U64(parser.get<U32>()) << 16;
        dirACL = 0;
    } else {
        parser.get(dirACL);
    }
    parser
        .get(i_faddr)
        .skip(2)
    ;
    using COS = Ext2Driver::CreatorOS;
    if (drv.creatorOS == COS::Hurd) {
        mode |= Mode(U32(parser.get<U16>()) << 16);
    } else {
        parser.skip(2);
    }
    switch (drv.creatorOS) {
    case COS::Linux:
        parser.skip(2);
        uId |= U32(parser.get<U16>()) << 16;
        gId |= U32(parser.get<U16>()) << 16;
        author = -1;
        break;
    case COS::Hurd:
        mode |= Mode(U32(parser.get<U16>()) << 16);
        uId |= U32(parser.get<U16>()) << 16;
        gId |= U32(parser.get<U16>()) << 16;
        parser.get(author);
        break;
    default:
        author = -1;
    }
}

bool Ext2INode::IsAlive()
{
    return linksCount != 0 || (mode != Mode::Deleted && deleteTime == 0);
}

void Ext2INode::Init(Mode mode, U32 uid, U32 gid)
{
    if (IsAlive()) {
        throw std::runtime_error("INode already alive");
    }
    this->mode = mode;
    uId = uid;
    gId = gid;
    std::time_t time;
    std::time(&time);
    createTime = time;
    accessTime = time;
    modifyTime = time;
    deleteTime = 0;
}

} // namespace e2bt
