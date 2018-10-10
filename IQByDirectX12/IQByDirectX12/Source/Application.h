/*
	Application.h
	アプリケーションのコア部分
	@author Ishibashi Ryuto
	@history
	2018/07/11 初版作成
*/
#pragma once
#include <windows.h>
#include <d3d12.h>
#include <vector>
#include <wrl.h>
#include <memory>
#include "Math/Math.h"

class Window;
class Device;
class CommandAllocator;
class CommandQueue;
class RenderTarget;
class DepthBuffer;
class VertexBuffer;
class RootSignature;
class Texture;
class TextureLoader;
class DescriptorHeap;
class ConstantBuffer;
class PMDLoader;
class PMDModelData;
class PMXLoader;
class PMXModelData;
class Model;
class Shader;
class SwapChain;
class Animation;
class Keyboard;
class GraphicsCommandList;

using Microsoft::WRL::ComPtr;

struct Vertex
{
	float position[3];
	float normal[3];
	float uv[2];
};

class Application
{
public:
	Application();
	~Application();

	/// @brief		初期化処理
	/// @param[in]	window ウィンドウクラス
	/// @retval		true: 初期化正常終了
	/// @retval		false: 初期化失敗
	bool Initialize(const Window& window);

	/// @brief		描画処理
	void Render();

	/// @brief		終了処理
	void Terminate();
private:

	/* 定数定義 */
	const int RENDER_TARGET_NUM = 2;		//!	レンダーターゲット数

	/* 変数宣言 */
	std::shared_ptr<SwapChain>				mSwapChain;				//! スワップチェイン
	ComPtr<ID3D12PipelineState>				mPipelineState;			//! パイプラインステート
	std::shared_ptr<GraphicsCommandList>	mCommandList;			//! コマンドリスト
	std::shared_ptr<Device>					mDevice;				//! デバイス
	std::shared_ptr<CommandQueue>			mCommandQueue;			//! コマンドキュー
	std::shared_ptr<RenderTarget>			mRenderTarget;			//! レンダーターゲット
	std::shared_ptr<RootSignature>			mRootSignature;			//! ルートシグネチャ
	std::shared_ptr<DepthBuffer>			mDepthBuffer;			//! 深度バッファ
	std::vector<D3D12_INPUT_ELEMENT_DESC>	mInputLayoutDescs;
	std::shared_ptr<TextureLoader>			mTextureLoader;
	int										mTextureHandle;
	std::shared_ptr<DescriptorHeap>			mDescriptorHeap;
	std::shared_ptr<ConstantBuffer>			mConstantBuffer;
	std::shared_ptr<Shader>					mPixelShaderClass;
	std::shared_ptr<Shader>					mVertexShaderClass;
	std::shared_ptr<PMDLoader>				mModelLoader;
	std::shared_ptr<Model>					mModelData;
	std::shared_ptr<PMXLoader>				mPMXModelLoader;
	std::shared_ptr<Model>					mPMXModelData;
	std::shared_ptr<Animation>				mAnimationData;
	std::vector<std::shared_ptr<Model>>		mInstancingTestModels;

	std::shared_ptr<Keyboard>			mKeyboard;

	Math::Matrix4x4 mWorldMatrix;
	Math::Matrix4x4 mViewMatrix;
	Math::Matrix4x4 mProjectionMatrix;
	Math::Matrix4x4 mAffineMatrix;


	int mWindowWidth;
	int mWindowHeight;

	/* ローカルメソッド定義 */

	/// @fn CreateRootSignature
	/// ルートシグネチャを生成する
	/// @retval ture: 生成成功, false: 生成失敗
	bool CreateRootSignature();

	/// @fn _DebugCreatePMDRootSignature
	bool _DebugCreatePMDRootSignature();

	/// @fn ReadShader
	/// シェーダを読み込む
	/// @retval true: 読込成功, false: 読込失敗
	bool ReadShader();

	bool _DebugReadPMDShader();

	/// @fn CreatePipelineState
	/// パイプラインステートの作成
	/// @retval ture: 生成成功, false: 生成失敗
	bool CreatePipelineState();

	/// @fn _DebugCreatePMDPipelineState
	/// PMD用パイプラインステートの作成
	/// @retval true: 生成成功, false: 生成失敗
	bool _DebugCreatePMDPipelineState();

	/// @fn CreateCommandList
	/// コマンドリストの生成
	/// @retval true: 生成成功, false: 生成失敗
	bool CreateCommandList();

	/// @fn LoadTexture
	/// テクスチャロード
	void LoadTexture();

	/// @fn CreateConstantBuffer
	/// コンスタントバッファ生成
	/// @retval 生成成功: true
	/// @retval 生成失敗: false
	bool CreateConstantBuffer();

	/// @fn SetWVPMatrix
	/// WVP行列をコンスタントバッファにセットする
	void SetWVPMatrix();

	/// @fn LoadPMD
	/// PMDモデルデータをロードする
	void LoadPMD();

	/// @fn LoadPMX
	/// PMXモデルデータをロードする
	void LoadPMX();

	/// @brief モーションデータをロードする
	void LoadMotion();
};

