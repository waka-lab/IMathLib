#ifndef _CAMERA_HPP
#define _CAMERA_HPP

#include "IMathLib/interface/interface.hpp"
#include "IMathLib/math/liner_algebra.hpp"

namespace iml {
	namespace i_f {

		//カメラ制御(現状ではdoubleかfloat)
		//オープンGLでは各行列を転置して扱わなければならない
		template <class T>
		class camera {
			template <class _T, imsize_t _N>
			friend class light;

			static const T		_pi;							//円周率

			T					camera_near, camera_far;		//3D空間の遠近
			vector3<T>			target_pos;						//注視点座標
			vector3<T>			coordinate;						//座標(angle_h,angle_v,length)
			
			//カメラの座標系
			vector3<T>			front_direction;
			vector3<T>			right_direction;
			vector3<T>			up_direction;

			rect<T>				surface_rect;					//カメラのnear面の領域

			using set_proj_call = function<void(double, double, double, double, double, double)>;
			set_proj_call		_Set_projection_matrix;
			using get_proj_call = function<matrix4<T>(T, T, T, T, T, T)>;
			get_proj_call		_Get_projection_matrix;

			//極座標系をデカルト座標系に変換(y-up系)
			static vector3<T> polar_convert(const T& angle_h, const T& angle_v) {
				return vector3<T>(cos(angle_v)*sin(angle_h), sin(angle_v), cos(angle_v)*cos(angle_h));
			}
			//射影行列のセット
			void set_projection_matrix() const {
				//射影行列の設定
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				//auto temp = iml::transpose_matrix(iml::projection_frustum_matrix(c.surface_rect.left, c.surface_rect.right, c.surface_rect.bottom, c.surface_rect.top
				//	, c.camera_near, c.camera_far));
				//glLoadMatrixf(temp[0]);
				_Set_projection_matrix(surface_rect.left, surface_rect.right, surface_rect.bottom, surface_rect.top
					, camera_near, camera_far);
			}
			//ビュー行列のセット
			void set_view_matrix() const {
				//カメラ位置の取得
				vector3<T> camera_pos = get_camera_pos();
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				//y-up系だからこの順序
				auto temp = transpose_view_matrix(camera_pos, right_direction, up_direction, front_direction);
				glLoadMatrixf(temp[0]);
				/*gluLookAt(
					camera_pos[0], camera_pos[1], camera_pos[2],			//視点の位置x,y,z;
					target_pos[0], target_pos[1], target_pos[2],			//視界の中心位置の参照点座標x,y,z
					up_direction[0], up_direction[1], up_direction[2]);		//視界の上方向のベクトルx,y,z
					*/
			}
		public:
			//デフォルトでは透視投影によるカメラとなる
			constexpr camera() : camera_near(20), camera_far(600), coordinate(0, 0, 100)
				, front_direction(0, 0, 1), right_direction(1, 0, 0), up_direction(0, 1, 0)
				, _Set_projection_matrix(glFrustum), _Get_projection_matrix(projection_frustum_matrix<T>) {
			}
			~camera() {}

			//カメラを透視投影にする
			void set_frustum_camera() {
				_Set_projection_matrix = glFrustum;
				_Get_projection_matrix = projection_frustum_matrix<T>;
			}
			//カメラを平行投影にする
			void set_ortho_camera() {
				_Set_projection_matrix = glOrtho;
				_Get_projection_matrix = projection_ortho_matrix<T>;
			}

