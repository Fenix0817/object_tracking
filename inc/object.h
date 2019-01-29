#ifndef OBJECT_H
#define OBJECT_H


#include "global.h"

const float box_object_len = 4;
const float box_object_wid = 1.5;
const float box_object_len_large = 8;
const float box_object_wid_large = 2.5;


class Object
{
public:
    Object();

    uint64_t id;
};

class BoxObject : public Object
{
public:
    BoxObject();

    float rx;
    float ry;
    float vx;
    float vy;
    float ax;
    float ay;

    cv::Point2f corner[4];  // left-top, left-down, right-down, right-top in 2d
    Eigen::Quaternionf yaw;
};

class RadarObject : public Object
{
public:
    RadarObject();

    float r;
    float theta;
    float vt;
};

#endif // OBJECT_H