﻿#ifndef _MODEL_HPP
#define _MODEL_HPP

#include "IMathLib/image/image.hpp"
#include "IMathLib/math/liner_algebra.hpp"
#include "IMathLib/math/quaternion_math.hpp"
#include "IMathLib/container/array.hpp"
#include "IMathLib/utility/tuple.hpp"

namespace iml {
	namespace m3d {

		//型定義
		using byte = unsigned char;
		using word = unsigned __int16;
		using dword = __int32;

		//ボーンデータ
		template <class T>
		struct bone_data {
			char				name[32];				//ボーン名
			dword				kind;					//このボーンのrootが属する種類
			dword				index;					//ボーンのインデックス番号(rootは0固定)
			vector3<T>			ref_relative_point;		//参照用のボーンの先端の相対座標(デフォルト値保持のため変更不可)

			bone_data<T>		*parent;				//親ボーン
			list<bone_data<T>>	children;				//子ボーン

			vector3<T>			absolute_point;			//現在のボーンの基点の絶対座標
			vector3<T>			relative_point;			//現在のボーンの基点から先端の相対座標

			vector3<T>			base_vector[3];			//y'軸がボーンの先端方向となる基底ベクトル(単位ベクトル)

			bool				flag;					//ボーン変形フラグ(親の変形や連動も伝達する)

			quaternion<T>		relative_trans;			//前回の姿勢変型からの相対変換クォータニオン
			T					twist_angle;			//現在の捩れ角
			T					twist_range[2];			//捩れ角の制限
			T					vertical_angle;			//現在の垂直角度
			T					vertical_range[2];		//垂直角度の制限
			quaternion<T>		base_relative;			//基底からの相対変換クォータニオン
		};
		//ボーン情報(rootは0固定)
		template <class T>
		using bone_info = dynamic_array<bone_data<T>*>;

		//面情報
		template <class T>
		struct face {
			dword			m;			//面の材質番号(m<0は存在しないという意味)
			dword			v[3];		//頂点番号の配列番号(インデックス)
			vector2<T>		uv[3];		//UVマップ
		};
		//材質情報(データの読み込みの際に使用)
		struct mat_data {
			color_32bit		col;			//色
			color_32bit		amb;			//環境光(アンビエントカラー)
			color_32bit		dif;			//拡散反射光(デフューズカラー)
			color_32bit		emi;			//自己放射光(エミッシブカラー)
			color_32bit		spe;			//鏡面反射光(スペキュラカラー)
			byte			power;			//反射光の強さ

			char		tex_name[256] = {};		//テクスチャファイルパス(tex_name[0]==0でテクスチャの有無をも判定)
			dword		tex_id = 0;				//テクスチャID
		};
		using mat_array_data = pair<shared_ptr<mat_data, deallocator<mat_data>::array_variable>, dword>;
		//頂点配列データ
		template <class T>
		struct vertex_data {
			vector3<T>		point;		//頂点座標
			vector3<T>		normal;		//頂点法線
		};
		//1つのオブジェクトあたりの頂点データ(1つのオブジェクトに対して2^16個まで)
		template <class T>
		using vertex_array_data = pair<shared_ptr<vertex_data<T>, deallocator<vertex_data<T>>::array_variable>, word>;

		//オブジェクトデータ
		template <class T>
		struct object_data {
			char		name[256];		//オブジェクト名
			dword		visible;		//可視状態
			dword		shading;		//シェーディング(0:グロー(フラット),1:スムーズ)

			dynamic_array<face<T>>
				face_info;					//面情報
			vertex_array_data<T>
				vertex_info;				//頂点情報
		};

		//モデルデータ(インターフェース機能を用いるときshared_ptrで管理しなければならない)
		template <class T>
		struct model_data {
			mat_array_data			mat;		//マテリアルデータ
			list<object_data<T>>	obj;		//オブジェクトデータ

			//shared_ptr<bone_info<T>>bone;		//ボーンデータ(getでボーンデータの有無を確認できる)
		};

		//1つのオブジェクトあたりの頂点インデックス
		using vertex_index = pair<shared_ptr<word, deallocator<word>::array_variable>, dword>;
		//マテリアル情報
		template <class T>
		struct material_info {
			bool	validity = true;	//マテリアル情報の有効
			bool	use_tex;			//テクスチャの有無
			dword	texture_id;			//テクスチャID
			dword	mat_index;			//マテリアルデータの添え字

			vertex_index				vtx_index;
			dynamic_array<vector2<T>>	uv_point;
		};
		//内部オブジェクト
		template <class T>
		struct inner_object {
			char			name[256];			//オブジェクト名
			dword			visible;			//可視
			dword			shading;			//シェーディング(0:グロー,1:スムーズ)

			dynamic_array<material_info<T>>
				mat;							//マテリアル配列
			vertex_array_data<T>
				vtx;							//1つのオブジェクトの頂点データ
		};
		//モデルデータ
		template <class T>
		struct model_object {
			dynamic_array<inner_object<T>>
				object;						//内部オブジェクト配列

			mat_array_data		mat;		//全体のマテリアルデータ
		};
	}
}


#endif