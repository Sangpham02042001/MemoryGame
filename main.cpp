#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <SDL.h>
#include <SDL_image.h>
using namespace std;

//--------------------------------------------------//
const int MEMORY_CELL_SIZE = 50;   // kích thước ảnh gốc
const int MEMORY_CELL_TIME = 20;   //
const int WINDOW_CELL_SIZE = 50;   // kích thước khi hiển thị của một ô
const int DEFAULT_NUM_ROWS = 6;    // Số lượng ô theo chiều ngang
const int DEFAULT_NUM_COLS = 6;    // Số lượng ô theo chiều dọc

const string SCREEN_TITLE = "MEMORY";   // Tiêu đề cửa sổ
const string MEMORY_PATH = "image.jpg";     // Đường dẫn đến file ảnh
const string LOST = "lost.jpg";
const string WIN = "win.jpg";
//-------------------------------------------------//

int Pos[36], Time = -1, click = 0, NowAction[36], PreviousValue = 0, Opened[36], dem = 0;

struct Graphic
{
    SDL_Window* window;                 // Cửa sổ hiển thị
    SDL_Texture* MemoryTexture;         // Ảnh load được từ file ảnh
    vector<SDL_Rect> MemoryRects;       // Vị trí các hình trong file ảnh
    SDL_Renderer* renderer;             // Dùng để vẽ hình vào cửa sổ
};

//------------------------------------------------------//

// Khởi tạo SDL, SDL_Image, tạo cửa sổ, load ảnh, ...  Trả về false nếu khởi
// tạo không thành công
bool initGraphic(Graphic& g, int nRows, int nCols);

// Huỷ khởi tạo những gì đã dùng từ SDL và SDL_Image
void finalizeGraphic(Graphic& g);

// Load ảnh từ đường dẫn path, trả về ảnh biểu diễn dưới dạng SDL_Texture
SDL_Texture* createTexture(SDL_Renderer* renderer, const string& path);

// Lưu vị trí các hình trong ảnh vào tham số rects
void initMemoryRects(vector<SDL_Rect>& rects);

// random mảng vt[36] có giá trị các phần tử từ 0 đến 35
void random(int nRows, int nCols);

// tạo trạng thái ban đầu cho game
void initGame(Graphic& g, int nRows, int nCols);

// cập nhât trạng thái cho game
void update(Graphic& g, int time, int NumberOfClick, int value , int PreviousValue);

// sự kiện bấm chuột
void ClickAction(SDL_Event& event, Graphic& raphic);

// Hiển thị cửa sổ thông báo lỗi
void err(const string& msg);


int main(int argc, char* argv[])
{
    srand(time(0)); // Khởi tạo seed cho hàm rand
    int nRows = DEFAULT_NUM_ROWS, //6
        nCols = DEFAULT_NUM_COLS; //6
    Graphic graphic;
    if (!initGraphic(graphic, nRows, nCols)) {
        finalizeGraphic(graphic);
        return EXIT_FAILURE;
    }
    random(nRows, nCols);
    initGame(graphic, nRows, nCols);
    SDL_Event event;
    while (Time < 300)
    {
        update(graphic, ++Time, 0, 0, 0);
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_MOUSEBUTTONDOWN) {    // Sự kiện click chuột
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
    }
    SDL_Delay(1000);
    finalizeGraphic(graphic);
    return EXIT_SUCCESS;
}

bool initGraphic(Graphic& g, int nRows, int nCols)
{
    g.window = NULL;
    g.renderer = NULL;
    g.MemoryTexture = NULL;
    int sdlFlags = SDL_INIT_VIDEO;  // khởi tạo hệ thống con sự kiện
    if (SDL_Init(sdlFlags) != 0) // khởi tạo thư viện SDL. trả về 0 khi thành công
    {
        err("SDL could not initialize!");
        return false;
    }
    int imgFlags = IMG_INIT_PNG;    // Dùng SDL_Image để load ảnh png
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        err("SDL_Image could not initialize!");
        return false;
    }
    g.window = SDL_CreateWindow(SCREEN_TITLE.c_str(),       // Tiêu đề MEMORY
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        nCols * WINDOW_CELL_SIZE,  // Chiều rộng
        nRows * WINDOW_CELL_SIZE + MEMORY_CELL_TIME, // Chiều cao
        SDL_WINDOW_SHOWN);          // Hiển thị

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
    SDL_Rect rt;
    for (long i = 1; i <= 2; ++i)
    {
        for (long j = 0; j < 5; ++j)
        {
            rt = { j * MEMORY_CELL_SIZE,(i == 1 ? MEMORY_CELL_TIME : MEMORY_CELL_TIME + MEMORY_CELL_SIZE),MEMORY_CELL_SIZE,MEMORY_CELL_SIZE };
            rects.push_back(rt);
        }
    }
    rt = { 0,0,1,MEMORY_CELL_TIME };        // thanh đỏ: thời gian còn lại
    rects.push_back(rt);
    rt = { 150,0,1,MEMORY_CELL_TIME };      //thanh trắng : thời gian đã trôi qua
    rects.push_back(rt);
    rt = { 0,MEMORY_CELL_TIME + 2 * MEMORY_CELL_SIZE,MEMORY_CELL_SIZE,MEMORY_CELL_SIZE }; // ô đen : đã mở đc 2 ô giống nhau
    rects.push_back(rt);
    // rects có 9 ảnh Pokemon + mặt cười + thanh đỏ + thanh trắng + ô đen
}


