/*
Mandlebrot set drawing with SDL
*/

#include <SDL/SDL.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#define ZOOM_ENABLED 1

#define WIDTH       500
#define HEIGHT      500
#define BPP         32
#define MANDELBROT_EVOLUTION (double)(4.)

#define MAX_PATH    256
#define OUTPUT_FILENAME "Mandelbrot.bmp"
#define OUTPUT_SEQUENTIAL_FILENAME "Mandelbrot%03d.bmp"
#define TRACE_FILENAME "results.txt"

double  MANDELBROT_XMIN,
        MANDELBROT_XMAX,
        MANDELBROT_YMIN,
        MANDELBROT_YMAX,
        MANDELBROT_PREC;

#define MAX_ITERATIONS 300
#define MAX_COLORS 100
#define WINDOWS_CAPTION "Mandlebrot"

Uint32  black   = 0x00000000,
        white   = 0x00FFFFFF, //Unused
        red     = 0x00FF0000, //Unused
        green   = 0x0000FF00, //Unused
        blue    = 0x000000FF,
        colors[MAX_COLORS];


static inline void putPixel(SDL_Surface * form, Uint16 x, Uint16 y, Uint32 color) {
    const Uint8 bpp = form->format->BytesPerPixel;
    register Uint32 * p = ((Uint8 *)form->pixels) + y * form->pitch + x * bpp;
    //if(bpp == 4)
    *p = color;
}

void lockAsNeeded(SDL_Surface * form) {
  	if(SDL_MUSTLOCK(form))
            SDL_LockSurface(form);
}

void redirectStdio(void) {
    freopen("CON", "w", stdout);
    freopen("CON", "r", stdin);
    freopen("CON", "w", stderr);
}

void buildColorTable(SDL_Surface* form, Uint32 colors[]) {
   for(int i = 0; i < 20; ++i) {
        colors[   i] = SDL_MapRGB(form->format, 127 + i*128./20., i*256./20.      , 0);
        colors[20+i] = SDL_MapRGB(form->format, 255             , 255 - i*9./2.   , 0);
        colors[40+i] = SDL_MapRGB(form->format, 255 - i*95./20. , 165 - i*133./20., i*12.);
        colors[60+i] = SDL_MapRGB(form->format, 160 - i*135./20., 32 - i*7./20.   , 240 - i*6.4);
        colors[80+i] = SDL_MapRGB(form->format,  25 + i*103./20., 25 - i*25./20.  , 112 - i*112./20.);
    }
}

void drawMandelbrotSet(SDL_Surface *form, int iterations) {
    for(Uint16 x = 0; x < WIDTH; ++x) {
        if(x % (WIDTH/100) == 0)
	    printf("\r%d %%", x/(WIDTH/100));
        for(Uint16 y = 0; y < HEIGHT; ++y) {
            register double  ca = x*(MANDELBROT_XMAX - MANDELBROT_XMIN)/WIDTH + MANDELBROT_XMIN,
                    cb = y*(MANDELBROT_YMAX - MANDELBROT_YMIN)/HEIGHT + MANDELBROT_YMIN,
                    za = 0,
                    zb = 0;
            register double tmpx, tmpy;
            register Uint32 i;
            for(i = 0; (za*za + zb*zb - MANDELBROT_EVOLUTION) < DBL_EPSILON && i < iterations; ++i) {
                tmpx = za; tmpy = zb;
                za = tmpx*tmpx - tmpy*tmpy;
                zb = tmpx*tmpy*2;
                za += ca;
                zb += cb;
            }
            if(i == iterations)
                putPixel(form, x, y, black);
            else {
            // Get the color by index
                putPixel(form, x, y, colors[MAX_COLORS*i/iterations]);
            }
        }
    }
    printf("\r100 %%\n"); // FIXME: ??!
}

SDL_Surface* initGraphics(void) {

    if(SDL_Init(SDL_INIT_VIDEO) < 0) return NULL;
    atexit(SDL_Quit);
    SDL_WM_SetCaption(WINDOWS_CAPTION, NULL);
    SDL_Surface *form = SDL_SetVideoMode(WIDTH, HEIGHT, BPP, SDL_DOUBLEBUF|SDL_HWSURFACE);

    return form;
}

static inline void clear(SDL_Surface * form, Uint32 color) {
     SDL_FillRect(form, NULL, color);
}

