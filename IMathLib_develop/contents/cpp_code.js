
//C++コードを色分けして置換
function cpp_code_replace() {
	//クラス名がcodeである者に対して全て
	var node = document.getElementsByClassName("cpp_code");
	//プリプロセッサ系
	var preprocessors = [
		"#include", "#define", "#undef", "#if", "#else", "#endif"
		, "#if defined", "#elif defined"
	];
	//基本型
	var types = [
		"unsigned", "signed", "char", "wchar_t", "short", "int", "long"
		, "__int8", "__int16", "__int32", "__int64"
		, "char16_t", "char32_t"
		, "float", "double", "void", "auto", "bool"
	];
	//識別子
	var idents = [
		"template", "class", "struct", "union", "inline", "using", "typename"
		, "static", "const", "switch", "case", "default", "break"
		, "for", "while", "do", "if", "else", "catch", "try", "throw"
		, "explicit", "constexpr", "typeid", "static_cast", "const_cast"
		, "dynamic_cast", "reinterpret_cast", "nullptr", "this", "return"
		, "public", "protected", "private", "friend", "namespace", "sizeof"
		, "true", "false", "static_assert", "noexcept", "typedef", "continue"
		, "delete[]", "delete", "new", "virtual", "volatile", "__cdecl"
		, "__clrcall", "__fastcall", "__stdcall", "__thiscall", "__vectorcall"
		, "decltype", "goto"
	];
	//<span>.*</span>タグでマッチしていない部分の判定をする
	for (var i = 0, k = node.length; i < k; ++i) {
		//HTMLの書き換え
		var str = node[i].innerHTML;
		//コメント
		str = str.replace(/(<span>((?!<\/span>).)*?<\/span>)|(\/\/.*)/g,
				function() { return (arguments[1]) ? arguments[1] : "<span>"+arguments[0].fontcolor("#088A4B")+"</span>"; }
			);
		str = str.replace(/(<span>((?!<\/span>).)*?<\/span>)|(\/\*([^*]|\*[^\/])*\*\/)/g,
				function() { return (arguments[1]) ? arguments[1] : "<span>"+arguments[0].fontcolor("#088A4B")+"</span>"; }
			);
		//リテラル文字列(\"はスキップ)
		str = str.replace(/(<span>((?!<\/span>).)*?<\/span>)|(L?\"(\\\"|[^\"])*\")/g,
				function() { return (arguments[1]) ? arguments[1] : "<span>"+arguments[0].fontcolor("#084B8A")+"</span>"; }
			);
		//リテラル文字(エスケープも考慮)
		str = str.replace(/(<span>((?!<\/span>).)*?<\/span>)|(L?\'(.|\\n|\\t|\\v|\\b|\\r|\\f|\\a|\\|\?|\\'|\\"|\0|\ooo|\xhhh)\')/g,
				function() { return (arguments[1]) ? arguments[1] : "<span>"+arguments[0].fontcolor("#084B8A")+"</span>"; }
			);
		//リテラル数字
		str = str.replace(/(<span>((?!<\/span>).)*?<\/span>)|(\b-?([1-9]\d*|0)(\.\d+)?\b)/g,
				function() { return (arguments[1]) ? arguments[1] : "<span>"+arguments[0].fontcolor("#868A08")+"</span>"; }
			);
		//プリプロセッサ系
		for(var j = 0; j < preprocessors.length; ++j)
			str = str.replace(new RegExp("(<span>((?!<\/span>).)*?<\/span>)|("+preprocessors[j]+"\\b)", 'g'),
					function() { return (arguments[1]) ? arguments[1] : "<span>"+arguments[0].fontcolor("#424242")+"</span>"; }
				);
		//基本型
		for(var j = 0; j < types.length; ++j)
			str = str.replace(new RegExp("(<span>((?!<\/span>).)*?<\/span>)|(\\b"+types[j]+"\\b)", 'g'),
					function() { return (arguments[1]) ? arguments[1] : "<span>"+arguments[0].fontcolor("#2E64FE")+"</span>"; }
				);
		//識別子
		for(var j = 0; j < idents.length; ++j)
			str = str.replace(new RegExp("(<span>((?!<\/span>).)*?<\/span>)|(\\b"+idents[j]+"\\b)", 'g'),
					function() { return (arguments[1]) ? arguments[1] : "<span>"+arguments[0].fontcolor("#0431B4").bold()+"</span>"; }
				);
		node[i].innerHTML = str;
	}
}

