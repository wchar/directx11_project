#include "FbxImporter.h"

FbxLoader* FbxLoader::create(wchar_t* file)
{
    FbxLoader* loader = new FbxLoader();
    if (loader->load(file))
    {
        return loader;
    }
    else
    {
        SAFE_DELETE(loader);
        return nullptr;
    }
}

bool FbxLoader::load(wchar_t* file)
{
    bool result = false;
    char str[MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, file, -1, str, MAX_PATH, NULL, NULL);

    FbxManager* lSdkManager;
    FbxScene* lScene;

    // Prepare the FBX SDK.
    initializeSdkObjects(lSdkManager, lScene);

    FbxString lFilePath(str);
    if (!lFilePath.IsEmpty())
    {
        result = loadScene(lSdkManager, lScene, lFilePath.Buffer());
    }

    if (result)
    {
        FbxNode* lNode = lScene->GetRootNode();

        if (lNode)
        {
            for (int i = 0; i < lNode->GetChildCount(); i++)
            {
                loadMesh(lNode->GetChild(i));
            }
        }
        //
        // parse animation
    }

    // Destroy all objects created by the FBX SDK.
    destroySdkObjects(lSdkManager, true);

    return result;
}

static UINT s_getPosition(FbxMesh* pMesh, XMFLOAT3* vOut, int iPolygon, int iVertex)
{
    UINT lControlPointIndex = pMesh->GetPolygonVertex(iPolygon, iVertex);
    FbxVector4* lControlPoints = pMesh->GetControlPoints();
    FbxVector4 pos = lControlPoints[lControlPointIndex];
    vOut->x = pos[0];
    vOut->y = pos[1];
    vOut->z = pos[2];

    return lControlPointIndex;
}

static void s_getUV(FbxMesh* pMesh, XMFLOAT2* vOut, int iPolygon, int iVertex)
{
    FbxVector2 texcoord;

    int lControlPointIndex = pMesh->GetPolygonVertex(iPolygon, iVertex);
    for (int l = 0; l < pMesh->GetElementUVCount(); ++l)
    {
        FbxGeometryElementUV* leUV = pMesh->GetElementUV(l);

        switch (leUV->GetMappingMode())
        {
        case FbxGeometryElement::eByControlPoint:
            switch (leUV->GetReferenceMode())
            {
            case FbxGeometryElement::eDirect:
                texcoord = leUV->GetDirectArray().GetAt(lControlPointIndex);
                break;
            case FbxGeometryElement::eIndexToDirect:
            {
                int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
                texcoord = leUV->GetDirectArray().GetAt(id);
            }
                break;
            default:
                break; // other reference modes not shown here!
            }
            break;

        case FbxGeometryElement::eByPolygonVertex:
        {
            int lTextureUVIndex = pMesh->GetTextureUVIndex(iPolygon, iVertex);
            switch (leUV->GetReferenceMode())
            {
            case FbxGeometryElement::eDirect:
            case FbxGeometryElement::eIndexToDirect:
            {
                texcoord = leUV->GetDirectArray().GetAt(lTextureUVIndex);
            }
            break;
            default:
            break; // other reference modes not shown here!
            }
        }
            break;

        case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
        case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
        case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
            break;
        }
    }

    vOut->x = texcoord[0];
    vOut->y = texcoord[1];
}


static void s_getNormal(FbxMesh* pMesh, XMFLOAT3* vOut, int iPolygon, int iVertex)
{
    FbxVector4 normal;

    pMesh->GetPolygonVertexNormal(iPolygon, iVertex, normal);

    vOut->x = normal[0];
    vOut->y = normal[1];
    vOut->z = normal[2];
}