void random(int nRows, int nCols)
{   // mảng vt đánh dấu các ô đã dùng
    // mảng arr lưu vị trí
    int valmax = nRows * nCols; // 6*6 = 36
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
    // kết thúc hàm, mảng vt có giá trị các phần tử từ 0 đến 35 và không có giá trị nào bị lặp lại
}


void initGame(Graphic& g, int nRows, int nCols)
{
    SDL_RenderClear(g.renderer);      // Xoá những gì đã vẽ
    SDL_Rect rt;
    for (int i = 0; i < nCols * MEMORY_CELL_SIZE; ++i)  // lấy ảnh thanh đỏ thời gian
    {
        rt = { i,0,1,MEMORY_CELL_TIME };            // vị trí hiện time trên của sổ
        SDL_RenderCopy(g.renderer, g.MemoryTexture,
                        &g.MemoryRects[10] ,     // 10 là vị trí thanh đỏ trong MemoryRect
                         &rt);
    }
    for (int i = 1; i <= nRows; ++i)
    {
        for (int j = 1; j <= nCols; ++j)
        {
            int val = (i - 1) * nRows + j - 1; // val từ 0 đến 35.
            rt = { (j - 1) * MEMORY_CELL_SIZE,MEMORY_CELL_TIME + (i - 1) * MEMORY_CELL_SIZE,MEMORY_CELL_SIZE,MEMORY_CELL_SIZE };
            SDL_RenderCopy(g.renderer, g.MemoryTexture,
                            &g.MemoryRects[Pos[val] / 4],  // có 9 hình ảnh Pokemon
                            &rt);
        }
    }
    SDL_RenderPresent(g.renderer);    // Hiển thị những gì vữa vẽ
    SDL_Delay(2000); // hiển thị tất cả trong 2s để ghi nhớ
}


void update(Graphic& g, int time, int NumberOfClick, int value, int Pre)
{
    SDL_RenderClear(g.renderer);      // Xoá những gì đã vẽ
    SDL_Rect rt;
    int nRows = DEFAULT_NUM_ROWS,
        nCols = DEFAULT_NUM_COLS;
    for (int i = 0; i < nCols * MEMORY_CELL_SIZE; ++i)  // lấy ảnh thanh đỏ thời gian
    {
        rt = { i,0,1,MEMORY_CELL_TIME };            // vị trí hiện time trên của sổ
        SDL_RenderCopy(g.renderer, g.MemoryTexture,
            &g.MemoryRects[i >= time ? 10 : 11]      // 10 là vị trí thanh đỏ trong MemoryRect và 11 là vị trí thanh trắng trong MemoryRect
            , &rt);
    }
    for (int i = 1; i <= nRows; ++i)
    {
        for (int j = 1; j <= nCols; ++j)
        {
            int val = (i - 1) * nRows + j - 1, k = 0;
            if (( NumberOfClick == 1 ||  NumberOfClick == 2) && val == value) k = 1; // click lần 1 => hiện và 2 giống nhau sẽ hiện cả 2
            if ( NumberOfClick == 2 && val == Pre ) k = 1; // click lần 2 => hiện
            if (Opened[val] != 0) k = 2;
            rt = { (j - 1) * MEMORY_CELL_SIZE,MEMORY_CELL_TIME + (i - 1) * MEMORY_CELL_SIZE,MEMORY_CELL_SIZE,MEMORY_CELL_SIZE };
            SDL_RenderCopy(g.renderer, g.MemoryTexture,
                           &g.MemoryRects[k == 2 ? 12 : k == 1 ? Pos[val] / 4 : 9] , // 12 là vị trí ô đen trong MemoryRect , 9 là hình mặt cười
                            &rt);
        }
    }
    SDL_RenderPresent(g.renderer);    // Hiển thị những gì vữa vẽ
    SDL_Delay(300);
}


void ClickAction(SDL_Event& event, Graphic& graphic)
{
    SDL_MouseButtonEvent mouse = event.button;
    if (mouse.y < MEMORY_CELL_TIME) update(graphic, ++Time, 0, 0, 0);// bấm chuột vào thanh thời gian = > tất cả các ô là hình mặt cười và ô đã mở(nếu có)
    else
    {
        int row = (mouse.y - MEMORY_CELL_TIME) / WINDOW_CELL_SIZE,
            col = mouse.x / WINDOW_CELL_SIZE;
        int value = row * 6 + col;
        if (Opened[value] || NowAction[value] )                 // bấm chuột vào ô đã mở hoặc cùng 1 ô 2 lần
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



