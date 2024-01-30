//SET PATH=C:\mingw64\bin;%PATH%
//CD C:\Users\jules.botte-magalha\Documents\to_do_list
//gcc -std=c17 to_do_list.c -IC:\Users\jules.botte-magalha\Documents\to_do_list\SDL2\include -LC:\Users\jules.botte-magalha\Documents\to_do_list\SDL2\lib -Wall -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -o to_do_list

#include <stdio.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define WIDTH 900
#define HEIGHT 600
#define SIZE 200
#define FPS 60

typedef struct {
    char text[256];
    SDL_Rect rect;
    bool isDragging;
    int offsetX;
    int offsetY;
} Task;

bool isInsideRect(int x, int y, SDL_Rect rect) {
    return (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h);
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 0;
    }
    SDL_Window *wind = SDL_CreateWindow("To Do List",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        WIDTH, HEIGHT, 0);
    if (!wind) {
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer *rend = SDL_CreateRenderer(wind, -1, render_flags);
    if (!rend) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(wind);
        SDL_Quit();
        return 0;
    }
    bool running = true;
    int x_pos = 0;
    int y_pos = 0;
    SDL_Rect to_do = {x_pos, y_pos, WIDTH / 3, HEIGHT};
    SDL_Rect in_work = {x_pos + 300, y_pos, WIDTH / 3, HEIGHT};
    SDL_Rect done = {x_pos + 600, y_pos, WIDTH / 3, HEIGHT};
    SDL_Rect new_task = {x_pos + 700, y_pos + 525, 175, 40};
    SDL_Rect delete_button = {x_pos + 700, y_pos + 480, 175, 40};
    SDL_Event event;
    int debug = 0;
    TTF_Init();

    Task tasks[100];
    int numTasks = 0;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if (isInsideRect(event.button.x, event.button.y, new_task)) {
                            snprintf(tasks[numTasks].text, sizeof(tasks[numTasks].text), "Task %d", numTasks + 1);
                            tasks[numTasks].rect.x = 500;
                            tasks[numTasks].rect.y = 525;
                            tasks[numTasks].rect.w = 175;
                            tasks[numTasks].rect.h = 40;
                            tasks[numTasks].isDragging = false;
                            numTasks++;
                        } else if (isInsideRect(event.button.x, event.button.y, delete_button)) {
                            // Supprimer la tâche sélectionnée
                            for (int i = 0; i < numTasks; i++) {
                                if (isInsideRect(event.button.x, event.button.y, tasks[i].rect)) {
                                    // Décaler toutes les tâches suivantes vers le haut
                                    for (int j = i; j < numTasks - 1; j++) {
                                        tasks[j] = tasks[j + 1];
                                    }
                                    numTasks--;
                                    break;
                                }
                            }
                        } else {
                            for (int i = 0; i < numTasks; i++) {
                                if (isInsideRect(event.button.x, event.button.y, tasks[i].rect)) {
                                    tasks[i].isDragging = true;
                                    tasks[i].offsetX = event.button.x - tasks[i].rect.x;
                                    tasks[i].offsetY = event.button.y - tasks[i].rect.y;
                                }
                            }
                        }
                    }
                    break;

                case SDL_MOUSEMOTION:
                    for (int i = 0; i < numTasks; i++) {
                        if (tasks[i].isDragging) {
                            tasks[i].rect.x = event.motion.x - tasks[i].offsetX;
                            tasks[i].rect.y = event.motion.y - tasks[i].offsetY;
                        }
                    }
                    break;

                case SDL_MOUSEBUTTONUP:
                    for (int i = 0; i < numTasks; i++) {
                        tasks[i].isDragging = false;
                    }
                    break;



                default:
                    break;
            }
        }

        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        SDL_RenderClear(rend);

        SDL_SetRenderDrawColor(rend, 253, 89, 53, 255);
        SDL_RenderFillRect(rend, &to_do);
        SDL_SetRenderDrawColor(rend, 252, 174, 62, 255);
        SDL_RenderFillRect(rend, &in_work);
        SDL_SetRenderDrawColor(rend, 241, 238, 20, 255);
        SDL_RenderFillRect(rend, &done);
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        SDL_RenderFillRect(rend, &new_task);
        SDL_RenderFillRect(rend, &delete_button);

        TTF_Font *font = TTF_OpenFont("arial.ttf", 30);
        SDL_Color textColor = {255, 255, 255, 255};

        SDL_Surface *surface = TTF_RenderText_Solid(font, "To Do", textColor);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(rend, surface);

        int text_width = surface->w;
        int text_height = surface->h;
        SDL_FreeSurface(surface);

        SDL_Rect renderQuad = {100, y_pos + 15, text_width, text_height};
        SDL_RenderCopy(rend, texture, NULL, &renderQuad);

        SDL_Surface *surface2 = TTF_RenderText_Solid(font, "In Work", textColor);
        SDL_Texture *texture2 = SDL_CreateTextureFromSurface(rend, surface2);

        int text_width2 = surface2->w;
        int text_height2 = surface2->h;
        SDL_FreeSurface(surface2);

        SDL_Rect renderQuad2 = {400, y_pos + 15, text_width2, text_height2};
        SDL_RenderCopy(rend, texture2, NULL, &renderQuad2);

        SDL_Surface *surface3 = TTF_RenderText_Solid(font, "Done", textColor);
        SDL_Texture *texture3 = SDL_CreateTextureFromSurface(rend, surface3);

        int text_width3 = surface3->w;
        int text_height3 = surface3->h;
        SDL_FreeSurface(surface3);

        SDL_Rect renderQuad3 = {700, y_pos + 15, text_width3, text_height3};
        SDL_RenderCopy(rend, texture3, NULL, &renderQuad3);

        TTF_SetFontStyle(font, 20);
        SDL_Surface *surface4 = TTF_RenderText_Solid(font, "New Task", textColor);
        SDL_Texture *texture4 = SDL_CreateTextureFromSurface(rend, surface4);

        int text_width4 = surface4->w;
        int text_height4 = surface4->h;
        SDL_FreeSurface(surface4);

        SDL_Rect renderQuad4 = {705, 530, text_width4, text_height4};
        SDL_RenderCopy(rend, texture4, NULL, &renderQuad4);

        SDL_Surface *surface5 = TTF_RenderText_Solid(font, "Delete Task", textColor);
        SDL_Texture *texture5 = SDL_CreateTextureFromSurface(rend, surface5);

        int text_width5 = surface5->w;
        int text_height5 = surface5->h;
        SDL_FreeSurface(surface5);

        SDL_Rect renderQuad5 = {705, 485, text_width5, text_height5};
        SDL_RenderCopy(rend, texture5, NULL, &renderQuad5);

        int x;
        int y;
        SDL_GetMouseState(&x, &y);

        for (int i = 0; i < numTasks; i++) {
            SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
            SDL_RenderFillRect(rend, &tasks[i].rect);

            TTF_Font *taskFont = TTF_OpenFont("arial.ttf", 20);
            SDL_Color taskColor = {0, 0, 0, 255};

            SDL_Surface *taskSurface = TTF_RenderText_Solid(taskFont, tasks[i].text, taskColor);
            SDL_Texture *taskTexture = SDL_CreateTextureFromSurface(rend, taskSurface);

            int taskTextWidth = taskSurface->w;
            int taskTextHeight = taskSurface->h;
            SDL_FreeSurface(taskSurface);

            SDL_Rect taskRenderQuad = {tasks[i].rect.x + 10, tasks[i].rect.y + 5, taskTextWidth, taskTextHeight};
            SDL_RenderCopy(rend, taskTexture, NULL, &taskRenderQuad);

            TTF_CloseFont(taskFont);
            SDL_DestroyTexture(taskTexture);
        }


        SDL_RenderPresent(rend);
        SDL_Delay(1000 / FPS);
    }

    TTF_Quit();
    TTF_CloseFont(font);

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(wind);
    SDL_Quit();
    return 0;
}