			//カメラの回転(vector2<T>(angle_h, angle_v))
			void move_angle(const vector2<T>& v) {
				//回転
				coordinate[0] += v[0]; coordinate[1] += v[1];
				coordinate[0] = fmod(coordinate[0], 2 * _pi);
				while (coordinate[1] > _pi) coordinate[1] -= 2 * _pi;
				while (coordinate[1] < -_pi) coordinate[1] += 2 * _pi;
				//カメラ座標系の変換(右手系だからfront_direction(z軸)は手前)
				front_direction = polar_convert(coordinate[0], coordinate[1]);
				//水平角度によって右は変わる
				right_direction = unit(
					(((coordinate[1] < -_pi / 2) || (coordinate[1] > _pi / 2)) ? vector3<T>(0, -1, 0) : vector3<T>(0, 1, 0))
					% front_direction);
				up_direction = unit(front_direction%right_direction);
			}
			void set_angle() {
				coordinate[0] = v[0]; coordinate[1] = v[1];
				coordinate[0] = fmod(coordinate[0], 2 * _pi);
				while (coordinate[1] > _pi) coordinate[1] -= 2 * _pi;
				while (coordinate[1] < -_pi) coordinate[1] += 2 * _pi;
				//カメラ座標系の変換(右手系だからfront_direction(z軸)は手前)
				front_direction = polar_convert(coordinate[0], coordinate[1]);
				//水平角度によって右は変わる
				right_direction = unit(
					(((coordinate[1] < -_pi / 2) || (coordinate[1] > _pi / 2)) ? vector3<T>(0, -1, 0) : vector3<T>(0, 1, 0))
					% front_direction);
				up_direction = unit(front_direction%right_direction);
			}
			//カメラのターゲットの平行移動(カメラの極座標系をも連動する)
			void move_target(const vector3<T>& v) {
				target_pos += v;
			}
			void set_target(const vector3<T>& v) {
				target_pos = v;
			}
			//カメラのターゲットのビューポート面に対する平行移動
			void move_target(const vector2<T>& v) {
				target_pos += (v[0] * right_direction + v[1] * up_direction);
			}
			//ターゲットとカメラの距離の移動
			void move_length(const T& l) {
				coordinate[2] += l;
				if (coordinate[2] < camera_near) coordinate[2] = camera_near;
				if (coordinate[2] > camera_far) coordinate[2] = camera_far;
			}
			void set_length(const T& l) {
				coordinate[2] = l;
				if (coordinate[2] < camera_near) coordinate[2] = camera_near;
				if (coordinate[2] > camera_far) coordinate[2] = camera_far;
			}
			//カメラの位置の移動(flag:abs(angle_v)がpi/2を超えるかのフラグ)
			void move_camera_pos(const vector3<T>& v, bool flag = false) {
				//直交座標系から極座標系への変換
				coordinate[2] = abs(camera_pos);
				coordinate[1] = asin(camera_pos[1] / coordinate[2]);
				coordinate[0] = (camera_pos[0] > 0)*acos(camera_pos[2] / sqrt(camera_pos[0] * camera_pos[0] + camera_pos[2] * camera_pos[2]));
				//垂直角度がpi/2を超えるときはその分の補正を加える
				if (flag) {
					coordinate[1] = _pi - camera_pos[1];
					if (coordinate[1] > _pi) coordinate[1] -= 2 * _pi;
					coordinate[0] = fmod(coordinate[0] + _pi, 2 * _pi);
				}
				//カメラ座標系の変換(右手系だからfront_direction(z軸)は手前)
				front_direction = polar_convert(coordinate[0], coordinate[1]);
				//水平角度によって右は変わる
				right_direction = unit(
					(((coordinate[1] <= -_pi / 2) || (coordinate[1] >= _pi / 2)) ? vector3<T>(0, -1, 0) : vector3<T>(0, 1, 0))
					% front_direction);
				up_direction = unit(front_direction%right_direction);
			}
			void set_camera_pos(const vector3<T>& v, bool flag = false) {
				vector3<T> camera_pos(v - target_pos);
				//直交座標系から極座標系への変換
				coordinate[2] = abs(camera_pos);
				coordinate[1] = asin(camera_pos[1] / coordinate[2]);
				coordinate[0] = (camera_pos[0] > 0)*acos(camera_pos[2] / sqrt(camera_pos[0] * camera_pos[0] + camera_pos[2] * camera_pos[2]));
				//垂直角度がpi/2を超えるときはその分の補正を加える
				if (flag) {
					coordinate[1] = _pi - camera_pos[1];
					if (coordinate[1] > _pi) coordinate[1] -= 2 * _pi;
					coordinate[0] = fmod(coordinate[0] + _pi, 2 * _pi);
				}
				//カメラ座標系の変換(右手系だからfront_direction(z軸)は手前)
				front_direction = polar_convert(coordinate[0], coordinate[1]);
				//水平角度によって右は変わる
				right_direction = unit(
					(((coordinate[1] <= -_pi / 2) || (coordinate[1] >= _pi / 2)) ? vector3<T>(0, -1, 0) : vector3<T>(0, 1, 0))
					% front_direction);
				up_direction = unit(front_direction%right_direction);
			}

