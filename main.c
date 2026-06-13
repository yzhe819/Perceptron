#include <assert.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 50
#define HEIGHT 50
#define PPM_SCALAR 25
#define BIAS 10
#define SAMPLE_SIZE 500
#define TRAIN_PASSES 100

typedef float Layer[HEIGHT][WIDTH];

static inline int clampi(int value, int min, int max) {
  return value < min ? min : value > max ? max : value;
}

static inline int rand_range(int low, int high) {
  assert(low < high);
  return rand() % (high - low) + low;
}

// ppm image format rather than png image
// use to make the layer human readable
void layer_save_as_ppm(Layer layer, const char *filename) {
  float min = FLT_MAX;
  float max = FLT_MIN;

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      if (layer[y][x] < min)
        min = layer[y][x];
      if (layer[y][x] > max)
        max = layer[y][x];
    }
  }

  FILE *file = fopen(filename, "wb");
  if (!file) {
    fprintf(stderr, "Failed to open file %s\n", filename);
    return;
  }
  fprintf(file, "P6\n%d %d\n255\n", WIDTH * PPM_SCALAR, HEIGHT * PPM_SCALAR);
  for (int y = 0; y < HEIGHT * PPM_SCALAR; y++) {
    for (int x = 0; x < WIDTH * PPM_SCALAR; x++) {
      // layer value -> based on the related different (max, min) -> scalar
      float s = (layer[y / PPM_SCALAR][x / PPM_SCALAR] - min) / (max - min);
      char pixel[3] = {(char)floorf(255 * (1.0f - s)), (char)floorf(255 * s),
                       0};
      fwrite(pixel, sizeof(pixel), 1, file);
    }
  }
  fclose(file);
}

void layer_save_as_bin(Layer layer, const char *filename) {
  FILE *file = fopen(filename, "wb");
  if (!file) {
    fprintf(stderr, "Failed to open file %s\n", filename);
    exit(1);
  }

  fwrite(layer, sizeof(Layer), 1, file);
  fclose(file);
}

// draw the rect
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

// draw the cicle
void layer_fill_circle(Layer layer, int cx, int cy, int r, float value) {
  assert(r > 0);
  int x0 = clampi(cx - r, 0, WIDTH - 1);
  int y0 = clampi(cy - r, 0, HEIGHT - 1);
  int x1 = clampi(cx + r, 0, WIDTH - 1);
  int y1 = clampi(cy + r, 0, HEIGHT - 1);
  for (int y = y0; y <= y1; ++y) {
    for (int x = x0; x <= x1; ++x) {
      int dx = x - cx;
      int dy = y - cy;
      if (dx * dx + dy * dy <= r * r) {
        layer[y][x] = value;
      }
    }
  }
}

void layer_random_rect(Layer layer) {
  layer_fill_rect(layer, 0, 0, WIDTH, HEIGHT, 0.0f);
  int x = rand_range(0, WIDTH);
  int y = rand_range(0, HEIGHT);
  int w = WIDTH - x;
  if (w < 2)
    w = 2;
  w = rand_range(1, w);
  int h = HEIGHT - x;
  if (h < 2)
    h = 2;
  h = rand_range(1, h);
  layer_fill_rect(layer, x, y, w, h, 1.0f);
}

void layer_random_circle(Layer layer) {
  layer_fill_rect(layer, 0, 0, WIDTH, HEIGHT, 0.0f);
  int cx = rand_range(0, WIDTH);
  int cy = rand_range(0, HEIGHT);
  int r = INT_MAX;
  if (r > cx)
    r = cx;
  if (r > cy)
    r = cy;
  if (r > WIDTH - cx)
    r = WIDTH - cx;
  if (r > HEIGHT - cy)
    r = HEIGHT - cy;
  if (r < 2) // too small circle handle
    r = 2;
  r = rand_range(1, r);
  layer_fill_circle(layer, cx, cy, r, 1.0f);
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

// excited
void add_input_from_weights(Layer inputs, Layer weights) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      weights[y][x] += inputs[y][x];
    }
  }
}

// suppress
void sub_input_from_weights(Layer inputs, Layer weights) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      weights[y][x] -= inputs[y][x];
    }
  }
}

static Layer inputs;
static Layer weights;

int train_pass(Layer inputs, Layer weights) {
  int adjusted = 0;
  for (int i = 0; i < SAMPLE_SIZE; i++) {
    layer_random_rect(inputs);
    if (infer(inputs, weights) > BIAS) {
      sub_input_from_weights(inputs, weights);
      adjusted++;
    }

    layer_random_circle(inputs);
    if (infer(inputs, weights) < BIAS) {
      add_input_from_weights(inputs, weights);
      adjusted++;
    }
  }
  return adjusted;
}

// only single output neuron
int main(void) {
  for (int i = 0; i < TRAIN_PASSES; i++) {
    srand(69);
    int adjusted = train_pass(
        inputs,
        weights); // holy shit, the adjusted count will convergert to zero!!
    printf("%d\n", adjusted);
  }

  layer_save_as_ppm(weights, "weights.ppm");

  return 0;
}