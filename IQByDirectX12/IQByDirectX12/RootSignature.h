/*
	RootSignature.h
	ルートシグネチャ
	@author Ishibashi Ryuto
	@date
	2018/09/05	初版作成
*/
#pragma once
// システムヘッダインクルード
#include<vector>
#include<memory>
#include<map>
#include<d3d12.h>
#include<wrl.h>

// 自作ヘッダインクルード

// クラス使用宣言
using Microsoft::WRL::ComPtr;

class RootSignature 
{
public:
	/// コンストラクタ
	RootSignature();

	/// デストラクタ
	~RootSignature();

	/// @fn Create
	/// ルートシグネチャを生成する
	/// @note RootSignatureクラスの実体はこのメソッドでのみ生成可能
	/// @retval		生成成功	: RootSignatureのポインタ
	/// @retval		生成失敗	: nullptr
	static std::shared_ptr<RootSignature> Create();

	/// @fn ConstructRootSignature
	/// 現在の設定項目でルートシグネチャを構築する
	/// @note 呼び出す度にRootSignatureを構築するため、重め。
	///		基本的には1回だけ呼び出せばいいようにしてください
	/// @param[in] device		: D3D12デバイス
	/// @retval	構築成功		: true
	/// @retval 構築失敗		: false
	bool ConstructRootSignature(ComPtr<ID3D12Device> device);

	/// @fn AddDescriptorRange
	/// ディスクリプタレンジを追加する
	/// @note 存在しないルートパラメータを参照した場合、追加は行われないので注意
	/// @param[in] rootParamIndex	: 追加対象ルートパラメータのインデックス
	/// @param[in] descriptorRange	: ディスクリプタレンジ
	void AddDescriptorRange(int rootParamIndex, const D3D12_DESCRIPTOR_RANGE & descriptorRange);

	/// @param[in] rootParamIndex		: 追加対象ルートパラメータのインデックス
	/// @param[in] rangeType			: ディスクリプタの種類
	/// @param[in] descriptorNum		: 読み取る出スクリプタの数
	/// @param[in] baseShaderRegister	: シェーダレジスタの開始番号
	void AddDescriptorRange(int rootParamIndex,
		D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
		UINT descriptorNum,
		UINT baseShaderRegister);

	/// @fn AddRootParameter
	/// ルートパラメータを追加する
	/// @param[in] parameterType	: パラメータタイプ(デフォルトでD3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
	/// @param[in] shaderVisibility	: シェーダ可視性(
	/// @note このメソッドはあくまでルートパラメータを「追加」するだけで、
	///			具体的な設定は行いません。
	///			具体的なディスクリプタレンジはAddDescriptorRangeを使って追加してください
	/// @retval	ルートパラメータ番号
	int AddRootParameter(D3D12_SHADER_VISIBILITY shaderVisibility, D3D12_ROOT_PARAMETER_TYPE paramType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);

	/// @fn GetRootSignature
	/// ルートシグネチャを取得する
	ComPtr<ID3D12RootSignature> GetRootSignature() const;

private:
	D3D12_STATIC_SAMPLER_DESC mStaticSamplerDesc;
	std::map<int ,std::vector<D3D12_DESCRIPTOR_RANGE> >	mDescriptorRanges;	// ディスクリプタレンジ
	std::vector<D3D12_ROOT_PARAMETER> mRootParameters;		// ルートパラメータ
	ComPtr<ID3D12RootSignature> mRootSignature;				// ルートシグネチャ群
};

