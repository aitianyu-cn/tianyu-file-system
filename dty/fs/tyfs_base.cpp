#include "./tyfs_base.hpp"

namespace dty::io::fs
{
    namespace def
    {
#pragma region block related
        const int32 _dty_fs_blocks_per_segment = 256;
        const int32 _dty_fs_block_start_in_segment = 1;
        const int32 _dty_fs_block_base_size = 4096;
#pragma endregion

#pragma region segment audit
        const byte _dty_fs_seg_audit_chg = (byte)0x10;
        const byte _dty_fs_seg_audit_del = (byte)0x20;
        const byte _dty_fs_seg_audit_nsg = (byte)0x01;
#pragma endregion
    }
}