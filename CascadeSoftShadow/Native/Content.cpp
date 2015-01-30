#include "Content.h"
#include "clover.h"

USING_WE

Content::Content()
{
}


Content::~Content()
{
}

bool Content::loadTextureFromFile(wchar_t* file)
{   
    return false;
}

void Content::addTexture(wchar_t* name, ID3D11ShaderResourceView* tex)
{
    _tableTextures[name] = tex;
}

ID3D11ShaderResourceView* Content::getTexture(wchar_t* name)
{
    return _tableTextures[name];
}

ID3DBlob* Content::getBlob(wchar_t* file, char* strType /* = "VS"*/)
{
    static struct
    {
        char type[64];
        char name[64];
        char virsion[64];
    } 
    s_blob[] = 
    {
        { "VS", "VSMain", "vs_5_0" },
        { "PS", "PSMain", "ps_5_0" },
    };

    auto blob = _tableBlobs[file];
    
    if (!blob)
    {    
        for (auto& b : s_blob)
        {
            if (strcmp(b.type, strType) == 0)
            {
                V(compileShaderFromFile(file, NULL, b.name, b.virsion, &blob));
                _tableBlobs[file] = blob;
                break;
            }   
        }
    }
    return blob;
}