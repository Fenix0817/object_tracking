#include "../inc/visualizer.h"

static cv::Point map2pixel(float x, float y)
{
    return cv::Point(y / map_scale + map_range_width / map_scale,
            map_range_length / map_scale - x / map_scale);
}

static cv::Point map2pixel(cv::Point2f pt)
{
    return map2pixel(pt.x, pt.y);
}

static void box_rot(const BoxObject obj, std::vector<cv::Point2f> &rotated_corner)
{
    for (auto pt : obj.corner)
    {
        cv::Point2f newpt;
        Eigen::Vector3f vec(pt.x, pt.y, 0);

//        printf("before rotation: %f, %f, ", vec(0), vec(1));
        vec = obj.yaw * vec;  // rotate by z axis
//        printf("after rotation: %f, %f\n", vec(0), vec(1));
        newpt.x = vec(0) + obj.rx;  // offset
        newpt.y = vec(1) + obj.ry;
        rotated_corner.push_back(newpt);
    }

    assert(4 == rotated_corner.size());
}

static void draw_obj(cv::Mat local_map, const BoxObject obj, cv::Scalar color)
{
    std::vector<cv::Point2f> rotated_corner;
    box_rot(obj, rotated_corner);

    cv::circle(local_map, map2pixel(obj.rx, obj.ry), 2, color, 2);
    cv::line(local_map, map2pixel(rotated_corner[0]), map2pixel(rotated_corner[1]), color, 2);
    cv::line(local_map, map2pixel(rotated_corner[0]), map2pixel(rotated_corner[3]), color, 2);
    cv::line(local_map, map2pixel(rotated_corner[2]), map2pixel(rotated_corner[1]), color, 2);
    cv::line(local_map, map2pixel(rotated_corner[2]), map2pixel(rotated_corner[3]), color, 2);
    cv::line(local_map, map2pixel(obj.rx, obj.ry), map2pixel((rotated_corner[0] + rotated_corner[3]) * 0.5), color, 2);
}

Visualizer::Visualizer()
{
}

void Visualizer::Init()
{
    local_map = cv::Mat::zeros(map_range_length / map_scale, map_range_width*2 / map_scale, CV_8UC3);
}

void Visualizer::DrawGT(const std::vector<BoxObject> &v, cv::Scalar color)
{
    for (auto obj : v)
    {
        draw_obj(local_map, obj, color);
    }
}

void Visualizer::DrawRadarObjs(const std::vector<RadarObject> &v, cv::Scalar color)
{
    for (auto obj : v)
    {
        float rx = obj.r * cos(obj.theta);
        float ry = obj.r * sin(obj.theta);
        cv::Point map_pt = map2pixel(rx, ry);

        map_pt.x = std::max(map_pt.x, 0);
        map_pt.x = std::min(map_pt.x, local_map.cols-1);
        map_pt.y = std::max(map_pt.y, 0);
        map_pt.y = std::min(map_pt.y, local_map.rows-1);

        cv::circle(local_map, map_pt, 2, color, 2);
//        ShowId(obj);
    }
}

void Visualizer::DrawLidarPts(const std::vector<LidarPoint> &v, cv::Scalar color)
{
    for (auto obj : v)
    {
        float rx = obj.rx;
        float ry = obj.ry;
        cv::Point map_pt = map2pixel(rx, ry);

        map_pt.x = std::max(map_pt.x, 0);
        map_pt.x = std::min(map_pt.x, local_map.cols-1);
        map_pt.y = std::max(map_pt.y, 0);
        map_pt.y = std::min(map_pt.y, local_map.rows-1);

        local_map.at<cv::Vec3b>(map_pt.y, map_pt.x)[0] = color[0];
        local_map.at<cv::Vec3b>(map_pt.y, map_pt.x)[1] = color[1];
        local_map.at<cv::Vec3b>(map_pt.y, map_pt.x)[2] = color[2];
    }
}

void Visualizer::DrawLidarObjs(const std::vector<BoxObject> &v, cv::Scalar color)
{
    for (auto obj : v)
    {
        draw_obj(local_map, obj, color);
        // ShowId(obj);
    }
}

void Visualizer::DrawFT(const std::vector<BoxObject> &v, cv::Scalar color)
{
    for (auto obj : v)
    {
        draw_obj(local_map, obj, color);
        ShowId(obj);
    }
}

void Visualizer::ShowId(const BoxObject &obj)
{
    cv::putText(local_map, std::to_string(obj.id), map2pixel(obj.rx, obj.ry),
                cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(255,255,255), 2);
}

void Visualizer::ShowId(const RadarObject &obj)
{
    float rx = obj.r * cos(obj.theta);
    float ry = obj.r * sin(obj.theta);
    cv::putText(local_map, std::to_string(obj.id), map2pixel(rx, ry),
                cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(255,255,255), 2);
}

void Visualizer::ShowMap() const
{
    cv::namedWindow("gtviser");
    cv::imshow("gtviser", local_map);
    cv::waitKey(5);
}

