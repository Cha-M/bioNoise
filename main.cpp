#include <algorithm>
#include <SDL.h>
#include <vector>
#include <thread>
#include <windows.h>

using std::vector;

int scrHeight = GetSystemMetrics(SM_CYSCREEN);
int scrWidth = GetSystemMetrics(SM_CXSCREEN);

int wRands = scrWidth/250;
int hRands = scrHeight/250;

vector<vector<vector<int>>> ten_rand_xyz (wRands, vector<vector<int>>(hRands, vector<int>(100, 0)));
float randsSizeX = float(wRands);
float randsSizeY = float(hRands);
float randsSizeZ = float(ten_rand_xyz[0][0].size());

int seed_rand()
{
    return rand() % 255;
}

float fromWave3d(float axisPositionX, float axisPositionY, float axisPositionZ, vector<vector<vector<int>>> rands)
{
    //linear interpolation formula

    float x = axisPositionX*randsSizeX;
    float ax1m = std::floor(axisPositionX*randsSizeX);
    float ax2 = ax1m+1.0;
    float ax2m = fmod(ax2, randsSizeX);

    float y = axisPositionY*randsSizeY;
    float ay1m = std::floor(axisPositionY*randsSizeY);
    float ay2 = ay1m+1.0;
    float ay2m = fmod(ay2, randsSizeY);

    float z = axisPositionZ*randsSizeZ;
    float az1m = std::floor(axisPositionZ*randsSizeZ);//fmod creates wrap around
    float az2 = az1m+1.0;
    float az2m = fmod(az2, randsSizeZ);

    float x1y1z1 = rands[ax1m][ay1m][az1m];
    float x2y1z1 = rands[ax2m][ay1m][az1m];
    float xy1z1 = ((ax2 - x)) * x1y1z1 + ((x - ax1m)) * x2y1z1;

    float x1y2z1 = rands[ax1m][ay2m][az1m];
    float x2y2z1 = rands[ax2m][ay2m][az1m];
    float xy2z1 = ((ax2 - x)) * x1y2z1 + ((x - ax1m)) * x2y2z1;

    float x1y1z2 = rands[ax1m][ay1m][az2m];
    float x2y1z2 = rands[ax2m][ay1m][az2m];
    float xy1z2 = ((ax2 - x)) * x1y1z2 + ((x - ax1m)) * x2y1z2;

    float x1y2z2 = rands[ax1m][ay2m][az2m];
    float x2y2z2 = rands[ax2m][ay2m][az2m];
    float xy2z2 = ((ax2 - x)) * x1y2z2 + ((x - ax1m)) * x2y2z2;

    float xyz1 = ((ay2 - y)) * xy1z1 + ((y - ay1m)) * xy2z1;
    float xyz2 = ((ay2 - y)) * xy1z2 + ((y - ay1m)) * xy2z2;

    float xyz = ((az2 - z)) * xyz1 + ((z - az1m)) * xyz2;

    return xyz;

}

int SCREEN_WIDTH;
int SCREEN_HEIGHT;

void randomStart()
{
    srand(time(NULL));
    int i = 0;
    for (vector<vector<int>> & r : ten_rand_xyz)
    {
        for (vector<int> & s : r)
        {
            for (int & t : s)
            {
                t = seed_rand();
                i++;
            }
        }
    }
}

//The window we'll be rendering
SDL_Window* chWindow = NULL;
SDL_Surface* chImageSurface = NULL;
SDL_Surface* chImageSurfaceB = NULL;
SDL_Surface* allFrames = NULL;
SDL_Rect position;//not initialising this one as a pointer
SDL_Surface* chScreenSurface = NULL;
SDL_Renderer *chRender = NULL;
SDL_Texture *chTex = NULL;
SDL_Texture *chTexB = NULL;
SDL_Event chEvent;

float pi = atan(1)*4;
float pp = 16/pi;

class pixValClass
{
    public:
    Uint32 pix;
    std::vector<Uint32> pixThr = {0,0,0,0,0,0};
    std::vector<Uint32> pixThrC = {0,0,0,0,0,0};
    std::vector<Uint32> pixThrM0 = {0,0,0,0,0,0};
    std::vector<Uint32> pixThrM1 = {0,0,0,0,0,0};
    Uint8 r8;
    std::vector<Uint8> r8ta = {0,0,0,0,0,0};
    Uint8 g8;
    std::vector<Uint8> g8ta = {0,0,0,0,0,0};
    Uint8 b8;
    std::vector<Uint8> b8ta = {0,0,0,0,0,0};
    Uint8 a8;
    std::vector<Uint8> a8ta = {0,0,0,0,0,0};
    Uint8 v8;
    std::vector<Uint8> v8ta = {0,0,0,0,0,0};
    float zR;
    std::vector<float> zRta = {0,0,0,0,0,0};
    float zG;
    std::vector<float> zGta = {0,0,0,0,0,0};
    float zB;
    std::vector<float> zBta = {0,0,0,0,0,0};
    float zA;
    std::vector<float> zAta = {0,0,0,0,0,0};
    float zM;
    std::vector<float> zMta = {0,0,0,0,0,0};

