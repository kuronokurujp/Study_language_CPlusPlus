# ゲーム「SuperStar」開発プロジェクト(未完成)

## 免責事項

このソフトウェアの使用または使用不可によって、いかなる問題が生じた場合も、著作者はその責任を負いません。バージョンアップや不具合に対する対応の責任も負わないものとします。

-   update: 2024/12/05
-   version: 0.4.0

## ゲーム

| ジャンル             | 描画 |
| -------------------- | ---- |
| シューティングゲーム | 2D   |

## 開発環境

| OS      | バージョン |
| ------- | ---------- |
| Windows | 2022       |

| IDE              | バージョン |
| ---------------- | ---------- |
| VisualStdio 2022 | 17.8.6     |

| 言語 | バージョン |
| ---- | ---------- |
| C++  | 17         |

| ツール                                                                | バージョン | 説明                                               |
| --------------------------------------------------------------------- | ---------- | -------------------------------------------------- |
| [CMake](https://cmake.org/)                                           | 3.30.0     | プロジェクト構築に利用                             |
| [vcpkg](https://learn.microsoft.com/ja-jp/vcpkg/get_started/overview) | ---        | プロジェクトで利用する C/C++ライブラリの管理に利用 |

## 利用ライブラリ一覧

| Name                                                                      | Version | 用途                       |
| ------------------------------------------------------------------------- | ------- | -------------------------- |
| [gtest](https://github.com/google/googletest)                               | 1.17.0  | コードの単体テスト         |
| [SDL2](https://www.libsdl.org/index.php)                                  | 2.30.9  | プラットフォームライブラリ |
| [tomlplusplus](https://github.com/marzer/tomlplusplus?tab=readme-ov-file) | 3.4.0   | toml ファイル制御          |
| [simdjson](https://github.com/simdjson/simdjson)                          | 3.9.4   | json ファイル制御          |
| [pugixml](https://github.com/simdjson/simdjson)                           | 1.14    | xml ファイル制御           |
| [Lua](https://www.lua.org/lua-l.html)                                     | 5.4.7   | lua スクリプト活用         |

## 自作ツール一覧

| Name                                                                                           | Version | 説明                     |
| ---------------------------------------------------------------------------------------------- | ------- | ------------------------ |
| [GameDataMaker](https://github.com/kuronokurujp/Stduy_language_Python/tree/main/GameDataMaker) | v0.0.1  | ゲームのパラメータ生成   |
| [FontConveter](https://github.com/kuronokurujp/Stduy_language_Python/tree/main/FontConverter)  | v0.0.1  | ゲームで使うフォント生成 |

## 開発目的

-   C++で利用したゲーム開発経験を詰むため
-   エンジン開発経験を詰むため
-   最新技術を試す環境づくり

## 開発環境構築手順

1. window パッケージ管理ソフト[chocolate](https://chocolatey.org/)をインストール

    - 参考サイト
        - [【絶対わかる】Windows へ Chocolatey のインストール方法と使い方](https://www.geeklibrary.jp/counter-attack/chocolatey/)
        - [Chocolatey とは、Windows 用のパッケージ管理システムの概要と特徴](https://www.issoh.co.jp/tech/details/3520/)

1. EnvBuild.bat を実行

    - chocolate を使用して開発に必要なアプリをインストールする

    - 注意点

        - chocolate でソフトウェアをインストールするのでバッチファイルは管理者権限で実行してください

    - chocolate を使って以下のソフトウェアをインストール
        - CMake
        - Visual Studio 2022 Community
        - Git

## 構築した開発環境削除

1. EnvClean.bat を実行

    - 注意点
        - chocolate でインストールした以下のソフトウェアは継続利用される可能性があるのでアンインストールしません<br>
          必要がなければ申し訳ございませんが、<br>手動アンインストールをしてください。
            - CMake
            - Visual Studio 2022 Community
            - Git
    - chocolate でインストールしたソフトウェアをアンインストール方法の参考サイト
        - [Chocolatey とは、Windows 用のパッケージ管理システムの概要と特徴](https://www.issoh.co.jp/tech/details/3520/)

## VS2022 プロジェクト作成手順

1. ProjBuild.bat ファイルを実行
1. コマンドプロンプトに"All Complete"が表示したら Build ディレクトリが作られているので,<br>
   その中の SuperStar.sln ファイルがあるので開く

- 注意点
    - Visual Studio 2022 Communityを更新したば場合<br>
      ProjClear.batを実行
      - 理由
        - Visual Studioの古いバージョンでビルドしたのが残っていると更新したバージョンとの違いでProBuild.batが失敗する

## clang-format ツールで c++コード整形の環境構築手順

#### ツール環境構築

1. インストール

    - VisualStudio2022
        - インストール時に自動整形ツール clang-format.exe / clang-tidy.exe が入っているのでツールインストールは不要
        - clang-format.exe がある場所
            - [VisualStudio2022 のインストールフォルダ]\VC\Tools\Llvm\bin\clang-format.exe
        - clang-tidy.exe がある場所
            - [VisualStudio2022 のインストールフォルダ]\VC\Tools\Llvm\bin\clang-tidy.exe
    - CMake
        - VS2022 のプロジェクト作成に使う
        - [インストール先](https://cmake.org/download/)
        - 項目 **Windows x64 Installer:** のインストーラーをダウンロード
        - [参考となるインストール方法](https://qiita.com/matskeng/items/c466c4751e1352f97ce6)

#### コード整形内容をファイルに記述

1. プロジェクトのルートディレクトリにある.clang-format ファイルを編集
    - どういった編集をするのかは以下のサイトが参考になる
        - [公式ドキュメント](https://clang.llvm.org/docs/ClangFormatStyleOptions.html)
        - [Clang-Format の書式について](https://qiita.com/koyukitukimino/items/9b8f74d1ae307a7017d3)

#### どういった整形内容になっているか

-   ファイルの改行コードを LF に統一
-   インデントはスペース

## コード整形方法

-   手動

    ※注意点<br>
    サードパーティーのコードを整形してはいけない

    1.  VisualStdio2022 のエディターで整形したいファイルを開く
    1.  「編集」-> 「詳細」->「ドキュメントのフォーマット」をクリック<br>
        <img width="480" alt="" src="https://github.com/user-attachments/assets/b15b0975-066d-42bb-a886-c3fb318920d7" /><br>

-   バッチファイルで一括整形

    1. HobbyEngine/Tool/CodeCleanup.bat を実行

        - 例: CodeClenupSample ディレクトリ内の cpp/h/hpp ファイルを全て自動整形

            ```
            >CodeCleanup.bat CodeClenaupSample\
            ```

            末尾に\をつけないとディレクトリとして認識されないので注意

## 自動整形の非対象ディレクトリ

```
OSS のライブラリなどサードパーティーのコードは ThirdParty ディレクトリに収めて自動整形しないようにしている
```

#### 自動整形の非対象ディレクトリの作成手順

1. ThirdParty ディレクトリを作成
1. 自動整形したくないファイルを ThirdParty ディレクトリに入れる

#### 自動整形にしない理由

1. OSS ライセンスには以下のタイプがある

    - コピーレフト型(FSF,GPL)
    - 準コピーレフト型(MPL)

    ```
    上記タイプのOSSだとソース改変すると公開義務が発生
    自動整形してしまうとソース改変して公開義務が発生してしまう
    なので自動整形しないようにしてソース改変を防いでいる
    ```

2. 自動整形することでコンパイルが通らない可能性をつぶすため

## モジュール追加手順

1. Engine/Script/Generater/CreateModule/CreateModule.bat を実行
    - バッチファイル実行すると作成するモジュール名とモジュールをどこに作成するかのパス入力があります。<br>
      モジュールを作成する箇所は HobbyPlugin ディレクトリにしてください。
2. vcpkg を使う場合
    1. モジュールで vcpkg が管理しているライブラリを使う場合は.vcpkg.json にインストールするライブラリを記述してください。
        - [vcpkg が取り扱っているライブラリの検索サイト](https://vcpkg.link/)
        - [vcpkg.json の書き方](https://learn.microsoft.com/ja-jp/vcpkg/reference/vcpkg-json)
        - 注意点
            - "builtin-baseline"項目は記述しないでください。
    1. CMakeLists.txt に vcpkg でインストールしたパッケージを使えるように記述
        - CMakeLists.txt にはスケルトンコードがあるでそれを書き換える

## コーディング規約

### 目的

**コードを読むだけで処理内容を考えずに理解できるようにするため**

### 変数名

変数名を見るだけで変数型を素早く理解するために<br>
ハンガリアン記法を採用<br>
型の記号はわかりやすくかつ変数名を読むのを邪魔にならない程度<br>
独自定義した型名に HE::をつけるようにした<br>
これは SDL2 との型名との重複を避けるための対応<br>

-   変数名の先頭につける記号一覧
    |データ型|プレフィックス|説明|
    |----|----|----|
    |HE::Sint8|なし|使う機会がほとんどないから|
    |HE::UInt8|なし|使う機会がほとんどないから|
    |HE::Int|なし|使う機会がほとんどないから|
    |HE::Sint16|is||
    |HE::Uint16|us||
    |HE::Sint32|i||
    |HE::Uint32|u||
    |HE::Sint64|il||
    |HE::Uint64|ul||
    |HE::Float32|f||
    |HE::Float64|d||
    |HE::Bool|b||
    |HE::Char|c||
    |HE::Char[] or Char\* or std::string などの文字列|sz||
    |ポインタ|p|p の前後に型記号はいれない|
    |参照|r||
    |std::shared*ptr|sp||
    |std::unique_ptr*ptr|up||
    |配列 または配列機能を持つクラス|a||
    |std::vector または 同様の機能を持つクラス|v||
    |std::list または 同様の機能を持つクラス|lst||
    |std::map または 同様の機能を持つクラス|m||
    |std::stack または 同様の機能を持つクラス|s||
    |テンプレート型|T|頭に T をつけて後はキャメルケースで記述|
    |メソッド引数の入力|in\*||
    |値をメソッド出力引数|out||
    |クラスのメンバー変数|\_||
    |構造体のメンバー変数|なし||
    |const|なし|つける記号が増えると逆に見づらくなるので|
    |enum|e||

-   変数名の先頭につける記号のつけ方

    記号は型を付けた左から右の順につける

    -   例 1)
        ```
        // Uintなのでu
        // *でポインターだからp
        // なのでupの記号をつける
        // しかしポインタなのでpのみにする
        HE::Uint32* pCount;
        ```
    -   例 2)
        ```
            class Human
            {
            public:
                // メンバー変数なので_
                // Uint32型なのでuをつける
                HE::Uint32 _uCount = 0;
            };
        ```
    -   例 3)
        ```
        template(class TYPE)
        void Func()
        {
            TYPE _aValue[32];
        }
        ```

### const について

-   関数の戻り値の型が組み込み型の場合は const はつけない
    -   ※ポインタ型の場合は const をつける

### クラス

#### メソッド名

-   protected or private は外部公開ができない事を読んで判断できるようにするためにメソッド名の先頭に\_をつける

```
    class Object
    {
    private:
        HE::Uint32 _GetIndex() { return 0; }
    };
```

#### 仮想メソッドには V をつける

-   宣言・定義の両面で見ても仮想メソッドと一目でわかるようにするため

    -   宣言は override で見て一目でわかるが、定義では override が付けれないので一目でわからず考える時間が生まれたから

    例)

    ```
        class Object
        {
        public:
            virtual void VRun() {}
        };
    ```

#### クラスのオーバーライド

-   オーバーライドしたメソッドは基本クラスのメソッドも呼び出す

    ※ 基本クラスのメソッドの中身が空で呼ぶ必要がないケースもあるが,
    呼ぶか呼ばないかを調べるコストが高くなってしまう

-   クラスには仮想デストラクタを宣言

    ※ 基本クラス型のポインタを使って派生クラスのインスタンスを削除した場合,
    派生クラスのデストラクタを必ず呼ぶため

-   クラスを派生しないなら必ず final を使う

    例)

    ```
    class Object
    {
    public:
        virtual ~Object() = default;
        virtual void VProcess() {}
    };

    class ObjectV2 final : public Object
    {
    public:
        void VProcess() override final { Object::VProcess(); }
    };
    ```

#### クラス内でのメンバー変数の参照

-   クラス内のメンバー変数を参照する時は this をつける

    例)

    ```
    class Object
    {
    public:
        HE::Uint32 Count() const { this->_uCount; }
    private:
        HE::Uint32 _uCount;
    }
    ```

#### 強制デストラクタは絶対しない

-   言語仕様でクラスの強制デストラクタが可能

    例)

    ```
    class Object
    {
    public:
        HE::Uint32 Count() const { this->_uCount; }
    private:
        HE::Uint32 _uCount;
    }

    Object obj;
    obj.~Object();
    ```

しかしデストラクタはクラスの破棄タイミングで呼ばれるのが仕様なので、
任意で実行すると意図しないバグを起こすので使用してはいけない

### ファイル名 / ディレクトリ名

#### 大文字のキャメルケース

```
TaskManager.cpp
TaskManager.h
TaskManager.hpp
...
```

### マクロ

-   マクロ名は全て大文字
-   エンジン内のマクロ
    -   プレフィックスは HE\_
-   ゲーム実装内のマクロ
    -   プレフィックスはなし

### enum 名

-   enum 名のプレフィックスに E をつける
-   enum の要素の前に enum 名を付ける
    -   これは enum の名前のかぶりを避けるため
-   enum の要素名を参照する時は enum 名をつけない

    例)

    ```
    enum EState
    {
        EState_Active = 0,
    };

    // OK
    EState e = EState_Active;

    // NG
    EState e1 = EState::EState_Active;
    ```

### プラグインで使うサードパーティーのヘッダーファイルのインクルードは cpp ファイルのみにする

#### 目的

    サードパーティーの利用範囲をプラグイン内に限定するため

#### なぜ?

    ヘッダーファイルにインクルードしてかつそのヘッダーファイルが他プラグインでインクルードされると他プラグインでサードパーティーの機能が利用できてしまうから

## 注意点

-   git の無視ファイルに Debug ディレクトリを指定しているのでディレクトリ名に Debug を使ってはいけない

# 開発時のトラブルシューティング

1. char 型配列に入れた UTF8 エンコードの日本語文字列を VisualStudio2022 のデバッガーで確認したら文字列が文字化け
    - 以下のサイトの説明通り対応したら解決した
        - https://qiita.com/m_nai/items/645c7836db27f6d2b436
    - サイトの説明では VSCode 上での出来事だったが, VisualStudio2022 通用した

# グラフィックスのレギュレーション

## 座標系や原点位置の表
| 座標系                | 原点 |  X軸|    Y軸|   Z軸|補足|
| -------------------- | ---- |---- | ----  | ---- |---- |
| 2D | 画面の中央 (0, 0)   |左方向: - / 右方向: +|上方向: + / 下方向: -| --- |OpenGLを使うので右手座標系|
| 3D | 画面の中央 (0, 0)   |左方向: - / 右方向: +|上方向: + / 下方向: -|奥方向: - / 手前方向: +|OpenGLを使うので右手座標系|
| 数学ライブラリ | 画面の中央 (0, 0)   |左方向: - / 右方向: +|上方向: + / 下方向: -| 奥方向: - / 手前方向: +|矩形など座標計算は右手座標系で原点中央で行う. |

## 座標系や原点位置の命名規則
|             | 略語 |
| -------------------- | ---- |
| 右手座標系 | R |
| 原点中心 | C |


- 例) 右手座標系で原点中心の計算をする矩形クラス
    ```
    // Rは右手座標系 / Cは原点中央での計算
    namespace RC
    {
        class Rect {};
    }
    ```


# 参考サイト

## chocolate(windows のパッケージ管理ツール)

-   [【絶対わかる】Windows へ Chocolatey のインストール方法と使い方](https://www.geeklibrary.jp/counter-attack/chocolatey/)
-   [Chocolatey とは、Windows 用のパッケージ管理システムの概要と特徴](https://www.issoh.co.jp/tech/details/3520/)

## vcpkg

-   [vcpkg 公式ドキュメント](https://learn.microsoft.com/ja-jp/vcpkg/)
-   [vcpkg が取り扱っているライブラリの検索サイト](https://vcpkg.link/)
-   [vcpkg.json の書き方](https://learn.microsoft.com/ja-jp/vcpkg/reference/vcpkg-json)

## Git

-   [Git で管理しているソースの改行コード対応](https://zenn.dev/gekal/articles/git-line-endings-practices)

## CMake

-   [C/C++プロジェクトを CMake でビルドする](https://qiita.com/Hiroya_W/items/049bfb4c6ad3dfe6ff0c)
    -   CMake を使ってプロジェクトを構築方法をシンプルに記載している
    -   初心者におすすめ
-   [CMake 再入門メモ](https://zenn.dev/rjkuro/articles/054dab5b0e4f40)
    -   CMake の基本機能説明をしている
    -   初心者におすすめ
-   [CMake: キャッシュ変数と環境変数](https://qiita.com/mrk_21/items/186439952a6665184444)
    -   グローバル変数のようなもの
    -   複数の CMakeLists.txt から変数を参照できた
-   [Visual Studio 向けに使う CMake](https://hikita12312.hatenablog.com/entry/2018/09/13/101924)

    -   VS のプリコンパイル済みのヘッダーファイルを CMake で作成する方法

-   [Visual Studio ユーザーに送る CMake テクニック ☆ (1)](https://elda27.hatenablog.com/entry/2016/11/29/235301)
    -   VS2022 プロジェクトのフィルタリングを CMake で作成する方法
    -   プロジェクトのファイルを整理して見やすくするために必要

## Lua

-   [Lua5.4 リファレンスマニュアル(日本語版)](https://inzkyk.xyz/lua_5_4/#gsc.tab=0)

## 参考書籍
