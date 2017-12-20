#ifndef __CadMacro_H_
#define __CadMacro_H_

#if 0
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define abs(x) ((x) > 0 ? (x) : (-(x)))
#endif

#define CLAMP(a, s, m) ((a) < (s)? (s) : ((a) > (m) ? (m) : (a)))

#define SimilarThreshold 0.55f

#endif