bool FbxLoader::getTextureFile(FbxProperty* lProperty, WCHAR* wstrFileName)
{
    if (!lProperty->IsValid())
        return false;

    int lTextureCount = lProperty->GetSrcObjectCount<FbxTexture>();

    for (int j = 0; j < lTextureCount; ++j)
    {
        //Here we have to check if it's layeredtextures, or just textures:
        FbxLayeredTexture *lLayeredTexture = lProperty->GetSrcObject<FbxLayeredTexture>(j);
        if (lLayeredTexture)
        {
            FbxLayeredTexture *lLayeredTexture = lProperty->GetSrcObject<FbxLayeredTexture>(j);
            int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
            for (int k = 0; k<lNbTextures; ++k)
            {
                FbxTexture* lTexture = lLayeredTexture->GetSrcObject<FbxTexture>(k);
                if (lTexture)
                {
                    FbxFileTexture *lFileTexture = FbxCast<FbxFileTexture>(lTexture);
                    if (lFileTexture)
                    {
                        const char* cstrFileName = lFileTexture->GetFileName();

                        MultiByteToWideChar(CP_ACP, 0, cstrFileName, -1, wstrFileName, MAX_PATH);

                        return true;
                    }
                }
            }
        }
        else
        {
            //no layered texture simply get on the property
            FbxTexture* lTexture = lProperty->GetSrcObject<FbxTexture>(j);
            if (lTexture)
            {
                FbxFileTexture *lFileTexture = FbxCast<FbxFileTexture>(lTexture);
                if (lFileTexture)
                {
                    const char* cstrFileName = lFileTexture->GetFileName();

                    MultiByteToWideChar(CP_ACP, 0, cstrFileName, -1, wstrFileName, MAX_PATH);

                    return true;
                }
            }

        }
    }

    return false;
}


