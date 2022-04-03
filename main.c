#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

#include <stdbool.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

const int width = 200;
const int height = 400;
const int unit = width / 10;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *surface;
SDL_Texture *texture;
SDL_Event event;

SDL_Rect screen;
SDL_Rect rect1;
SDL_Rect rect2;
SDL_Rect rect3;
SDL_Rect rect4;
SDL_Rect pixel_on;
SDL_Rect pixel_off;
SDL_Rect pixel_current;

bool run = true;

bool world[10][20] = { 0 };
bool gun[3][2] = { { 0, 1 }, { 1, 1 }, { 0, 1 } };

static int init(void)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS |
		     SDL_INIT_TIMER) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
			     "Couldn't initialize SDL: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	window = SDL_CreateWindow("Brick", SDL_WINDOWPOS_CENTERED,
				  SDL_WINDOWPOS_CENTERED, width, height,
				  SDL_WINDOW_ALLOW_HIGHDPI);

	if (window == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
			     "Couldn't create window: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	renderer = SDL_CreateRenderer(window, -1,
				      SDL_RENDERER_ACCELERATED |
					      SDL_RENDERER_PRESENTVSYNC);

	if (renderer == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
			     "Couldn't create renderer: %s", SDL_GetError());
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
	return 0;
}

static Uint32 color(bool state)
{
	if (state) {
		return SDL_MapRGB(surface->format, 191, 191, 191);
	} else {
		return SDL_MapRGB(surface->format, 31, 31, 31);
	}
}

static void cleanup(void)
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	/* SDL_Quit(); */
}

static void event_handler(void)
{
	SDL_PollEvent(&event);
	switch (event.type) {
	case SDL_QUIT:
		run = false;
		cleanup();
		return;
		break;
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE:
			run = false;
			cleanup();
			return;
			break;
		}
	}
}

static void render(void)
{
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 20; j++) {
			pixel_current.x = i * unit;
			pixel_current.y = j * unit;
			if (world[i][j] == 1) {
				SDL_RenderCopy(renderer, texture, &pixel_on,
					       &pixel_current);
			} else {
				SDL_RenderCopy(renderer, texture, &pixel_off,
					       &pixel_current);
			}
		}
	}

	SDL_RenderPresent(renderer);
	SDL_Delay(32);
	event_handler();
}

static void blank_world(void)
{
	memset(world, 0, sizeof(world));
}

static void render_frame(void)
{
	blank_world();
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 10; j++) {
			if ((i & j) % 2 == 0) {
				world[j][i] = true;
				render();
			} else {
				render();
			}
		}
	}

	blank_world();
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 10; j++) {
			if ((i ^ j) % 2 == 0) {
				world[j][i] = true;
				render();
			} else {
				render();
			}
		}
	}
}

int main(void)
{
	screen.w = width;
	screen.h = height;

	pixel_current.w = unit;
	pixel_current.h = unit;

	if (init() != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
			     "Error while initializing: %s", SDL_GetError());
		return 1;
	}

	SDL_SetRenderDrawColor(renderer, 191, 191, 191, 191);
	SDL_RenderClear(renderer);

	surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
	SDL_FillRect(surface, NULL, color(1));

	rect1.x = 0;
	rect1.y = 0;
	rect1.w = 10;
	rect1.h = 10;

	rect2.x = rect1.x + 1;
	rect2.y = rect1.y + 1;
	rect2.w = rect1.w - 2;
	rect2.h = rect1.h - 2;

	rect3.x = rect2.x + 1;
	rect3.y = rect2.y + 1;
	rect3.w = rect2.w - 2;
	rect3.h = rect2.h - 2;

	rect4.x = rect3.x + 1;
	rect4.y = rect3.y + 1;
	rect4.w = rect3.w - 2;
	rect4.h = rect3.h - 2;

	pixel_on.x = 0;
	pixel_on.y = 0;
	pixel_on.w = 10;
	pixel_on.h = 10;
	pixel_off.x = unit;
	pixel_off.y = 0;
	pixel_off.w = 10;
	pixel_off.h = 10;

	SDL_FillRect(surface, &rect1, color(1));
	SDL_FillRect(surface, &rect2, color(0));
	SDL_FillRect(surface, &rect3, color(1));
	SDL_FillRect(surface, &rect4, color(0));
	SDL_FillRect(surface, &pixel_off, color(1));

	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	while (run) {
		render_frame();
#ifdef __EMSCRIPTEN__
		emscripten_sleep(60);
#endif
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
