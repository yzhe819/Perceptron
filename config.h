#ifndef CONFIG_H_
#define CONFIG_H_

#define WIDTH 50
#define HEIGHT 50

#define BIAS 10

#define PPM_SCALAR 25

#define SAMPLE_SIZE 1000
#define TRAIN_PASSES 150

#define TRAIN_SEED 69
#define CHECK_SEED 420

typedef float Layer[HEIGHT][WIDTH];

#endif