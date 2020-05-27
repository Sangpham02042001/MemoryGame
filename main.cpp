#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <SDL.h>
#include <SDL_image.h>

#include "memory.h"

using namespace std;

//--------------------------------------------------//



int main(int argc, char* argv[])
{
    srand(time(0)); // Khoi tao seed cho ham rand
    int nRows = DEFAULT_NUM_ROWS,
        nCols = DEFAULT_NUM_COLS;
    Graphic graphic;
    if (!initGraphic(graphic, nRows, nCols)) {
        finalizeGraphic(graphic);
        return EXIT_FAILURE;
    }
    mix_click = Mix_LoadWAV("mix_doodle.mp3");
    mix_game = Mix_LoadWAV("music.mp3");
    mix_win = Mix_LoadWAV("win_music.mp3");
    random(nRows, nCols);
    initGame(graphic, nRows, nCols);
    SDL_Event event;
    while (Time < 200)
    {
        update(graphic, ++Time, 0, 0, 0);
        Mix_PlayChannel(-1,mix_game,0);
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                ClickAction(event, graphic);
            }
        }
        if (dem == 36) break;
    }
    if (dem < 36)
    {
        SDL_RenderClear(graphic.renderer);
        graphic.MemoryTexture = createTexture(graphic.renderer, LOST);
        SDL_RenderCopy(graphic.renderer, graphic.MemoryTexture, 0, 0);
        SDL_RenderPresent(graphic.renderer);
    }
    else
    {
        SDL_RenderClear(graphic.renderer);
        graphic.MemoryTexture = createTexture(graphic.renderer, WIN);
        SDL_RenderCopy(graphic.renderer, graphic.MemoryTexture, 0, 0);
        SDL_RenderPresent(graphic.renderer);
        Mix_PlayChannel(-1,mix_win,0);
    }
    SDL_Delay(2000);
    finalizeGraphic(graphic);
    return EXIT_SUCCESS;
}


bool initGraphic(Graphic& g, int nRows, int nCols)
{
    g.window = NULL;
    g.renderer = NULL;
    g.MemoryTexture = NULL;
    int sdlFlags = SDL_INIT_VIDEO;  // khoi tao he thong con su kien
    if (SDL_Init(sdlFlags) != 0) // khoi tao thu vien SDL, tra ve 0 neu thanh cong
    {
        err("SDL could not initialize!");
        return false;
    }
    int imgFlags = IMG_INIT_PNG;    // dung SDL_image de load anh
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        err("SDL_Image could not initialize!");
        return false;
    }
    g.window = SDL_CreateWindow(SCREEN_TITLE.c_str(),       // tieu de Memory
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        nCols * WINDOW_CELL_SIZE,
        nRows * WINDOW_CELL_SIZE + MEMORY_CELL_TIME,
        SDL_WINDOW_SHOWN);

    if (g.window == NULL)
    {
        err("Window could not be created!");
        return false;
    }
    g.renderer = SDL_CreateRenderer(g.window, -1, SDL_RENDERER_ACCELERATED);
    if (g.renderer == NULL)
    {
        err("Renderer could not be created!");
        return false;
    }
    g.MemoryTexture = createTexture(g.renderer, MEMORY_PATH);
    if (g.MemoryTexture == NULL)
    {
        err("Unable to create texture from " + MEMORY_PATH + "!");
        return false;
    }
    if(Mix_OpenAudio(22050,MIX_DEFAULT_FORMAT,2 , 4096) == -1) return false;
    initMemoryRects(g.MemoryRects);
    return true;
}


void finalizeGraphic(Graphic& g) {
    SDL_DestroyTexture(g.MemoryTexture);
    SDL_DestroyRenderer(g.renderer);
    SDL_DestroyWindow(g.window);
    IMG_Quit();
    SDL_Quit();
}


