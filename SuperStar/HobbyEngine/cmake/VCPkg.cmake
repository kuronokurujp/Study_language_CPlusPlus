# 初回設定
function(vcpkg_configuration)

	message("configuration vcpkg")

	# vcpkgのルートディレクトリ
	set(VCPKG_ROOT "${CMAKE_SOURCE_DIR}/ThirdParty/vcpkg" PARENT_SCOPE)

	# 環境変数からvcpkgのパスを取得してツールチェインを設定
	set(TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" PARENT_SCOPE)

endfunction()

# VCPKGのライブラリをインストール
function(vcpkg_install_package install_dir vcpkg_triplet)

	message("install vcpkg")
	message(${VCPKG_ROOT})

	# すでにvcpkgがインストール済みかどうかチェック
	# statusファイルのパス
	# vcpkg.jsonで記述したパッケージは全てインストールしているという前提でのチェック
	# 特定のパッケージだけoptionalにした場合はチェック方法が変わるので注意
	set(STATUS_FILE "${install_dir}/ThirdParty/VcPkg/vcpkg/status")

	# statusファイルの存在を確認
	if(EXISTS "${STATUS_FILE}")
		message(STATUS "vcpkgパッケージはすでにインストールされています。")
		return()
	endif()

	# 一旦vcpkg関連のファイルを削除
	# これはvcpkgを入れなおしたら環境再構築するため
	if(EXISTS ${install_dir}/vcpkg-configuration.json)
		file(REMOVE ${install_dir}/vcpkg-configuration.json)
	endif()

	if(EXISTS ${install_dir}/vcpkg.json)
		file(REMOVE ${install_dir}/vcpkg.json)
	endif()

	# プラグインが用意した.vcpkg.jsonがあればvcpkg管理のライブラリをインストールする
	if(EXISTS ${install_dir}/.vcpkg.json)
		file(SIZE ${install_dir}/.vcpkg.json VCPKG_FILE_SIZE)
		if (VCPKG_FILE_SIZE GREATER 0)
			# vcpkgの環境を構築
			execute_process(
				COMMAND cmd /c "${VCPKG_ROOT}/vcpkg.exe" "--vcpkg-root=${VCPKG_ROOT}" new --application
				WORKING_DIRECTORY ${install_dir}
				OUTPUT_VARIABLE CMD_OUTPUT
				ERROR_VARIABLE CMD_ERROR
				RESULT_VARIABLE CMD_RESULT
			)

			# プラグインが用意した.vcpkg.jsonをvcpkg.jsonにコピー
			configure_file(${install_dir}/.vcpkg.json ${install_dir}/vcpkg.json COPYONLY)

			execute_process(
				COMMAND cmd /c "${VCPKG_ROOT}/vcpkg.exe" "--vcpkg-root=${VCPKG_ROOT}" install --clean-after-build "--x-manifest-root=${install_dir}" "--x-install-root=${install_dir}/ThirdParty/VcPkg" "--triplet=${vcpkg_triplet}"
				WORKING_DIRECTORY ${install_dir}
				OUTPUT_VARIABLE CMD_OUTPUT
				ERROR_VARIABLE CMD_ERROR
				RESULT_VARIABLE CMD_RESULT
			)

			message(STATUS "Command Output: ${CMD_OUTPUT}")
			message(STATUS "Command Error: ${CMD_ERROR}")
			message(STATUS "Command Result: ${CMD_RESULT}")

		endif()
	endif()

endfunction()

# includeで取り込んで初回実行
vcpkg_configuration()

