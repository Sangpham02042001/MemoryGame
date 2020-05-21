#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>

using namespace std;

const int MEMORY_CELL_SIZE = 50;   // kich thuoc anh goc
const int MEMORY_CELL_TIME = 20;   //
const int WINDOW_CELL_SIZE = 50;   // kich thuoc khi hien thi cua 1 o
const int DEFAULT_NUM_ROWS = 6;    // so luong o theo chieu ngang
const int DEFAULT_NUM_COLS = 6;    // so luong o theo chieu doc

const string SCREEN_TITLE = "MEMORY";   // Tieu de cua so
const string MEMORY_PATH = "image.jpg";     // duong dan den file anh
const string LOST = "lost.jpg";
const string WIN = "win.jpg";
//-------------------------------------------------//

int Pos[36], Time = -1, click = 0, NowAction[36], PreviousValue = 0, Opened[36], dem = 0;

struct Graphic
{
    SDL_Window* window;                 // Cua so hien thi
    SDL_Texture* MemoryTexture;         // Anh load dc tu file anh
    vector<SDL_Rect> MemoryRects;       // Vi tri cac hinh trong file anh
    SDL_Renderer* renderer;             // Load anh len man hinh
};

//------------------------------------------------------//

// Khoi tao SDL, SDL_Image, tao cua so, load anh, ...  tra ve false neu khoi
// tao khong thanh cong
bool initGraphic(Graphic& g, int nRows, int nCols);

// Huy khoi tao
void finalizeGraphic(Graphic& g);

// Load anh tu duong dan path, tra ve anh bieu dien duoi dangSDL_Texture
SDL_Texture* createTexture(SDL_Renderer* renderer, const string& path);

// Luu vi tri cac hinh trong anh vao tham so rects
void initMemoryRects(vector<SDL_Rect>& rects);

// random mang Pos[35], co gia tri cac phan tu tu 0 den 35
void random(int nRows, int nCols);

// tao trang thai ban dau cho game
void initGame(Graphic& g, int nRows, int nCols);

// cap nhat trang thai cho game
void update(Graphic& g, int time, int NumberOfClick, int value , int PreviousValue);

// su kien bam chuot
void ClickAction(SDL_Event& event, Graphic& raphic);

// hien thi cua so thong bao loi
void err(const string& msg);

#endif // MEMORY_H_INCLUDED
