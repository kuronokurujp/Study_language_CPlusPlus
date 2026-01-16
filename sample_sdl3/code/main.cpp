#include "SDL3/SDL.h"
#include "SDL3/SDL_render.h"

/// <summary>
/// SDL3のサンプル
/// </summary>
/// <returns></returns>
int main()
{
	// SDLの初期化
	if (SDL_Init(SDL_INIT_VIDEO) == false)
		return 1;

	const int windows_w = 640;
	const int windows_h = 480;

	// ウィンドウ作成
	SDL_Window* window_p = SDL_CreateWindow("Hello SDL", windows_w, windows_h, 0);
	if (window_p == NULL)
		return 1;

	// レンダリング作成
	SDL_Renderer* renderer_p = SDL_CreateRenderer(window_p, NULL);
	if (renderer_p == NULL)
		return 1;

	// 画面クリアする色を指定
	SDL_SetRenderDrawColor(renderer_p, 255, 255, 255, 255);
	// 画面を上記設定した色でクリア
	SDL_RenderClear(renderer_p);
	// 画面反映
	SDL_RenderPresent(renderer_p);

	SDL_Event ev;
	bool loop_flag = true;
	// 画面ループ
	while (loop_flag)
	{
		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_EVENT_QUIT)
			{
				loop_flag = false;
				break;
			}
		}
	}

	SDL_DestroyWindow(window_p);
	SDL_DestroyRenderer(renderer_p);

	return 0;
}