

var path_array = document.URL.split("/").reverse();
var i;
for (i = 0; path_array[i] != "contents"; ++i);			//"contents"が事実上の最上位のディレクトリ
//現在のファイルの存在する絶対パス
var dir_path = path_array.slice(i).reverse().join("/");
dir_path += "/";

//ヘッダーパスのテーブル(単語区切りができないため別途で定義)
var header_path_table = [
	["&lt;IMathLib\\/IMathLib_config\\.hpp&gt;", "IMathLib_config.html"]
	
	, ["&lt;IMathLib\\/utility\\/utility\\.hpp&gt;", "utility/utility.html"]
	, ["&lt;IMathLib\\/utility\\/type_traits\\.hpp&gt;", "utility/type_traits.html"]
	, ["&lt;IMathLib\\/utility\\/algorithm\\.hpp&gt;", "utility/algorithm.html"]
	
	, ["&lt;IMathLib\\/container\\/container\\.hpp&gt;", "container/container.html"]
	, ["&lt;IMathLib\\/container\\/array\\.hpp&gt;", "container/array.html"]
	
	, ["&lt;IMathLib\\/string\\/string\\.hpp&gt;", "string/string.html"]
	
	, ["&lt;IMathLib\\/math\\/complex\\.hpp&gt;", "math/complex.html"]
	, ["&lt;IMathLib\\/math\\/dual_numbers\\.hpp&gt;", "math/dual_numbers.html"]
	, ["&lt;IMathLib\\/math\\/split_complex\\.hpp&gt;", "math/split_complex.html"]
	, ["&lt;IMathLib\\/math\\/quaternion\\.hpp&gt;", "math/quaternion.html"]
	, ["&lt;IMathLib\\/math\\/octonion\\.hpp&gt;", "math/octonion.html"]
	, ["&lt;IMathLib\\/math\\/vector\\.hpp&gt;", "math/vector.html"]
	, ["&lt;IMathLib\\/math\\/matrix\\.hpp&gt;", "math/matrix.html"]
	, ["&lt;IMathLib\\/math\\/bitset\\.hpp&gt;", "math/bitset.html"]
	
	, ["&lt;IMathLib\\/math\\/math\\.hpp&gt;", "math/math.html"]
	, ["&lt;IMathLib\\/math\\/differential_equation\\.hpp&gt;", "math/differential_equation.html"]
	
	, ["&lt;IMathLib\\/interface\\/interface\\.hpp&gt;", "interface/interface.html"]
];

