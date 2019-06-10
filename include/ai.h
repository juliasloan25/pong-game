#ifndef __AI_H__
#define __AI_H__

#include "scene.h"
#include "body.h"
#include "constants.h"
#include <time.h>

typedef enum {
    EASY,
    MEDIUM,
    HARD,
} AiDifficulty;

void set_paddle_vel(Body *paddle, Body *ball, Vector axis);

double time_since_last_ai_tick(void);


#endif // #ifndef __AI_H__
