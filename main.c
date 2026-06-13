#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 20
#define HEIGHT 20

typedef float Layer[HEIGHT][WIDTH];

static inline int clampi(int value, int min, int max) {
  return value < min ? min : value > max ? max : value;
}

// ppm image format rather than png image
void layer_save_as_ppm(Layer layer, const char *filename) {
  FILE *file = fopen(filename, "w");
  if (!file) {
    fprintf(stderr, "Failed to open file %s\n", filename);
    return;
  }
  fprintf(file, "P3\n%d %d\n255\n", WIDTH, HEIGHT);
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      fprintf(file, "%d %d %d\n", (int)(layer[y][x] * 255),
              (int)(layer[y][x] * 255), (int)(layer[y][x] * 255));
    }
  }
}

void layer_fill_rect(Layer layer, int x, int y, int w, int h, float value) {

  assert(w > 0 && h > 0);
  int x0 = clampi(x, 0, WIDTH - 1);
  int y0 = clampi(y, 0, HEIGHT - 1);
  int x1 = clampi(x0 + w - 1, 0, WIDTH - 1);
  int y1 = clampi(y0 + h - 1, 0, HEIGHT - 1);

  for (int x = x0; x <= x1; x++) {
    for (int y = y0; y <= y1; y++) {
      layer[y][x] = value;
    }
  }
}

// apply weights to input and return the output
// the input in c will be come pointer of floot array
// pointer -> float *[WIDTH]
float infer(Layer input, Layer weights) {
  float output = 0.0f;

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      output += input[y][x] * weights[y][x];
    }
  }

  return output;
}

static Layer inputs;
static Layer weights;

int main(void) {
  errno = 69;
  printf("Error: %s\n", strerror(errno));
  return 0;
}

// only single output neuron
int main2(void) {
  printf("Hello, World!\n");

  layer_fill_rect(inputs, 0, 0, WIDTH / 2, HEIGHT / 2, 1.0f);

  float output = infer(inputs, weights);
  printf("Output: %f\n", output);

  return 0;
}