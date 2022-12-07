/**
 * @file tyfs_base.hpp(fs)
 * @author ysydty
 * @brief
 * @version 0.1
 * @date 2022-04-24
 *
 * @copyright aitianyu.cn Copyright (c) 2022
 *
 */

#ifndef __DTY_SYSTEM_IO_FS_TYFS_BASE_H_PLUS_PLUS__
#define __DTY_SYSTEM_IO_FS_TYFS_BASE_H_PLUS_PLUS__

#include ".\core.hpp"
#include "tianyu-native\native\prime\bitmap.hpp"
#include "tianyu-native\native\prime\guid.hpp"

#define __DTY_FS_ID_SIZE__ 16

#define __DTY_FS_USER_TABLE_COUNT__ 256
#define __DTY_FS_SEGMENT_AUDIT_COUNT__ 128
#define __DTY_FS_SEGMENT_BITMAP_SIZE__ 32

#define __DTY_FS_ATTRUBITE_COUNT__ 32
#define __DTY_FS_AUTHORITY_OP_COUNT__ 8
#define __DTY_FS_USER_TABLE_SIZE__ 256

namespace dty::io::fs
{
    namespace def
    {
        // Define the count of block in each segment
        __DEFAULT__ extern const int32 _dty_fs_blocks_per_segment;

        // Define the start block in each segment
        // 256 blocks in each segment and the first block(index is 0) is used for segment
        // description and statuses control.
        __DEFAULT__ extern const int32 _dty_fs_block_start_in_segment;

        // Define the block size
        // the minimum block size is 4K Bytes, and the growth must to times to base.
        __DEFAULT__ extern const int32 _dty_fs_block_base_size;

        // Block operation record state
        // used for block is changed.
        __DEFAULT__ extern const byte _dty_fs_seg_audit_chg;
        // Block operation record state
        // used for block is cleared.
        __DEFAULT__ extern const byte _dty_fs_seg_audit_del;
        // Block operation record state
        // used for the current segment is created and all of the blocks are not used.
        __DEFAULT__ extern const byte _dty_fs_seg_audit_nsg;

        // The type of Tianyu File System
        // includes 4 types of TYFS,
        enum class TianyuFSType : byte
        {
            // Physical Disk
            PhysicalDisk,
            // Virtual Disk
            VirtualDisk,
            // Virtual Disk optimized for Relational Database
            SqlDatabase,
            // Virtual Disk optimized for Dimension Database
            NonSqlDatabase,
            // Package File
            // Used for static file structure (No dynamic content growth)
            Package
        };

        enum class TianyuFSAttrubite : byte
        {
            // Multi File/Disk Support
            // Indicates whether the current file system supports paging storage
            // For Physical Disk: support 65536 maximum partities are combined to 1 TYFS
            // For Virtual Disk:  support 65536 maximum files are combined to 1 logic Disk
            Page_Support = 0,

            // Data Compress Available
            // Indicates whether the current file system has enabled data compress to save time.
            Compress,
        };

        // Tianyu File System Authority State
        enum class TianyuFSAuthorityState : byte
        {
            // Explicit Forbidden Permission
            // Access will be prevented even though the authority of data item is allowed.
            Deter = 0xFF,
            // Default Authority
            // This option is the inital state of authority.
            // Access will be allowed if the authority of data item is not prevented.
            Default = 0x00,
            // Explicit Permission
            // Access will be allowed if the autiority of data item is not prevented.
            Allow = 0x01
        };

        // Tianyu File System Authority Types
        enum class TianyuFSAuthorityType : byte
        {
            // Full Control Authority
            // Indicates user whether has the full control authority of current file/disk.
            // If this item is allowed, other authority items will be overwrited.
            Full_Control = 0,
            // Modify Authority
            // Indicates user whether has the authority to modify data.
            // If this item is allowed, Read-Write and List File items will be overwrited.
            Modify = 1,
            // Read File & Execute Application Authority
            // Indicates user whether has the authority to read files and execute the applications.
            // If this item is allowed, Read and List File items will be overwrited.
            Read_And_Execute = 2,
            // List Files Authority
            // Indicates user whether has the authority to see the files of file/disk.
            List_File_Content = 3,
            // Read Files Authority
            // Indicates user whether has the authority to read the files of file/disk.
            Read = 4,
            // Write Files Authority
            // Indicates user whether has the authority to edit the data in file/disk.
            Write = 5,
            // Special Authority
            // Currently, this item is not used.
            Special = 6
        };
    }

    namespace attach
    {
        class FSPage : public virtual TianyuObject
        {
            __PRI__ int32                       __VARIABLE__ _PageIndex;
            __PRI__ dty::collection::Bitmap     __POINTER__  _PageStates;
        };

        class FSCompress : public virtual TianyuObject
        {

        };
    }

    namespace base
    {
        typedef struct tyfs_user_table_item
        {

        } FSUserItem;

