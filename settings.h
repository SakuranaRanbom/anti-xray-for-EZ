#pragma once

#include <yaml.h>
#include <string>

struct Settings {
  int xoffset  = 5;
  int yoffset  = 0;
  int zoffset  = 5;

 

  template <typename IO> static inline bool io(IO f, Settings &settings, YAML::Node &node) {
    return f(settings.xoffset, node["x-offset"]) && f(settings.yoffset, node["y-offset"]) &&
           f(settings.zoffset, node["z-offset"]);
  }
};

inline Settings settings;
