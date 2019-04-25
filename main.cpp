//Programmed by Karen and Nicolas

#include <SDL/SDL.h>
#include <SDL/fmod.h>
#include <SDL/SDL_ttf.h>                    //We include all the libraries we need
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define LF 1920
#define HF 1080
#define widthline 2
#define widthlineborder 4
#define widthborder 4


typedef struct                              //This structure stores the parameters choosen by the user
{
    unsigned short difficulty;           //0 is easy, 1 is nomal time limit, 2 is normal life limit, 3  is hard, 4 is hardcore

    unsigned short mode;                  //0 is random, 1 is image in the bank of image, 2 is custom

} Parameters;

typedef struct                              //This structure handle keyboard input
{
	bool key[SDLK_LAST];                    //The key pressed
	unsigned short mousex,mousey;           //The position of the mouse
	bool mousebuttons[2];                   //The mouse clicks
	unsigned short boxx;                    //The box x index on which the user clicked
	unsigned short boxy;                    //The box y index on which the user clicked

} Input;

typedef struct                              //This structure handle the text surface
{
    TTF_Font *font = NULL;                  //This will contain the font

    SDL_Surface *surface = NULL;            //This the surface we will blit

    SDL_Color couleur;                      //This the color of the text

    SDL_Rect posText;                       //This the position of the text

} Text;

typedef struct                              //This structure is the bidimensional array containing the image. The 1s are where the player need to click
{
    bool **tab;                             //This is our bidimensional array
    unsigned short number = 0;              //This is the number of one contained

}BiBoolArray;

typedef struct                              //This structure represents all the information about each image
{
    unsigned short **tab;
}ImageArray;

typedef struct                              //This will contain the text for the high score
{
    char text[40];

}BiCharArray;

typedef struct                              //This structure is the bidimensional array containing the info about the user board. 0 for nothing, 1 for correct, 2 for cross
{
    unsigned short **tab;                   //This is our bidimensional array
    unsigned short number = 0;              //This is the number of one contained

}BiShortArrayUser;

typedef struct                              //This structure is for the information about the pixel
{
    unsigned short r;
    unsigned short g;
    unsigned short b;

}pixel;

typedef struct                              //This structure is the tridimensional array containing the pixels and the color in rgb of the input image
{
    pixel **tab;                            //This is our bidimensional array
    unsigned short number = 0;              //This is the number of boxes you have to click on

}BiPixelArray;

typedef struct                              //This stucture is the bidimensional array containing the subsequence of each line
{
    unsigned short **tab;
    unsigned short maxSub = 0;              //This will contain the maximum of sub sequences to the print out array correctly
    unsigned short width;                   //This will contain the width of the array
    unsigned short height;                  //This will contain the width of the array
    double WB;                              //This will contain the width of the box
    double HB;                              //This will contain the height of the box

}BiShortArray;

typedef struct                              //This structure is for the lines is just contain a surface and a position
{
    SDL_Surface *surface;
    SDL_Rect pos;

}surfpos;

typedef struct                              //This structure contains all informations about the clock
{
    unsigned int start;                     //When it started

    unsigned int currentSec;                //The current second

    short duration;                         //The overall duration

    short timeLeft;                         //How much time there is left

    Text ticking;                           //The text we will use to print the time

} Clock;

typedef struct                              //This structure contains all informations about the life
{
    unsigned short totalLife;               //The total amount of life

    unsigned short lifeLeft;                //How much life there is left

    Text tHpLeft;                           //The text we will use to print the life

} Life;

typedef struct                      //This is the structure we use for our function that is being call back using timer and pointers of function
{
    SDL_Surface *screen;            //In in we need all the variable we will need cause we can only pass one parameter to our function so the only parameter we use is a structure with everything we need in it

    SDL_Surface *surface[10];       //Different glitch sprite

    SDL_Rect pos;                   //Their position

    FMOD_SYSTEM *system;            //The system variable we need to play the sound

    FMOD_SOUND *sGlitch[3];        //Different sound of glitch

}Glitcher;

typedef struct
{
    surfpos white;                      //This is for the white screen in case of win
    surfpos lines[2];                   //This is for the dark lines in case of lose

}AnimatorWinLose;

typedef struct                              //This structure contains all informations about the current game set
{
    Parameters para;                        //The parameter of mode and difficulty

    BiBoolArray board;                      //The image board

    BiShortArrayUser gamerBoard;                 //The gamer board

    BiShortArray upperArray;                //The upper array

    BiShortArray leftArray;                 //The left array

    BiPixelArray image;                      //The array with the pixel info

    unsigned short w;                       //The width chosen

    unsigned short h;                       //The height chosen

    Clock gameClock;                        //The time left to the player

    Life healthPoint;                       //The heal points left to the player

    bool lostwin;                           //If he won or lost

    SDL_TimerID timer;                      //The timer that we'll use to call back our glitch function

    Glitcher glitch;                        //The structure use to print and play the sound of glitches

    SDL_Surface *img;                       //This user image

    AnimatorWinLose winLoseAnim;            //This is for the win lose animation

    ImageArray boardImg;                    //This is in the case of the random image mode

}GameBoard;

typedef struct                              //This structure contains all the variable for our sounds in the menus
{
    FMOD_SYSTEM *system;
    FMOD_SOUND *sIn;
    FMOD_SOUND *sMenuMusic;                 //All the sounds we want to play and the system variable to play them
    FMOD_SOUND *sGameMusic;
    FMOD_SOUND *sGoodMusic;
    FMOD_SOUND *sBadMusic;
    FMOD_SOUND *sWinMusic;
    FMOD_SOUND *sLoseMusic;
    FMOD_SOUND *sAnimLoseMusic;
    FMOD_SOUND *sAnimWinMusic;
    FMOD_SOUND *sFace;

} soundBox;

typedef struct                              //This structure is used to fade in and out the menus with a dark surface
{
    SDL_Surface *blackScreen = SDL_CreateRGBSurface(SDL_HWSURFACE, 1920, 1080, 32, 0, 0, 0, 0);     //A dark rectangle of size of the screen

    SDL_Rect pos;                                                                                   //Its position

} fader;

typedef struct linkedHighScore;                 //This structure will contain all of our text document containing highscore (doesn't work)
struct linkedHighScore                          //We need to declare it like that cause we use the name inside
{
    char text[40] = "";
    linkedHighScore *next;

};

typedef struct                              //This structure will be used to manipulate our linked list (doesn't work)
{
    linkedHighScore *first;

} linkedList;

typedef struct                              //This structure is used to animate the sans in the top left corner
{
    SDL_Surface *surface[4];                //All the sprites

    SDL_Rect pos[4];                        //Its different position in function of the sprite to keep it always at the same pos

    Clock timerGood;                        //This timer for the animation of a good click

    Clock timerBad;                         //This timer for the animation of a bad click

    Clock timerSteady;                      //This timer for the animation of a good click

    unsigned short currSprite = 0;          //This is used to go from 1 sprite to another to create the animation

}Animator;


void UpdateEvents(Input* in)                //This is used to put all our events in our structure in
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_KEYDOWN:
			in->key[event.key.keysym.sym] = 1;
			break;                           //This is used to put the keyboard inputs in our structure
		case SDL_KEYUP:
			in->key[event.key.keysym.sym] = 0;
			break;
        case SDL_MOUSEBUTTONDOWN:           //This puts the click event in our structure
            if(event.button.button == SDL_BUTTON_LEFT)
                in->mousebuttons[0] = 1;
            else if(event.button.button == SDL_BUTTON_RIGHT)
                in->mousebuttons[1] = 1;
            break;
        case SDL_MOUSEBUTTONUP:             //This puts the click event in our structure
            if(event.button.button == SDL_BUTTON_LEFT)
                in->mousebuttons[0] = 0;
            else if(event.button.button == SDL_BUTTON_RIGHT)
                in->mousebuttons[1] = 0;
            break;
		default:
			break;
		}
	}
	in->mousex = event.button.x;            //This puts the mouse location in our structure
	in->mousey = event.button.y;
}

Uint32 startGlitch(Uint32 intervalle, void *para)               //This is the function that will create glitches in the menus
{
    Glitcher *glitch = (Glitcher*)para;                                                             //We need to cast our parameter into the structure type
    FMOD_System_PlaySound(glitch->system, FMOD_CHANNEL_FREE, glitch->sGlitch[rand()%3], 0, NULL);   //We play a random glitch sound
    SDL_BlitSurface(glitch->surface[rand()%10],NULL,glitch->screen,&glitch->pos);                   //We blit a random glitch image
    SDL_Flip(glitch->screen);
    SDL_Delay(100);
    SDL_BlitSurface(glitch->surface[rand()%10],NULL,glitch->screen,&glitch->pos);                   //We wait and blit another one
    SDL_Flip(glitch->screen);
    SDL_Delay(100);
    if(rand()%2)
        return intervalle - rand()%1000;                                                            //We use this to add or remove a bit of time randomly to the next call back
    else
        return intervalle + rand()%1000;
}

void initDrawing(unsigned short h, unsigned short w, BiBoolArray *tabl) //This is used to initialize our image bidimensional array with random 1s and 0s
{
    tabl->tab = (bool **)malloc(h * sizeof(bool *));
    for (unsigned short i=0; i<h; i++)
         tabl->tab[i] = (bool *)malloc(w * sizeof(bool));           //We use bool type + dynamic allocation to optimize the memory at max
    for(unsigned short i = 0; i < h; i++)
    {
        for(unsigned short j = 0; j < w; j++)
        {
            tabl->tab[i][j] = rand() % 2;
            if(tabl->tab[i][j])
                tabl->number += 1;  //If there is a 1 we increamte
        }
    }
}

void initImageArrayUS(GameBoard *mB)                                         //This is used to initialize the image board
{
    mB->boardImg.tab = (unsigned short**)malloc(mB->h * sizeof(unsigned short*));
    for (unsigned short i=0; i<mB->h; i++)
         mB->boardImg.tab[i] = (unsigned short*)malloc(mB->w * sizeof(unsigned short));           //We use bool type + dynamic allocation to optimize the memory at max
}

void initImageArrayB(GameBoard *mB)                                         //This is used to initialize the image board
{
    mB->board.tab = (bool **)malloc(mB->h * sizeof(bool*));
    for (unsigned short i=0; i<mB->h; i++)
         mB->board.tab[i] = (bool*)malloc(mB->w * sizeof(bool));           //We use bool type + dynamic allocation to optimize the memory at max
}

void initRandDrawing(unsigned short h, unsigned short w, BiBoolArray *tabl) //This is used to reinitialize our array for the hardcore mode
{
    for(unsigned short i = 0; i < h; i++)
    {
        for(unsigned short j = 0; j < w; j++)
        {
            tabl->tab[i][j] = rand() % 2;
            if(tabl->tab[i][j])
                tabl->number += 1;  //If there is a 1 we increamte
        }
    }
}

void initArrays(unsigned short h, unsigned short w, BiShortArray *tablSizeLine, BiShortArray *tablSizeRow) //This is used to initialise our subsequences counting bidimensional arrays
{
    unsigned short maxWidth = (w+1)/2, maxHeight = (h+1)/2;
    tablSizeLine->tab = (unsigned short **)calloc(h, sizeof(unsigned short *));
    for (unsigned short i=0; i<h; i++)
        tablSizeLine->tab[i] = (unsigned short *)calloc(maxWidth, sizeof(unsigned short));
    tablSizeRow->tab = (unsigned short **)calloc(maxHeight, sizeof(unsigned short *));
    for (unsigned short i=0; i<maxHeight; i++)                                                      //We use short type + dynamic allocation to optimize the memory at max
        tablSizeRow->tab[i] = (unsigned short*)calloc(w, sizeof(unsigned short));
}

void initUserArray(unsigned short h, unsigned short w, BiShortArrayUser *tabl) //This is used to initialise our player board bidimensional array
{
    tabl->tab = (unsigned short **)calloc(h, sizeof(unsigned short *));
    for (unsigned short i=0; i<h; i++)
         tabl->tab[i] = (unsigned short *)calloc(w, sizeof(unsigned short));           //We use bool type + dynamic allocation to optimize the memory at max
}

