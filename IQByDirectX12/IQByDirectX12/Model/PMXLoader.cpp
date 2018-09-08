#include "PMXLoader.h"
#include <cstdio>
#include <iostream>

PMXLoader::PMXLoader(ComPtr<ID3D12Device> device)
	: ModelLoader(device)
{
}


PMXLoader::~PMXLoader()
{
}

std::shared_ptr<PMXLoader> PMXLoader::Create(ComPtr<ID3D12Device> device)
{
	auto modelLoader = std::shared_ptr<PMXLoader>(new PMXLoader(device));
	return modelLoader;
}

std::shared_ptr<Model> PMXLoader::LoadModel(const std::string & filePath)
{
	auto handleItr = mModelHandleManager.find(filePath);

	if (handleItr == mModelHandleManager.end() || !mModelDataManager.IsExist((*handleItr).second))
	{

		FILE *fp;
		PMX::ModelDataDesc modelDataDesc;

		auto err = fopen_s(&fp, filePath.c_str(), "rb");
		if (err != 0)
		{
#ifdef _DEBUG
			std::cout << "Failed File Open \"" << filePath << std::endl;
#endif
			return nullptr;
		}

		std::string fileSignature;
		fileSignature.clear();
		fileSignature.resize(SIGNATURE_SIZE);
		fread((void*)fileSignature.data(), SIGNATURE_SIZE, 1, fp);
		if (fileSignature != FILE_SIGNATURE)
		{
#ifdef _DEBUG
			std::cout << "This file is not PMX. Prease load \".pmx\" file." << std::endl;
#endif
			fclose(fp);
			return nullptr;
		}

		// ヘッダデータ読み込み
		LoadHeader(modelDataDesc.header, fp);

		// モデル情報の読み込み
		LoadModelInfo(modelDataDesc.modelInfo, fp);

		// 頂点情報読み込み
		LoadVertexData(modelDataDesc.vertices, modelDataDesc.header, fp);

		// インデックス情報読み込み
		LoadIndexData(modelDataDesc.indexies, modelDataDesc.header, fp);

		// テクスチャ情報読み込み
		LoadTextureData(modelDataDesc.textures, fp);

		// マテリアル情報読み込み
		LoadMaterial(modelDataDesc.materials, modelDataDesc.header, fp);

		fclose(fp);

		auto modelData = PMXModelData::Create(mDevice, modelDataDesc);
		mModelHandleManager[filePath] = mModelDataManager.Regist(modelData);
	}
	return std::shared_ptr<Model>(new Model(mModelHandleManager[filePath]));
}

void PMXLoader::ClearModelData()
{
}

std::string PMXLoader::ReadTextBuf(FILE * fp)
{
	std::string text;

	int bufSize;
	fread(&bufSize, sizeof(int), 1, fp);
	fread((void*)text.data(), sizeof(char) * bufSize, 1, fp);

	return text;
}

void PMXLoader::LoadHeader(PMX::Header & header, FILE * fp)
{
	fread(&header.version, sizeof(header.version), 1, fp);
	fread(&header.byteSize, sizeof(header.byteSize), 1, fp);

	if (header.byteSize <= 0)
	{
		return;
	}

	header.pmxDataInfo.resize(header.byteSize);
	fread((void*)header.pmxDataInfo.data(), sizeof(header.pmxDataInfo[0]), header.pmxDataInfo.size(), fp);
}

void PMXLoader::LoadModelInfo(PMX::ModelInfo & modelInfo, FILE * fp)
{
	modelInfo.modelName = ReadTextBuf(fp);
	modelInfo.modelNameEng = ReadTextBuf(fp);
	modelInfo.comment = ReadTextBuf(fp);
	modelInfo.commentEng = ReadTextBuf(fp);
}

