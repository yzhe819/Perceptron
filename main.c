#include <assert.h>
#include <stdio.h>

#define WIDTH 20
#define HEIGHT 20

typedef float Layer[HEIGHT][WIDTH];

static inline int clampi(int value, int min, int max) {
  return value < min ? min : value > max ? max : value;
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

// only single output neuron
int main(void) {
  printf("Hello, World!\n");

  float output = infer(inputs, weights);
  printf("Output: %f\n", output);

  return 0;
}