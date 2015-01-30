#pragma once

#include <fbxsdk.h>

void initializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
void destroySdkObjects(FbxManager* pManager, bool pExitStatus);
void createAndFillIOSettings(FbxManager* pManager);

bool saveScene(FbxManager* pManager, 
    FbxDocument* pScene, 
    const char* pFilename, 
    int pFileFormat = -1, 
    bool pEmbedMedia = false);

bool loadScene(FbxManager* pManager, 
    FbxDocument* pScene, 
    const char* pFilename);
