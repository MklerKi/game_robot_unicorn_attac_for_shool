#define _USE_MATH_DEFINES
#include<math.h>
#include<iostream>
#include<stdio.h>
#include<string.h>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define ALL_LIVES 5

struct Object {
	SDL_Surface* image;
	int x;
	int y;
	int width;
	int height;
};

// narysowanie napisu txt na powierzchni screen, zaczynaj�c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj�ca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt srodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
	};


// rysowanie linii o dlugosci l w pionie (gdy dx = 0, dy = 1) 
// badz poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};


// rysowanie prostokata o dlugosci bokow l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};

//funkcja, która odejmuje zycie i zwraca wszystko do początkowych pozycji
void babah(Object* fon, Object* star, Object* unicorn, Object* podloga, Object* podloga2, Object* sufit, int* lives, double* etiSpeed, int* tyme) {
	(*lives)--;
	*etiSpeed = 1;
	(*fon).x = 0;
	(*star).x = SCREEN_WIDTH;
	(*sufit).x = SCREEN_WIDTH;
	*tyme = 0;
	(*podloga).x = 0;
	(*podloga).y = SCREEN_HEIGHT / 2 + (*unicorn).height;
	(*podloga2).y = SCREEN_HEIGHT / 2 + (*unicorn).height;
	(*fon).y = -400;
}

// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	//wszystkie zmienne
	int t1, t2, quit, frames, rc, lives, mozliwosc_skoku, skok, ziemia, tyme, przOn, klawiatura, podloga1_On, odleglosc, czislo;
	double delta, worldTime, fpsTimer, fps, distance, etiSpeed, wysokosc_skoku;
	SDL_Event event;
	SDL_Surface *screen, *charset, *fon_im, *unicorn_im, *star_im, *podloga_im, *sufit;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;

	printf("wyjscie printfa trafia do tego okienka\n");
	printf("printf output goes here\n");

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	// tryb pelnoekranowy / fullscreen mode
	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer);
	//rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 154, 18, 179, 1); //a set violet color to ???

	SDL_SetWindowTitle(window, "Unicorn attac!");

	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                           SDL_TEXTUREACCESS_STREAMING,
	                           SCREEN_WIDTH, SCREEN_HEIGHT);


	// wylaczenie widocznosci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./images/cs8x8.bmp");
	if(charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(charset, true, 0x000000);
	//wczytujemy foto tła i robimy z niego objekt
	fon_im = SDL_LoadBMP("./images/fon3.bmp");
	Object fon = { fon_im, 0, -400, fon_im->w, fon_im->h };
	//to samo dla przeszkody
	star_im = SDL_LoadBMP("./images/zvizda.bmp");
	Object star = { star_im, 0, 0, 40, 40};
	//to samo dla jednorozca
	unicorn_im = SDL_LoadBMP("./images/unohorn.bmp");
	Object unicorn = { unicorn_im, 0, 0, unicorn_im->w, unicorn_im->h };
	//to samo dla podłogi i drugiej podlogi
	podloga_im = SDL_LoadBMP("./images/podloga.bmp");
	Object podloga = {podloga_im, 0, SCREEN_HEIGHT / 2 + unicorn.height, podloga_im->w, podloga_im->h };
	Object podloga2 = {podloga_im, 0, SCREEN_HEIGHT / 2 + unicorn.height, podloga_im->w, podloga_im->h };
	//to samo dla sufita
	sufit = SDL_LoadBMP("./images/sufit.bmp");
	Object wiszaca_pl = {sufit, SCREEN_WIDTH, SCREEN_HEIGHT / 2 + unicorn.height, sufit->w, sufit->h };

	if(unicorn_im == NULL) {
		printf("SDL_LoadBMP(unohorn.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	char text[128];
	//robimt kolory
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int rorzowy = SDL_MapRGB(screen->format, 0xF3, 0x4D, 0xB6);
	int violet = SDL_MapRGB(screen->format, 0x9C, 0x1F, 0xE9);
	int dark_violet = SDL_MapRGB(screen->format, 0x56, 0x19, 0x86);

	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	distance = 0;
	etiSpeed = 1;

	lives = ALL_LIVES;

	//jednorozec ma takie namiary zawsze
	unicorn.x = 30;
	unicorn.y = podloga.y - unicorn.height;

	mozliwosc_skoku = 2;
	skok = 0;
	wysokosc_skoku = 70;

	ziemia = 0;
	tyme = 0;

	star.x = SCREEN_WIDTH;
	star.y = podloga2.y - star.height;

	przOn = 0;
	klawiatura = 0;
	podloga1_On = 0;
	odleglosc = 190;
	czislo = 0;
	//dopóki mamy zycie
	while(lives != 0) {
		t2 = SDL_GetTicks();

		// w tym momencie t2-t1 to czas w milisekundach, jaki uplynal od ostatniego narysowania ekranu
		delta = (t2 - t1) * 0.001;
		t1 = t2;
		worldTime += delta;
		distance += etiSpeed * delta;
		fpsTimer += delta;
		if (fpsTimer > 0.5) {
			fps = frames * 2;
			frames = 0;
			fpsTimer -= 0.5;
		};

		//zwiekszamy szybkosc
		if (tyme > 4000 && etiSpeed < 5) {
			etiSpeed += 1;
			tyme = 0;
			//zwiekszamy odleglosc miedzy podlogami
			odleglosc += 80;
		}
		else {
			tyme++;
		}

		//skok
		if (skok == 1) {
			//jezeli mamy jeszcze odleglosc dla skoku
			if (wysokosc_skoku != 0) {
				if (fon.y > -1)
					wysokosc_skoku = 0;
				//dla skoka musimy zwiekszyc wszysko(bez jednorozca) po y
				fon.y += 1;
				podloga.y += 1;
				podloga2.y += 1;
				wysokosc_skoku -= 0.5;
			}
			//jerzeli jednorozec jest na maksymalnej wysokosci skoku, zmniejszamy y
			else if (wysokosc_skoku == 0) {
				//jezeli jest na powierzchni podlogi, zatrzymujemy skok
				if (unicorn.y == podloga.y - unicorn.height && unicorn.x < podloga.x + podloga.width) {
					//zwracamy mozliwosc skoku i wysokosc
					mozliwosc_skoku = 2;
					skok = 0;
					wysokosc_skoku = 70;
				}
				else {
					fon.y -= 1;
					podloga.y -= 1;
					podloga2.y -= 1;
				}
			}
		}

		//ruch tla(jerzeli doszedl do konca foto, zwraca x na początek foto)
		if (abs(fon.x) >= fon.width - SCREEN_WIDTH) {
			fon.x = 0;
		}
		//rysujemy tlo
		fon.x -= etiSpeed;
		DrawSurface(screen, fon.image, fon.x, fon.y);
		//unicorn
		DrawSurface(screen, unicorn.image, unicorn.x, unicorn.y);
		//podloga + ruch podlogi
		podloga.x -= etiSpeed;
		//pierwsza podłoga
		DrawSurface(screen, podloga.image, podloga.x, podloga.y);
		//jezeli druga podloga jest juz na ekranie, robimy ją pierwszej i rysujemy drugą
		if (podloga2.x + podloga2.width < SCREEN_WIDTH + 1) {
			//jezeli przeszkoda juz jest na ekranie
			if(star.x < SCREEN_WIDTH)
				podloga1_On = 1;
			podloga.x = podloga2.x;
			podloga2.x = podloga.x + podloga.width + odleglosc;
			podloga.y = podloga2.y;
			//zadajemy y dla drugiej podlogi
			srand(time(NULL));
			if (fon.y > -400 && fon.y < -100) {
				czislo = podloga.y + rand() % 80;
				podloga2.y = czislo;
			}
			else if (fon.y < -400 && fon.y > -700) {
				czislo = podloga.y - 80 + rand() % 40;
				podloga2.y = czislo;
			}
			else
				podloga2.y = podloga.y - 80 + rand() % 160;
		}
		//rysujemy 2 podłogę
		else {
			podloga2.x = podloga.x + podloga.width + odleglosc;
			DrawSurface(screen, podloga2.image, podloga2.x, podloga2.y);
		}
		
		//rysujemy gwiazdy + ruch gwiazd + sufit
		//jerzeli gwiazda i sufit juz znikli zwracamy je na pozycje początkowę
		if (star.x + wiszaca_pl.width < 0) {
			przOn = 0;
			podloga1_On = 0;
			star.x = SCREEN_WIDTH;
			wiszaca_pl.x = star.x;
		}//gwiazda wlaczona i jest na podłodze
		else if (przOn == 1 && 
			((star.x > podloga.x + 10 && star.x + star.width < podloga.x + podloga.width - 10) || 
				(star.x > podloga2.x + 10 && star.x + star.width < podloga2.x + podloga2.width + 10))) {
			//ruch gwiazdy i sufita
			star.x -= etiSpeed;
			wiszaca_pl.x = star.x;
			//jerzeli podloga 2 juz jest jako podloga 1
			//rysujemy gwiazge i sufit
			if (podloga1_On == 1) {
				star.y = podloga.y - star.height;
				wiszaca_pl.y = podloga.y - 330;
				DrawSurface(screen, star.image, star.x, star.y);
				DrawSurface(screen, wiszaca_pl.image, wiszaca_pl.x, wiszaca_pl.y);
			}
			//jerzeli podloga 2 jeszcze jest jako podloga 2
			//rysujemy gwiazge i sufit
			else if(podloga1_On == 0){
				star.y = podloga2.y - star.height;
				wiszaca_pl.y = podloga2.y - 330;
				DrawSurface(screen, star.image, star.x, star.y);
				DrawSurface(screen, wiszaca_pl.image, wiszaca_pl.x, wiszaca_pl.y);
			}

		}
		//kazde 500 pnk włączamy gwiazdę
		else if (abs(fon.x) % 500 == 0 && przOn == 0 && fon.x != 0) {
			przOn = 1;
			star.x = SCREEN_WIDTH;
		}

		//obsluga kolizji
		//dotkniecie do gwiazdy
		if ((unicorn.y + unicorn.height > star.y - 1) && 
			(((unicorn.x + unicorn.width < star.x + star.width + 1) && (unicorn.x + unicorn.width > star.x - 1)) ||
			((unicorn.x < star.x + star.width + 1) && (unicorn.x > star.x - 1)))) {
			babah(&fon, &star, &unicorn, &podloga, &podloga2, &wiszaca_pl, &lives, &etiSpeed, &tyme);
			SDL_Delay(1000);
		}
		//dotkniecie do sufita
		if (((unicorn.x > wiszaca_pl.x && unicorn.x < wiszaca_pl.x + wiszaca_pl.width) ||
			(unicorn.x + unicorn.width > wiszaca_pl.x && unicorn.x + unicorn.width < wiszaca_pl.x + wiszaca_pl.width)) &&
			((unicorn.y > wiszaca_pl.y && unicorn.y < wiszaca_pl.y + wiszaca_pl.height) ||
				(unicorn.y + unicorn.height > wiszaca_pl.y && unicorn.y + unicorn.height < wiszaca_pl.y + wiszaca_pl.height)))
		{
			babah(&fon, &star, &unicorn, &podloga, &podloga2, &wiszaca_pl, &lives, &etiSpeed, &tyme);
			SDL_Delay(1000);
		}
		//obsluga wypadniecia poza podloge
		if ((unicorn.y > podloga.y - unicorn.height || 
			unicorn.y == podloga.y - unicorn.height) && unicorn.x > podloga.x + podloga.width) {
			skok = 1;
			wysokosc_skoku = 0;
		}
		if (podloga.y < -50) {
			babah(&fon, &star, &unicorn, &podloga, &podloga2, &wiszaca_pl, &lives, &etiSpeed, &tyme);
			SDL_Delay(1000);
		}

		// tekst informacyjny / info text
		DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czarny, rorzowy);
		//            "template for the second project, elapsed time = %.1lf s  %.0lf frames / s"
		sprintf(text, "Lives: %d, Czas trwania = %.1lfs, fps: %.0lf klatek/s\n", lives, worldTime, fps);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
		//	      "Esc - exit, \030 - faster, \031 - slower"
		sprintf(text, "Esc - wyjscie, \030 - przyspieszenie, \031 - zwolnienie");
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
//		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		// obsluga zdarzen (o ile jakies zaszly)
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_ESCAPE) lives = 0;
					else if (event.key.keysym.sym == SDLK_UP) {
						if (klawiatura == 0) {
							//jerzeli jeszcze jest mozliwosc skoku
							if (mozliwosc_skoku > 0) {
								mozliwosc_skoku--;
								skok = 1;
								wysokosc_skoku = 70;
							}
						}
					}
					else if (event.key.keysym.sym == SDLK_RIGHT) {
						//zryw na 40 pnkt
						if (klawiatura == 0) {
							fon.x -= 100;
							mozliwosc_skoku = 2;
						}
					}
					//nowa rozrywka, przełączamy wszystko do początkowych pozycji
					else if (event.key.keysym.sym == SDLK_n) {
						babah(&fon, &star, &unicorn, &podloga, &podloga2, &wiszaca_pl, &lives, &etiSpeed, &tyme);
						przOn = 0;
						lives = ALL_LIVES;
						worldTime = 0;
						distance = 0;
						SDL_Delay(1000);
					}
					//to samo, co w UP
					else if (event.key.keysym.sym == SDLK_z) {
						if (klawiatura == 1) {
							if (mozliwosc_skoku > 0) {
								mozliwosc_skoku--;
								skok = 1;
								wysokosc_skoku = 70;
							}
						}
					}
					//to samo, co w RIGHT
					else if (event.key.keysym.sym == SDLK_x) {
						if (klawiatura == 1) {
							fon.x -= 40;
							mozliwosc_skoku = 2;
						}
					}
					//przełączenie klawiatury
					else if (event.key.keysym.sym == SDLK_b) {
						if (klawiatura == 1) {
							klawiatura = 0;
						}
						else if (klawiatura == 0) {
							klawiatura = 1;
						}
					}

					break;
				case SDL_QUIT:
					quit = 1;
					break;
			};
		};
		frames++;
	};
	//screen z wynikami(czas rozrywki)
	if (lives == 0) {
		SDL_FillRect(screen, NULL, czarny);

		// tekst informacyjny / info text
		DrawSurface(screen, fon.image, 0, 0);
		DrawRectangle(screen, 4, 30, SCREEN_WIDTH - 8, 50, czarny, rorzowy);
		//            "template for the second project, elapsed time = %.1lf s  %.0lf frames / s"
		sprintf(text, "Czas trwania = %.1lf s", worldTime);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 50, text, charset);

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		//		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		SDL_Delay(2000);
	}

	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
};