unsigned short nb_sub_hz(unsigned short width, BiBoolArray *tabl, BiShortArray *tabsize, unsigned short h, unsigned short left_array_width) //This is to count the number of subsequences for the horizontal lines and put it in the left array
{
    unsigned short nb_seq = 0, I= left_array_width-1;    //This o is here to mark the moment we start having a subsequence
    for(short n = width-1;n >= 0; n--)
    {
        if(tabl->tab[h][n] == 1)
        {
            tabsize->tab[h][I] += 1;

        if((n!=0 )&& (tabl->tab[h][n] > tabl->tab[h][n-1]) )// here it will start y testing the first condition and since it is fullfilled until the last loop he will consider the second condition
        {
            nb_seq++;                                       //If we go from a 1 to a 0 to 0 and add 1 sequence to out counter
            I--;
        }
       if (n==0)
            nb_seq++;
        }
    }

    return nb_seq;
}

unsigned short nb_sub_vt(unsigned short height, BiBoolArray *tabl, BiShortArray *tablsize, unsigned short w, unsigned short upper_array_height)  //This is to count the number of subsequences for the vertical lines and put it in the upper array
{
    unsigned short nb_seq = 0, I=upper_array_height-1;
    for(short n = height-1; n >= 0; n--)
    {
        if(tabl->tab[n][w] == 1)
        {

            tablsize->tab[I][w] += 1;
        if( (n !=0 )&& (tabl->tab[n][w] > tabl->tab[n-1][w]) )        //This works the same as the other one just for the vertical part
        {
            nb_seq++;
            I--;
        }
        if (n==0) nb_seq++;
    }

    }
    return nb_seq;                                                  //We return the number of sequence found
}

bool checkRow(GameBoard *mB, unsigned short w)         //This is used to check if a row was completed
{
    for(unsigned short i = 0; i < mB->h; i++)
    {
        if(mB->board.tab[i][w] != mB->gamerBoard.tab[i][w])         //We go through the arrays, if they match the user did check all the needed boxes
            return 0;
    }
    return 1;
}

bool checkLine(GameBoard *mB, unsigned short h)         //This is used to check if a line was completed
{
    for(unsigned short i = 0; i < mB->w; i++)
    {
        if(mB->board.tab[h][i] != mB->gamerBoard.tab[h][i])         //We go through the arrays, if they match the user did check all the needed boxes
            return 0;
    }
    return 1;
}

bool fadeInMenu(SDL_Surface *screen, short *n, fader *black)         //This is used to fade in and out a menu   (for now doesn't work)
{
    if(*n > 0)
        *n-=5;                                            //We can't make the entire animation here other wise it will block us into doing the things that are in the menu in which we are
                                                     //Instead we do step by step, calling the function every time and making the animation moving forward every time
    if(*n <= 0)                                       //By doing this we do not get "stuck" here waiting for the animation to finis
        return 1;                                       //This is not to go above 255
                                                        //When it's done fading we return 1 to stop calling the function in the menu

    SDL_SetAlpha(black->blackScreen, SDL_SRCALPHA | SDL_RLEACCEL, *n); //We change the opacity by 1 every time to make the black screen appear more and more transparent

    SDL_BlitSurface(black->blackScreen, NULL, screen, &black->pos);

    return 0;
}

void initGreyBGText(Text *t, const char *text, Sint16 x, Sint16 y, GameBoard *mB)    //This is used to initialize our text structure but with a gray background
{
    int fontsize = 32;                                                                  //This is our default font size which is good until a certain point
    while(fontsize * 1.25 >= mB->upperArray.HB || fontsize * 1.25 >= mB->leftArray.HB)  //If the height of the surface (font * 1.25) is too big for the box then we reduce the size until it's small enough
    {
        fontsize /= 1.2;
    }

    t->couleur = {255, 255, 255};
    t->font = TTF_OpenFont("century.ttf", fontsize);
    t->surface = TTF_RenderText_Blended(t->font, text, t->couleur);
    t->posText.x = x - t->surface->w/2;
    t->posText.y = y - t->surface->h/2;         //We use this to center them with the size they add themselve

}

void initText(Text *t, const char *text, Sint16 x, Sint16 y)    //This is used to initialize our text structure
{

    t->couleur = {255, 255, 255};
    t->font = TTF_OpenFont("century.ttf", 32);
    t->surface = TTF_RenderText_Shaded(t->font, text, t->couleur, {0,0,0});
    t->posText.x = x - t->surface->w/2;
    t->posText.y = y - t->surface->h/2;         //We use this to center them with the size they add themselve

}

void initStaticText(Text *t, const char *text, Sint16 x, Sint16 y)    //This is used to initialize our text structure but without the - t->surface->/2 arrangement
{

    t->couleur = {255, 255, 255};
    t->font = TTF_OpenFont("century.ttf", 32);
    t->surface = TTF_RenderText_Shaded(t->font, text, t->couleur, {0,0,0});
    t->posText.x = x;
    t->posText.y = y;

}

void initTextExitMenu(Text *ty, Text *tn, Text *tQExit)         //This is used to initialize texts in the exit menu
{
    initText(ty, "Yes",  LF/2 - LF/4,  HF/2 + HF/4);

    initText(tn, "No", LF/2 + LF/4, HF/2 + HF/4);

    initText(tQExit, "Are you sure you want to exit the game ?",  LF/2,  HF/2);
}

bool checkIfClicked(Text *t, Input *in)    //This is used to know if the user clicked the text
{
    if(in->mousebuttons[0])
    {
        if(in->mousex < t->posText.x + t->surface->w && in->mousex > t->posText.x && in->mousey > t->posText.y && in->mousey < t->posText.y + t->posText.h)
            return 1;                   //If the mouse is between the border of the box the it returns 1
    }
    return 0;
}

void printText(Text *intputText, SDL_Surface *screen)   //This is used to print text
{
    SDL_BlitSurface(intputText->surface, NULL, screen, &(intputText->posText));
}

void putTo0(GameBoard *mB)                  //This is used to put all the arrays to 0 for the hardcore mode to reset everything
{
    for(int y = 0; y < mB->h; y++)
    {
        for(int x = 0; x < mB->w; x++)
        {
            mB->gamerBoard.tab[y][x] = 0;
        }
    }

    for(int y = 0; y < (mB->h+1)/2; y++)
    {
        for(int x = 0; x < mB->w; x++)
        {
            mB->upperArray.tab[y][x] = 0;
        }
    }

    for(int y = 0; y < mB->h; y++)
    {
        for(int x = 0; x < (mB->w+1)/2; x++)
        {
            mB->leftArray.tab[y][x] = 0;
        }
    }
    mB->leftArray.maxSub = 0;
    mB->upperArray.maxSub = 0;
    mB->gamerBoard.number = 0;
    mB->board.number = 0;
}

void putTo0User(GameBoard *mB)                   //This is used to put the user array to 0 in case he wants to replay
{
    for(int y = 0; y < mB->h; y++)
    {
        for(int x = 0; x < mB->w; x++)
        {
            mB->gamerBoard.tab[y][x] = 0;       //Every box back to 0
        }
    }
    mB->gamerBoard.number = 0;                  //The number of correct click back to 0
}

void initBiggerText(Text *t, const char *text, Sint16 x, Sint16 y)    //This is used to initialize our text structure with a bigger font
{

    t->couleur = {255, 255, 255};
    t->font = TTF_OpenFont("century.ttf", 45);
    t->surface = TTF_RenderText_Shaded(t->font, text, t->couleur, {0,0,0});
    t->posText.x = x - t->surface->w/2;
    t->posText.y = y - t->surface->h/2;         //We use this to center them with the size they add themselve

}

void printFadeOut(Text *t, Text *tE, SDL_Surface *screen)           //This is used to fade between the menus
{
    for(short n = 255; n >= 0; n--)
    {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));  //This prints a blackscreen to clean the previous one
        SDL_SetAlpha(t->surface, SDL_SRCALPHA | SDL_RLEACCEL, n);
        SDL_BlitSurface(t->surface, NULL, screen, &t->posText);
        SDL_BlitSurface(tE->surface, NULL, screen, &tE->posText);
        SDL_Flip(screen);
        if(n<=255/2.5)
            SDL_Delay(1);
    }
}

void initAnimLose(GameBoard *mB, SDL_Surface *screen)               //This is used to initiate the variable for the win animation
{
    for(unsigned short i = 0; i < 2; i++)
    {
        mB->winLoseAnim.lines[i].surface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, LF/20, 10, 32, 0, 0, 0, 0);            //Just two black lines
        SDL_FillRect(mB->winLoseAnim.lines[i].surface, NULL, SDL_MapRGB(screen->format, 0,0,0));
    }
}

void animLose(GameBoard *mB, SDL_Surface *screen)                   //This is used to make the losing animation
{
    mB->winLoseAnim.lines[0].pos.x = LF - LF/20;                    //The two lines start on the left and right of the screen
    mB->winLoseAnim.lines[0].pos.x = 0;
    for(unsigned x = 0; x <= LF/2; x+=LF/20)                       //This for make the black lines come closer and closer to the center of the screen
    {
        for(unsigned short y = 0; y < HF; y+=10)                    //This for makes the black lines go down the screen
        {
            mB->winLoseAnim.lines[0].pos.y = y;
            mB->winLoseAnim.lines[1].pos.y = y;
            SDL_BlitSurface(mB->winLoseAnim.lines[0].surface, NULL, screen, &mB->winLoseAnim.lines[0].pos);
            SDL_BlitSurface(mB->winLoseAnim.lines[1].surface, NULL, screen, &mB->winLoseAnim.lines[1].pos);
            SDL_Flip(screen);
        }
        mB->winLoseAnim.lines[0].pos.x = x;
        mB->winLoseAnim.lines[1].pos.x = LF-LF/20 - x;
    }
}

void initAnimWin(surfpos *white)                                    //This is used to initiate the variable for the win animation
{
    white->surface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, 1920, 1800, 32, 0, 0, 0, 0); //Just a white screen
    white->pos.x = 0;
    white->pos.y = 0;
    SDL_FillRect(white->surface, NULL, SDL_MapRGB(white->surface->format, 255,255,255));
}

bool animWin(surfpos *white, SDL_Surface *screen, unsigned short *n)     //This is used to make the winning animation
{
    if(*n > 0)
        *n-=1;

    if(*n <= 0)
        return 1;

    SDL_SetAlpha(white->surface, SDL_SRCALPHA | SDL_RLEACCEL, *n);          //This works like the fadeinmenu() function but here it's white

    SDL_BlitSurface(white->surface, NULL, screen, &white->pos);

    return 0;
}

void animation321(SDL_Surface *screen)                              //This is the animation which will put 3 2 1 Fight on the screen
{
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));  //This prints a blackscreen to clean the previous one
    Text tNumber, tEscape;
    initBiggerText(&tNumber,"3", LF/2, HF/2);
    initBiggerText(&tEscape, "Pause : Esc", LF/2, HF/4);
    char num[5] = "";
    for(short i = 3; i > 0; i--)
    {
        sprintf(num, "%hi", i);
        tNumber.surface = TTF_RenderText_Shaded(tNumber.font, num, tNumber.couleur, {0,0,0});
        printFadeOut(&tNumber, &tEscape, screen);
    }
    initBiggerText(&tNumber, "Fight", LF/2, HF/2);                //We need to do this cause not same width as the numbers
    printFadeOut(&tNumber, &tEscape, screen);
}

void loadNumbers(GameBoard *mB)                                     //This is used to load the upper and left arrays with the number
{
    unsigned short cMaxSub = 0;
    for(unsigned short x = 0; x < mB->h; x++)
    {
        cMaxSub = nb_sub_hz(mB->w, &mB->board, &mB->leftArray, x, (mB->w+1)/2);
        if(mB->leftArray.maxSub < cMaxSub)                                                  //Here we store the max width of our left array
            mB->leftArray.maxSub = cMaxSub;
    }
    cMaxSub = 0;
    for(unsigned short y = 0; y < mB->w; y++)
    {
        cMaxSub = nb_sub_vt(mB->h, &mB->board, &mB->upperArray, y, (mB->h+1)/2);
        if(mB->upperArray.maxSub < cMaxSub)                                                 //Here we store the max height of our upper array
            mB->upperArray.maxSub = cMaxSub;
    }

}

void exitMenu(SDL_Surface *screen)                                  //This is the exit menu
{
    Input in;
    Text tYes, tNo, tQExit;
    initTextExitMenu(&tYes, &tNo, &tQExit);
    while(1)   //In this loop we can only go out by exiting or returning a value
    {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));  //This prints a blackscreen to clean the previous one
        UpdateEvents(&in);                                              //This handles input
        printText(&tYes, screen);
        printText(&tNo, screen);
        printText(&tQExit, screen);
        if(checkIfClicked(&tYes, &in))
            exit(-1);   //If the user wants to leave we exit
        else if(checkIfClicked(&tNo, &in))
            break;      //Else we just break the while and go back to the place before
        SDL_Flip(screen);
    }
}

