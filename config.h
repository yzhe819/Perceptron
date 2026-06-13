#ifndef CONFIG_H_
#define CONFIG_H_

#define WIDTH 80
#define HEIGHT 80

#define BIAS 20

#define PPM_SCALAR 25

#define SAMPLE_SIZE 500
#define TRAIN_PASSES 40

#define TRAIN_SEED 69
#define CHECK_SEED 420

typedef float Layer[HEIGHT][WIDTH];

#endif