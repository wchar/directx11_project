#include "Content.h"
#include "clover.h"

USING_WE

Content::Content()
{
}

Content::~Content()
{
}

bool Content::loadTextureFromFile(wchar_t *file)
{
    return false;
}

void Content::addTexture(wchar_t *name, ID3D11ShaderResourceView *tex)
{
    _tableTextures[name] = tex;
}

ID3D11ShaderResourceView *Content::getTexture(wchar_t *name)
{
    return _tableTextures[name];
}