void initTextReplayMenu(Text *tReplay, Text *tExit, Text *tMain, Text *tLose, Text *tWin)   //This is used to initialize the text of the replay menu
{
    initText(tReplay, "Play again", LF/2, HF/16*6);
    initText(tMain, "Main menu", LF/2, HF/16*10);
    initText(tExit, "Exit", LF/2, HF/16*15);
    initBiggerText(tLose, "You lost", LF/2, HF/16);
    initBiggerText(tWin, "You won", LF/2, HF/16);
}

void freeArrays(GameBoard *mB)                                          //Here we free everything
{
    for(unsigned short i = 0; i < mB->h; i++)
    {
        free(mB->board.tab[i]);
        free(mB->gamerBoard.tab[i]);
        free(mB->leftArray.tab);
    }
    for(unsigned short i = 0; i < (mB->h+1)/2; i++)
    {
        free(mB->upperArray.tab[i]);
    }
    free(mB->board.tab);
    free(mB->gamerBoard.tab);
    free(mB->leftArray.tab);
    free(mB->upperArray.tab);
    mB->board.tab = NULL;
    mB->leftArray.tab = NULL;
    mB->upperArray.tab = NULL;
    mB->gamerBoard.tab = NULL;
}

bool replayMenu(GameBoard *mB, SDL_Surface *screen, soundBox *mSB)        //This is the replay menu
{
    Input in;
    memset(&in,0,sizeof(in));   //Puts everything in our structure at 0

    bool f = 0;
    unsigned short fade = 255;

    Text tReplay, tExit, tMain, tLose, tWin;
    initTextReplayMenu(&tReplay, &tExit, &tMain, &tLose, &tWin);
    if(mB->lostwin)
        FMOD_System_PlaySound(mSB->system, FMOD_CHANNEL_FREE, mSB->sWinMusic, 0, NULL);         //We start the music accordingly to which one of the two
    else
        FMOD_System_PlaySound(mSB->system, FMOD_CHANNEL_FREE, mSB->sLoseMusic, 0, NULL);

    while(1)
    {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));  //This prints a blackscreen to clean the previous one
        printText(&tReplay, screen);
        printText(&tExit, screen);
        printText(&tMain, screen);
        if(!mB->lostwin)
            printText(&tLose, screen);                                  //If the player either lost or won we print accordingly
        else
            printText(&tWin, screen);
        UpdateEvents(&in);
        if(checkIfClicked(&tReplay, &in))                   //If we wants to replay
        {
            if(mB->lostwin)
                FMOD_Sound_Release(mSB->sWinMusic);
            else
                FMOD_Sound_Release(mSB->sLoseMusic);        //We remove the win or lose music that was playing

            if(!mB->lostwin)
                FMOD_System_CreateSound(mSB->system, "sLose.wav", FMOD_LOOP_NORMAL | FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, 0, &mSB->sLoseMusic); //We declare them again in case we need to replay them after
            else
                FMOD_System_CreateSound(mSB->system, "sWin.wav", FMOD_LOOP_NORMAL | FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, 0, &mSB->sWinMusic);

            FMOD_System_CreateSound(mSB->system, "gameMusic.wav", FMOD_LOOP_NORMAL | FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, 0, &mSB->sGameMusic);
            FMOD_Sound_SetLoopCount(mSB->sGameMusic, -1);                                                                                         //We replay the game music at the beginning
            FMOD_System_PlaySound(mSB->system, FMOD_CHANNEL_FREE, mSB->sGameMusic, 0, NULL);
            putTo0User(mB);                                     //This put the user board back to 0 before replaying
            initAnimLose(mB, screen);
            initAnimWin(&mB->winLoseAnim.white);                //We reinitialize the win and lose animations
            animation321(screen);
            SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));  //This prints a blackscreen to clean the previous one
            return 1;
        }
        else if(checkIfClicked(&tMain, &in))                //If he wants to go back ti the main menu
        {
            if(mB->lostwin)
                FMOD_Sound_Release(mSB->sWinMusic);
            else
                FMOD_Sound_Release(mSB->sLoseMusic);            //Here it's basicly the same reseting if he wanna go back to the main

            if(!mB->lostwin)
                FMOD_System_CreateSound(mSB->system, "sLose.wav", FMOD_LOOP_NORMAL | FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, 0, &mSB->sLoseMusic);
            else
                FMOD_System_CreateSound(mSB->system, "sWin.wav", FMOD_LOOP_NORMAL | FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, 0, &mSB->sWinMusic);

            FMOD_System_CreateSound(mSB->system, "menuMusic.wav", FMOD_LOOP_NORMAL | FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, 0, &mSB->sMenuMusic);
            FMOD_Sound_SetLoopCount(mSB->sMenuMusic, -1);                                                                                         //We stop the game music and put back the menu music
            FMOD_System_PlaySound(mSB->system, FMOD_CHANNEL_FREE, mSB->sMenuMusic, 0, NULL);
            mB->timer = SDL_AddTimer(6000,startGlitch, &mB->glitch);            //We put back the glitcher
            putTo0(mB);
            freeArrays(mB);
            return 0;
        }
        else if(checkIfClicked(&tExit, &in))
            exitMenu(screen);

        if(!f && mB->lostwin == 1)
            f = animWin(&mB->winLoseAnim.white, screen, &fade);         //This is for the win animation

        SDL_Flip(screen);
        SDL_Delay(1);
    }
}

void initTextHighScoreReadMenu(Text tHighScore[], BiCharArray tab[], Text *tContinue, Text *tHigh, unsigned short n)       //This is used to initialize the text of the read high score menu (for now doesn't work)
{
    initBiggerText(tHigh, "High score", LF/2, HF/16);

    for(unsigned short i = 0; i < n; i++)
    {
        initText(&tHighScore[i], tab[i].text, LF/8, HF/16*(i+1) + HF/8*2);
    }

    initText(tContinue, "Continue", LF/2, HF/16*15);           //Here we can't just do like usual cause we want a limited size*/
}

bool highScoreReadMenu(GameBoard *mB, SDL_Surface *screen, soundBox *mSB)     //This is the highscore read menu (doesn't work)
{
    Input in;
    memset(&in,0,sizeof(in));   //Puts everything in our structure at 0
    Text tHighScore[10], tContinue, tHigh;

    FILE* fhigh = NULL;
	fhigh = fopen("highscore.txt","r");             //We open the file
	if(!fhigh)
        exit(-1);

	BiCharArray tab[10];                    //This whill contain our highscore

	unsigned short n = fgetc(fhigh) - '0';        //We put the number of high score in this variable

    fseek(fhigh, 2, SEEK_CUR);                     //We place the cursor at the correct pos

	for(unsigned short i = 0; i < n; i++)
        fgets(tab[i].text, 40, fhigh);

    unsigned short carac = 0;

    for(unsigned short i = 0; i < n; i++)
    {
        while(tab[i].text[carac] != 10)
        {
            carac++;                                //This is used to remove the \n at the end of the line which, if not removed, appear as a square at the end
        }
        tab[i].text[carac] = 0;
        carac = 0;
    }

    initTextHighScoreReadMenu(tHighScore, tab, &tContinue, &tHigh, n);

    fclose(fhigh);

    while(1)
    {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));  //This prints a blackscreen to clean the previous one
        UpdateEvents(&in);                                              //This handles input

        for(unsigned short i = 0; i < 10; i++)
            printText(&tHighScore[i], screen);

        printText(&tContinue, screen);
        printText(&tHigh, screen);

        if(checkIfClicked(&tContinue, &in))
            if(replayMenu(mB, screen, mSB))
                return 1;

        SDL_Flip(screen);
    }
}

void initTextHighScoreWriteMenu(Text *tContinue, Text *tWrite, Text *tWin, Text *tEmpty)     //This is used to initialize the text of the write high score menu (for now doesn't work)
{
    initText(tContinue, "Press enter to continue", LF/2, HF/16*15);
    initText(tWrite, "Write your name here for the high score:", LF/4, HF/2);
    initBiggerText(tWin, "You won", LF/2, HF/16);
    initText(tEmpty, "You need to write at least one character", LF/2, HF/4);
}

void addNewLinkTexted(linkedHighScore *linker, FILE *f, unsigned short n)    //This is used to add a new link to our linked list and put the text inside it (doesn't work)
{
    linker->next = NULL;                                                     //We add this so that our last linker->next is effectively set to NULL
    if(n != 0)                                                               //This is our termination condition
    {
        linker->next = (linkedHighScore *)malloc(sizeof(*linker));          //We initialize the next link of the linker we just received
        fgets(linker->next->text, 40, f);                                   //We put the content of the line inside the text variable of our new link
        addNewLinkTexted(linker->next, f, n-1);                             //We make a recursive call sending our new link to be initialized
    }
}

bool scoreGreater(linkedHighScore *linker, unsigned short n, unsigned short *index)                //This is to know if the score inside the text portion is bigger than the player score (doesn't work)
{
    unsigned short nbr = 0;                                                            //This variable will contain the number inside the record
    char garbage[10] = "";                                                             //This will contain the text that we are not interested about
    sscanf(linker->text, "%s %s %hu %s", garbage, garbage, &nbr, garbage);             //This put the score inside our nbr variable (cause it of format Name - SCORE points)
    if(nbr > n)
        return 1;                                                                       //If the score is better we return 1
    return 0;
}

void playIn(soundBox *mSB)      //This is used to play the sound to go from one menu to another
{
    FMOD_System_PlaySound(mSB->system, FMOD_CHANNEL_FREE, mSB->sIn, 0, NULL);
}

void insertLinker(linkedHighScore *linker, unsigned short index, unsigned short n, char name[]) //This is used to insert the new text in the linked list (for now doesn't work)
{
    for(unsigned short a = 0; a < index; a++)
    {
        linker = linker->next;                                                          //We go through the linked list by going from next to next
    }
    linkedHighScore *newlinker = (linkedHighScore *)malloc(sizeof(*newlinker));
    sscanf(newlinker->text, "%s - %hu points", name, &n);
    newlinker->next = linker->next;
    linker->next = newlinker;
    if(n == 9)
    {
        for(unsigned short a = index; a < n; a++)
        {
            linker = linker->next;
        }
        linker->next = NULL;
    }
}

bool getKey(char *c, Input *in)                 //This is used in the high score menu to read the input text (for now doesn't work)
{
    SDL_Event event;
	while(SDL_PollEvent(&event))
	{
	    if(event.type == SDL_KEYDOWN)
		{
		    *c = event.key.keysym.sym;
            return 1;
		}
	}
    return 0;
}

void animDot(GameBoard *mB, SDL_Surface *screen, Input *in, surfpos *dot)      //This is used to put a white space if the user clicked on a correct box
{
    dot->pos.x = HF/16*5 + in->boxx*widthline + in->boxx*mB->upperArray.WB + widthlineborder;
    dot->pos.y = HF/16*5 + in->boxy*widthline + in->boxy*mB->leftArray.HB + widthlineborder;
    SDL_BlitSurface(dot->surface, NULL, screen, &dot->pos);

}

void animCustomColor(GameBoard *mB, SDL_Surface *screen, Input *in, surfpos *dot)      //This is used to put a white space if the user clicked on a correct box
{
    dot->pos.x = HF/16*5 + in->boxx*widthline + in->boxx*mB->upperArray.WB + widthlineborder;
    dot->pos.y = HF/16*5 + in->boxy*widthline + in->boxy*mB->leftArray.HB + widthlineborder;
    SDL_FillRect(dot->surface, NULL, SDL_MapRGB(screen->format, mB->image.tab[in->boxy][in->boxx].r, mB->image.tab[in->boxy][in->boxx].g, mB->image.tab[in->boxy][in->boxx].b));
    SDL_BlitSurface(dot->surface, NULL, screen, &dot->pos);

}

