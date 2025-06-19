# プロジェクト全体で利用する基本となる文字コードをUTF8にする
function(utf8_configure_encoding)
    add_compile_definitions(HE_CHARACTER_CODE_UTF8)
endfunction()

# ファイルが include() されたタイミングで自動実行
# cmakeファイルをインクルードするだけ
utf8_configure_encoding()