//全ての要素のパスのテーブル(ヘッダごとに定義)
var path_table = [
	//<IMathLib/IMathLib_config.hpp>
	["iml::imint", "IMathLib_config.html#integral"]
	, ["iml::imsize_t", "IMathLib_config.html#integral"]
	, ["iml::ptrdiff_t", "IMathLib_config.html#integral"]
	//<IMathLib/utility/utility.hpp>
	, ["iml::object_type", "utility/utility/object_type.html"]
	, ["iml::placeholder", "utility/utility/placeholder.html"]
	, ["iml::hierarchy_type", "utility/utility/hierarchy_type.html"]
	, ["iml::move", "utility/utility/move.html"]
	, ["iml::forward", "utility/utility/forward.html"]
	, ["iml::swap", "utility/utility/swap.html"]
	, ["iml::this_return", "utility/utility/this_return.html"]
	//<IMathLib/utility/type_traits.hpp>
	, ["iml::type_comparison", "utility/type_traits/type_comparison.html"]
	
	
	//<IMathLib/utility/algorithm.hpp>
	, ["iml::copy_order", "utility/algorithm/copy_order.html"]
	, ["iml::copy_reverse_order", "utility/algorithm/copy_reverse_order.html"]
	, ["iml::copy_if", "utility/algorithm/copy_if.html"]
	, ["iml::fill", "utility/algorithm/fill.html"]
	, ["iml::sort", "utility/algorithm/sort.html"]
	, ["iml::find", "utility/algorithm/find.html"]
	, ["iml::rfind", "utility/algorithm/rfind.html"]
	, ["iml::find_if", "utility/algorithm/find_if.html"]
	, ["iml::rfind_if", "utility/algorithm/rfind_if.html"]
	, ["iml::for_each", "utility/algorithm/for_each.html"]
	
	//<IMathLib/string/string.hpp>
	, ["iml::string", "string/string/string.html"]
	, ["iml::cstring", "string/string/string.html"]
	, ["iml::wstring", "string/string/string.html"]
	
	
	//<IMathLib/math/math.hpp>
	, ["iml::mod", "math/math/mod.html"]
	, ["iml::abs", "math/math/abs.html"]
	, ["iml::conj", "math/math/conj.html"]
	, ["iml::isqrt", "math/math/isqrt.html"]
	, ["iml::sqrt", "math/math/sqrt.html"]
	, ["iml::pi", "math/math/pi.html"]
	, ["iml::lgamma", "math/math/lgamma.html"]
	, ["iml::gamma", "math/math/gamma.html"]
	, ["iml::gamma1", "math/math/gamma1.html"]
	, ["iml::gamma2", "math/math/gamma2.html"]
	, ["iml::digamma", "math/math/digamma.html"]
	, ["iml::euler_constant", "math/math/euler_constant.html"]
	, ["iml::beta", "math/math/beta.html"]
	, ["iml::beta_i", "math/math/beta_i.html"]
	, ["iml::dirichlet_eta", "math/math/dirichlet_eta.html"]
	, ["iml::riemann_zeta", "math/math/riemann_zeta.html"]
	, ["iml::laguerre_polynomials", "math/math/laguerre_polynomials.html"]
	, ["iml::associated_laguerre_polynomials", "math/math/associated_laguerre_polynomials.html"]
	
	//<IMathLib/math/differential_equation.hpp>
	, ["iml::ode::euler_method", "math/differential_equation/euler_method.html"]
	, ["iml::ode::runge_kutta_method", "math/differential_equation/runge_kutta_method.html"]
	, ["iml::ode::taylor_method", "math/differential_equation/taylor_method.html"]
	
	//<IMathLib/interface/interface.hpp>
	, ["iml::i_f::rect", "interface/interface/rect.html"]
];

//基本的なパス情報の記述(毎回調整するのは面倒なため)
function draw_path() {
	document.write("<script type=\"text/javascript\" src=\""+dir_path+"cpp_code.js\"></script>"
		+"<link rel=\"stylesheet\" href=\""+dir_path+"style.css\"　type=\"text/css\">");
}
//パス情報からフレームの描画(毎回調整するのは面倒なため)
function draw_frame() {
	document.write("<iframe src=\""
		+dir_path+"menu.html\" width=\"20%\" height=\"100%\" frameborder=\"0\">iframe対応ブラウザを使用してください。</iframe>");
}

//<span></span>以外の場所で<code></code>間のパスの自動生成
function create_path() {
	var tag = document.getElementsByTagName("code");
	
	for (var i = 0; i < tag.length; ++i) {
		var str = tag[i].innerHTML;
		//ヘッダーパスの置き換え
		for(var j = 0; j < header_path_table.length; ++j)
			str = str.replace(new RegExp("(<span>((?!<\/span>).)*?<\/span>)|("+header_path_table[j][0]+")", 'g'),
					function() { return (arguments[1]) ? arguments[1] :
					"<span><a href=\""+dir_path+header_path_table[j][1]+"\">"+arguments[0]+"</a></span>"; }
				);
		//関数等のパスの置き換え
		for(var j = 0; j < path_table.length; ++j)
			str = str.replace(new RegExp("(<span>((?!<\/span>).)*?<\/span>)|(\\b"+path_table[j][0]+"\\b)", 'g'),
					function() { return (arguments[1]) ? arguments[1] :
					"<span><a href=\""+dir_path+path_table[j][1]+"\">"+arguments[0]+"</a></span>"; }
				);
		
		//メンバの置き換え(メンバには名前空間を置かないため現在のページの要素に合わせて置き換える)
		
		
		tag[i].innerHTML = str;
	}
	
	//ヘッダのホームへのリンクの生成
	tag = document.getElementsByTagName("header");
	for (var i = 0; i < tag.length; ++i) {
		var str = tag[i].innerHTML;
		str = str.replace(/\[ホーム\]/g, "<a href=\""+dir_path+"top.html\">$&</a>");
		tag[i].innerHTML = str;
	}
}

