#ifndef _MODEL_CONTROL_HPP
#define _MODEL_CONTROL_HPP

#include "IMathLib/3d/model.hpp"
#include "IMathLib/utility/smart_ptr.hpp"
#include "IMathLib/utility/type_traits.hpp"
#include "IMathLib/interface/camera.hpp"

namespace iml {
	namespace i_f {

		//型定義
		using byte = m3d::byte;
		using word = m3d::word;
		using dword = m3d::dword;

		//頂点配列用のデータ
		template <class T>
		struct vertex_data {
			vector3<T>		point;		//頂点座標
			vector3<T>		normal;		//頂点法線
			vector2<T>		uv;			//UV
		};
		//頂点配列データ(頂点配列,サイズ)
		template <class T>
		using vertex_array_data = pair<shared_ptr<vertex_data<T>, deallocator<vertex_data<T>>::array_variable>, dword>;

		//一つあたりのオブジェクトデータ
		template <class T>
		struct object_data {

		};

		//モデルデータ
		template <class T>
		struct model_data {
			//Tはdoubleかfloatでなければならない
			static_assert(is_same<T, double>::value || is_same<T, float>::value, "T must be float or double.");
			//OpenGLの座標データの型指定用の定数
			static constexpr int gl_type_constant = (is_same<T, double>::value) ? GL_DOUBLE : GL_FLOAT;



			constexpr model_data() {}

		};
		//衝突モデルデータ
		template <class T>
		struct collision_model_data {
			//Tはdoubleかfloatでなければならない
			static_assert(is_same<T, double>::value || is_same<T, float>::value, "T must be float or double.");
			//OpenGLの座標データの型指定用の定数
			static constexpr int gl_type_constant = (is_same<T, double>::value) ? GL_DOUBLE : GL_FLOAT;

			vertex_array_data<T>		vertex;
		};

		//モデルの描画
		template <class T>
		inline void draw_model(m3d::model_object<T>* model) {
			if (model == nullptr) return;
			//アルファブレンドをする
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glPushMatrix();

			//内部オブジェクトのループ
			for (auto obj = model->object.begin(); obj != model->object.end(); ++obj) {
				//不可視のときはスキップ
				if (!obj->visible) continue;
				glShadeModel((obj->shading == 0) ? GL_FLAT : GL_SMOOTH);

				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_NORMAL_ARRAY);

				//バイトオフセットを指定して頂点配列と法線配列を設定する
				glVertexPointer(3, GL_FLOAT, sizeof(m3d::vertex_data<T>), &obj->vtx.first->point[0]);
				glNormalPointer(GL_FLOAT, sizeof(m3d::vertex_data<T>), &obj->vtx.first->normal[0]);

				for (auto mat = obj->mat.begin(); mat != obj->mat.end(); ++mat) {
					//マテリアルの設定
					if (mat->validity) {
						glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, to_normal_color<T>(model->mat.first[mat->mat_index].dif).v);
						glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, to_normal_color<T>(model->mat.first[mat->mat_index].amb).v);
						glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, to_normal_color<T>(model->mat.first[mat->mat_index].spe).v);
						glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, to_normal_color<T>(model->mat.first[mat->mat_index].emi).v);

						glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, model->mat.first[mat->mat_index].power / 2.f);
					}

					//ポリゴン描画処理
					{
						//色
						glColor4f(model->mat.first[mat->mat_index].col.c.r / 255.f, model->mat.first[mat->mat_index].col.c.g / 255.f, model->mat.first[mat->mat_index].col.c.b / 255.f
							, model->mat.first[mat->mat_index].col.c.a / 255.f);

						//描画
						/*if (mat->use_tex) {
							glMultiTexCoordPointerEXT(GL_TEXTURE0, 2, GL_FLOAT, sizeof(vector2<T>), &mat->uv_point[0][0]);
							glTexCoordPointer(2, GL_FLOAT, sizeof(vector2<T>), &mat->uv_point[0][0]);

							glEnableClientState(GL_TEXTURE_COORD_ARRAY);
							glActiveTexture(GL_TEXTURE0);
							glBindTexture(GL_TEXTURE_2D, mat->texture_id);
							glEnable(GL_TEXTURE_2D);
							glDrawElements(GL_TRIANGLES, mat->vtx_index.second, GL_UNSIGNED_SHORT, mat->vtx_index.first.get());
							glDisable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D, 0);
							glDisableClientState(GL_TEXTURE_COORD_ARRAY);
						}
						else */glDrawElements(GL_TRIANGLES, mat->vtx_index.second, GL_UNSIGNED_SHORT, mat->vtx_index.first.get());
					}
				}
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_NORMAL_ARRAY);

			}

			glPopMatrix();
			glDisable(GL_BLEND);
		}

		//ワイヤーフレームの描画
		template <class T>
		inline void draw_wire_frame(m3d::model_object<T>* model, const normal_color<T>& color) {
			if (model == nullptr) return;

			glPushMatrix();

			//アルファブレンドをする
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(color.r, color.g, color.b, color.a);

			//内部オブジェクトのループ
			for (auto obj = model->object.begin(); obj != model->object.end(); ++obj) {
				//不可視のときはスキップ
				if (!obj->visible) continue;

				for (auto mat = obj->mat.begin(); mat != obj->mat.end(); ++mat) {
					glEnableClientState(GL_VERTEX_ARRAY);
					//アルファブレンドをする
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

					glVertexPointer(3, GL_FLOAT, sizeof(m3d::vertex_data<T>), &mat->vertex[0].point[0]);
					for (imsize_t i = 0; i < mat->vertex.size(); i += 6)
						glDrawArrays(GL_LINE_LOOP, i, 6);

					glDisable(GL_BLEND);
					glDisableClientState(GL_VERTEX_ARRAY);
				}

			}
			glDisable(GL_BLEND);
			glPopMatrix();
		}
	}
}