void animColor(GameBoard *mB, SDL_Surface *screen, Input *in, surfpos *dot, pixel p)      //This is used to put a white space if the user clicked on a correct box
{
    dot->pos.x = HF/16*5 + in->boxx*widthline + in->boxx*mB->upperArray.WB + widthlineborder;
    dot->pos.y = HF/16*5 + in->boxy*widthline + in->boxy*mB->leftArray.HB + widthlineborder;
    switch(mB->boardImg.tab[in->boxy][in->boxx])
    {
        case 1:
            p.r = 255;
            p.g = 255;
            p.b = 255;
            break;
        case 0:
            p.r = 0;
            p.g = 0;
            p.b = 0;
            break;
        case 2:
            p.r = 128;
            p.g = 128;
            p.b = 128;
            break;
        case 3 :
            p.r = 192;
            p.g = 192;
            p.b = 192;
            break;
        case 4:
            p.r = 218;
            p.g = 165;
            p.b = 32;
            break;
        case 5:
            p.r = 255;
            p.g = 220;
            p.b = 0;
            break;
        case 6:
            p.r = 255;
            p.g = 228;
            p.b = 196;
            break;
        case 7:
            p.r = 30;
            p.g = 144;
            p.b = 255;
            break;
        case 8:
            p.r = 160;
            p.g = 82;
            p.b = 45;
            break;
    }
    SDL_FillRect(dot->surface, NULL, SDL_MapRGB(screen->format, p.r, p.g, p.b));    //We fill the box with the corresponding color
    SDL_BlitSurface(dot->surface, NULL, screen, &dot->pos);

}

void createHline(unsigned short w, unsigned short x, unsigned short y, surfpos *l, SDL_Surface *screen) //This is used to create an horizontal line
{
    l->surface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, w, widthline, 32, 0, 0, 0, 0);                   //We create a line surface
    SDL_SetAlpha(l->surface, SDL_SRCALPHA | SDL_RLEACCEL, 128);
    SDL_FillRect(l->surface, NULL, SDL_MapRGB(screen->format, 255,255,255));
    l->pos.x = x;
    l->pos.y = y;
    SDL_BlitSurface(l->surface, NULL, screen, &(l->pos));                                       //We blit it
}

void createVline(unsigned short h, unsigned short x, unsigned short y, surfpos *l, SDL_Surface *screen) //This is used to create a vertical line
{
    l->surface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, widthline, h, 32, 0, 0, 0, 0);                   //We create a line surface
    SDL_SetAlpha(l->surface, SDL_SRCALPHA | SDL_RLEACCEL, 128);
    SDL_FillRect(l->surface, NULL, SDL_MapRGB(screen->format, 255,255,255));
    l->pos.x = x;
    l->pos.y = y;
    SDL_BlitSurface(l->surface, NULL, screen, &(l->pos));                                       //And blit it at the correct place
}

void createHborderline(unsigned short w, unsigned short x, unsigned short y, surfpos *l, SDL_Surface *screen) //This is used to create an horizontal line
{
    l->surface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, w, widthborder, 32, 0, 0, 0, 0);                   //We create a line surface
    SDL_SetAlpha(l->surface, SDL_SRCALPHA | SDL_RLEACCEL, 128);
    SDL_FillRect(l->surface, NULL, SDL_MapRGB(screen->format, 255,0,0));
    l->pos.x = x;
    l->pos.y = y;
    SDL_BlitSurface(l->surface, NULL, screen, &(l->pos));                                       //We blit it
}

void createVborderline(unsigned short h, unsigned short x, unsigned short y, surfpos *l, SDL_Surface *screen) //This is used to create a vertical line
{
    l->surface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, widthlineborder, h, 32, 0, 0, 0, 0);                   //We create a line surface
    SDL_SetAlpha(l->surface, SDL_SRCALPHA | SDL_RLEACCEL, 128);
    SDL_FillRect(l->surface,NULL, SDL_MapRGB(screen->format, 255,0,0));
    l->pos.x = x;
    l->pos.y = y;
    SDL_BlitSurface(l->surface, NULL, screen, &(l->pos));                                       //And blit it at the correct place
}

void createUpperArray(GameBoard *mB, surfpos *l, surfpos *rect, SDL_Surface *screen)           //This function is used to create the lines of the upper array
{
    mB->upperArray.HB = ((HF/16.0*5 - widthborder - widthline*(mB->upperArray.maxSub - 1))/mB->upperArray.maxSub);    //Calculation for the space between each horizontal line

    /*We remove for the height, the width of the two line already there, then the width of the line we are going to add and we finally divide it by the number of max sub*/

    mB->upperArray.WB = (((LF - HF/16.0*5) - 2*widthborder) - widthline*(mB->w - 1))/mB->w;                             //Calculation for the space between each vertical line

    //We do the same for the other lines

    //We had the .0 to calculate the value of the decimals

    rect->surface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, LF-HF/16*5,HF/16*5, 32, 0, 0, 0, 0);          //We went through complexe calculation with karen to find a way to print properly
    rect->pos.x = HF/16*5;
    rect->pos.y = 0;                                                                                    //Here it's the grey background between the two arrays
    SDL_SetAlpha(rect->surface, SDL_SRCALPHA | SDL_RLEACCEL, 128);
    SDL_FillRect(rect->surface, NULL, SDL_MapRGB(screen->format, 120,120,120));
    SDL_BlitSurface(rect->surface, NULL, screen, &rect->pos);


    createHborderline(LF-HF/16*5, HF/16*5, 0, l, screen);                                               //Here we create the first and last horizontal and vertical lines
    createVborderline(HF, HF/16*5, 0, l, screen);
    createVborderline(HF, LF - widthborder, 0, l, screen);

    for(int y = 1; y < mB->upperArray.maxSub; y++)
        createHline(LF-HF/16*5-2*widthborder, HF/16*5+widthborder, widthborder+y*mB->upperArray.HB + (y-1)*widthline, l, screen);
    for(int x = 1; x < mB->w; x++)
        createVline(HF-2*widthborder, widthborder + HF/16*5 + x*mB->upperArray.WB + (x-1)*widthline,widthborder, l, screen);
    SDL_Flip(screen);
}

void createLeftArray(GameBoard *mB, surfpos *l, surfpos *rect,SDL_Surface *screen)              //This is used to create the upper array (all the lines)
{
    mB->leftArray.WB = ((HF/16.0*5 - widthborder) - widthline*(mB->leftArray.maxSub - 1))/mB->leftArray.maxSub; //Calculation for the space between each vertical line


    //We remove for the full width the width of the two line already there then the width of the line we are going to add and we finally divide it by the number of max sub

    mB->leftArray.HB = (((HF-HF/16.0*5) - 2*widthborder) - widthline*(mB->h - 1))/mB->h;                          //Calculation for the space between each horizontal line

    //Here we do the same but for the horizontal lines

     //We had the .0 to calculate the value of the decimals

    rect->surface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, HF/16*5, HF-HF/16*5, 32, 0, 0, 0, 0);
    rect->pos.x = 0;
    rect->pos.y = HF/16*5;
    SDL_SetAlpha(rect->surface, SDL_SRCALPHA | SDL_RLEACCEL, 128);
    SDL_FillRect(rect->surface, NULL, SDL_MapRGB(screen->format, 120,120,120));
    SDL_BlitSurface(rect->surface, NULL, screen, &rect->pos);

    createVborderline(HF-HF/16*5, 0, HF/16*5, l, screen);                                                          //Here it's the first one

    createHborderline(LF, 0, HF/16*5, l, screen);
    createHborderline(LF, 0, HF - widthborder, l, screen);

    for(int y = 1; y < mB->h; y++)
        createHline(LF-2*widthborder, widthborder, widthborder + HF/16*5 + y*mB->leftArray.HB + (y-1)*(widthline), l, screen);           //We print all the lines
    for(int x = 1; x < mB->leftArray.maxSub; x++)
        createVline(HF-HF/16*5 - 2*widthborder, x*mB->leftArray.WB + widthborder+(x-1)*widthline + widthborder,  HF/16*5, l, screen);                   //We print all the lines



    SDL_Flip(screen);
}

void printLeftArray(GameBoard *mB, SDL_Surface *screen)            //This is used to print the left array
{
    Text number;
    char num[3] = "";
    unsigned short k=0;
    for(unsigned short int i = 0; i < mB->h; i++)
    {
        k= (mB->w+1) /2 - mB->leftArray.maxSub;
        for(unsigned short int j = 0; j < mB->leftArray.maxSub; j++)            //We go through our left array
        {
            if(mB->leftArray.tab[i][k] > 0)
            {

                sprintf(num, "%d", mB->leftArray.tab[i][k]);                        //We take the width/height of each box and take half of it and don't forget to take into consideration the width of the lines
                initGreyBGText(&number, num, widthborder + j*mB->leftArray.WB + mB->leftArray.WB/2 + widthline*j,  widthborder + HF/16*5 +i*mB->leftArray.HB +mB->leftArray.HB/2 + widthline*i, mB);
                printText(&number, screen);
                SDL_Flip(screen);

            }
            k++;
        }
    }
}

void printUpperArray(GameBoard *mB, SDL_Surface *screen)            //This is used to print the upper array
{
    Text number;
    char num[3] = "";
    unsigned short k=0;
    for(unsigned short int j = 0; j < mB->w; j++)
    { k= (mB->h+1)/2 -mB->upperArray.maxSub;
        for(unsigned short int i = 0; i < mB->upperArray.maxSub; i++)                           //Same as for printLeftArray
        {

            if(mB->upperArray.tab[k][j]>0){
            sprintf(num, "%d", mB->upperArray.tab[k][j]);
            initGreyBGText(&number, num,widthborder + HF/16*5 + j*mB->upperArray.WB +mB->upperArray.WB/2 + widthline*j, widthborder+ i*mB->upperArray.HB+ mB->upperArray.HB/2 + widthline*i, mB);
            printText(&number, screen);
            SDL_Flip(screen);

            } k++;
        }
    }
}

bool highScoreWriteMenu(GameBoard *mB, SDL_Surface *screen, soundBox *mSB)     //This is the highscore write menu (for now doesn't work)
{
    Input in;
    memset(&in,0,sizeof(in));   //Puts everything in our structure at 0

    Text tContinue, tWrite, tWon, tEmpty;

    initTextHighScoreWriteMenu(&tContinue, &tWrite, &tWon, &tEmpty);

    unsigned short i = 0;
    char name[10] = "";
    bool wrote = 0, error = 0;
    Text t;
    t.posText.x = tWrite.surface->w + LF/8;
    t.posText.y = HF/2 - tWrite.surface->h/2;
    while(!wrote)
    {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));  //This prints a blackscreen to clean the previous one
        if(getKey(&name[i], &in))                //We check if a key was pressed
        {
            if(name[i] == 8)                //If he presses delete
            {
                if(i > 0)                   //If there is a character to remove
                {
                    name[i] = NULL;         //We remove the delete character
                    name[i-1] = NULL;       //We remove the last character
                    initStaticText(&t, name, t.posText.x, t.posText.y); //We initialize the new text with a static position cause otherwise since the size of the surface is always changing it changed it's position every time
                    i--;                    //We decrement i by one to then be able to write in the area before
                }
            }
            else if(name[i] == 13)                  //We check if he typed on enter
            {
                name[i] = NULL;                     //We remove the enter character
                if(i > 0)
                {
                    playIn(mSB);
                    wrote = 1;                      //If he wrote letter and press continue we can move forward
                }
                else
                    error = 1;                       //If he didn't write anything we don't leave and press the error message
            }
            else if(i < 9)                  //Even though the string is of size 10 we can't write more than 9 letter otherwise it prints error
            {
                initStaticText(&t, name, t.posText.x, t.posText.y); //We print the new text with the new character inside
                i++;                                                //We increment i to go to the next area in the array of char
            }
        }

        if(error)
            printText(&tEmpty, screen);
        printText(&t, screen);
        printText(&tContinue, screen);
        printText(&tWrite, screen);
        printText(&tWon, screen);
        SDL_Flip(screen);
    }

    FILE* fhigh = NULL;
	fhigh = fopen("highscore.txt","r+");
	if(!fhigh)
        exit(-1);

    /*We can't write in the middle of the text document, we can only write a new characters by placing it at a position and removing thus the old one
    or we can add without removing anything but at the end of the document. Because of that we will start by storing all of our text informations in
    a linked list. We cannot use an array cause we need to be able to put information in the middle. Then we will manipulate the information and then
    print is back inside the text document.*/

    linkedList *begList = (linkedList *)malloc(sizeof(*begList));           //We initialize the structure which will contain the first link of our linked list
    linkedHighScore *linker = (linkedHighScore *)malloc(sizeof(*linker));   //We initialize the first link
    begList->first = linker;                                                //We link the address of our first link with our beginning structure


    unsigned short n = fgetc(fhigh) - '0';                                  //We put the number of high score in this variable

    fseek(fhigh, 2, SEEK_CUR);                                              //We put the cursor at the right position

    fgets(linker->text, 40, fhigh);                                         //We initialize the text of our first link

    unsigned short playerScore = 6500;//mB->w * mB->w / (mB->gameClock.start - mB->gameClock.timeLeft);      //We calculate the score of the player

    unsigned short index = n - 1;                                               //This will contain the index at which we will place our new score by default it's the last so the worst score

    if(scoreGreater(linker, playerScore, &index))                                       //We try to see if the score is better with our first high score
    {
        index = 0;                                                              //If it's the case we keep in memory the index of the first link
        insertLinker(begList->first, index, playerScore, name);
    }


    addNewLinkTexted(linker, fhigh, n);                                 //We call our recursive function which will initialize everything

    fclose(fhigh);                                                      //We close the file

    fhigh = fopen("highscore.txt","a");                                                  //To then reopen it in add mode to just add our new text erasing the former one

    //printtBackText()



    fclose(fhigh);

    while(1)
    {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));  //This prints a blackscreen to clean the previous one
        UpdateEvents(&in);                                              //This handles input
        if(checkIfClicked(&tContinue, &in))
            if(highScoreReadMenu(mB, screen, mSB))
                return 1;
        SDL_Flip(screen);
    }
}

