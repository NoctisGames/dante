//
//  apple_asset_data_handler.h
//  noctisgames
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__asset_data_handler__
#define __noctisgames__asset_data_handler__

#include <framework/file/portable/FileData.h>

FileData appleGetAssetData(const char* relative_path);

void appleReleaseAssetData(const FileData* file_data);

const char * getPathInsideNSDocuments(const char* relative_path);

const char * getBundlePath(const char* relative_path);

#endif /* defined(__noctisgames__asset_data_handler__) */
