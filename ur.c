#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_scancode.h>
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

#define DRAW_TYPE 1
#define DEL_TYPE 2

typedef struct Coordinate {
  int x;
  int y;
} Coordinate;

// draw type 1, del type 2
typedef struct Value {
  Coordinate coor;
  int type;
} Value;

typedef struct Stack {
  Value value;
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

void push(Stack **head, Value data) {
  Stack *newNode = malloc(sizeof(Stack));
  newNode->value = data;
  newNode->next = *head;

  *head = newNode;
}

Value pop(Stack **head) {
  if (*head == NULL)
    return (Value){{0, 0}, -1};

  Stack *temp = *head;
  Value popValue = (Value)temp->value;

  *head = (*head)->next;
  free(temp);

  return popValue;
}

bool is_empty(Stack **head) { return *head == NULL; }

void clear_stack(Stack **head) {
  while (*head != NULL) {
    Stack *temp = *head;
    *head = (*head)->next;
    free(temp);
  }
}

int main() {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window =
      SDL_CreateWindow("Undo Redo Stack", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  SDL_Surface *surface = SDL_GetWindowSurface(window);

  int running = 1;
  SDL_Event event;
  const Uint8 *keyState = SDL_GetKeyboardState(NULL);

  Stack *undo = NULL;
  Stack *redo = NULL;

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
          clear_stack(&redo);
          push(&undo, (Value){{x, y}, DRAW_TYPE});
        }

        if (event.button.button == SDL_BUTTON_RIGHT) {
          draw_cell(surface, (Coordinate){x, y}, 1);
          push(&undo, (Value){{x, y}, DEL_TYPE});
        }
      }

      if (keyState[SDL_SCANCODE_LCTRL] && keyState[SDL_SCANCODE_Z]) {
        if (!is_empty(&undo)) {
          Value popped = pop(&undo);
          if (popped.type != -1) {
            draw_cell(surface, (Coordinate){popped.coor.x, popped.coor.y}, 1);
            push(&redo, (Value){{popped.coor.x, popped.coor.y}, popped.type});
          }
        } else {
          printf("no undo\n");
        }
      }

      if (keyState[SDL_SCANCODE_LCTRL] && keyState[SDL_SCANCODE_R]) {
        if (!is_empty(&redo)) {
          Value popped = pop(&redo);
          if (popped.type != -1) {
            if (popped.type == DRAW_TYPE) {
              draw_cell(surface, (Coordinate){popped.coor.x, popped.coor.y}, 0);
            } else {
              draw_cell(surface, (Coordinate){popped.coor.x, popped.coor.y}, 1);
            }
            push(&undo, (Value){{popped.coor.x, popped.coor.y}, popped.type});
          }
        } else {
          printf("no redo\n");
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