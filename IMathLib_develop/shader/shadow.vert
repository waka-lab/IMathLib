#version 120

//フラグメントシェーダに値を渡すための変数
varying vec4 pos;
varying vec3 normal;
varying vec4 shadow_coord;		//シャドウデプスマップの参照用座標

uniform mat4 tex_mat;			//テクスチャ変換行列
 
void main(void) {
	//フラグメントシェーダ用のカメラの基底に変換
	pos = gl_ModelViewMatrix * gl_Vertex;				//頂点位置
	normal = normalize(gl_NormalMatrix * gl_Normal);	//頂点法線
	shadow_coord = tex_mat * gl_Vertex;					//影用座標値(光源中心座標)

	//スクリーン描画用
	gl_Position = gl_ProjectionMatrix * pos;		//頂点位置
	gl_FrontColor = gl_Color;						//頂点色
	gl_TexCoord[0] = gl_MultiTexCoord0;				//頂点テクスチャ座標
}
