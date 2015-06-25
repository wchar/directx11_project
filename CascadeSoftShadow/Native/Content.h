#pragma once

#include "require.h"

NS_WE_BEGAN

class DLL_EXPORT Content
{
public:
    friend Content *gContent();

    // texture
    bool loadTextureFromFile(wchar_t *);
    void addTexture(wchar_t *, ID3D11ShaderResourceView *);
    ID3D11ShaderResourceView *getTexture(wchar_t *);

    // blob
	ID3DBlob *getBlob(const string& key)
	{
		return _tableBlobs[key];
	}

	void addBlob(const string& key, ID3DBlob * blob)
	{
		_tableBlobs[key] = blob;
	}
private:
    Content();
    ~Content();
    unordered_map<const wchar_t *, ID3D11ShaderResourceView *> _tableTextures;
	unordered_map<string, ID3DBlob *> _tableBlobs;
};

NS_WE_END