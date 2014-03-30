#include <SDL/SDL.h>
#include <float.h>
#include <math.h>
#include <time.h>

//#define ZOOM

#define WIDTH       500
#define HEIGHT      500
#define MANDELBROT_EVOLUTION (double)(4.)
#ifdef ZOOM
double  MANDELBROT_XMIN,
        MANDELBROT_XMAX,
        MANDELBROT_YMIN,
        MANDELBROT_YMAX,
        MANDELBROT_PREC = 0.4;
#else
double  MANDELBROT_XMIN,
        MANDELBROT_XMAX,
        MANDELBROT_YMIN,
        MANDELBROT_YMAX;
#endif // ZOOM
Uint32  black   = 0x00000000,
        white   = 0x00FFFFFF,
        red     = 0x00FF0000,
        green   = 0x0000FF00,
        blue    = 0x000000FF,
        colors[100],
        iterations = 100;

void putPixel(SDL_Surface * surface, Uint16 x, Uint16 y, Uint32 color) {
    Uint8 bpp = surface->format->BytesPerPixel;
    Uint8 * p = ((Uint8 *)surface->pixels) + y * surface->pitch + x * bpp;
    if(bpp == 4)
        *(Uint32 *)p = color;
}

void DessinerMandelbrot(SDL_Surface *srf) {
    for(int x = 0; x < WIDTH; ++x) {
        if(x % (WIDTH/100) == 0)
        printf("\r%d %%", x/(WIDTH/100));
        for(int y = 0; y < HEIGHT; ++y) {
            double  ca = x*(MANDELBROT_XMAX - MANDELBROT_XMIN)/WIDTH + MANDELBROT_XMIN,
                    cb = y*(MANDELBROT_YMAX - MANDELBROT_YMIN)/HEIGHT + MANDELBROT_YMIN,
                    za = 0,
                    zb = 0;
            double tmpx, tmpy;
            Uint32 i;
            for(i = 0; (za*za + zb*zb - 4.) < DBL_EPSILON && i < iterations; ++i) {
                tmpx = za; tmpy = zb;
                za = tmpx*tmpx - tmpy*tmpy;
                zb = 2*tmpx*tmpy;
                za += ca;
                zb += cb;
            }
            if(i == iterations)
                putPixel(srf, x, y, black);
            else
                putPixel(srf, x, y, colors[100*i/iterations]);
        }
    }
    printf("\r100 %%\n");
}

int main(int ac, char **av) {
freopen("CON", "w", stdout);
freopen("CON", "r", stdin);
freopen("CON", "w", stderr);
    clock_t t1, t2;
#ifdef ZOOM
    double tmpx1, tmpx2, tmpy1, tmpy2;
    SDL_Event ev;
    int loop = 1;
#endif
    if(SDL_Init(SDL_INIT_VIDEO) < 0) return EXIT_FAILURE;
    atexit(SDL_Quit);
    SDL_WM_SetCaption("Mandelbrot", NULL);
    SDL_Surface *ecran = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_DOUBLEBUF|SDL_HWSURFACE);
    if(ecran == NULL) return EXIT_FAILURE;
    for(int i = 0; i < 20; ++i) {
        colors[i] = SDL_MapRGB(ecran->format, 127 + i*128./20., i*256./20., 0);
        colors[20+i] = SDL_MapRGB(ecran->format, 255, 255 - i*9./2., 0);
        colors[40+i] = SDL_MapRGB(ecran->format, 255 - i*95./20., 165 - i*133./20., i*12.);
        colors[60+i] = SDL_MapRGB(ecran->format, 160 - i*135./20., 32 - i*7./20., 240 - i*6.4);
        colors[80+i] = SDL_MapRGB(ecran->format, 25 + i*103./20., 25 - i*25./20., 112 - i*112./20.);
    }
    SDL_FillRect(ecran, NULL, black);
#ifdef ZOOM
    MANDELBROT_YMAX = +1.2;
    MANDELBROT_YMIN = -1.2;
    MANDELBROT_XMIN = -2.1;
    MANDELBROT_XMAX = -2.1 + WIDTH*2.4/HEIGHT;
    while(loop == 1) {
        if(SDL_MUSTLOCK(ecran))
            SDL_LockSurface(ecran);
        printf("\tXMIN : %.10g\n\tXMAX : %.10g\n\tYMIN : %.10g\n\tYMAX : %.10g\n", MANDELBROT_XMIN, MANDELBROT_XMAX, MANDELBROT_YMIN, MANDELBROT_YMAX);
        t1 = clock();
        DessinerMandelbrot(ecran);
        t2 = clock() - t1;
        printf("Genere en %ld ms\n", t2);
        if(SDL_MUSTLOCK(ecran))
            SDL_UnlockSurface(ecran);
        SDL_Flip(ecran);
        do {
            SDL_WaitEvent(&ev);
        } while(ev.type != SDL_KEYDOWN && ev.type != SDL_QUIT && ev.type != SDL_MOUSEBUTTONDOWN);
        if(ev.type == SDL_KEYDOWN || ev.type == SDL_QUIT)
            loop = 0;
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
        }
    }
    SDL_SaveBMP(ecran, "Mandelbrot.bmp");
    FILE *f = fopen("results.txt", "w+");
    fprintf(f, "%.10g | %.10g | %.10g | %.10g | ", MANDELBROT_XMIN, MANDELBROT_XMAX, MANDELBROT_YMIN, MANDELBROT_YMAX);
    fclose(f);
#else
    clock_t t3 = clock();
    FILE *f = fopen("results.txt", "r");
    fscanf(f, "%lf | %lf | %lf | %lf | ", &MANDELBROT_XMIN, &MANDELBROT_XMAX, &MANDELBROT_YMIN, &MANDELBROT_YMAX);
    fclose(f);
    char path[20];
    for(iterations = 30; iterations <= 300; iterations += 2) {
        if(SDL_MUSTLOCK(ecran))
            SDL_LockSurface(ecran);
        t1 = clock();
        DessinerMandelbrot(ecran);
        t2 = clock() - t1;
        printf("%u : Genere en %ld ms\n", iterations, t2);
        if(SDL_MUSTLOCK(ecran))
            SDL_UnlockSurface(ecran);
        sprintf(path, "Mandelbrot%03d.bmp", iterations);
        SDL_SaveBMP(ecran, path);
    }
    printf("\n\n\t\tProgramme execute en %u ms !", clock() - t3);
#endif
    return EXIT_SUCCESS;
    (void)(ac);
    (void)(av);
}
