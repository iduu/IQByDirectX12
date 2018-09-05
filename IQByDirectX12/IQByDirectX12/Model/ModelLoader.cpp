#include "ModelLoader.h"



ModelLoader::ModelLoader(ComPtr<ID3D12Device> device)
	: mDevice(device)
	, mModelDataManager(ModelDataManager::GetInstance())
{
}


ModelLoader::~ModelLoader()
{
}
