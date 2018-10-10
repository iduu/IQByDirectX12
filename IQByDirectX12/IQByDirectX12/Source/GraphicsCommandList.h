///		@fn		GrpahicsCommandList.h
///		@brief	GrahpicsCommandListのラッパークラス
///		@author	Ishibashi Ryuto
///		@date
///		2018/09/18	初版作成
#pragma once
/*システムヘッダインクルード*/
#include <memory>
#include <string>
#include <d3d12.h>
#include <wrl.h>

/*クラス使用宣言*/
class CommandAllocator;
class Device;
using Microsoft::WRL::ComPtr;

/// @brief グラフィックスコマンドリストのラッパークラス
/// @note	現状、リスト1つに対してアロケータを1つしか用意していないため、
///		フェンス処理が必要な点に注意
class GraphicsCommandList
{
public:
	/// @brief コンストラクタ
	GraphicsCommandList(std::shared_ptr<Device> device, D3D12_COMMAND_LIST_TYPE commandListType, const std::wstring & name);

	/// @brief デストラクタ
	~GraphicsCommandList();

	/// @brief		オブジェクトの生成処理
	/// @note		GraphicsCommandListクラスはこのメソッドを通じてのみ生成可能
	/// @param[in]	device			: デバイス
	/// @param[in]	commandListType : コマンドリストタイプ
	/// @param[in]	name			: コマンドリストの名前
	static std::shared_ptr<GraphicsCommandList> Create(std::shared_ptr<Device> device, D3D12_COMMAND_LIST_TYPE commandListType, const std::wstring& name = L"");

	/// @brief コマンドリストを取得する
	ComPtr<ID3D12GraphicsCommandList> GetCommandList();


	/// @brief コマンドリストをクローズする
	void Close();


	/// @brief コマンドリストをリセットする
	/// @note 要コマンドクローズ & 実行 & 実行待ち
	void Reset();
	void Reset(ComPtr<ID3D12PipelineState> pipelineState);

	/// @brief コマンドリストへのアクセスを行うオペレータオーバーロード
	ID3D12GraphicsCommandList* const operator->();

private:
	/*変数宣言*/
	ComPtr<ID3D12GraphicsCommandList> mCommandList;			// コマンドリストの実体へのポインタ
	std::shared_ptr<CommandAllocator> mCommandAllocator;	// コマンドアロケータ
};