        // Audit for tyfs segment
        // to track the operation: change, delete, new-seg
        // to record operation result: success, redo, failed
        // to record the operation time
        typedef struct tyfs_segment_audit_item
        {
            // User Id
            // to record the operation creater.
            // this item value is mapped to File System User Table Index.
            // for each FS, support 256 users and the 0 and 1 index are used by default.
            byte  __VARIABLE__ UserId;

            // Operation Id
            // to record the operation id
            // this item value is mapped to File Transaction Recording.
            // use 16bytes Guid to record
            byte  __VARIABLE__ OperationId[__DTY_FS_ID_SIZE__];

            // Operation Time
            // to record the operation time
            int64 __VARIABLE__ Timestamp;

            // Operation Target Block Index
            // If target block is the first block of segment, the operation must to be "New"
            byte  __VARIABLE__ TargetBlock;

            // Operation Type
            // Delete, Modify, New
            byte  __VARIABLE__ Operation;

            // Operation Result
            // Not-Do, Done, Failed
            byte  __VARIABLE__ State;
        } SegmentAuditItem;

        class FSSegment final : public virtual TianyuObject
        {
            __PRI__ dty::collection::Bitmap __VARIABLE__ _Bitmap;
            __PRI__ byte                    __VARIABLE__ _DesId[__DTY_FS_ID_SIZE__];
            __PRI__ SegmentAuditItem        __VARIABLE__ _Audits[__DTY_FS_SEGMENT_AUDIT_COUNT__];
            __PRI__ byte                    __VARIABLE__ _AuditCount;
        };


        class FSAttrubite : public virtual TianyuObject
        {
            __PRI__ byte __VARIABLE__ _Attrubites[__DTY_FS_ATTRUBITE_COUNT__];

            __PUB__         FSAttrubite(byte __VARIABLE__ attrubites []);
            __PUB__ virtual ~FSAttrubite() __override_func;

            __PUB__ byte __VARIABLE__ GetAttrubiteBytes(int32 attrubiteIndex);

            __PUB__ void __VARIABLE__ IsAttrubiteValid(def::TianyuFSAttrubite __VARIABLE__ attrubite);
            __PUB__ void __VARIABLE__ IsAttrubiteInvalid(def::TianyuFSAttrubite __VARIABLE__ attrubite);

            __PUB__ bool __VARIABLE__ SetAttrubite(def::TianyuFSAttrubite __VARIABLE__ attrubite, bool __VARIABLE__ valid);

            __PUB__ virtual ::string __VARIABLE__ ToString() noexcept __override_func;
            __PUB__ virtual uint64   __VARIABLE__ GetTypeId();
            __PUB__ virtual uint64   __VARIABLE__ GetHashCode();
        };

        class FSAuthorityList : public virtual TianyuObject
        {
            using TYFSAuthorityStateItem = def::TianyuFSAuthorityState[__DTY_FS_AUTHORITY_OP_COUNT__];

            __PRI__ TYFSAuthorityStateItem __VARIABLE__ _Autiorities[__DTY_FS_USER_TABLE_SIZE__];

            __PUB__         FSAuthorityList();
            __PUB__ virtual ~FSAuthorityList() __override_func;

            __PUB__ def::TianyuFSAuthorityState __VARIABLE__ IsAllowed(int32 __VARIABLE__ userIndex, def::TianyuFSAuthorityType __VARIABLE__ authority);
            __PUB__ def::TianyuFSAuthorityState __VARIABLE__ IsDetered(int32 __VARIABLE__ userIndex, def::TianyuFSAuthorityType __VARIABLE__ authority);

            __PUB__ byte __POINTER__ GetBytesWhiteList();
            __PUB__ byte __POINTER__ GetBytesBlackList();

            __PUB__ int32 __POINTER__ GetAllowedUsers(def::TianyuFSAuthorityType __VARIABLE__ authority, int32 __REFERENCE__ count);
            __PUB__ int32 __POINTER__ GetDeteredUsers(def::TianyuFSAuthorityType __VARIABLE__ authority, int32 __REFERENCE__ count);

            __PUB__ virtual ::string __VARIABLE__ ToString() noexcept __override_func;
            __PUB__ virtual uint64   __VARIABLE__ GetTypeId();
            __PUB__ virtual uint64   __VARIABLE__ GetHashCode();
        };

        class FSInstance : public virtual TianyuObject
        {
#pragma region Tianyu File System Basic Info
            __PRI__ Property<def::TianyuFSType> __VARIABLE__ _FSType;
            __PRI__ Guid                        __VARIABLE__ _FileId;
            __PRI__::string                     __VARIABLE__ _FileName;
#pragma endregion

#pragma region Tianyu File System Data Structure Infos
            __PRI__ Property<int32>             __VARIABLE__ _BlockSize;
            __PRI__ int64                       __VARIABLE__ _DRBPosition;
            __PRI__ FSAttrubite                 __VARIABLE__ _Attrubites;
            __PRI__ FSAuthorityList             __VARIABLE__ _Authorities;
#pragma endregion

#pragma region Tianyu File System Additional Infos
            __PRI__ attach::FSPage              __POINTER__  _Page;
            __PRI__ attach::FSCompress          __POINTER__  _Compression;
#pragma endregion

            __PUB__ IPropertyGetter<def::TianyuFSType> __REFERENCE__ FileSystemType;
            __PUB__ IPropertyGetter<int32>             __REFERENCE__ FSBlockSize;

            __PUB__         FSInstance();
            __PUB__ virtual ~FSInstance() __override_func;

            __PUB__ virtual ::string __VARIABLE__ ToString() noexcept __override_func;
            __PUB__ virtual uint64   __VARIABLE__ GetTypeId();
            __PUB__ virtual uint64   __VARIABLE__ GetHashCode();
        };
    }
}

#endif // !__DTY_SYSTEM_IO_FS_TYFS_BASE_H_PLUS_PLUS__