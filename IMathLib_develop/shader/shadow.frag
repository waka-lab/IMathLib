#version 120

varying vec4 pos;
varying vec3 normal;
varying vec4 shadow_coord;

uniform sampler2D tex;			//模様
uniform sampler2D depth_tex;	//デプス値テクスチャ

//影の濃さ
uniform float shadow_ambient;

//フォンシェーディングによる色を返す
vec4 phong_shading(void) {
	vec3 light = normalize(gl_LightSource[0].position.xyz - pos.xyz);	/*ライトベクトル*/

	/*環境光の計算
	- OpenGLが計算した光源強度と反射係数の積(gl_FrontLightProduct)を用いる．*/
	vec4 ambient = gl_FrontLightProduct[0].ambient;

	//拡散反射の計算
	float dcoef = max(dot(light, normal), 0.0);
	vec4 diffuse = gl_FrontLightProduct[0].diffuse * dcoef;

	//鏡面反射の計算
	vec4 specular = vec4(0.0);
	if(dcoef > 0.0){
		vec3 view = normalize(-pos.xyz);		//視線ベクトル

		//反射ベクトルの計算(フォン反射モデル)
		vec3 ref = 2.0 * dot(normal, light) * normal - light;
		float specularLight = pow(max(dot(ref, view), 0.0), gl_FrontMaterial.shininess);

		specular = gl_FrontLightProduct[0].specular * specularLight;
	}
	return ambient + diffuse + specular;
}

//影係数の算出
float shadow_coef(void) {
	//光源座標
	vec4 shadow_coord1 = shadow_coord / shadow_coord.w;

	//光源からのデプス値(視点)誤差評価をも含む
	float view_d = shadow_coord1.z - 0.0001;
	
	//光源からの最小デプス値を取得
	float light_d = texture2D(depth_tex, shadow_coord1.xy).x;

	//影で0,日向で1
	float shadow_coef = 1.0;
	if(shadow_coord.w > 0.0) shadow_coef = light_d < view_d ? 0.0 : 1.0;

	return shadow_coef;
}

void main(void) {	
	vec4 light_col = phong_shading();

	gl_FragColor = shadow_ambient * shadow_coef() * light_col + (1.0 - shadow_ambient) * light_col;
	gl_FragColor.a = light_col.a;
}
