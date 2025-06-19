# 変数が存在しないか空かをチェックし、エラーを出す関数.
function(common_check_variable var)
    if(NOT DEFINED ${var} OR "${${var}}" STREQUAL "")
        message(FATAL_ERROR "Variable '${var}' is not defined or is empty.")
    endif()
endfunction()
