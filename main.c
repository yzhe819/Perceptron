#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 50
#define HEIGHT 50
#define PPM_SCALAR 25
#define SAMPLE_SIZE 10

typedef float Layer[HEIGHT][WIDTH];

static inline int clampi(int value, int min, int max) {
  return value < min ? min : value > max ? max : value;
}

// ppm image format rather than png image
// use to make the layer human readable
void layer_save_as_ppm(Layer layer, const char *filename) {
  FILE *file = fopen(filename, "wb");
  if (!file) {
    fprintf(stderr, "Failed to open file %s\n", filename);
    return;
  }
  fprintf(file, "P6\n%d %d\n255\n", WIDTH * PPM_SCALAR, HEIGHT * PPM_SCALAR);
  for (int y = 0; y < HEIGHT * PPM_SCALAR; y++) {
    for (int x = 0; x < WIDTH * PPM_SCALAR; x++) {
      // layer value -> scalar
      float s = layer[y / PPM_SCALAR][x / PPM_SCALAR];
      char pixel[3] = {(char)floorf(s * 255), 0, 0};
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
// static Layer weights;

int rand_range(int low, int high) {
  assert(low < high);
  return rand() % (high - low) + low;
}

// only single output neuron
int main(void) {
  char file_path[256];

  for (int i = 0; i < SAMPLE_SIZE; i++) {

    printf("[INFO] generating rect-%02d.bin\n", i);
    layer_fill_rect(inputs, 0, 0, WIDTH, HEIGHT, 0.0f);
    int x = rand_range(0, WIDTH);
    int y = rand_range(0, HEIGHT);
    int w = rand_range(1, WIDTH);
    int h = rand_range(1, HEIGHT);
    layer_fill_rect(inputs, x, y, w, h, 1.0f);
    snprintf(file_path, sizeof(file_path), "rect-%02d.bin", i);
    layer_save_as_bin(inputs, file_path);
    snprintf(file_path, sizeof(file_path), "rect-%02d.ppm", i);
    layer_save_as_ppm(inputs, file_path);
  }

  //   layer_fill_rect(inputs, 0, 0, WIDTH / 2, HEIGHT / 2, 1.0f);
  //   layer_save_as_ppm(inputs, "rect.ppm");
  //   layer_save_as_bin(inputs, "input.bin");

  //   layer_fill_circle(weights, WIDTH / 2, HEIGHT / 2, WIDTH / 2, 1.0f);
  //   layer_save_as_ppm(weights, "circle.ppm");
  //   float output = infer(inputs, weights);
  //   printf("Output: %f\n", output);

  printf("Hello, World!\n");
  return 0;
}