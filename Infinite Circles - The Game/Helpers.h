bool Init(SDL_Window** ppFrame, SDL_Renderer** ppScene);

void ApplyTexture(SDL_Renderer* pFrame, SDL_Texture* pSrc, SDL_Rect* pSrcClp, int x, int y);

///loading a PNG and converting it to a texture. Unneccessary here
bool LoadTexture(std::string path, SDL_Texture** ppTextImg, SDL_Renderer* pRend);

bool CheckCollision(SDL_Rect A, SDL_Rect B);

Uint32 getPixel(SDL_Surface* pSurface, int x, int y);

void putPixel(SDL_Surface* pSurface, int x, int y, Uint32 pixel);
