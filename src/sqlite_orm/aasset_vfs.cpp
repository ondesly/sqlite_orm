//
//  aasset_vfs.cpp
//  sqlite_orm
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 29.08.2022.
//  Copyright © 2022 Dmitrii Torkhov. All rights reserved.
//

#ifdef ANDROID

#include <android/asset_manager.h>
#include <sqlite3.h>
#include <string>

#include "sqlite_orm/aasset_vfs.h"

namespace sqlite {

    const char *default_vfs_name = "aasset_vfs";

    namespace {

        const int max_pathname_len = 256;
        const int sector_size = 512;

        struct aasset_vfs {
            sqlite3_vfs vfs;
            sqlite3_vfs *vfs_default;
            const struct sqlite3_io_methods *methods;

            const char *name;
            AAssetManager *asset_manager;
        };

        struct aasset_file {
            const sqlite3_io_methods *methods;
            AAsset *asset;
            const void *buf;
            off_t len;
        };

    }

    static int aasset_open(sqlite3_vfs *vfs, const char *path, sqlite3_file *file,
                           int flags, int *out_flags) {
        const auto aasset_vfs = (sqlite::aasset_vfs *) vfs;
        auto aasset_file = (sqlite::aasset_file *) file;

        aasset_file->methods = nullptr;

        if (!path) {
            return SQLITE_PERM;
        }

        if (!(flags & SQLITE_OPEN_READONLY) ||
            !(flags & SQLITE_OPEN_MAIN_DB) ||
            (flags & SQLITE_OPEN_DELETEONCLOSE) ||
            (flags & SQLITE_OPEN_READWRITE) ||
            (flags & SQLITE_OPEN_CREATE)) {
            return SQLITE_PERM;
        }

        auto asset = AAssetManager_open(aasset_vfs->asset_manager, path, AASSET_MODE_RANDOM);
        if (!asset) {
            return SQLITE_CANTOPEN;
        }

        const auto buf = AAsset_getBuffer(asset);
        if (!buf) {
            AAsset_close(asset);
            return SQLITE_ERROR;
        }

        aasset_file->methods = aasset_vfs->methods;
        aasset_file->asset = asset;
        aasset_file->buf = buf;
        aasset_file->len = AAsset_getLength(asset);

        if (out_flags) {
            *out_flags = flags;
        }

        return SQLITE_OK;
    }

    static int aasset_delete(sqlite3_vfs *, const char *, int) {
        return SQLITE_ERROR;
    }

    static int aasset_access(sqlite3_vfs *vfs, const char *path, int flags, int *res_out) {
        const auto aasset_vfs = (sqlite::aasset_vfs *) vfs;

        *res_out = 0;

        switch (flags) {
            case SQLITE_ACCESS_EXISTS:
            case SQLITE_ACCESS_READ: {
                auto asset = AAssetManager_open(aasset_vfs->asset_manager, path,
                                                AASSET_MODE_RANDOM);
                if (asset) {
                    AAsset_close(asset);
                    *res_out = 1;
                }

                break;
            }
            default:
                break;
        }

        return SQLITE_OK;
    }

    static int aasset_full_pathname(sqlite3_vfs *, const char *path, int out_len, char *out) {
        if (!path || !out) {
            return SQLITE_ERROR;
        }

        strcpy(out, path);

        return SQLITE_OK;
    }

    static int aasset_randomness(sqlite3_vfs *vfs, int buf_len, char *buf) {
        const auto aasset_vfs = (sqlite::aasset_vfs *) vfs;
        return aasset_vfs->vfs_default->xRandomness(aasset_vfs->vfs_default, buf_len, buf);
    }

    static int aasset_sleep(sqlite3_vfs *vfs, int microseconds) {
        const auto aasset_vfs = (sqlite::aasset_vfs *) vfs;
        return aasset_vfs->vfs_default->xSleep(aasset_vfs->vfs_default, microseconds);
    }

    static int aasset_current_time(sqlite3_vfs *vfs, double *now) {
        const auto aasset_vfs = (sqlite::aasset_vfs *) vfs;
        return aasset_vfs->vfs_default->xCurrentTime(aasset_vfs->vfs_default, now);
    }

    static int aasset_current_time_int_64(sqlite3_vfs *vfs, sqlite3_int64 *now) {
        const auto aasset_vfs = (sqlite::aasset_vfs *) vfs;
        return aasset_vfs->vfs_default->xCurrentTimeInt64(aasset_vfs->vfs_default, now);
    }

    static int aasset_get_last_error(sqlite3_vfs *, int, char *) {
        return 0;
    }

    static int aasset_file_close(sqlite3_file *file) {
        auto aasset_file = (sqlite::aasset_file *) file;

        if (aasset_file->asset) {
            AAsset_close(aasset_file->asset);
            aasset_file->asset = nullptr;
            aasset_file->buf = nullptr;
            aasset_file->len = 0;
        }

        return SQLITE_OK;
    }

