#!/bin/sh

set -xe

ffmpeg -i data/%d.ppm -vf "fps=3,scale=1000:-1:flags=lanczos" output.gif