#ifndef CONFIG_H_
#define CONFIG_H_

#define WIDTH 30
#define HEIGHT 30

#define BIAS 20

#define PPM_SCALAR 25

#define SAMPLE_SIZE 200
#define TRAIN_PASSES 50

#define TRAIN_SEED 69
#define CHECK_SEED 420

typedef float Layer[HEIGHT][WIDTH];

#endif