void PMXLoader::LoadVertexData(std::vector<PMX::Vertex>& vertexData, const PMX::Header & header, FILE * fp)
{
	int vertexCount;
	bool isUseAppendUV = (header.pmxDataInfo[(int)PMX::DataInfo::appendUVCount] > 0);
	size_t boneIndexSize = header.pmxDataInfo[(int)PMX::DataInfo::boneIndexSize];

	fread(&vertexCount, sizeof(vertexCount), 1, fp);
	vertexData.resize(vertexCount);

	for (auto& vertex : vertexData)
	{
		fread(&vertex.position, sizeof(vertex.position), 1, fp);
		fread(&vertex.normal, sizeof(vertex.normal), 1, fp);
		fread(&vertex.uv, sizeof(vertex.uv), 1, fp);
		
		if (isUseAppendUV)
		{
			vertex.appendUV.resize(header.pmxDataInfo[(int)PMX::DataInfo::appendUVCount]);
			fread((void*)vertex.appendUV.data(), sizeof(vertex.appendUV[0]), vertex.appendUV.size(), fp);
		}

		fread(&vertex.weightDeformType, sizeof(char), 1, fp);
		
		switch (vertex.weightDeformType)
		{
		case PMX::WeightDeformType::BDEF1:
			fread(&vertex.deformParam.boneIndex[0], boneIndexSize, 1, fp);
			break;

		case PMX::WeightDeformType::BDEF2:
			fread(&vertex.deformParam.boneIndex[0], boneIndexSize, 2, fp);
			fread(&vertex.deformParam.boneWeight[0], sizeof(vertex.deformParam.boneWeight[0]), 1, fp);
			break;

		case PMX::WeightDeformType::BDEF4:
			fread(&vertex.deformParam.boneIndex[0], boneIndexSize, 4, fp);
			fread(&vertex.deformParam.boneWeight[0], sizeof(vertex.deformParam.boneWeight[0]), 4, fp);
			break;

		case PMX::WeightDeformType::SDEF:
			fread(&vertex.deformParam.boneIndex, boneIndexSize, 2, fp);
			fread(&vertex.deformParam.boneWeight, sizeof(vertex.deformParam.boneWeight[0]), 1, fp);
			fread(&vertex.deformParam.c, sizeof(vertex.deformParam.c), 1, fp);
			fread(&vertex.deformParam.r0, sizeof(vertex.deformParam.r0), 1, fp);
			fread(&vertex.deformParam.r1, sizeof(vertex.deformParam.r1), 1, fp);
			break;
		default:
#ifdef _DEBUG
			std::cout << "PMX File Load Error." << std::endl;
#endif
			return;
			break;
		}
		fread(&vertex.edgeScale, sizeof(vertex.edgeScale), 1, fp);
	}
}

void PMXLoader::LoadIndexData(std::vector<PMX::Index>& indexData, const PMX::Header & header, FILE * fp)
{
	int indexCount;
	fread(&indexCount, sizeof(indexCount), 1, fp);
	indexData.resize(indexCount);
	size_t vertIndexSize = header.pmxDataInfo[(int)PMX::DataInfo::vertexIndexSize];
	
	for (auto& index : indexData)
	{
		fread(&index.vertIndex, vertIndexSize, 1, fp);
	}
}

void PMXLoader::LoadTextureData(std::vector<PMX::Texture>& textureData, FILE * fp)
{
	int textureNum;
	fread(&textureNum, sizeof(textureNum), 1, fp);
	textureData.resize(textureNum);
	for (auto& texture : textureData)
	{
		texture.texturePath = ReadTextBuf(fp);
	}
}

void PMXLoader::LoadMaterial(std::vector<PMX::Material>& materialData, const PMX::Header & header, FILE * fp)
{
	int materialNum;
	int textureIndexSize = header.pmxDataInfo[(int)PMX::DataInfo::textureIndexSize];
	fread(&materialNum, sizeof(materialNum), 1, fp);
	materialData.resize(materialNum);
	for (auto& material : materialData)
	{
		material.name = ReadTextBuf(fp);
		material.nameEng = ReadTextBuf(fp);
		fread(&material.diffuse, sizeof(material.diffuse), 1, fp);
		fread(&material.specular, sizeof(material.specular), 1, fp);
		fread(&material.specularFactor, sizeof(material.specularFactor), 1, fp);
		fread(&material.ambient, sizeof(material.ambient), 1, fp);
		fread(&material.drawFlags, sizeof(material.drawFlags), 1, fp);
		fread(&material.edgeColor, sizeof(material.edgeColor), 1, fp);
		fread(&material.edgeSize, sizeof(material.edgeSize), 1, fp);
		fread(&material.textureIndex, textureIndexSize, 1, fp);
		fread(&material.sphereTextureIndex, textureIndexSize, 1, fp);
		fread(&material.sphereMode, sizeof(material.sphereMode), 1, fp);
		fread(&material.shareToonFlag, sizeof(material.shareToonFlag), 1, fp);
		if (material.shareToonFlag == 0)
		{
			fread(&material.toonTextureIndex, textureIndexSize, 1, fp);
		}
		else if (material.shareToonFlag == 1)
		{
			fread(&material.shareToonTexture, sizeof(material.shareToonTexture), 1, fp);
		}

		material.materialMemo = ReadTextBuf(fp);
		fread(&material.vertNum, sizeof(material.vertNum), 1, fp);
	}
}