//ライトの制御(OpenGLの仕様上8個は作ることができる)
namespace iml {
	namespace i_f {

		//ライト制御クラス(0<=N<=7)GL_MAX_LIGHTS
		template <class T, imsize_t N>
		class light :public singleton<light<T, N>> {
			//Tはdoubleかfloatでなければならない
			static_assert(is_same<T, double>::value || is_same<T, float>::value, "T must be float or double.");
			static_assert(N <= 7 && N >= 0, "N must be greater than or equal to 0 and less than or equal to 7.");

			friend class singleton<light<T, N>>;

			normal_color<T>		ambient;			//環境光
			normal_color<T>		diffuse;			//拡散光
			normal_color<T>		specular;			//鏡面光
			vector4<T>			position;			//位置

			T				const_attenuation = 1;			//定数減衰率
			T				liner_attenuation = 0;			//線形減衰率
			T				quadratic_attenuation = 0;		//二次減衰率

			vector3<T>		direction;				//スポットライトの方向
			T				exponent = 0;			//輝度分布定数
			T				cut_off = 180;			//最大放射角度(0~90か特別値180)

			constexpr light()
				:ambient(0, 0, 0, 1)
				, diffuse(1, 1, 1, 1)
				, specular(1, 1, 1, 1)
				, position(0, 0, 1, 0)
				, direction(0, 0, -1)
			{}
		public:
			~light() {}

			//ライト有効
			void enable() const { glEnable(GL_LIGHT0 + N); }
			//ライト無効
			void disable() const { glDisable(GL_LIGHT0 + N); }

			//情報のセット
			void set_ambient(const normal_color<T>& c) { ambient = c; glLightfv(GL_LIGHT0 + N, GL_AMBIENT, ambient.v); }
			void set_diffuse(const normal_color<T>& c) { diffuse = c; glLightfv(GL_LIGHT0 + N, GL_DIFFUSE, diffuse.v); }
			void set_specular(const normal_color<T>& c) { specular = c; glLightfv(GL_LIGHT0 + N, GL_SPECULAR, specular.v); }
			void set_position(const vector<T, 4>& p) { position = p; glLightfv(GL_LIGHT0 + N, GL_POSITION, position.begin()._Ptr()); }
			void set_const_attenuation(const T& a) { const_attenuation = a; glLightf(GL_LIGHT0 + N, GL_CONSTANT_ATTENUATION, const_attenuation); }
			void set_liner_attenuation(const T& a) { liner_attenuation = a; glLightf(GL_LIGHT0 + N, GL_LINEAR_ATTENUATION, liner_attenuation); }
			void set_quadratic_attenuation(const T& a) { quadratic_attenuation = a; glLightf(GL_LIGHT0 + N, GL_QUADRATIC_ATTENUATION, quadratic_attenuation); }
			void set_direction(const vector3<T>& d) { direction = d; glLightfv(GL_LIGHT0 + N, GL_SPOT_DIRECTION, direction.begin()._Ptr()); }
			void set_exponent(const T& e) { exponent = e; glLightf(GL_LIGHT0 + N, GL_SPOT_EXPONENT, exponent); }
			void set_cut_off(const T& co) { cut_off = co; glLightf(GL_LIGHT0 + N, GL_SPOT_CUTOFF, cut_off); }
			//情報の取得
			const normal_color<T>& get_ambient() const { return ambient; }
			const normal_color<T>& get_diffuse() const { return diffuse; }
			const normal_color<T>& get_specular() const { return specular; }
			const vector<T, 4>& get_position() const { return position; }
			T get_const_attenuation() const { return const_attenuation; }
			T get_liner_attenuation() const { return liner_attenuation; }
			T get_quadratic_attenuation() const { return quadratic_attenuation; }
			const vector3<T>& get_direction() const { return direction; }
			T get_exponent() const { return exponent; }
			T get_cut_off() const { return cut_off; }
		};

