﻿#ifndef IMATH_IMATHLIB_HPP
#define IMATH_IMATHLIB_HPP

//これをインクルードすることで全ての機能を利用可能となる

#define IMATHLIB_VERSION

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <typeinfo>
#include <vector>

//ユーザー定義マクロ
#include "IMathLib/IMathLib_config.hpp"

#include "IMathLib/utility/template_algorithm.hpp"

//コンテナライブラリ
#include "IMathLib/container/array.hpp"
#include "IMathLib/container/list.hpp"
#include "IMathLib/container/map.hpp"
#include "IMathLib/container/stack.hpp"
#include "IMathLib/container/queue.hpp"

//文字列ライブラリ
#include "IMathLib/string/string.hpp"
#include "IMathLib/string/static_string.hpp"

//数学関数ライブラリ


//画像処理ライブラリ
/*#include "IMathLib/image/image.hpp"
#include "IMathLib/image/filter.hpp"
#include "IMathLib/image/synthesis.hpp"
*/
//暗号化ライブラリ
#include "IMathLib/encryption/lzss.hpp"


//数学関数
#include "IMathLib/math/math.hpp"						//数学関数
#include "IMathLib/math/extra_math.hpp"					//数学関数の拡張
//#include "IMathLib/math/complex_math.hpp"				//複素数の数学関数
#include "IMathLib/math/liner_algebra.hpp"				//線形代数演算
#include "IMathLib/math/hypercomplex.hpp"				//多元数
#include "IMathLib/math/automatic_differentiation.hpp"	//自動微分
#include "IMathLib/math/interpolation.hpp"				//補間・近似曲線
#include "IMathLib/math/fourier.hpp"					//フーリエ変換
#include "IMathLib/math/differential_equation.hpp"		//微分方程式の数値解法
#include "IMathLib/math/statistics.hpp"					//統計演算
#include "IMathLib/math/random.hpp"						//一様乱数生成

#include "IMathLib/math/automatic_differentiation.hpp"
#include "IMathLib/math/expression_template.hpp"

//3Dモデル制御
#include "IMathLib/3d/model.hpp"
#include "IMathLib/3d/metasequoia.hpp"

//OpenGLとSDL2が有効であるとき利用可能なインターフェース
#ifdef IMATHLIB_OPENGL_AND_SDL2
#include "IMathLib/interface/IMathLib_interface.hpp"
#endif


#endif