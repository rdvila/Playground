#include "PCH.h"
#include "MeshLoader.h"

using namespace Playground::Graphics;


MeshLoader::MeshLoader(const std::filesystem::path& path)
	: mPath(path), mVextexCount(0), mIndexCount(0)
{
}

void MeshLoader::Initialize()
{
	Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(mPath.string(),
        aiProcess_OptimizeMeshes |
        aiProcess_OptimizeGraph |
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_FlipUVs |
        aiProcess_ConvertToLeftHanded);

    if (scene == nullptr) {
        throw std::runtime_error(importer.GetErrorString());
        return;
    }

    aiMesh *mesh = scene->mMeshes[0];
    mVextexCount = mesh->mNumVertices;

    for (unsigned int i = 0; i < mVextexCount; ++i)
    {
        mVertices.push_back((mesh->mVertices + i)->x);
        mVertices.push_back((mesh->mVertices + i)->y);
        mVertices.push_back((mesh->mVertices + i)->z);
    }

    aiFace *face = &mesh->mFaces[0];
    mIndexCount = face->mNumIndices;

    for (unsigned int i = 0; i < mIndexCount; ++i)
    {
        mIndices.push_back(*(face->mIndices + i));
    }
}

void MeshLoader::Destroy()
{
}