    static int aasset_file_read(sqlite3_file *file, void *buf, int amt, sqlite3_int64 offset) {
        const auto aasset_file = (sqlite::aasset_file *) file;
        if (aasset_file->asset == nullptr) {
            return SQLITE_IOERR_READ;
        }

        int got;
        int rc;
        int off = (int) offset;

        if (off + amt <= aasset_file->len) {
            got = amt;
            rc = SQLITE_OK;
        } else {
            got = aasset_file->len - off;
            if (got < 0) {
                rc = SQLITE_IOERR_READ;
            } else {
                rc = SQLITE_IOERR_SHORT_READ;
                memset(&((char *) buf)[got], 0, amt - got);
            }
        }

        if (got > 0) {
            memcpy(buf, (char *) aasset_file->buf + off, got);
        }

        return rc;
    }

    static int aasset_file_write(sqlite3_file *, const void *, int, sqlite3_int64) {
        return SQLITE_IOERR_WRITE;
    }

    static int aasset_file_truncate(sqlite3_file *, sqlite3_int64) {
        return SQLITE_IOERR_TRUNCATE;
    }

    static int aasset_file_sync(sqlite3_file *, int) {
        return SQLITE_IOERR_FSYNC;
    }

    static int aasset_file_size(sqlite3_file *file, sqlite3_int64 *size) {
        const auto aasset_file = (sqlite::aasset_file *) file;
        *size = aasset_file->len;

        return SQLITE_OK;
    }

    static int aasset_file_lock(sqlite3_file *, int) {
        return SQLITE_OK;
    }

    static int aasset_file_unlock(sqlite3_file *, int) {
        return SQLITE_OK;
    }

    static int aasset_file_check_reserved_lock(sqlite3_file *, int *res) {
        *res = 0;
        return SQLITE_OK;
    }

    static int aasset_file_control(sqlite3_file *, int, void *) {
        return SQLITE_NOTFOUND;
    }

    static int aasset_file_sector_size(sqlite3_file *) {
        return sector_size;
    }

    static int aasset_file_device_characteristics(sqlite3_file *) {
        return 0;
    }

    int register_aasset_vfs(AAssetManager *asset_manager, const char *name) {
        static sqlite::aasset_vfs aasset_vfs;

        if (!asset_manager) {
            return SQLITE_ERROR;
        }

        if (strlen(name) >= max_pathname_len) {
            return SQLITE_ERROR;
        }

        if (aasset_vfs.asset_manager == asset_manager && strcmp(aasset_vfs.name, name) == 0) {
            return SQLITE_OK;
        }

        if (aasset_vfs.asset_manager) {
            sqlite3_vfs_unregister(&aasset_vfs.vfs);
        }

        //

        aasset_vfs.name = name;
        aasset_vfs.asset_manager = asset_manager;

        aasset_vfs.vfs_default = sqlite3_vfs_find(nullptr);
        if (!aasset_vfs.vfs_default) {
            return SQLITE_ERROR;
        }

        static const sqlite3_io_methods methods = {
                1,
                aasset_file_close,
                aasset_file_read,
                aasset_file_write,
                aasset_file_truncate,
                aasset_file_sync,
                aasset_file_size,
                aasset_file_lock,
                aasset_file_unlock,
                aasset_file_check_reserved_lock,
                aasset_file_control,
                aasset_file_sector_size,
                aasset_file_device_characteristics
        };
        aasset_vfs.methods = &methods;

        aasset_vfs.vfs.iVersion = 3;
        aasset_vfs.vfs.szOsFile = sizeof(aasset_file);
        aasset_vfs.vfs.mxPathname = max_pathname_len;
        aasset_vfs.vfs.pNext = nullptr;
        aasset_vfs.vfs.zName = name;
        aasset_vfs.vfs.pAppData = nullptr;
        aasset_vfs.vfs.xOpen = aasset_open;
        aasset_vfs.vfs.xDelete = aasset_delete;
        aasset_vfs.vfs.xAccess = aasset_access;
        aasset_vfs.vfs.xFullPathname = aasset_full_pathname;
        aasset_vfs.vfs.xDlOpen = nullptr;
        aasset_vfs.vfs.xDlError = nullptr;
        aasset_vfs.vfs.xDlSym = nullptr;
        aasset_vfs.vfs.xDlClose = nullptr;
        aasset_vfs.vfs.xRandomness = aasset_randomness;
        aasset_vfs.vfs.xSleep = aasset_sleep;
        aasset_vfs.vfs.xCurrentTime = aasset_current_time;
        aasset_vfs.vfs.xGetLastError = aasset_get_last_error;
        aasset_vfs.vfs.xCurrentTimeInt64 = aasset_current_time_int_64;
        aasset_vfs.vfs.xSetSystemCall = nullptr;
        aasset_vfs.vfs.xGetSystemCall = nullptr;
        aasset_vfs.vfs.xNextSystemCall = nullptr;

        //

        const auto result = sqlite3_vfs_register(&aasset_vfs.vfs, 0);

        if (result != SQLITE_OK) {
            aasset_vfs.asset_manager = nullptr;
        }

        return result;
    }

}

#endif