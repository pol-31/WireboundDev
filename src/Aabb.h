#ifndef WIREBOUNDDEV_SRC_AABB_H_
#define WIREBOUNDDEV_SRC_AABB_H_

#include <limits>

struct Point2f {
  float x;
  float y;
};

struct Point3f {
  float x;
  float y;
  float z;
};

struct Aabb2 {
  Point2f min = {
      std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max()
  };
  Point2f max = {
      std::numeric_limits<float>::min(),
      std::numeric_limits<float>::min()
  };
};

struct Aabb3 {
  Point3f min = {
      std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max()
  };
  Point3f max = {
      std::numeric_limits<float>::min(),
      std::numeric_limits<float>::min(),
      std::numeric_limits<float>::min()
  };
};

// useful for text rendering
struct TexCoordsAabb {
  float left = std::numeric_limits<float>::max();
  float right = std::numeric_limits<float>::min();
  float top = std::numeric_limits<float>::min();
  float bottom = std::numeric_limits<float>::max();

  [[nodiscard]] bool Intersects(const TexCoordsAabb& other) const {
    return IntersectsH(other) && IntersectsV(other);
  }

  [[nodiscard]] bool IntersectsH(const TexCoordsAabb& other) const {
    return !(right < other.left || left > other.right);
  }

  [[nodiscard]] bool IntersectsV(const TexCoordsAabb& other) const {
    return !(bottom < other.top || top > other.bottom);
  }

  [[nodiscard]] bool IntersectsH(float left_border, float right_border) const {
    return !(right < left_border || left > right_border);
  }

  [[nodiscard]] bool IntersectsV(float top_border, float bottom_border) const {
    return !(bottom < top_border || top > bottom_border);
  }
};

#endif  // WIREBOUNDDEV_SRC_AABB_H_