void initTextPauseMenu(Text *tResume, Text *tExit, Text *tMain) //This is used to initialize texts in the pause menu
{
    initText(tResume, "Resume", LF/2, HF/16*7);
    initText(tMain, "Go back to main menu", LF/2, HF/16*9);
    initText(tExit, "Exit", LF/2, HF/16*11);
}

bool pauseMenu(GameBoard *mB, SDL_Surface *screen)         //This is the pause menu
{
    Input in;
    memset(&in,0,sizeof(in));   //Puts everything in our structure at 0
    Text tResume, tExit, tMain, tPaused;
    initTextPauseMenu(&tResume, &tExit, &tMain);
    initBiggerText(&tPaused, "Game paused", LF/2, HF/8);
    while(1)   //In this loop we can only go out by leaving the game or returning a value
    {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));  //This prints a blackscreen to clean the previous one
        UpdateEvents(&in);                                              //This handles input
        printText(&tResume, screen);
        printText(&tExit, screen);
        printText(&tMain, screen);
        printText(&tPaused, screen);

        if(checkIfClicked(&tMain, &in))
        {
            putTo0(mB);
            freeArrays(mB);
            mB->timer = SDL_AddTimer(6000,startGlitch, &mB->glitch);
            return 1;                       //Here we go back to the main menu
        }
        else if(checkIfClicked(&tResume, &in))
            return 0;                       //We remove the pause
        else if(checkIfClicked(&tExit, &in))
            exitMenu(screen);               //We go to the exitMenu
        SDL_Flip(screen);
        SDL_Delay(1);
    }
}

void initTextLife(Life *hp)             //This is used to initialize life text
{
    char text[20] = "";
    sprintf(text, "Life : %hi", hp->totalLife);
    initStaticText(&hp->tHpLeft, text, LF/256, HF/16*3);
}

void initTextClock(Clock *t)           //This is used to initialize clock text
{
    char text[30] = "";
    sprintf(text, "Time : %hi seconds", t->duration);
    initStaticText(&t->ticking, text, LF/256, HF/16);
}

void initClock(Clock *time, short duree)     //This is used to initialize our clock
{
    time->duration = duree;
    time->timeLeft = time->duration;
    time->start = SDL_GetTicks();
    time->currentSec = SDL_GetTicks();
    initTextClock(time);
}

bool calcSecClock(Clock *time)   //This is used to calculate if one second has passed
{
    if(SDL_GetTicks() - time->currentSec >= 1000)
    {
        time->currentSec = SDL_GetTicks();              //If one second or more has elapsed, the function returns 1
        return 1;
    }
    return 0;
}

bool calcHalfSecClock(Clock *time)   //This is used to calculate if one second has passed
{
    if(SDL_GetTicks() - time->currentSec >= 500)
    {
        time->currentSec = SDL_GetTicks();              //If one second or more has elapsed, the function returns 1
        return 1;
    }
    return 0;
}

void printNumberTime(Text *t, short n, SDL_Surface *screen)     //This function is used to print a number
{
    char text[20] = "";
    sprintf(text, "Time : %d seconds", n);                     //This is used to put the number variable inside a string
    t->surface = TTF_RenderText_Shaded(t->font, text, t->couleur, {120,120,120});
    SDL_BlitSurface(t->surface, NULL, screen, &t->posText);
}

void printNumberLife(Text *t, short n, SDL_Surface *screen)     //This function is used to print a number
{
    char text[20] = "";
    sprintf(text, "Life : %d", n);                     //This is used to put the number variable inside a string
    t->surface = TTF_RenderText_Shaded(t->font, text, t->couleur, {120,120,120});
    SDL_BlitSurface(t->surface, NULL, screen, &t->posText);
}

void printTimeLeft(Clock *time, SDL_Surface *screen)            //This is used to print how much time there is left
{
    if(calcSecClock(time))
    {
            time->timeLeft-=1;                                  //If one second went by we decrement by one and print the new time
    }
    printNumberTime(&time->ticking, time->timeLeft, screen);
}

void printLifeLeft(Life *hp, SDL_Surface *screen)               //This is used to print how much life has the user left
{
    printNumberLife(&hp->tHpLeft, hp->lifeLeft, screen);
}

void initLife(GameBoard *mB)                                   //This is used to initilize how much life the user will have based on the size of the array
{
    unsigned short life = ((mB->w * mB->h)*log(mB->w * mB->h)/50) + 1;          //This enables us to have a linearly going up life amount for big arrays
    if(life <= 0)
        mB->healthPoint.totalLife = 1;
    else
        mB->healthPoint.totalLife = life;
    mB->healthPoint.lifeLeft = mB->healthPoint.totalLife;
    initTextLife(&mB->healthPoint);
}

void initDuration(GameBoard *mB)    //This is used to initialize the duration of the game in function of the size of the game board (for now bad)
{
    mB->gameClock.duration = (int)(mB->w * mB->h)*log(mB->w * mB->h)/2;     //This enables us to have a linearly going up time for big arrays
}

bool timeLeft(Clock *time)          //This is used to check if there is time left
{
    if(time->timeLeft <= 0)         //If there is less or 0 seconde, the function return 1
        return 1;
    return 0;
}

bool checkWin(GameBoard *mB)        //This is used to check if the user won
{
    if(mB->board.number == mB->gamerBoard.number)       //If the number of correct click is equal to the number of one in the drawing array it's won
        return 1;
    return 0;
}

bool checkGood(GameBoard *mB, Input *in)                                        //This is used to check if the click is correct
{
    if(in->mousex < HF/16*5 || in->mousey < HF/16*5)                            //If he clicks outside of the border we return 0
        return 0;
    in->boxx = (in->mousex - HF/16.0*5) / ((LF - HF/16.0*5 - widthline)/mB->w);                  //Here we "convert" the mouse x to an index of the array
    in->boxy = (in->mousey - HF/16.0*5) / ((HF - HF/16.0*5 - widthline) / mB->h);

    if(mB->board.tab[in->boxy][in->boxx] > 0)
    {
        if(mB->gamerBoard.tab[in->boxy][in->boxx] == 0)            //If it was not already clicked
        {
            mB->gamerBoard.tab[in->boxy][in->boxx] = 1;           //If there was a 1 where he clicked we put a one inside the gamer board and return 1
            mB->gamerBoard.number += 1;                           //We also add 1 to the "good results" to eventually know when the user win
        }
        return 1;
    }
    return 0;
}

void initUpperLeftRect(surfpos *black, SDL_Surface *screen)                     //This is used to initialize the upper left black screen to refresh the things printed on it
{
    black->surface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, HF/16*5 - 6,HF/16*5 - 6, 32, 0, 0, 0, 0);
    black->pos.x = 3;
    black->pos.y = 3;
    SDL_FillRect(black->surface, NULL, SDL_MapRGB(screen->format,120 ,120,120));
}

void printUpperLeftRect(surfpos *black, SDL_Surface *screen)                    //This is used to print the upper left black refresher
{
    SDL_BlitSurface(black->surface, NULL, screen, &black->pos);
}

void initSans(Animator *sans)                                                   //This is used to initialize the images in the Animator structure
{
    sans->surface[0] = SDL_LoadBMP("sprites/1.bmp");
    sans->surface[1] = SDL_LoadBMP("sprites/2.bmp");
    sans->surface[2] = SDL_LoadBMP("sprites/3.bmp");
    sans->surface[3] = SDL_LoadBMP("sprites/4.bmp");
    for(unsigned short i = 0; i < 4; i++)
    {
        SDL_SetColorKey(sans->surface[i], SDL_SRCCOLORKEY, SDL_MapRGB(sans->surface[i]->format, 255, 255, 255));
        sans->pos[i].y = HF/16*5 - sans->surface[i]->h;
        sans->pos[i].x = LF/8 - sans->surface[i]->w/2;
    }
}

void animSansSteady(Animator *sans, SDL_Surface *screen)                        //This is used to animate Sans when nothing is happening
{
    if(calcHalfSecClock(&sans->timerSteady))                                    //If one second has elapsed
    {
        if(sans->currSprite != 1)                                               //If it was one it becomes 0 and opposite
            sans->currSprite = 1;
        else
            sans->currSprite = 0;
    }
    SDL_BlitSurface(sans->surface[sans->currSprite], NULL, screen, &sans->pos[sans->currSprite]);
}

bool animSansBad(Animator *sans, SDL_Surface *screen)                           //This is used to animate Sans when the click is incorrect
{
    if(calcHalfSecClock(&sans->timerBad))                                       //If one second has pasted
    {
        return 0;
    }
    else
    {
        SDL_BlitSurface(sans->surface[2], NULL, screen, &sans->pos[2]);         //We blit the corresponding sprite
        return 1;
    }
}

bool animSansGood(Animator *sans, SDL_Surface *screen)                          //This is used to animate Sans when a line or row is completed
{
    if(calcHalfSecClock(&sans->timerGood))                                      //If one second has passed
    {
        return 0;
    }
    else
    {
        SDL_BlitSurface(sans->surface[3], NULL, screen, &sans->pos[3]);         //We blit the corresponding sprite
        return 1;
    }
}

void printBoard(GameBoard *mB, SDL_Surface *screen)                                 //This is the group of all the functions to print the board
{
        surfpos l;
        surfpos rect;
        mB->leftArray.height = mB->h;
        mB->upperArray.width = mB->w;
        createUpperArray(mB, &l, &rect, screen);
        createLeftArray(mB, &l, &rect, screen);
        printLeftArray(mB, screen);
        printUpperArray(mB, screen);
}

void animFace(surfpos *face, SDL_Surface *screen, soundBox *mSB)                    //This is used to print the scary face
{
    FMOD_System_PlaySound(mSB->system, FMOD_CHANNEL_FREE, mSB->sFace, 0, NULL);     //We play the sound
    for(short i = 0; i < 10; i++)
    {
        SDL_BlitSurface(face->surface, NULL, screen, &(face->pos));                 //We print the face with a delay between each every time
        SDL_Flip(screen);
        SDL_Delay(i);
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));
        SDL_Flip(screen);
    }
}

void printDot(GameBoard *mB, SDL_Surface *screen, unsigned short x, unsigned short y, surfpos *dot) //This is used to print the white when there is a correct click
{
    dot->pos.x = HF/16*5 + x*widthline + x*mB->upperArray.WB + widthborder;
    dot->pos.y = HF/16*5 + y*widthline + y*mB->leftArray.HB + widthborder;
    SDL_BlitSurface(dot->surface, NULL, screen, &dot->pos);
}

bool checkCross(GameBoard *mB, Input *in)   //This is used to check if there is already a cross
{
    if(mB->gamerBoard.tab[in->boxy][in->boxx] == 2)              //If the space is already white we return 1
    {
        return 1;
    }
    return 0;
}

void initCross(surfpos cross[2], GameBoard *mB, SDL_Surface *screen)                    //This is used to initialize the cross in the box in case of a right click
{
    cross[0].surface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, (int)round(mB->upperArray.WB), 1, 32, 0, 0, 0, 0);                   //We create the first line surface
    cross[1].surface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, 1, (int)(round(mB->leftArray.HB)), 32, 0, 0, 0, 0);                   //We create the second line surface
    SDL_FillRect(cross[0].surface, NULL, SDL_MapRGB(screen->format, 255,255,255));
    SDL_FillRect(cross[1].surface, NULL, SDL_MapRGB(screen->format, 255,255,255));
}

