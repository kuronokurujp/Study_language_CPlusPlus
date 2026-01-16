# sdl3の学習用プロジェクト

## 最終更新日時
- 2026/01/12

## 目的
- sdl3の機能把握

## 環境ツール一覧
- cmake
- vscode
- python
- vc2022

## 環境構築
1. sdl3関連のデータを取得
- git submoduleから取得できる
- 取得コマンド一覧
git submodule add https://github.com/libsdl-org/SDL_image
git submodule add https://github.com/libsdl-org/SDL_mixer
git submodule add https://github.com/libsdl-org/SDL_ttf
git submodule add https://github.com/libsdl-org/SDL

1. SDL_ttfをコンパイルするのに必要なfreetypeなどのライブラリをダウンロード
cd SDL_ttf/external
./download.sh

1. SDL_imageをコンパイルするのに必要なデータをダウンロード
cd SDL_image/external
./download.sh

1. SDL_mixerをコンパイルするのに必要なデータをダウンロード
cd SDL_mixer/external
./download.sh

1. プロジェクト全体をビルド
cmake -S . -B build

これでbuildディレクトリにslnファイルが作成されるのでvs2022で開く

## 参考サイト
- [udemny Cyber Monday SDL3 Action RPG in C++](https://www.udemy.com/course/cyber-monday-sdl3-action-rpg-in-c/)
    - udemyでSDL3用の講座
- [SDL3のサンプルプロジェクト](https://github.com/Ravbug/sdl3-sample/tree/main)
    - 最低限のアセットのみで構築方法のサンプルとなるプロジェクト
- [SDL3のチュートリアル](https://www.lazyfoo.net/tutorials/SDL3/index.php)

## 免責事項 
このソフトウェアの使用または使用不可によって、いかなる問題が生じた場合も、著作者はその責任を負いません。バージョンアップや不具合に対する対応の責任も負わないものとします。