void initMandelbrotSetBounds(void) {
    MANDELBROT_YMAX = +1.2;
    MANDELBROT_YMIN = -1.2;
    MANDELBROT_XMIN = -2.1;
    MANDELBROT_XMAX = -2.1 + WIDTH*2.4/HEIGHT;
    MANDELBROT_PREC = 0.4;
}

int main(int argc, char **argv) {

    redirectStdio();

    clock_t renderStart, renderDuration;
    SDL_Surface* screen;

#if ZOOM_ENABLED == 1
    double tmpx1, tmpx2, tmpy1, tmpy2;
    SDL_Event ev;
    int done = 0;
#endif

    screen = initGraphics();
    if(screen == NULL) return EXIT_FAILURE;

    buildColorTable(screen, colors);
    clear(screen, black);

#if ZOOM_ENABLED == 1
    initMandelbrotSetBounds();

    while(!done) {

	lockAsNeeded(screen);
        printf("\tXMIN : %.10g\n\tXMAX : %.10g\n\tYMIN : %.10g\n\tYMAX : %.10g\n", \
	     MANDELBROT_XMIN, MANDELBROT_XMAX, MANDELBROT_YMIN, MANDELBROT_YMAX);
        renderStart = clock();
        drawMandelbrotSet(screen, MAX_ITERATIONS);
        renderDuration = clock() - renderStart;
        printf("Genere en %ld ms\n", renderDuration);
	lockAsNeeded(screen);
        SDL_Flip(screen);
        do {
            SDL_WaitEvent(&ev);
        } while(ev.type != SDL_KEYDOWN && ev.type != SDL_QUIT && ev.type != SDL_MOUSEBUTTONDOWN);
        if(ev.type == SDL_KEYDOWN || ev.type == SDL_QUIT)
            done = 1;
        else if(ev.type == SDL_MOUSEBUTTONDOWN) {
            if(ev.button.button == SDL_BUTTON_LEFT) {
                tmpx1 = ev.button.x*(MANDELBROT_XMAX - MANDELBROT_XMIN)/WIDTH + MANDELBROT_XMIN - MANDELBROT_PREC;
                tmpx2 = ev.button.x*(MANDELBROT_XMAX - MANDELBROT_XMIN)/WIDTH + MANDELBROT_XMIN + MANDELBROT_PREC;
                tmpy1 = ev.button.y*(MANDELBROT_YMAX - MANDELBROT_YMIN)/HEIGHT + MANDELBROT_YMIN - MANDELBROT_PREC;
                tmpy2 = ev.button.y*(MANDELBROT_YMAX - MANDELBROT_YMIN)/HEIGHT + MANDELBROT_YMIN + MANDELBROT_PREC;
                MANDELBROT_XMIN = tmpx1;
                MANDELBROT_XMAX = tmpx2;
                MANDELBROT_YMIN = tmpy1;
                MANDELBROT_YMAX = tmpy2;
                MANDELBROT_PREC /= MANDELBROT_EVOLUTION;
            }
            if(ev.button.button == SDL_BUTTON_RIGHT) {
		initMandelbrotSetBounds();
            }
        }
    }
    SDL_SaveBMP(screen, OUTPUT_FILENAME);
    FILE *f = fopen(TRACE_FILENAME, "w+");
    fprintf(f, "%.10g | %.10g | %.10g | %.10g | ", MANDELBROT_XMIN, MANDELBROT_XMAX, MANDELBROT_YMIN, MANDELBROT_YMAX);
    fclose(f);
#else
    clock_t runStart = clock();
    FILE *f = fopen(TRACE_FILENAME, "r");
    fscanf(f, "%lf | %lf | %lf | %lf | ", &MANDELBROT_XMIN, &MANDELBROT_XMAX, &MANDELBROT_YMIN, &MANDELBROT_YMAX);
    fclose(f);
    char imageSavePath[MAX_PATH+1];
    //FIXME: remove magic numbers
    int frame = 0;
    for(int iterations = 30; iterations <= 300; iterations += 2) {
	lockAsNeeded(screen);
        renderStart = clock();
        drawMandelbrotSet(screen, iterations);
        renderDuration = clock() - renderStart;
        printf("%u : Genere en %ld ms\n", iterations, renderDuration);
	lockAsNeeded(screen);
        sprintf(imageSavePath, OUTPUT_SEQUENTIAL_FILENAME, frame++);

        SDL_SaveBMP(screen, imageSavePath);
    }
    printf("\n\n\t\tProgramme execute en %u ms.", clock() - runStart);
#endif
    return EXIT_SUCCESS;
}