void printCross(surfpos cross[2], Input *in, GameBoard *mB, SDL_Surface *screen)                           //This is used to print the cross
{
    cross[0].pos.x = HF/16*5 + in->boxx*widthline + in->boxx*mB->upperArray.WB + widthlineborder;
    cross[0].pos.y = HF/16*5 + in->boxy*widthline + in->boxy*mB->leftArray.HB + mB->leftArray.HB/2 + widthlineborder;
    cross[1].pos.x = HF/16*5 + in->boxx*widthline + in->boxx*mB->upperArray.WB + mB->upperArray.WB/2 + widthlineborder;
    cross[1].pos.y = HF/16*5 + in->boxy*widthline + in->boxy*mB->leftArray.HB + widthlineborder;
    SDL_BlitSurface(cross[0].surface, NULL, screen, &cross[0].pos);
    SDL_BlitSurface(cross[1].surface, NULL, screen, &cross[1].pos);
}

void printBackCross(surfpos cross[2], Input *in, GameBoard *mB, SDL_Surface *screen, unsigned short x, unsigned short y)    //This is used to print back the crosses after a pause
{
    cross[0].pos.x = HF/16*5 + x*widthline + x*mB->upperArray.WB + + widthlineborder;
    cross[0].pos.y = HF/16*5 + y*widthline + y*mB->leftArray.HB + mB->leftArray.HB/2 + widthlineborder;
    cross[1].pos.x = HF/16*5 + x*widthline + x*mB->upperArray.WB + mB->upperArray.WB/2 + widthlineborder;
    cross[1].pos.y = HF/16*5 + y*widthline + y*mB->leftArray.HB + widthlineborder;
    SDL_BlitSurface(cross[0].surface, NULL, screen, &cross[0].pos);
    SDL_BlitSurface(cross[1].surface, NULL, screen, &cross[1].pos);
}

void printBackWhiteCross(GameBoard *mB, SDL_Surface *screen, Input *in, surfpos *dot, surfpos cross[])    //This is used to print back the white squares and crosses after a pause
{
    for(unsigned short y = 0; y < mB->h; y++)
    {
        for(unsigned short x = 0; x < mB->w; x++)
        {
            if(mB->gamerBoard.tab[y][x] == 1)
                printDot(mB, screen, x, y, dot);
            else if(mB->gamerBoard.tab[y][x] == 2)
            {

                printBackCross(cross, in, mB, screen,x,y);
            }
        }
    }
}

bool checkWhite(GameBoard *mB, Input *in)   //This is used to check if the space is already white
{
    if(mB->gamerBoard.tab[in->boxy][in->boxx] == 1)              //If the space is already white we return 1
    {
        return 1;
    }
    return 0;
}

void printBlack(surfpos *bB, Input *in, GameBoard *mB, SDL_Surface *screen)         //This is used to print a black box if the user wants to remove a cross
{
    bB->pos.x = HF/16*5 + in->boxx*widthline + in->boxx*mB->upperArray.WB + widthlineborder;
    bB->pos.y = HF/16*5 + in->boxy*widthline + in->boxy*mB->leftArray.HB + widthlineborder;
    SDL_BlitSurface(bB->surface, NULL, screen, &bB->pos);
}

bool game(GameBoard *mB, SDL_Surface *screen, soundBox *mSB, surfpos lines[2])            //This is the main game function
{
    pixel p;

    surfpos dot;
    dot.surface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, round(mB->upperArray.WB), round(mB->leftArray.HB), 32, 0, 0, 0, 0);
    SDL_FillRect(dot.surface, NULL, SDL_MapRGB(screen->format,255,255,255));

    surfpos black;                                                                                      //This is the black screen for the upper left corner

    surfpos blackBox;                                                                                   //This is used to print back a black box if there was a cross before
    if(mB->para.mode == 2)
    {
        blackBox.surface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, round(mB->upperArray.WB), round(mB->leftArray.HB), 32, 0, 0, 0, 0);
        SDL_FillRect(blackBox.surface, NULL, SDL_MapRGB(screen->format, mB->image.tab[0][0].r,mB->image.tab[0][0].g,mB->image.tab[0][0].b));  //If we are in custom image mode the black screen is not black but the color of the removed color
    }
    else
    {
        blackBox.surface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, round(mB->upperArray.WB), round(mB->leftArray.HB), 32, 0, 0, 0, 0);
        SDL_FillRect(blackBox.surface, NULL, SDL_MapRGB(screen->format,0,0,0));
    }

    surfpos cross[2];
    initCross(cross, mB, screen);

    surfpos face;                                                                                       //Here we initialize the face (screamer) for the hardcore mode
    face.surface = SDL_LoadBMP("face.bmp");
    SDL_SetColorKey(face.surface, SDL_SRCCOLORKEY, SDL_MapRGB(face.surface->format, 255, 255, 255));    //We remove the white
    face.pos.x = 0;
    face.pos.y = 0;                     //We've tryed to initialize the screamer only for the hardcore mode but it would create a compile error

    Animator sans;                                                                                      //This is the little sans in the top left corner

    bool bad = 0;                                                                          //This is for the white

    surfpos color;
    color.surface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, round(mB->upperArray.WB), round(mB->leftArray.HB), 32, 0, 0, 0, 0);

    bool choice = 0, win = 0, lose = 0, good = 0;
    Input in;
    memset(&in,0,sizeof(in));   //Puts everything in our structure at 0

    initDuration(mB);                                   //Here we initialize the duration which is proportional to the size of our game board
    initClock(&mB->gameClock, mB->gameClock.duration);  //Here we initialize the clock
    initLife(mB);
    initSans(&sans);
    initUpperLeftRect(&black, screen);

    while(1)
    {
        memset(&in,0,sizeof(in));   //Puts everything in our structure at 0
        UpdateEvents(&in);
        printUpperLeftRect(&black, screen);
        if(mB->para.difficulty == 1 || mB->para.difficulty == 4 || mB->para.difficulty == 3)
           printTimeLeft(&mB->gameClock, screen);                          //We print the time reamning
        if(mB->para.difficulty == 3 || mB->para.difficulty == 2 || mB->para.difficulty == 4)
           printLifeLeft(&mB->healthPoint, screen);                        //We print the life reamning


        if(!bad && !good)
            animSansSteady(&sans, screen);

        if(in.mousebuttons[0])
        {
            if(checkGood(mB, &in))
            {
                if(mB->para.mode == 0)                  //If we are not in the image mode from the user it's just white
                    animDot(mB,screen, &in,&dot);
                else if(mB->para.mode == 2)
                    animCustomColor(mB, screen, &in, &color);
                else
                    animColor(mB, screen, &in, &color, p);

                if(checkLine(mB, in.boxy))
                    good = 1;

                if(checkRow(mB, in.boxx))
                    good = 1;

                if(checkWin(mB))                        //We check if the number of correct clicks are equal to the number of pixel in the image
                    win = 1;
                sans.timerGood.currentSec = SDL_GetTicks();
            }
            else
            {
                if(mB->para.difficulty == 4)
                {
                    if(mB->para.mode == 0)                                      //If there is an incorrect click and it's not image mode
                    {
                        putTo0(mB);
                        initRandDrawing(mB->h, mB->w, &mB->board);              //We take a new drawing
                        loadNumbers(mB);
                    }
                    animFace(&face, screen, mSB);
                    printBoard(mB, screen);
                }
                if(mB->para.difficulty == 4 || mB->para.difficulty == 3 ||mB->para.difficulty == 2)
                    mB->healthPoint.lifeLeft -= 1;
                sans.timerBad.currentSec = SDL_GetTicks();
                bad = 1;
            }
        }
        if(in.mousebuttons[1])
        {
            in.boxx = (in.mousex - HF/16.0*5) / ((LF - HF/16.0*5 - widthline)/mB->w);                  //Here we "convert" the mouse x & y to an index of the array
            in.boxy = (in.mousey - HF/16.0*5) / ((HF - HF/16.0*5 - widthline) / mB->h);
            if(!checkWhite(mB, &in))
            {
                if(checkCross(mB, &in))
                {
                    mB->gamerBoard.tab[in.boxy][in.boxx] = 0;
                    printBlack(&blackBox, &in, mB, screen);                                                 //He we check if in the array at that position if there is a already a cross
                }                                                                                           //If there is we print a black box
                else
                {
                    mB->gamerBoard.tab[in.boxy][in.boxx] = 2;
                    printCross(cross, &in, mB, screen);                                                     //Else we print a cross
                }
            }
        }

        if(bad)
            bad = animSansBad(&sans, screen);


        if(good)
        {
            good = animSansGood(&sans, screen);

        }

        if(in.key[SDLK_ESCAPE])                                         //If escape is pressed we break the loop to go to the pause menu
        {
            if(pauseMenu(mB,screen))
            {
                FMOD_Sound_Release(mSB->sGameMusic);
                FMOD_System_CreateSound(mSB->system, "menuMusic.wav", FMOD_LOOP_NORMAL | FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, 0, &mSB->sMenuMusic);
                FMOD_Sound_SetLoopCount(mSB->sMenuMusic, -1);                                                                                         //We stop the game music and put back the menu music
                FMOD_System_PlaySound(mSB->system, FMOD_CHANNEL_FREE, mSB->sMenuMusic, 0, NULL);
                return 1;                                               //If it's yes we return 1 and go back to the main menu
            }                                               //If it's yes we return 1 and go back to the main menu
            in.key[SDLK_ESCAPE] = 0;                                    //Otherwise goes back into the pause menu
            SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));  //This prints a blackscreen to clean the previous one
            printBoard(mB, screen);                     //We print again the boards
            printBackWhiteCross(mB,screen, &in, &dot, cross);
        }

        if(mB->para.difficulty == 3 || mB->para.difficulty == 1 || mB->para.difficulty == 4)
            lose = timeLeft(&mB->gameClock);

        if(mB->para.difficulty == 3 || mB->para.difficulty == 2 || mB->para.difficulty == 4)
            if(mB->healthPoint.lifeLeft == 0)
                lose = 1;


        if(lose || win)                                   //If there is no time left lose is true and the loop ends
        {
            FMOD_Sound_Release(mSB->sGameMusic);
            if(win)
            {
                SDL_BlitSurface(mB->winLoseAnim.white.surface, NULL, screen, &mB->winLoseAnim.white.pos);
                SDL_Flip(screen);
                FMOD_System_PlaySound(mSB->system, FMOD_CHANNEL_FREE, mSB->sAnimWinMusic, 0, NULL);
                SDL_Delay(2000);
                mB->lostwin = 1;
            }
            else
            {
                FMOD_System_PlaySound(mSB->system, FMOD_CHANNEL_FREE, mSB->sAnimLoseMusic, 0, NULL);
                animLose(mB,screen);
                mB->lostwin = 0;
            }
            choice = replayMenu(mB, screen, mSB);                                //If he wants to go back to main menu we break the loop, else we reinitilize everything
            if(!choice)
            {
                mB->leftArray.maxSub = 0;                                       //We need to do that cause if the player play with a big board then a small one he will keep the former maxSub
                mB->upperArray.maxSub = 0;
                return 1;
            }
            else
            {
                if(mB->para.difficulty == 1 || mB->para.difficulty == 3 || mB->para.difficulty == 4)
                    mB->gameClock.timeLeft = mB->gameClock.duration;                                    //Here we reinitilize everything
                if(mB->para.difficulty == 2 || mB->para.difficulty == 3 || mB->para.difficulty == 4)
                    mB->healthPoint.lifeLeft = mB->healthPoint.totalLife;
                printBoard(mB, screen);
                putTo0User(mB);
                win = 0;
                lose = 0;
            }

        }

        SDL_Flip(screen);
    }
}

void initTextMain(Text *tTitle, Text *tStart, Text *tExit)  //This is used to initialize texts in our main menu
{
    initBiggerText(tTitle, "Picross", LF/2, HF/8);

    initText(tStart, "Start", LF/2, HF/2);

    initText(tExit, "Exit", LF/2, HF/2 + HF/4);
}

void initTextModeMenu(Text *rand, Text *image, Text *tGoBack)    //This is used to initialize texts in our mode menu
{
    initText(rand, "Random mode", LF/2, HF/2 - HF/8);

    initText(image, "Image mode", LF/2, HF/2 + HF/8);

    initText(tGoBack, "Back", LF/2, 7*HF/8);
}

void initImage(GameBoard *mB, const char *image)                               //This is used to load the image informations inside the array
{
    FILE *FIC = NULL;
    FIC = fopen(image, "r");
    unsigned int h, w, nbr;
    fscanf(FIC, "%hu %hu", &h, &w);
    mB->w = w;
    mB->h = h;
    initImageArrayUS(mB);
    initImageArrayB(mB);
    for (unsigned short i=0; i<mB->h; i++)
    {
        for (unsigned short j=0; j<mB->w; j++)
        {
            fscanf(FIC, "%1d", &nbr);
            if(nbr > 0)
                mB->board.tab[i][j] = 1;
            else
                mB->board.tab[i][j] = 0;
            mB->boardImg.tab[i][j] = nbr;
        }
    }

}