    Uint8 convR;
    std::vector<Uint8> convRta = {0,0,0,0,0,0};
    Uint8 convG;
    std::vector<Uint8> convGta = {0,0,0,0,0,0};
    Uint8 convB;
    std::vector<Uint8> convBta = {0,0,0,0,0,0};
    Uint8 convA;
    std::vector<Uint8> convAta = {0,0,0,0,0,0};
    Uint32 * pixels;
    int x;
    int y;
    std::thread thrA;
    std::thread thrB;
    std::thread thrC;
    std::thread thrD;

}pixVal;


class colourParametersClass
{
    public:
    float colour;
    int increment;
    int speed = 1;//, 4, 1.6, 0, 128);//256/4 = 64
    float piMulti = 1.6;
    int piMulti10 = 16;
    float gradPiMulti = 1.6;
    float gradPiMultiR = 1.6;
    float gradPiMultiG = 1.6;
    float gradPiMultiB = 1.6;
    float gradPiMultiA = 1.6;
    int offSet = 0;
    int offSet0 = 0;
    int offSet1 = 2;
    int offSet2 = 4;
    int modiR = 0;
    int modiG = 0;
    int modiB = 0;
    int delay = 30;
    int height = 128;
    float transWeight = 1.2;

}colourParameters;

int chIncrement = 0;//6530;

void incrementUp1 ()
    {
        if (chIncrement < 6545) { chIncrement++; }
        else {chIncrement = 1445;}

    }

class coordinates
{
    public:
    int x;
    int y;
    int z;
    std::vector<int> xThr = {0,0,0,0,0,0};
    std::vector<int> yThr = {0,0,0,0,0,0};
    std::vector<int> zThr = {0,0,0,0,0,0};
    std::vector<float> xThrEffect = {0,0,0,0,0,0};
    std::vector<float> yThrEffect = {0,0,0,0,0,0};
    std::vector<float> zThrEffect = {0,0,0,0,0,0};

}co;

float bhaskaraSine (float x)
{
    x = fmod(x, 6.28318531f);
    float sinn;

    if (x < -3.14159265f) {x += 6.28318531f;}
    else if (x >  3.14159265f) {x -= 6.28318531f;}

    //compute sine
    if (x < 0) {sinn = 1.27323954f * x + .405284735f * x * x;}
    else {sinn = 1.27323954f * x - 0.405284735f * x * x;}

    return sinn;

}

Uint8 sineColour8UpperLimit( float colour, int increment, int speed, float piMulti, int offSet, int height)
{

    colour = bhaskaraSine((((colour+increment*speed)/255)  *  pi*piMulti + (offSet*pi/3))) * height + (height-1);

    if (colour > 255) {colour = 255;}
    if (colour < 0) {colour = 0;}

    Uint8 colour8 = Uint8(colour);
    return colour8;
}

void zColours(int z)
{
    pixVal.zRta[z] = pixVal.v8ta[z];
    pixVal.zGta[z] = pixVal.v8ta[z];
    pixVal.zBta[z] = pixVal.v8ta[z];
    pixVal.zAta[z] = pixVal.v8ta[z];
    pixVal.zMta[z] = pixVal.v8ta[z];
}

void convertColours(int z)
{
    pixVal.convRta[z] = sineColour8UpperLimit(pixVal.zRta[z], chIncrement, colourParameters.speed, colourParameters.gradPiMulti, colourParameters.offSet0, colourParameters.height+colourParameters.modiR);
    pixVal.convGta[z] = sineColour8UpperLimit(pixVal.zGta[z], chIncrement, colourParameters.speed, colourParameters.gradPiMulti, colourParameters.offSet1, colourParameters.height+colourParameters.modiG);//need 3 different height vars
    pixVal.convBta[z] = sineColour8UpperLimit(pixVal.zBta[z], chIncrement, colourParameters.speed, colourParameters.gradPiMulti, colourParameters.offSet2, colourParameters.height+colourParameters.modiB);//-50*pixVal.sinIncr)
    pixVal.convAta[z] = sineColour8UpperLimit(pixVal.zAta[z], chIncrement, colourParameters.speed, colourParameters.gradPiMulti, 4, colourParameters.height);
}