		//全体のライト有効
		inline void enable_light() { glEnable(GL_LIGHTING); }
		//全体のライト無効
		inline void disable_light() { glDisable(GL_LIGHTING); }


		//シャドウマッピングをするためのクラス(floatを用いるのはシェーダーを用いるため)
		class shadow_mapping :public camera<float> {
			render_frame_buffer_object	*render;
			program_object	program;				//影の計算のシェーダー

			matrix4<float>	tex_mat;				//テクスチャ変換行列
			float			shadow_ambient;			//影の環境光

		public:
			//w:横,h:縦,vtx_file:バーテクスシェーダーのファイル,flag_file:フラグメントシェーダーのファイル
			template <class CharT>
			shadow_mapping(imsize_t w, imsize_t h, const CharT* vtx_file, const CharT* frag_file)
				:render(new render_frame_buffer_object(w, h)), shadow_ambient(1) {
				//ファイルを読み込んでアタッチ/リンクする
				program.attach(shader(text_load<CharT>(vtx_file, true), shader::vertex)
					, shader(text_load<CharT>(frag_file, true), shader::fragment));
				program.link();
			}
			~shadow_mapping() { delete render; }

			//影構築(f:影情報を入れる描画関数)
			template <class F>
			void create_shadow(F f) {
				//ビューポートエリアを合わせる
				auto client = interface_control::inst()->get_client_area();
				interface_control::inst()->set_client_area(rect<iml::imint_t>(0, render->width(), render->height(), 0));

				//テクスチャ変換行列の構成
				tex_mat << 0.5, 0, 0, 0.5
					, 0, 0.5, 0, 0.5
					, 0, 0, 0.5, 0.5
					, 0, 0, 0, 1;
				tex_mat *= get_projection_matrix()*get_view_matrix();

				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();

				//カメラ位置の変更
				projection_update(this);
				view_update(this);

				//フレームバッファに対する描画の開始
				set_draw_framebuffer_object(render);
				glEnable(GL_DEPTH_TEST);
				glClearDepth(1);
				glClear(GL_DEPTH_BUFFER_BIT);
				//デプス値以外のレンダリングを無効化
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

				//Zファイティング防止
				glPolygonOffset(1.0f, 1.0f);
				glEnable(GL_POLYGON_OFFSET_FILL);

				//前面を破棄する
				//glEnable(GL_CULL_FACE);
				//glCullFace(GL_FRONT);

				f();

				//glDisable(GL_CULL_FACE);
				glDisable(GL_POLYGON_OFFSET_FILL);
				//フレームバッファに対する描画の終了
				iml::i_f::set_draw_framebuffer_object();
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				glDisable(GL_DEPTH_TEST);

				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();

				//ビュー行列を元に戻す
				interface_control::inst()->set_client_area(client);
			}
			//影の描画
			void start() {
				//カメラから見たとき
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_TEXTURE_2D);
				// デプステクスチャを貼り付け
				glActiveTexture(GL_TEXTURE0 + 7);
				glBindTexture(GL_TEXTURE_2D, render->handle());

				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				program.shader_start();
				program.set_variable("depth_tex", 7);
				program.set_variable("shadow_ambient", shadow_ambient);
				program.set_variable("tex_mat", tex_mat);
				glActiveTexture(GL_TEXTURE0);
			}
			void end() {
				program.shader_end();
				glDisable(GL_CULL_FACE);
				glActiveTexture(GL_TEXTURE0 + 7);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE0);

				glDisable(GL_TEXTURE_2D);
				glDisable(GL_DEPTH_TEST);
			}

			//影の環境光
			void set_shadow_ambient(float amb) { shadow_ambient = amb; }
			float get_shadow_ambient() const { return shadow_ambient; }

			imsize_t width() const { return render->width(); }
			imsize_t height() const { return render->height(); }
			render_frame_buffer_object* get_fbo() const { return render; }
		};
	}
}


#endif