void FbxLoader::fillMaterialStructure(FbxMaterial* pMaterial, FbxSurfaceMaterial *lFbxMaterial)
{
    FbxPropertyT<FbxDouble3> lKFbxDouble3;
    FbxPropertyT<FbxDouble> lKFbxDouble1;
    FbxColor theColor;

    ZeroMemory(pMaterial, sizeof(FbxMaterial));

    const char* cstrmtlName = lFbxMaterial->GetName();
    MultiByteToWideChar(CP_ACP, 0, cstrmtlName, -1, pMaterial->strMaterialName, MAX_PATH);

    if (lFbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
    {
        // We found a Phong material.  Display its properties.

        // Display the Ambient Color
        lKFbxDouble3 = ((FbxSurfacePhong *)lFbxMaterial)->Ambient;
        theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

        pMaterial->ambient.x = (float)theColor.mRed;
        pMaterial->ambient.y = (float)theColor.mGreen;
        pMaterial->ambient.z = (float)theColor.mBlue;

        // Display the Diffuse Color
        lKFbxDouble3 = ((FbxSurfacePhong *)lFbxMaterial)->Diffuse;
        theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

        pMaterial->diffuse.x = (float)theColor.mRed;
        pMaterial->diffuse.y = (float)theColor.mGreen;
        pMaterial->diffuse.z = (float)theColor.mBlue;

        // Display the Specular Color (unique to Phong materials)
        lKFbxDouble3 = ((FbxSurfacePhong *)lFbxMaterial)->Specular;
        theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

        pMaterial->specular.x = (float)theColor.mRed;
        pMaterial->specular.y = (float)theColor.mGreen;
        pMaterial->specular.z = (float)theColor.mBlue;

        // Display the Emissive Color
        lKFbxDouble3 = ((FbxSurfacePhong *)lFbxMaterial)->Emissive;
        theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

        pMaterial->emissive.x = (float)theColor.mRed;
        pMaterial->emissive.y = (float)theColor.mGreen;
        pMaterial->emissive.z = (float)theColor.mBlue;

        //Opacity is Transparency factor now
        lKFbxDouble1 = ((FbxSurfacePhong *)lFbxMaterial)->TransparencyFactor;

        pMaterial->opacity = 1.0f - (float)lKFbxDouble1.Get();

        // Display the Shininess
        lKFbxDouble1 = ((FbxSurfacePhong *)lFbxMaterial)->Shininess;

        pMaterial->shininess = (float)lKFbxDouble1.Get();

        // Display the Reflectivity
        lKFbxDouble1 = ((FbxSurfacePhong *)lFbxMaterial)->ReflectionFactor;

        pMaterial->reflectivity = (float)lKFbxDouble1.Get();

    }
    else if (lFbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
    {
        // We found a Lambert material. Display its properties.
        // Display the Ambient Color
        lKFbxDouble3 = ((FbxSurfaceLambert *)lFbxMaterial)->Ambient;
        theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

        pMaterial->ambient.x = (float)theColor.mRed;
        pMaterial->ambient.y = (float)theColor.mGreen;
        pMaterial->ambient.z = (float)theColor.mBlue;

        // Display the Diffuse Color
        lKFbxDouble3 = ((FbxSurfaceLambert *)lFbxMaterial)->Diffuse;
        theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

        pMaterial->diffuse.x = (float)theColor.mRed;
        pMaterial->diffuse.y = (float)theColor.mGreen;
        pMaterial->diffuse.z = (float)theColor.mBlue;

        // Display the Emissive
        lKFbxDouble3 = ((FbxSurfaceLambert *)lFbxMaterial)->Emissive;
        theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

        pMaterial->emissive.x = (float)theColor.mRed;
        pMaterial->emissive.y = (float)theColor.mGreen;
        pMaterial->emissive.z = (float)theColor.mBlue;

        // Display the Opacity
        lKFbxDouble1 = ((FbxSurfaceLambert *)lFbxMaterial)->TransparencyFactor;

        pMaterial->opacity = 1.0f - (float)lKFbxDouble1.Get();
    }


    FbxProperty lProperty;
    //go through all the possible textures

    lProperty = lFbxMaterial->FindProperty(lFbxMaterial->sDiffuse);

    if (!getTextureFile(&lProperty, pMaterial->strDiffuseTexture))
    {
        lProperty = lFbxMaterial->FindProperty(lFbxMaterial->sDiffuseFactor);
        getTextureFile(&lProperty, pMaterial->strDiffuseTexture);
    }

    lProperty = lFbxMaterial->FindProperty(lFbxMaterial->sEmissive);
    getTextureFile(&lProperty, pMaterial->strEmissiveTexture);

    lProperty = lFbxMaterial->FindProperty(lFbxMaterial->sNormalMap);
    getTextureFile(&lProperty, pMaterial->strNormalMap);
}

UINT FbxLoader::parseMaterial(FbxGeometry* pGeometry)
{
    int lMaterialCount = 0;
    FbxNode* lNode = NULL;
    if (pGeometry){
        lNode = pGeometry->GetNode();
        if (lNode)
            lMaterialCount = lNode->GetMaterialCount();
    }

    if (lMaterialCount > 0)
    {
        FbxPropertyT<FbxDouble3> lKFbxDouble3;
        FbxPropertyT<FbxDouble> lKFbxDouble1;
        FbxColor theColor;

        for (int lCount = 0; lCount < lMaterialCount; lCount++)
        {
            FbxSurfaceMaterial *lMaterial = lNode->GetMaterial(lCount);

            // Return if the material exist.
            WCHAR mtlNamel[MAX_PATH];
            MultiByteToWideChar(CP_ACP, 0, lMaterial->GetName(), -1, mtlNamel, MAX_PATH);
            for (int j = 0; j < _arrvMaterial.size(); j++)
            {
                WCHAR* cmpMtlName = _arrvMaterial[j].strMaterialName;
                if (0 == wcscmp(cmpMtlName, mtlNamel))
                {
                    return j;
                }
            }

            // Push back a new material strucure.
            FbxMaterial newMaterial;
            fillMaterialStructure(&newMaterial, lMaterial);
            _arrvMaterial.push_back(newMaterial);

            return _arrvMaterial.size() - 1;
        }
    }
    return 0;
}

void FbxLoader::parseMesh(FbxMesh* pMesh)
{
    UINT uPolygonCount = pMesh->GetPolygonCount();

    UINT controlPoint3[3];
    XMFLOAT3 position3[3];
    XMFLOAT3 normal3[3];
    XMFLOAT2 texcoord3[3];

    UINT uVertexId = 0;
    UINT uTriangleId = 0;

    UINT uMaterialIndex = parseMaterial(pMesh);

    for (UINT iPolygon = 0; iPolygon < uPolygonCount; iPolygon++)
    {
        UINT triangleCount = pMesh->GetPolygonSize(iPolygon) - 2;

        for (UINT iTriangle = 0; iTriangle < triangleCount; iTriangle++)
        {

            controlPoint3[0] = s_getPosition(pMesh, &position3[0], iPolygon, 0);
            controlPoint3[1] = s_getPosition(pMesh, &position3[1], iPolygon, iTriangle + 1);
            controlPoint3[2] = s_getPosition(pMesh, &position3[2], iPolygon, iTriangle + 2);

            s_getUV(pMesh, &texcoord3[0], iPolygon, 0);
            s_getUV(pMesh, &texcoord3[1], iPolygon, iTriangle + 1);
            s_getUV(pMesh, &texcoord3[2], iPolygon, iTriangle + 2);

            s_getNormal(pMesh, &normal3[0], iPolygon, 0);
            s_getNormal(pMesh, &normal3[1], iPolygon, iTriangle + 1);
            s_getNormal(pMesh, &normal3[2], iPolygon, iTriangle + 2);

            int lControlPointIndex0 = pMesh->GetPolygonVertex(iPolygon, 0);
            int lControlPointIndex1 = pMesh->GetPolygonVertex(iPolygon, iTriangle + 1);
            int lControlPointIndex2 = pMesh->GetPolygonVertex(iPolygon, iTriangle + 2);


            // Push back a triangle
            for (int i = 0; i < 3; i++)
            {
                _arrvIdxControlPoint.push_back(controlPoint3[i]);
                _arrvPosition.push_back(position3[i]);
                _arrvNormal.push_back(normal3[i]);
                _arrvTexcoord.push_back(texcoord3[i]);
            }
            _arrvTriangleMaterialIndex.push_back(uMaterialIndex);
        } // for polygon triangle count
    } // for polygonCount
}

void FbxLoader::loadMesh(FbxNode* pNode)
{
    FbxNodeAttribute::EType lAttributeType;
    int i;

    if (pNode->GetNodeAttribute() != NULL)
    {
        lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

        switch (lAttributeType)
        {
        case FbxNodeAttribute::eMesh:
            parseMesh(pNode->GetMesh());
            break;
        }
    }

    for (i = 0; i < pNode->GetChildCount(); i++)
    {
        loadMesh(pNode->GetChild(i));
    }
}

UINT FbxLoader::getVertex(FbxVertex* pVertex, UINT idx)
{
    pVertex->position = _arrvPosition[idx];
    pVertex->normal = _arrvNormal[idx];
    pVertex->texcoord = _arrvTexcoord[idx];

    return _arrvIdxControlPoint[idx];
}

//-----------------------------------------------------------------------
// FbxMeshImporter
//-----------------------------------------------------------------------
FbxMeshImporter::FbxMeshImporter()
{

}

FbxMeshImporter* FbxMeshImporter::create(wchar_t* file)
{
    FbxMeshImporter* importer = new FbxMeshImporter();
    if (importer->import(file))
    {
        return importer;
    }
    else
    {
        SAFE_DELETE(importer);
        return nullptr;
    }
}

bool FbxMeshImporter::import(wchar_t* file)
{
    FbxLoader* loader = FbxLoader::create(file);
    if (!loader)
    {
        return false;
    }

    UINT materialCount = loader->_arrvMaterial.size();
    FbxVertex vertex;
    UINT iPosition, vertexIndex, idxVertex;
    Subset subset;
    for (int i = 0; i < materialCount; i++)
    {
        subset.indexCount = 0;
        subset.indexStart = indices.size();
        subset.materialID = i;
        subset.vertexStart = 0;

        for (int j = 0; j < loader->_arrvTriangleMaterialIndex.size(); j++)
        {
            if (loader->_arrvTriangleMaterialIndex[j] == i)
            {
                idxVertex = j * 3;
                // Triangle corner0
                iPosition = loader->getVertex(&vertex, idxVertex + 0);
                vertexIndex = addVertex(iPosition, &vertex);
                indices.push_back(vertexIndex);

                // Triangle corner1
                iPosition = loader->getVertex(&vertex, idxVertex + 1);
                vertexIndex = addVertex(iPosition, &vertex);
                indices.push_back(vertexIndex);

                // Triangle corner2
                iPosition = loader->getVertex(&vertex, idxVertex + 2);
                vertexIndex = addVertex(iPosition, &vertex);
                indices.push_back(vertexIndex);

                subset.indexCount += 3;
            }
        }

        materials.push_back(loader->_arrvMaterial[i]);

        subsets.push_back(subset);
    }

    releaseCacheEntry();
    return true;
}

void FbxMeshImporter::releaseCacheEntry()
{
    // Iterate through all the elements in the cache and subsequent linked lists
    for (UINT i = 0; i < _vertexCache.size(); i++)
    {
        CacheEntry* entry = _vertexCache.at(i);
        while (entry != NULL)
        {
            CacheEntry* next = entry->next;
            SAFE_DELETE(entry);
            entry = next;
        }
    }

    _vertexCache.clear();
}

DWORD FbxMeshImporter::addVertex(UINT hash, FbxVertex* pVertex)
{
    // If this vertex doesn't already exist in the Vertices list, create a new entry.
    // Add the index of the vertex to the Indices list.
    bool foundInList = false;
    DWORD index = 0;

    // Since it's very slow to check every element in the vertex list, a hashtable stores
    // vertex indices according to the vertex position's index as reported by the OBJ file
    if ((UINT)_vertexCache.size() > hash)
    {
        CacheEntry* entry = _vertexCache.at(hash);
        while (entry != NULL)
        {
            FbxVertex* cacheVertex = vertices.data() + entry->index;

            // If this vertex is identical to the vertex already in the list, simply
            // point the index buffer to the existing vertex
            if (0 == memcmp(pVertex, cacheVertex, sizeof(FbxVertex)))
            {
                foundInList = true;
                index = entry->index;
                break;
            }

            entry = entry->next;
        }
    }

    // Vertex was not found in the list. Create a new entry, both within the Vertices list
    // and also within the hashtable cache
    if (!foundInList)
    {
        // Add to the Vertices list
        index = vertices.size();
        vertices.push_back(*pVertex);

        // Add this to the hashtable
        CacheEntry* pNewEntry = new CacheEntry;
        if (pNewEntry == NULL)
            return (DWORD)-1;

        pNewEntry->index = index;
        pNewEntry->next = NULL;

        // Grow the cache if needed
        while ((UINT)_vertexCache.size() <= hash)
        {
            _vertexCache.push_back(NULL);
        }

        // Add to the end of the linked list
        CacheEntry* pCurEntry = _vertexCache.at(hash);
        if (pCurEntry == NULL)
        {
            // This is the head element
            _vertexCache[hash] = pNewEntry;
        }
        else
        {
            // Find the tail
            while (pCurEntry->next != NULL)
            {
                pCurEntry = pCurEntry->next;
            }

            pCurEntry->next = pNewEntry;
        }
    }

    return index;
}