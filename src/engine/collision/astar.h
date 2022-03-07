#ifndef _astar_h_
#define _astar_h_

bool astar(
      const float* weights, const int h, const int w,
      const int start, const int goal, 
      int* paths);

#endif
