#version 120

//�t���O�����g�V�F�[�_�ɒl��n�����߂̕ϐ�
varying vec4 pos;
varying vec3 normal;
varying vec4 shadow_coord;		//�V���h�E�f�v�X�}�b�v�̎Q�Ɨp���W

uniform mat4 tex_mat;			//�e�N�X�`���ϊ��s��
 
void main(void) {
	//�t���O�����g�V�F�[�_�p�̃J�����̊��ɕϊ�
	pos = gl_ModelViewMatrix * gl_Vertex;				//���_�ʒu
	normal = normalize(gl_NormalMatrix * gl_Normal);	//���_�@��
	shadow_coord = tex_mat * gl_Vertex;					//�e�p���W�l(�������S���W)

	//�X�N���[���`��p
	gl_Position = gl_ProjectionMatrix * pos;		//���_�ʒu
	gl_FrontColor = gl_Color;						//���_�F
	gl_TexCoord[0] = gl_MultiTexCoord0;				//���_�e�N�X�`�����W
}