SDL_Texture* createTexture(SDL_Renderer* renderer, const string& path)
{
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (surface == NULL)
    {
        err("Unable to load image " + path + "!");
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}


void initMemoryRects(vector<SDL_Rect>& rects)
{
    SDL_Rect rect;
    for (long i = 1; i <= 2; ++i)
    {
        for (long j = 0; j < 5; ++j)
        {
            rect = { j * MEMORY_CELL_SIZE,(i == 1 ? MEMORY_CELL_TIME : MEMORY_CELL_TIME + MEMORY_CELL_SIZE),MEMORY_CELL_SIZE,MEMORY_CELL_SIZE };
            rects.push_back(rect);
        }
    }
    rect = { 0,0,1,MEMORY_CELL_TIME };        // thanh do : thoi gian con lai
    rects.push_back(rect);
    rect = { 150,0,1,MEMORY_CELL_TIME };      // thanh trang : thoi gian da troi qua
    rects.push_back(rect);
    rect = { 0,MEMORY_CELL_TIME + 2 * MEMORY_CELL_SIZE,MEMORY_CELL_SIZE,MEMORY_CELL_SIZE }; // o den : mo dc 2 o giong nhau
    rects.push_back(rect);
    // rect co 9 anh Pokemon + mat cuoi + thanh do + thanh trang + o den
}


void random(int nRows, int nCols)
{   // mang Pos danh dau cac o da dung
    // mang arr luu vi tri
    int valmax = nRows * nCols;
    int arr[valmax];
    for (int i = 0; i < valmax; ++i)
    {
        arr[i] = rand() % valmax;
        if (Pos[arr[i]])
        {
            for (int j = 0; j < valmax; ++j)
            {
                if (Pos[j]==0)
                {
                    arr[i] = j;
                    break;
                }
            }
        }
        Pos[arr[i]] = 1;
    }
    for (int i = 0; i < valmax; ++i)
    {
        Pos[arr[i]] = i;
    }
    // ket thuc ham, mang Pos co gia tri cac phan tu tu 0 den 35( random vi tri) vaf k co gia tri bi lap lai
}


void initGame(Graphic& g, int nRows, int nCols)
{
    SDL_RenderClear(g.renderer);      // Xoa nhung gi da ve
    SDL_Rect rect;
    for (int i = 0; i < nCols * MEMORY_CELL_SIZE; ++i)  // lay thanh do thoi gian
    {
        rect = { i,0,1,MEMORY_CELL_TIME };            // vị trí hiện time trên của sổ
        SDL_RenderCopy(g.renderer, g.MemoryTexture,
                        &g.MemoryRects[10] ,     // 10 là vị trí thanh đỏ trong MemoryRect
                         &rect);
    }
    for (int i = 1; i <= nRows; ++i)
    {
        for (int j = 1; j <= nCols; ++j)
        {
            int val = (i - 1) * nRows + j - 1; // val tu 0 den 35
            rect = { (j - 1) * MEMORY_CELL_SIZE,MEMORY_CELL_TIME + (i - 1) * MEMORY_CELL_SIZE,MEMORY_CELL_SIZE,MEMORY_CELL_SIZE };
            SDL_RenderCopy(g.renderer, g.MemoryTexture,
                            &g.MemoryRects[Pos[val] / 4],  // co 9 shinh anh Pokemon
                            &rect);
        }
    }
    SDL_RenderPresent(g.renderer);    // Hien thi nhung gi da ve
    SDL_Delay(2000); // Hien thi tat ca cac anh trong 2s de ghi nho
}


void update(Graphic& g, int time, int NumberOfClick, int value, int Pre)
{
    SDL_RenderClear(g.renderer);      // Xoa nhung gi da ve
    SDL_Rect rect;
    int nRows = DEFAULT_NUM_ROWS,
        nCols = DEFAULT_NUM_COLS;
    for (int i = 0; i < nCols * MEMORY_CELL_SIZE; ++i)
    {
        rect = { i,0,1,MEMORY_CELL_TIME };
        SDL_RenderCopy(g.renderer, g.MemoryTexture,
            &g.MemoryRects[i >= time ? 10 : 11]      // 10 la vi tri thanh do trong MemoryRect và 11 la vi tri thanh trang trong MemoryRect
            , &rect);
    }
    for (int i = 1; i <= nRows; ++i)
    {
        for (int j = 1; j <= nCols; ++j)
        {
            int val = (i - 1) * nRows + j - 1, k = 0;
            if (( NumberOfClick == 1 ||  NumberOfClick == 2) && val == value) k = 1; // click lan 1 => hien va  2 giong nhau se hien ca 2
            if ( NumberOfClick == 2 && val == Pre ) k = 1; // click lan 2 => hien
            if (Opened[val] != 0) k = 2;
            rect = { (j - 1) * MEMORY_CELL_SIZE,MEMORY_CELL_TIME + (i - 1) * MEMORY_CELL_SIZE,MEMORY_CELL_SIZE,MEMORY_CELL_SIZE };
            SDL_RenderCopy(g.renderer, g.MemoryTexture,
                           &g.MemoryRects[k == 2 ? 12 : k == 1 ? Pos[val] / 4 : 9] , // 12 la vi tri o den trong MemoryRect, 9 la hinh mat cuoi
                            &rect);
        }
    }
    SDL_RenderPresent(g.renderer);
    SDL_Delay(600);
}


void ClickAction(SDL_Event& event, Graphic& graphic)
{
    SDL_MouseButtonEvent mouse = event.button;
    if (mouse.y < MEMORY_CELL_TIME) update(graphic, ++Time, 0, 0, 0);// bam chuot vao thanh thoi gian => tat ca cac o la hinh mat cuoi va o da mo(neu co)
    else
    {
        Mix_PlayChannel(-1, mix_click, 0);
        int row = (mouse.y - MEMORY_CELL_TIME) / WINDOW_CELL_SIZE,
            col = mouse.x / WINDOW_CELL_SIZE;
        int value = row * 6 + col;
        if (Opened[value] || NowAction[value] )                 // bam chuot vao o da mo hoac vao cung 1 o 2 lan
        {
            update(graphic, ++Time, 0, value, PreviousValue);
        }
        else
        {
            click += 1;
            NowAction[value] = 1;
            if (click == 2)
            {
                if ((Pos[value] / 4) == (Pos[PreviousValue] / 4))
                {
                    update(graphic, ++Time, 2, value, PreviousValue);
                    Opened[value] = 1;
                    Opened[PreviousValue] = 1;
                    dem += 2;
                }
                else
                {
                    update(graphic, ++Time , 1, value, PreviousValue);
                    NowAction[PreviousValue] = 0;
                    NowAction[value] = 0;
                }
                click = 0;
            }
            else
            {
                PreviousValue = value;
                update(graphic, ++Time, 1, value, PreviousValue);
            }
        }
    }
}
void err(const string& m)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", m.c_str(), NULL);
}