Uint32 getpixel(SDL_Surface *surface, int x, int y)                           //This is used to extract the formated pixel (found on the internet)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;
    }
}

void initCustomImageArray(GameBoard *mB)                                    //This is used to initialize our board for the custom image
{
    mB->image.tab = (pixel **)malloc(mB->h*sizeof(pixel *));                //Here we dynamically allocate the space for the custom image board
    for (unsigned short i=0; i<mB->h; i++)
        mB->image.tab[i] = (pixel *)malloc(mB->w*sizeof(pixel));
}

void loadCustomImage(GameBoard *mB)                                         //This is used to load the custom image in the array
{
    Uint8 r0, g0 ,b0;
    Uint8 r, g ,b;
    SDL_GetRGB(getpixel(mB->img, 0, 0), mB->img->format, &r0, &g0, &b0); //We get the color of the first pixel to know what to ignore
    for(int y = 0; y < mB->img->h; y++)
    {
        for(int x = 0; x < mB->img->w; x++)
        {
            SDL_LockSurface(mB->img);
            SDL_GetRGB(getpixel(mB->img, x, y), mB->img->format, &r, &g, &b);
            SDL_UnlockSurface(mB->img);
            mB->image.tab[y][x].r = r;
            mB->image.tab[y][x].g = g;
            mB->image.tab[y][x].b = b;
            if(r == r0 && g == g0 && b == b0)
               mB->board.tab[y][x] = 0;
            else
            {
                mB->board.tab[y][x] = 1;
                mB->board.number += 1;
            }
        }
    }
}

void loadGame(GameBoard *mB, surfpos lines[2], SDL_Surface *screen)                                //This is used to load the game using the input parameters  (works for 3)
{
    if(mB->para.mode == 0)                                   //If he chose the random mode we initalize the gameboard with random values
        initDrawing(mB->h, mB->w, &mB->board);
    else if(mB->para.mode == 1)
    {
        unsigned short n = rand()%3 + 1;                             //Random number between 1 and 3
        char image[30] = "";
        sprintf(image, "images/image%hu.txt", n);                  //This put the number next the image word so that it pick the right image
        initImage(mB, image);
    }
    else if(mB->para.mode == 2)
    {
        mB->w = mB->img->w;
        mB->h = mB->img->h;
        initImageArrayB(mB);
        initCustomImageArray(mB);
        loadCustomImage(mB);
    }

    initArrays(mB->h, mB->w, &mB->leftArray, &mB->upperArray);
    loadNumbers(mB);
    initUserArray(mB->h, mB->w, &mB->gamerBoard);
    initAnimLose(mB, screen);
    initAnimWin(&mB->winLoseAnim.white);
}

void initTextSizeMenu(Text *tHeight, Text *tWidth, Text *nHeight, Text *nWidth, Text *tBack, Text *tStart) //This is used to initialize texts in the size menu
{
    initText(tHeight, "Height :", LF/2 - LF/8, HF/2);
    initText(tWidth, "Width :", LF/2 + LF/8, HF/2);
    initText(nWidth, "1", LF/2 + LF/8, HF/2 + HF/16);
    initText(nHeight, "1", LF/2 - LF/8, HF/2 + HF/16);
    initText(tBack, "Back", LF/2, 7*HF/8);
    initText(tStart, "Start", LF/2, 6*HF/8);
}

void updateWidth(Text *t, unsigned short n)             //This is used to update the new width selected by the user
{
    char text[3] = "";
    sprintf(text, "%hu", n);
    t->surface = TTF_RenderText_Shaded(t->font, text, t->couleur, {0,0,0});
}

void updateHeight(Text *t, unsigned short n)            //This is used to update the new height selected by the user
{
    char text[3] = "";
    sprintf(text, "%hu", n);
    t->surface = TTF_RenderText_Shaded(t->font, text, t->couleur, {0,0,0});
}

bool menuSize(GameBoard *mB, SDL_Surface *screen, soundBox *mSB)           //This menu asks the user the size
{
    bool goBack = 0, f = 0;
    short fade = 255;                                                                   //Variable used for the menu fading
    fader black;
    black.pos.x = 0;
    black.pos.y = 0;
    Input in;
    memset(&in,0,sizeof(in));   //Puts everything in our structure at 0
    Text tHeight, tWidth, nHeight, nWidth, tBack, tArrow, tStart;
    initTextSizeMenu(&tHeight, &tWidth, &nHeight, &nWidth, &tBack, &tStart);
    initBiggerText(&tArrow, "To choose the size: Use the keyboard arrow", LF/2, 2*HF/8);
    tArrow.couleur = {255,0,0};
    tArrow.surface = TTF_RenderText_Shaded(tArrow.font, "To choose the size: Use the keyboard arrow", tArrow.couleur, {0,0,0});

    bool choice = 0;
    mB->h = 1;
    mB->w = 1;
    nHeight.couleur = {255,0,0};                                                            //We start by highlighting the height in red cause that's what the user can modify
    nHeight.surface = TTF_RenderText_Shaded(nHeight.font, "1", nHeight.couleur, {0,0,0});
    char width[4] = "";
    char height[4] = "";

    SDL_EnableKeyRepeat(200,50);                       //Allow us the maintain the click if he wants a big number

    while(!goBack && mB->para.mode == 0)            //If he selected the image mode we don't go through the selection of the size
    {
        memset(&in,0,sizeof(in));
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));  //This prints a blackscreen to clean the previous one
        UpdateEvents(&in);

        printText(&tHeight, screen);
        printText(&tWidth, screen);
        printText(&nHeight, screen);
        printText(&nWidth, screen);
        printText(&tBack, screen);
        printText(&tArrow, screen);
        printText(&tStart, screen);

        if(in.key[SDLK_LEFT])
        {
            choice = 0;
            nHeight.couleur = {255,0,0};
            nWidth.couleur = {255,255,255};
            sprintf(height, "%hu", mB->h);
            sprintf(width, "%hu", mB->w);
            nHeight.surface = TTF_RenderText_Shaded(nHeight.font, height, nHeight.couleur, {0,0,0});
            nWidth.surface = TTF_RenderText_Shaded(nWidth.font, width, nWidth.couleur, {0,0,0});

        }
        else if(in.key[SDLK_RIGHT])                                                             //According to the choice we change the color to show which one is selected
        {                                                                                       //And we change the value of choice to enable the user to change the width or height
            choice = 1;
            nWidth.couleur = {255,0,0};
            nHeight.couleur = {255,255,255};
            sprintf(height, "%hu", mB->h);
            sprintf(width, "%hu", mB->w);
            nHeight.surface = TTF_RenderText_Shaded(nHeight.font, height, nHeight.couleur, {0,0,0});
            nWidth.surface = TTF_RenderText_Shaded(nWidth.font, width, nWidth.couleur, {0,0,0});
        }

        if(!choice)
        {
            if(in.key[SDLK_UP])
            {
                mB->h += 1;
                updateHeight(&nHeight, mB->h);
                in.key[SDLK_UP] = 0;
            }
            else if(in.key[SDLK_DOWN])
            {
                if(mB->h > 1)
                {
                    mB->h -= 1;
                    updateHeight(&nHeight, mB->h);                //Here we just change the width/height and the text link to it with respect to what the user selected
                    in.key[SDLK_DOWN] = 0;                        //Everytime we put it back to 0 otherwise when we just press a little bit on the key it take +5,6 or 7
                }

            }
        }
        else
        {
            if(in.key[SDLK_UP])
            {
                mB->w += 1;
                updateWidth(&nWidth, mB->w);
                in.key[SDLK_UP] = 0;
            }
            else if(in.key[SDLK_DOWN])
            {
                if(mB->w > 1)
                {
                    mB->w -= 1;
                    updateWidth(&nWidth, mB->w);
                    in.key[SDLK_DOWN] = 0;
                }

            }
        }


        if(checkIfClicked(&tBack, &in))
            return 0;                                         //If the user wants to go back we return 0 and go back to the previous menu
        else if(checkIfClicked(&tStart, &in))                 //If he want to start we break the loop and go inside the different
        {
            playIn(mSB);
            break;
        }

        if(!f)
            f = fadeInMenu(screen, &fade, &black);
        SDL_Flip(screen);
    }

    surfpos lines[2];

    loadGame(mB, lines, screen);

    FMOD_Sound_Release(mSB->sMenuMusic);
    FMOD_System_CreateSound(mSB->system, "gameMusic.wav", FMOD_LOOP_NORMAL | FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, 0, &mSB->sGameMusic); //Here we create a stream cause the music is long
    FMOD_Sound_SetLoopCount(mSB->sGameMusic, -1);                                                                                                    //This will loop the music indefinitly
    FMOD_System_PlaySound(mSB->system, FMOD_CHANNEL_FREE, mSB->sGameMusic, 0, NULL);

    SDL_RemoveTimer(mB->timer); /*We stop the timer to not have glitches in the game*/

    animation321(screen);

    if(mB->para.mode == 2)
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, mB->image.tab[0][0].r,mB->image.tab[0][0].g,mB->image.tab[0][0].b)); //If we are in custom image mode the board is not black but the color of the removed pixel color
    else
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format,0,0,0));

    printBoard(mB, screen);

    if(game(mB, screen, mSB, lines))
        return 1;

}

bool menuWhichImage(GameBoard *mB, SDL_Surface *screen, soundBox *mSB)      //This menu asks the user wants a random image or it's own
{
    bool goBack = 0, goBackMain = 0, f = 0, printInst = 0;
    short fade = 255;                                       //Variable used for the menu fading
    fader black;
    black.pos.x = 0;
    black.pos.y = 0;
    Input in;
    memset(&in,0,sizeof(in));   //Puts everything in our structure at 0
    Text tRandomImage, tOwnImage, tInstruct1, tInstruct2, tInstruct3, tInstruct4, tGoBack;
    initText(&tGoBack, "Back", LF/2, 14*HF/16);
    initText(&tRandomImage, "Use a random image", LF/2, 2*HF/16);
    initText(&tOwnImage, "Import you own image", LF/2, 4*HF/16);
    initText(&tInstruct1, "To play with your own image it needs : ", LF/2, 8*HF/16);
    initText(&tInstruct2, "To be of type .bmp | To be small | Have a background color different from the rest (it will be removed)", LF/2, 9*HF/16);
    initText(&tInstruct3, "Close the game, put your image where Picross.exe is", LF/2, 11*HF/16);
    initText(&tInstruct4, "Name it \"Image\" then execute again the game and click on the same option", LF/2, 12*HF/16);

    mB->img = SDL_LoadBMP("Image.bmp");

    while(!goBack)
    {
        memset(&in,0, sizeof(in));
        UpdateEvents(&in);

        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));  //This prints a blackscreen to clean the previous one
        printText(&tGoBack, screen);
        printText(&tRandomImage, screen);
        printText(&tOwnImage, screen);
        if(printInst)
        {
            printText(&tInstruct1, screen);
            printText(&tInstruct2, screen);
            printText(&tInstruct3, screen);
            printText(&tInstruct4, screen);
        }
        if(checkIfClicked(&tRandomImage, &in))
        {
            playIn(mSB);
            mB->para.mode = 1;
            goBackMain = menuSize(mB, screen, mSB);
            goBack = goBackMain;
        }
        else if(checkIfClicked(&tOwnImage, &in))
        {
            if(mB->img == NULL)                 //If the image isn't here we print the instruction
            {
                printInst = 1;
            }
            else
            {
                playIn(mSB);
                mB->para.mode = 2;                      //Else we can start the game
                goBackMain = menuSize(mB, screen, mSB);
                goBack = goBackMain;
            }
        }
        else if(checkIfClicked(&tGoBack, &in))
            goBack = 1;
        if(!f)
            f = fadeInMenu(screen, &fade, &black);
        SDL_Flip(screen);
    }
    if(goBackMain)
        return 1;
    else
        return 0;

}

