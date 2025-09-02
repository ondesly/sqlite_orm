//
//  aasset_vfs.h
//  sqlite_orm
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 29.08.2022.
//  Copyright © 2022 Dmitrii Torkhov. All rights reserved.
//

#pragma once

class AAssetManager;

namespace sqlite {

    extern const char *default_vfs_name;

    int register_aasset_vfs(AAssetManager *asset_manager, const char *name = default_vfs_name);

}