void bakePixel(int z)
{
    pixVal.pixThr[z] = (0xFF << 24) | (pixVal.convRta[z] << 16) | (pixVal.convGta[z] << 8) | pixVal.convBta[z];
}

void vNoise(int z)
{
        co.xThrEffect[z] = (fmod((co.xThr[z]), SCREEN_WIDTH) / float(SCREEN_WIDTH));
        co.yThrEffect[z] = (co.yThr[z] / float(SCREEN_HEIGHT));
        co.zThrEffect[z] = chIncrement % ((512/colourParameters.speed)-1) * (1.0/((512/colourParameters.speed)));

        pixVal.v8ta[z] = fromWave3d(co.xThrEffect[z], co.yThrEffect[z], co.zThrEffect[z], ten_rand_xyz);//*255.0;

        zColours(z);

        pixVal.convRta[z] = pixVal.zRta[z];
        pixVal.convGta[z] = pixVal.zGta[z];
        pixVal.convBta[z] = pixVal.zBta[z];
        pixVal.convAta[z] = pixVal.zAta[z];
        bakePixel(z);

}

void waveOnly(int z)
{
    zColours(z);
    convertColours(z);
    bakePixel(z);
}

void colourThread(int z, int over4)
{
    int maxY = z*over4;
    for (co.yThr[z]= z*over4-over4; co.yThr[z] < maxY; co.yThr[z] += 1)
    //iterates over y
    {
        for (co.xThr[z] = 0; co.xThr[z] < chImageSurface->w; co.xThr[z] += 1)
        {
            pixVal.v8ta[z] = pixVal.pixThr[z] & 0xFF;
            vNoise(z);
            waveOnly(z);
            pixVal.pixels[co.yThr[z] * chImageSurface->w + co.xThr[z]] = pixVal.pixThr[z];
        }
    }
}

int displayI()
    {

            while(SDL_PollEvent(&chEvent))
            {
                switch(chEvent.type)
                {
                    case SDL_KEYDOWN:
                        return 0;
                        SDL_Quit();

                    case SDL_MOUSEBUTTONDOWN:
                        return 0;
                        SDL_Quit();
                }
            }

            co.xThr[0],co.xThr[1],co.xThr[2],co.xThr[3],co.xThr[4],co.xThr[5],
            co.yThr[0],co.yThr[1],co.yThr[2],co.yThr[3],co.yThr[4],co.yThr[5]
            = 0;
            int over4 = chImageSurface->h/4;

            std::thread thrA (colourThread, 1, over4);
            std::thread thrB (colourThread, 2, over4);
            std::thread thrC (colourThread, 3, over4);
            std::thread thrD (colourThread, 4, over4);

            thrA.join();
            thrB.join();
            thrC.join();
            thrD.join();

            SDL_UpdateTexture(chTex, NULL, chImageSurface->pixels,chImageSurface->w * sizeof(Uint32));

            incrementUp1();

            SDL_RenderCopy( chRender, chTex, NULL, NULL );
            SDL_RenderPresent( chRender );
            SDL_Delay( colourParameters.delay );

            SDL_BlitSurface(chImageSurfaceB, NULL, chImageSurface, NULL);

            displayI();
    }

int main (int argc, char *argv[])
{
    randomStart();
    setlocale(LC_ALL, "en_UK.UTF-8");

    chImageSurface = SDL_CreateRGBSurface(0, scrWidth/8, scrHeight/8, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    chImageSurfaceB = SDL_CreateRGBSurface(0, scrWidth/8, scrHeight/8, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

    SCREEN_HEIGHT = chImageSurface->h;
    SCREEN_WIDTH = chImageSurface->w;

    chWindow = SDL_CreateWindow( "bioNoise", SDL_WINDOWPOS_UNDEFINED, 400, scrWidth, scrHeight, SDL_WINDOW_SHOWN );
    chRender = SDL_CreateRenderer(chWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    chImageSurface = SDL_ConvertSurfaceFormat(chImageSurface, SDL_PIXELFORMAT_ARGB8888, 0);
    chScreenSurface = SDL_GetWindowSurface( chWindow );

    chTex = SDL_CreateTexture(chRender,SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC,chImageSurface->w, chImageSurface->h);
    chTexB = SDL_CreateTexture(chRender,SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC,chImageSurface->w, chImageSurface->h);
    SDL_SetTextureBlendMode(chTex, SDL_BLENDMODE_BLEND);

    SDL_UpdateTexture(chTex, NULL, chImageSurface->pixels, chImageSurface->w * sizeof(Uint32));
    pixVal.pixels = (Uint32 *)chImageSurface->pixels;

    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetWindowFullscreen(chWindow, SDL_WINDOW_FULLSCREEN);
    displayI();

  return 0;
}
