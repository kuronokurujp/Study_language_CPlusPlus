> ⚠️ このリポジトリは現在開発中であり、内容は予告なく変更される場合があります。

# ゲーム「SuperStar」開発プロジェクト

## 免責事項

このソフトウェアの使用または使用不可によって、いかなる問題が生じた場合も、著作者はその責任を負いません。バージョンアップや不具合に対する対応の責任も負わないものとします。

## 更新

-   update: 2025/07/06
-   version: 0.4.0

## 開発目的

-   C++を用いたゲーム開発経験を積むため
-   自作エンジン開発の経験を積むため
-   最新技術を試す環境づくり

## 📘 目次

- [ゲーム概要](#ゲーム)
- [開発環境](#開発環境)
- [ライブラリ一覧](#利用ライブラリ一覧)
- [自作ツール](#自作ツール一覧)
- [環境構築](#開発環境構築)
- [コード整形](#コードフォーマットにclang-format-ツールを利用)
- [モジュール追加](#モジュール追加手順)
- [コーディング規約](#コーディング規約)
- [トラブルシューティング](#開発時のトラブルシューティング)
- [グラフィックスのレギュレーション](#グラフィックスのレギュレーション)
- [参考サイト](#参考サイト)

## ゲーム

- ジャンル：2Dシューティングゲーム
- 描画方式：OpenGLベースの2Dレンダリング

[↑ 目次へ戻る](#📘-目次)

## 開発環境

| OS      | バージョン |
| ------- | ---------- |
| Windows | 2022       |

| IDE              | バージョン |
| ---------------- | ---------- |
| Visual Studio 2022 | 17.8.6     |

| 言語 | バージョン |
| ---- | ---------- |
| C++  | 17         |

| ツール                                                                | バージョン | 説明                                               |
| --------------------------------------------------------------------- | ---------- | -------------------------------------------------- |
| [cmake](https://cmake.org/)                                           | 3.30.0     | プロジェクト構築に利用                             |
| [vcpkg](https://learn.microsoft.com/ja-jp/vcpkg/get_started/overview) | ---        | プロジェクトで利用する C/C++ライブラリの管理に利用 |

[↑ 目次へ戻る](#📘-目次)

## 利用ライブラリ一覧

| Name                                                                      | Version | 用途                       |
| ------------------------------------------------------------------------- | ------- | -------------------------- |
| [gtest](https://github.com/google/googletest)                               | 1.17.0  | コードの単体テスト         |
| [SDL2](https://www.libsdl.org/index.php)                                  | 2.30.9  | プラットフォームライブラリ |
| [tomlplusplus](https://github.com/marzer/tomlplusplus?tab=readme-ov-file) | 3.4.0   | toml ファイル制御          |
| [simdjson](https://github.com/simdjson/simdjson)                          | 3.9.4   | json ファイル制御          |
| [pugixml](https://github.com/simdjson/simdjson)                           | 1.14    | xml ファイル制御           |
| [Lua](https://www.lua.org/lua-l.html)                                     | 5.4.7   | lua スクリプト活用         |


[↑ 目次へ戻る](#📘-目次)

## 自作ツール一覧

| Name                                                                                           | Version | 説明                     |
| ---------------------------------------------------------------------------------------------- | ------- | ------------------------ |
| [GameDataMaker](https://github.com/kuronokurujp/Stduy_language_Python/tree/main/GameDataMaker) | v0.0.1  | ゲームのパラメータ生成   |
| [FontConverter](https://github.com/kuronokurujp/Stduy_language_Python/tree/main/FontConverter)  | v0.0.1  | ゲームで使うフォント生成 |

[↑ 目次へ戻る](#📘-目次)

## 開発環境構築

### 環境作成

1. window パッケージ管理ソフト[chocolate](https://chocolatey.org/)をインストール

    - 参考サイト
        - [【絶対わかる】Windows へ chocolatey のインストール方法と使い方](https://www.geeklibrary.jp/counter-attack/chocolatey/)
        - [chocolatey とは、Windows 用のパッケージ管理システムの概要と特徴](https://www.issoh.co.jp/tech/details/3520/)

1. EnvBuild.bat を実行

    - chocolate を使用して開発に必要なアプリをインストールする

    - 注意点

        - chocolate でソフトウェアをインストールするのでバッチファイルは管理者権限で実行してください

    - chocolate を使って以下のソフトウェアをインストール
        - cmake
        - Visual Studio 2022 Community
        - git

### 環境削除

1. EnvClean.bat を実行

    - 注意点
        - chocolate でインストールした以下のソフトウェアは継続利用される可能性があるのでアンインストールしません<br>
          必要がなければ申し訳ございませんが、<br>手動アンインストールをしてください。
            - cmake
            - Visual Studio 2022 Community
            - git
    - chocolate でインストールしたソフトウェアをアンインストール方法の参考サイト
        - [chocolatey とは、Windows 用のパッケージ管理システムの概要と特徴](https://www.issoh.co.jp/tech/details/3520/)

### VS2022 プロジェクト作成手順

1. **ProjBuild.bat** ファイルを実行
1. コマンドプロンプトに"All Complete"が表示したら Build ディレクトリが作られているので,<br>
   その中の SuperStar.sln ファイルがあるので開く

- 注意点
    - Visual Studio 2022 Communityを更新したば場合<br>
      ProjClear.batを実行
      - 理由
        - Visual Studioの古いバージョンでビルドしたのが残っていると更新したバージョンとの違いでProjBuild.batが失敗する

[↑ 目次へ戻る](#📘-目次)

## コードフォーマットにclang-format ツールを利用

### ツール環境構築

1. インストール

    - Visual Studio 2022
        - インストール時に自動フォーマットツール clang-format.exe / clang-tidy.exe が入っているのでツールインストールは不要
        - clang-format.exe がある場所
            - [Visual Studio 2022 のインストールフォルダ]\VC\Tools\Llvm\bin\clang-format.exe
        - clang-tidy.exe がある場所
            - [Visual Studio 2022 のインストールフォルダ]\VC\Tools\Llvm\bin\clang-tidy.exe
    - cmake
        - Visual Studio 2022 のプロジェクト作成に使う
        - [インストール先](https://cmake.org/download/)
        - 項目 **Windows x64 Installer:** のインストーラーをダウンロード
        - [参考となるインストール方法](https://qiita.com/matskeng/items/c466c4751e1352f97ce6)

1. 必要であればプロジェクトのルートディレクトリにある.clang-format ファイルを編集
    - どういった編集をするのかは以下のサイトが参考になる
        - [公式ドキュメント](https://clang.llvm.org/docs/ClangFormatStyleOptions.html)
        - [Clang-Format の書式について](https://qiita.com/koyukitukimino/items/9b8f74d1ae307a7017d3)

### clang-formatでのコードフォーマット方法

-   手動

    ※注意点<br>
    サードパーティーのコードをフォーマットしてはいけない

    1.  Visual Studio 2022 のエディターでフォーマットしたいファイルを開く
    1.  「編集」-> 「詳細」->「ドキュメントのフォーマット」をクリック<br>
        <img width="480" alt="" src="https://github.com/user-attachments/assets/b15b0975-066d-42bb-a886-c3fb318920d7" /><br>

-   バッチファイルで一括フォーマット

    1. HobbyEngine/Tool/CodeCleanup.bat を実行

        - 例: CodeCleanupSample ディレクトリ内の cpp/h/hpp ファイルを全て自動フォーマット

            ```
            >CodeCleanup.bat CodeCleanupSample\
            ```

            末尾に\をつけないとディレクトリとして認識されないので注意

### clang-formatでのフォーマットルール

-   ファイルの改行コードを LF に統一
-   インデントはスペース
-   OSS のライブラリなどサードパーティーのコードは ThirdParty ディレクトリに収めて自動フォーマットしないようにしている

### 自動フォーマットの非対象ディレクトリの作成手順

1. ThirdParty ディレクトリを作成
1. 自動フォーマットしたくないファイルを ThirdParty ディレクトリに入れる

#### 自動フォーマットにしない理由

1. OSS ライセンスには以下のタイプがある

    - コピーレフト型(FSF,GPL)
    - 準コピーレフト型(MPL)

    ```
    上記タイプのOSSだとソース改変すると公開義務が発生
    自動フォーマットしてしまうとソース改変して公開義務が発生してしまう
    なので自動フォーマットしないようにしてソース改変を防いでいる
    ```

2. 自動フォーマットすることでコンパイルが通らない可能性をつぶすため

[↑ 目次へ戻る](#📘-目次)

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
    1. cmakeLists.txt に vcpkg でインストールしたパッケージを使えるように記述
        - cmakeLists.txt にはスケルトンコードがあるでそれを書き換える

[↑ 目次へ戻る](#📘-目次)

## コーディング規約

### 目的

**コードを読むだけで処理内容を考えずに理解できるようにするため**

### HE::接頭語

開発の一貫した型表記と、標準型との混同を避けるため、<br>
基本的な型に対して **HE::** 名前空間を付与した型エイリアスの定義

- 利点:
    - SDL2 や標準C++ライブラリと名前と衝突対応
    - 型名を見ただけで、ゲーム固有かの判別
    - 型定義を切り替えても影響範囲を小さく

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
    |Core::Memory::SharedPtr|sp||
    |Core::Memory::UniquePtr|up||
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

しかしデストラクタはクラスの破棄タイミングで呼ばれるのが仕様なので、<br>
オブジェクトのライフサイクルを壊し、未定義動作の原因となるので使用してはいけない。

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

### 1つのcppファイルの全域で利用する変数や関数はLocalというnamespaceでくくる
#### なぜ?
    関数内のローカル変数 / static変数を見た目で区別するため

[↑ 目次へ戻る](#📘-目次)

## 開発時のトラブルシューティング

1. char 型配列に入れた UTF8 エンコードの日本語文字列を **Visual Studio 2022** のデバッガーで確認したら文字列が文字化け
    - 以下のサイトの説明通り対応したら解決した
        - https://qiita.com/m_nai/items/645c7836db27f6d2b436
    - サイトの説明では VSCode 上での出来事だったが, **Visual Studio 2022** 通用した

[↑ 目次へ戻る](#📘-目次)

## グラフィックスのレギュレーション

### 座標系や原点位置の表
| 座標系                | 原点 |  X軸|    Y軸|   Z軸|補足|
| -------------------- | ---- |---- | ----  | ---- |---- |
| 2D | 画面の中央 (0, 0)   |左方向: - / 右方向: +|上方向: + / 下方向: -| --- |OpenGLを使うので右手座標系|
| 3D | 画面の中央 (0, 0)   |左方向: - / 右方向: +|上方向: + / 下方向: -|奥方向: - / 手前方向: +|OpenGLを使うので右手座標系|
| 数学ライブラリ | 画面の中央 (0, 0)   |左方向: - / 右方向: +|上方向: + / 下方向: -| 奥方向: - / 手前方向: +|矩形など座標計算は右手座標系で原点中央で行う. |

### 座標系や原点位置の命名規則
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

[↑ 目次へ戻る](#📘-目次)

## 注意点

-   git の無視ファイルに Debug ディレクトリを指定しているのでディレクトリ名に Debug を使ってはいけない

[↑ 目次へ戻る](#📘-目次)

## 参考サイト

### chocolate(windows のパッケージ管理ツール)

-   [【絶対わかる】Windows へ chocolatey のインストール方法と使い方](https://www.geeklibrary.jp/counter-attack/chocolatey/)
-   [chocolatey とは、Windows 用のパッケージ管理システムの概要と特徴](https://www.issoh.co.jp/tech/details/3520/)

### vcpkg

-   [vcpkg 公式ドキュメント](https://learn.microsoft.com/ja-jp/vcpkg/)
-   [vcpkg が取り扱っているライブラリの検索サイト](https://vcpkg.link/)
-   [vcpkg.json の書き方](https://learn.microsoft.com/ja-jp/vcpkg/reference/vcpkg-json)

### git

-   [git で管理しているソースの改行コード対応](https://zenn.dev/gekal/articles/git-line-endings-practices)

### cmake

-   [C/C++プロジェクトを cmake でビルドする](https://qiita.com/Hiroya_W/items/049bfb4c6ad3dfe6ff0c)
    -   cmake を使ってプロジェクトを構築方法をシンプルに記載している
    -   初心者におすすめ
-   [cmake 再入門メモ](https://zenn.dev/rjkuro/articles/054dab5b0e4f40)
    -   cmake の基本機能説明をしている
    -   初心者におすすめ
-   [cmake: キャッシュ変数と環境変数](https://qiita.com/mrk_21/items/186439952a6665184444)
    -   グローバル変数のようなもの
    -   複数の cmakeLists.txt から変数を参照できた
-   [Visual Studio 向けに使う cmake](https://hikita12312.hatenablog.com/entry/2018/09/13/101924)

    -   VS のプリコンパイル済みのヘッダーファイルを cmake で作成する方法

-   [Visual Studio ユーザーに送る cmake テクニック ☆ (1)](https://elda27.hatenablog.com/entry/2016/11/29/235301)
    -   VS2022 プロジェクトのフィルタリングを cmake で作成する方法
    -   プロジェクトのファイルを整理して見やすくするために必要

### Lua

-   [Lua5.4 リファレンスマニュアル(日本語版)](https://inzkyk.xyz/lua_5_4/#gsc.tab=0)

[↑ 目次へ戻る](#📘-目次)

## 参考書籍

[↑ 目次へ戻る](#📘-目次)

