#version 120

varying vec4 pos;
varying vec3 normal;
varying vec4 shadow_coord;

uniform sampler2D tex;			//�͗l
uniform sampler2D depth_tex;	//�f�v�X�l�e�N�X�`��

//�e�̔Z��
uniform float shadow_ambient;

//�t�H���V�F�[�f�B���O�ɂ��F��Ԃ�
vec4 phong_shading(void) {
	vec3 light = normalize(gl_LightSource[0].position.xyz - pos.xyz);	/*���C�g�x�N�g��*/

	/*�����̌v�Z
	- OpenGL���v�Z�����������x�Ɣ��ˌW���̐�(gl_FrontLightProduct)��p����D*/
	vec4 ambient = gl_FrontLightProduct[0].ambient;

	//�g�U���˂̌v�Z
	float dcoef = max(dot(light, normal), 0.0);
	vec4 diffuse = gl_FrontLightProduct[0].diffuse * dcoef;

	//���ʔ��˂̌v�Z
	vec4 specular = vec4(0.0);
	if(dcoef > 0.0){
		vec3 view = normalize(-pos.xyz);		//�����x�N�g��

		//���˃x�N�g���̌v�Z(�t�H�����˃��f��)
		vec3 ref = 2.0 * dot(normal, light) * normal - light;
		float specularLight = pow(max(dot(ref, view), 0.0), gl_FrontMaterial.shininess);

		specular = gl_FrontLightProduct[0].specular * specularLight;
	}
	return ambient + diffuse + specular;
}

//�e�W���̎Z�o
float shadow_coef(void) {
	//�������W
	vec4 shadow_coord1 = shadow_coord / shadow_coord.w;

	//��������̃f�v�X�l(���_)�덷�]�������܂�
	float view_d = shadow_coord1.z - 0.0001;
	
	//��������̍ŏ��f�v�X�l���擾
	float light_d = texture2D(depth_tex, shadow_coord1.xy).x;

	//�e��0,������1
	float shadow_coef = 1.0;
	if(shadow_coord.w > 0.0) shadow_coef = light_d < view_d ? 0.0 : 1.0;

	return shadow_coef;
}

void main(void) {	
	vec4 light_col = phong_shading();

	gl_FragColor = shadow_ambient * shadow_coef() * light_col + (1.0 - shadow_ambient) * light_col;
	gl_FragColor.a = light_col.a;
}