bool menuMode(SDL_Surface *screen, GameBoard *mB, soundBox *mSB)          //This is the mode menu
{
    bool goBack = 0, goBackMain = 0, f = 0;
    short fade = 255;                                       //Variable used for the menu fading
    fader black;
    black.pos.x = 0;
    black.pos.y = 0;
    Input in;
    memset(&in,0,sizeof(in));   //Puts everything in our structure at 0
    Text tRandomMode, tImageMode, tGoBack;
    initTextModeMenu(&tRandomMode, &tImageMode, &tGoBack);

    while(!goBack)
    {
        memset(&in,0, sizeof(in));
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));  //This prints a blackscreen to clean the previous one
        UpdateEvents(&in);
        printText(&tRandomMode, screen);
        printText(&tImageMode, screen);
        printText(&tGoBack, screen);
        if(checkIfClicked(&tRandomMode, &in))
        {
            playIn(mSB);
            mB->para.mode = 0;
            goBackMain = menuSize(mB, screen, mSB);
            goBack = goBackMain;
        }
        else if(checkIfClicked(&tImageMode, &in))
        {
            playIn(mSB);
            goBackMain = menuWhichImage(mB, screen, mSB);
            goBack = goBackMain;
        }
        else if(checkIfClicked(&tGoBack, &in))
            goBack = 1;
        if(!f)
            f = fadeInMenu(screen, &fade, &black);
        SDL_Flip(screen);
    }
    if(goBackMain)
        return 1;
    else
        return 0;
}

void initTextDifficultyMenu(Text *tEasy, Text *tMedium, Text *tHard, Text *tHardcore, Text *tTime, Text *tLife, Text *tGoBack)  //This is used to initialize texts in our difficulty menu
{
    initText(tEasy, "Easy", LF/2, 2*HF/8);
    initText(tMedium, "Medium", LF/2, 3*HF/8);
    initText(tHard, "Hard", LF/2, 4*HF/8);
    initText(tHardcore, "Hardcore", LF/2, 5*HF/8);
    initText(tGoBack, "Back", LF/2, 7*HF/8);
    initText(tTime, "Time limitation", LF/2 + LF/8, 3*HF/8 - HF/32);
    initText(tLife, "Life limitation", LF/2 + LF/8, 3*HF/8 + HF/32);
}

void fadeText(Text *t, SDL_Surface *screen, bool f, short *n)         //This is used to fade in and out the text
{
                            //We can't make the entire animation here other wise it will block us into doing the things that are in the menu in which we are
                            //Instead we do step by step, calling the function everytime and making the animation moving forward everytime
    if(f)                   //By doing this we do not get "stuck" here waiting for the animation to finish
        *n+=1;
    else
        *n-=1;

    if(*n <= 0)
        *n = 0;              //This is not to go below or above 0 and 255
    else if(*n >= 255)
        *n = 255;

    SDL_SetAlpha(t->surface, SDL_SRCALPHA | SDL_RLEACCEL, *n);

    printText(t, screen);
}

void menuDifficulty(SDL_Surface *screen, GameBoard *mB, soundBox *mSB)                    //This is the difficulty menu
{
    bool goBack = 0, goBackMain = 0, mediumClicked = 0, fade = 0;
    short n = 0, f = 255;                                                   //One variable is used for the fading of the life and time limitation, the other for the fading of the menu
    fader black;
    black.pos.x = 0;
    black.pos.y = 0;
    Input in;
    memset(&in,0,sizeof(in));                                               //Puts everything in our structure at 0
    Text tEasy, tMedium, tHard, tHardcore, tTime, tLife, tGoBack;
    initTextDifficultyMenu(&tEasy, &tMedium, &tHard, &tHardcore, &tTime, &tLife, &tGoBack);

    while(!goBack)
    {
        memset(&in,0, sizeof(in));                                      //We put this here otherwise it keeps the clicks in mermory and goes back of 2 menus
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));  //This prints a blackscreen to clean the previous one
        UpdateEvents(&in);                                              //This handles input
        printText(&tEasy, screen);
        printText(&tMedium, screen);
        printText(&tHard, screen);
        printText(&tHardcore, screen);
        printText(&tGoBack, screen);

        if(checkIfClicked(&tEasy, &in))
        {
            playIn(mSB);
            n = 0;
            mediumClicked = 0;
            mB->para.difficulty = 0;
            goBackMain = menuMode(screen, mB, mSB);              //Our go back system is used for 2 things :
            goBack = goBackMain;                                //First, if in the next menus the person want to go back from one menu back at a time
        }                                                       //Second, when the person will finish one game, if he wants to go back to the menu we need to use our system
        else if(checkIfClicked(&tHard, &in))                    //Meaning that to save memory we will not call again our function but go back inside of them by leaving our while loops
        {
            playIn(mSB);                                        //It's the "poupée russe" system, other wise we would have countless amout of functions the variable created when the user
            n = 0;                                              //Start a new game over and over again
            mediumClicked = 0;
            mB->para.difficulty = 3;
            goBackMain = menuMode(screen, mB, mSB);                  //Here everytime we check if one of the button is pressed
            goBack = goBackMain;                                //If it's the case we change our game parameters according to which button was clicked
        }                                                       //Then we put our mediumClicked and n variable to 0 to that if the person then goes back to this menu
        else if(checkIfClicked(&tHardcore, &in))                //The animation will be reinitialize to the beginning
        {
            playIn(mSB);                                                       //Finally we go to the next menu
            n = 0;
            mediumClicked = 0;
            mB->para.difficulty = 4;
            goBackMain = menuMode(screen, mB, mSB);
            goBack = goBackMain;
        }
        else if(checkIfClicked(&tGoBack, &in))
        {
            n = 0;
            mediumClicked = 0;
            goBack = 1;
        }
        else if(checkIfClicked(&tMedium, &in))
        {
            if(mediumClicked)
                mediumClicked = 0;
            else
                mediumClicked = 1;
        }
        else if(checkIfClicked(&tTime, &in) && mediumClicked)
        {
            playIn(mSB);
            n = 0;
            mediumClicked = 0;
            mB->para.difficulty = 1;
            goBackMain = menuMode(screen, mB, mSB);
            goBack = goBackMain;
        }
        else if(checkIfClicked(&tLife, &in) && mediumClicked)
        {
            playIn(mSB);
            n = 0;
            mediumClicked = 0;
            mB->para.difficulty = 2;
            goBackMain = menuMode(screen, mB, mSB);
            goBack = goBackMain;
        }

        fadeText(&tTime, screen, mediumClicked, &n);         //For animation purposes we need to go throught a variable
        fadeText(&tLife, screen, mediumClicked, &n);

        if(!fade)
            fade = fadeInMenu(screen, &f, &black);                  //If the menu was fader we stop the fading

        SDL_Flip(screen);
    }
}

int main(int argc, char * argv[])
{
    freopen("CON", "w", stdout);
    freopen("CON", "r", stdin);                 //This is used to see the console
    freopen("CON", "w", stderr);

    /*Initialisation of FMOD*/
    soundBox sB;
    FMOD_System_Create(&sB.system);
    FMOD_System_Init(sB.system, 64, FMOD_INIT_NORMAL, NULL);
    FMOD_System_CreateSound(sB.system, "menuMusic.wav", FMOD_LOOP_NORMAL | FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, 0, &sB.sMenuMusic); //Here we create a stream cause the music is long
    FMOD_System_CreateSound(sB.system, "sLose.wav", FMOD_LOOP_NORMAL | FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, 0, &sB.sLoseMusic);
    FMOD_System_CreateSound(sB.system, "sWin.wav", FMOD_LOOP_NORMAL | FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, 0, &sB.sWinMusic);
    FMOD_System_CreateSound(sB.system, "SoundIn.wav", FMOD_CREATESAMPLE, 0, &sB.sIn);
    FMOD_System_CreateSound(sB.system, "sFace.wav", FMOD_CREATESAMPLE, 0, &sB.sFace);
    FMOD_System_CreateSound(sB.system, "sAnimLose.wav", FMOD_CREATESAMPLE, 0, &sB.sAnimLoseMusic);
    FMOD_System_CreateSound(sB.system, "sAnimWin.wav", FMOD_CREATESAMPLE, 0, &sB.sAnimWinMusic);
    FMOD_Sound_SetLoopCount(sB.sMenuMusic, -1);
    FMOD_Sound_SetLoopCount(sB.sWinMusic, -1);
    FMOD_Sound_SetLoopCount(sB.sLoseMusic, -1);

    /*Initialisation of FMOD over*/

    /*Initialisation of SDL_ttf*/

    TTF_Init();

    Text tWidth, tHeight, tQExit, tTitle, tStart, tExit;

    initTextMain(&tTitle, &tStart, &tExit);

    /*Initialisation of SDL_ttf over*/

    /*Initialisation of SDL*/

    SDL_Surface *screen = SDL_SetVideoMode(1920, 1080, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    SDL_WM_SetCaption("Picross", NULL);

    /*Initialisation of SDL over*/

    /*Initialisation of the game board*/

    srand(time(NULL));

    GameBoard mainBoard;

    /*Initialisation of the arrays over*/

    /*Keyboard and mouse input initialisation*/

    Input in;

    memset(&in,0,sizeof(in));   //Puts everything in our structure at 0

    /*Keyboard and mouse input initialisation over*/

    /*Timer initialization*/

    mainBoard.glitch.pos.x = 0;
    mainBoard.glitch.pos.y = 0;
    mainBoard.glitch.screen = screen;
    char filename[30] = "";
    for(unsigned short i = 0; i < 10; i++)
    {
        sprintf(filename, "sprites/glitch%hi.bmp", i+1);                //Here we use this to simplify the selection of a random image (cause the image are called image1, image2, image3...
        mainBoard.glitch.surface[i] = SDL_LoadBMP(filename);                      //So it would be really long to write everything
    }
    mainBoard.glitch.screen = screen;                                             //We have to have our screen and sound box inside the structure
    mainBoard.glitch.system = sB.system;
    FMOD_System_CreateSound(sB.system, "sGlitch1.wav", FMOD_CREATESAMPLE, 0, &mainBoard.glitch.sGlitch[0]);       //We initialize our 3 sounds
    FMOD_System_CreateSound(sB.system, "sGlitch2.wav", FMOD_CREATESAMPLE, 0, &mainBoard.glitch.sGlitch[1]);
    FMOD_System_CreateSound(sB.system, "sGlitch3.wav", FMOD_CREATESAMPLE, 0, &mainBoard.glitch.sGlitch[2]);
    mainBoard.timer = SDL_AddTimer(5000,startGlitch, &mainBoard.glitch);          //We start our timer which will call back the function multiple time and automatically
                                                                        //We need to keep the timer in our mainBoard structure to then be able to stop it
    /*Timer initialization over*/

    /*Init menu animator*/

    Animator mSans[2];
    mSans[0].currSprite = 0;
    mSans[0].surface[0] = SDL_LoadBMP("sprites/main1.bmp");
    mSans[0].surface[1] = SDL_LoadBMP("sprites/main2.bmp");
    mSans[0].pos[0].x = LF/2 - mSans[0].surface[0]->w*2;
    mSans[0].pos[0].y = HF/8 - 6 - mSans[0].surface[0]->h/2;
    mSans[0].pos[1].x = LF/2 - mSans[0].surface[0]->w*2;
    mSans[0].pos[1].y = HF/8 - mSans[0].surface[0]->h/2;
    mSans[1].currSprite = 1;
    mSans[1].surface[0] = SDL_LoadBMP("sprites/main1.bmp");
    mSans[1].surface[1] = SDL_LoadBMP("sprites/main2.bmp");
    mSans[1].pos[0].x = LF/2 + mSans[1].surface[0]->w;
    mSans[1].pos[0].y = HF/8 - 6 - mSans[1].surface[0]->h/2;
    mSans[1].pos[1].x = LF/2 + mSans[1].surface[0]->w;
    mSans[1].pos[1].y = HF/8 - mSans[1].surface[0]->h/2;

    /*Init menu animation over*/


    FMOD_System_PlaySound(sB.system, FMOD_CHANNEL_FREE, sB.sMenuMusic, 0, NULL);    //We play the music of the menu

    while(1)
    {
        memset(&in,0,sizeof(in));
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));  //This prints a blackscreen to clean the previous one
        UpdateEvents(&in);                                              //This handles input
        printText(&tTitle, screen);
        printText(&tExit, screen);
        printText(&tStart, screen);
        if(checkIfClicked(&tStart, &in))
        {
            playIn(&sB);
            menuDifficulty(screen, &mainBoard, &sB);
        }
        else if(checkIfClicked(&tExit, &in))
        {
            exitMenu(screen);
        }
        animSansSteady(&mSans[0], screen);
        animSansSteady(&mSans[1], screen);
        SDL_Flip(screen);
    }

    /*Main working game loop and functions over*/

    return 0;
}