			//視野角からnear面の領域の構築
			void set_viewing_angle(const T& angle_x, const T& angle_y) {
				surface_rect.right = camera_near*tan(angle_x / 2);
				surface_rect.left = -surface_rect.right;
				surface_rect.top = camera_near*tan(angle_y / 2);
				surface_rect.bottom = -surface_rect.top;
			}
			//視野角からnear面の領域の構築(比から)
			void set_viewing_angle_magnific(const T& angle_x, const T& ratio) {
				surface_rect.right = camera_near*tan(angle_x / 2);
				surface_rect.left = -surface_rect.right;
				surface_rect.top = camera_near*tan(angle_x*ratio / 2);
				surface_rect.bottom = -surface_rect.top;
			}
			//直接領域を構築
			template <class S>
			void set_surface_rect(const  rect<S>& rect) {
				surface_rect = rect;
			}

			const T& get_near() const { return camera_near; }
			const T& get_far() const { return camera_far; }
			const T& get_length() const { return coordinate[2]; }
			const vector3<T>& get_target_pos() const { return target_pos; }
			//カメラ座標の更新
			vector3<T> get_camera_pos() const { return target_pos + coordinate[2] * polar_convert(coordinate[0], coordinate[1]); }

			static const T& pi() { return _pi; }

			//射影行列の取得
			matrix4<T> get_projection_matrix() const {
				return _Get_projection_matrix(surface_rect.left, surface_rect.right, surface_rect.bottom, surface_rect.top
					, camera_near, camera_far);
			}
			//ビュー行列の取得
			matrix4<T> get_view_matrix() const {
				return view_matrix(get_camera_pos(), right_direction, up_direction, front_direction);
			}

			//各種カメラ更新のための関数
			template <class S>
			friend void view_update(const camera<S>* c);
			template <class S>
			friend void projection_update(const camera<S>* c);
			//カメラ関係の座標取得関数
			template <class S>
			friend vector3<S> world_to_screen(const camera<S>& c, const vector3<S>& pos);
			template <class S>
			friend vector3<S> screen_to_world(const camera<S>& c, const vector3<S>& pos);
		};
		template <class T>
		const T camera<T>::_pi = iml::math_constant<T>::pi;

		//ビュー行列の更新
		template <class T>
		inline void view_update(const camera<T>* c) {
			c->set_view_matrix();
		}
		//射影行列の更新
		template <class T>
		inline void projection_update(const camera<T>* c) {
			c->set_projection_matrix();
		}
		//スクリーンモード開始
		inline void screen_start(const rect<imint_t>& screen = interface_control::inst()->get_client_area()) {
			//射影行列の設定
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			//左上が0になるようにする
			auto temp = iml::transpose_matrix(iml::projection_ortho_matrix<float>(0, screen.right - screen.left, screen.bottom - screen.top, 0, -1, 1));
			glLoadMatrixf(temp[0]);
			//gluOrtho2D(0, screen.right - screen.left, screen.bottom - screen.top, 0);
			//スクリーンモードが終了するまでビュー行列の制御
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
		}
		//スクリーンモード終了
		inline void screen_end() {
			//スクリーンモードに対する行列をポップする
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}
		//ワールド座標をスクリーン座標に変換
		template <class T>
		inline vector3<T> world_to_screen(const camera<T>& c, const vector3<T>& pos) {
			vector4<T> temp(pos[0], pos[1], pos[2], 1);
			temp = interface_control::inst()->get_viewport_matrix<T>()
				*c.get_projection_matrix()
				*c.get_view_matrix()
				*temp;
			return vector3<T>(temp[0], temp[1], temp[2]) / temp[3];
		}
		//スクリーン座標をワールド座標に変換
		template <class T>
		inline vector3<T> screen_to_world(const camera<T>& c, const vector3<T>& pos) {
			vector4<T> temp(pos[0], pos[1], pos[2], 1);
			temp = inverse_matrix(interface_control::inst()->get_viewport_matrix<T>()
				*c.get_projection_matrix()
				*c.get_view_matrix())
				*temp;
			return vector3<T>(temp[0], temp[1], temp[2]) / temp[3];
		}
	}
}

#endif