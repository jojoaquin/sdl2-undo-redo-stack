#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600

#define BLACK 0x0000000
#define WHITE 0xffffffff
#define RED 0xff0000

#define CELL_SIZE 60
#define LINE_WIDTH 2

typedef struct Coordinate {
  int x;
  int y;
} Coordinate;

typedef struct Stack {
  Coordinate value;
  struct Stack *next;
} Stack;

// SDL
void draw_cell(SDL_Surface *surface, Coordinate data, bool del) {
  int x = data.x * CELL_SIZE;
  int y = data.y * CELL_SIZE;

  SDL_Rect rect = {x, y, CELL_SIZE, CELL_SIZE};
  if (del) {
    SDL_FillRect(surface, &rect, BLACK);
  } else {
    SDL_FillRect(surface, &rect, WHITE);
  }
}

void draw_grid(SDL_Surface *surface) {
  int row = SCREEN_HEIGHT / CELL_SIZE;
  int column = SCREEN_WIDTH / CELL_SIZE;
  for (size_t i = 0; i <= row; i++) {
    SDL_Rect line = {0, i * CELL_SIZE, SCREEN_WIDTH, LINE_WIDTH};
    SDL_FillRect(surface, &line, RED);
  }

  for (size_t i = 0; i <= column; i++) {
    SDL_Rect line = {i * CELL_SIZE, 0, LINE_WIDTH, SCREEN_HEIGHT};
    SDL_FillRect(surface, &line, RED);
  }
}

// Stack

void push(Stack **head, Coordinate data) {
  Stack *newNode = malloc(sizeof(Stack));
  newNode->value = data;
  newNode->next = *head;

  *head = newNode;
}

Coordinate pop(Stack **head) {
  if (head == NULL)
    return (Coordinate){0, 0};

  Stack *temp = *head;
  Coordinate popValue = (Coordinate)temp->value;

  *head = (*head)->next;
  free(temp);

  return popValue;
}

bool is_empty(Stack **head) { return *head == NULL; }

int main() {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window =
      SDL_CreateWindow("Undo Redo Stack", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  SDL_Surface *surface = SDL_GetWindowSurface(window);

  int running = 1;
  SDL_Event event;
  Uint32 mouseState = SDL_GetMouseState(NULL, NULL);

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = 0;
      }

      if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x = event.button.x / CELL_SIZE;
        int y = event.button.y / CELL_SIZE;

        if (event.button.button == SDL_BUTTON_LEFT) {
          draw_cell(surface, (Coordinate){x, y}, 0);
        }

        if (event.button.button == SDL_BUTTON_RIGHT) {
          draw_cell(surface, (Coordinate){x, y}, 1);
        }
      }
    }

    draw_grid(surface);

    SDL_UpdateWindowSurface(window);
    SDL_Delay(16);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}