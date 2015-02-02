#pragma once

#include "require.h"

NS_WE_BEGAN

class DLL_EXPORT Content
{
public:
    friend Content* gContent();

    // texture
    bool loadTextureFromFile(wchar_t*);
    void addTexture(wchar_t*, ID3D11ShaderResourceView*);
    ID3D11ShaderResourceView* getTexture(wchar_t*);

    // blob
	ID3DBlob* getBlob(wchar_t*, char* type = "VS");
private:
    Content();
    ~Content();
    hash_map<const wchar_t*, ID3D11ShaderResourceView*> _tableTextures;
    hash_map<wchar_t*, ID3DBlob*> _tableBlobs;
};

NS_WE_END