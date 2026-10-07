#pragma once

// Row-vector convention: v' = v * M, as in the JS reference.
// All angles are in radians.  Screen-space: x right, y down.
// See docs/CONVENTIONS.md for the full list of decisions.

#include <cmath>

namespace ascii3d {

// ---------------------------------------------------------------------------
// Vector2
// ---------------------------------------------------------------------------

struct Vector2 {
    float x = 0.0f;
    float y = 0.0f;

    [[nodiscard]] constexpr Vector2 operator+(Vector2 rhs) const noexcept {
        return {x + rhs.x, y + rhs.y};
    }
    [[nodiscard]] constexpr Vector2 operator-(Vector2 rhs) const noexcept {
        return {x - rhs.x, y - rhs.y};
    }
    [[nodiscard]] constexpr Vector2 operator*(float s) const noexcept {
        return {x * s, y * s};
    }
    [[nodiscard]] constexpr Vector2 operator/(float s) const noexcept {
        return {x / s, y / s};
    }
    [[nodiscard]] constexpr Vector2 operator-() const noexcept {
        return {-x, -y};
    }
    [[nodiscard]] constexpr bool operator==(const Vector2& rhs) const noexcept = default;
};

// ---------------------------------------------------------------------------
// Vector3
// ---------------------------------------------------------------------------

struct Vector3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    [[nodiscard]] constexpr Vector3 operator+(Vector3 rhs) const noexcept {
        return {x + rhs.x, y + rhs.y, z + rhs.z};
    }
    [[nodiscard]] constexpr Vector3 operator-(Vector3 rhs) const noexcept {
        return {x - rhs.x, y - rhs.y, z - rhs.z};
    }
    [[nodiscard]] constexpr Vector3 operator*(float s) const noexcept {
        return {x * s, y * s, z * s};
    }
    [[nodiscard]] constexpr Vector3 operator/(float s) const noexcept {
        return {x / s, y / s, z / s};
    }
    [[nodiscard]] constexpr Vector3 operator-() const noexcept {
        return {-x, -y, -z};
    }
    [[nodiscard]] constexpr bool operator==(const Vector3& rhs) const noexcept = default;
};

// ---------------------------------------------------------------------------
// Vector4  (homogeneous; w is used for the perspective divide)
// ---------------------------------------------------------------------------

struct Vector4 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

    [[nodiscard]] constexpr Vector4 operator+(Vector4 rhs) const noexcept {
        return {x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w};
    }
    [[nodiscard]] constexpr Vector4 operator-(Vector4 rhs) const noexcept {
        return {x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w};
    }
    [[nodiscard]] constexpr Vector4 operator*(float s) const noexcept {
        return {x * s, y * s, z * s, w * s};
    }
    [[nodiscard]] constexpr Vector4 operator/(float s) const noexcept {
        return {x / s, y / s, z / s, w / s};
    }
    [[nodiscard]] constexpr Vector4 operator-() const noexcept {
        return {-x, -y, -z, -w};
    }
    [[nodiscard]] constexpr bool operator==(const Vector4& rhs) const noexcept = default;
};

// ---------------------------------------------------------------------------
// Scalar * Vector (commutative forms)
// ---------------------------------------------------------------------------

[[nodiscard]] constexpr Vector2 operator*(float s, Vector2 v) noexcept { return v * s; }
[[nodiscard]] constexpr Vector3 operator*(float s, Vector3 v) noexcept { return v * s; }
[[nodiscard]] constexpr Vector4 operator*(float s, Vector4 v) noexcept { return v * s; }

// ---------------------------------------------------------------------------
// Vector2 free functions
// ---------------------------------------------------------------------------

[[nodiscard]] constexpr float dot(Vector2 a, Vector2 b) noexcept {
    return a.x * b.x + a.y * b.y;
}

[[nodiscard]] constexpr float length_squared(Vector2 v) noexcept {
    return dot(v, v);
}

[[nodiscard]] constexpr float length(Vector2 v) noexcept {
    return std::sqrt(length_squared(v));
}

// Returns a zero vector when v is zero — never produces NaN.
[[nodiscard]] constexpr Vector2 normalize(Vector2 v) noexcept {
    const float len = length(v);
    return len > 0.0f ? v / len : Vector2{};
}

// ---------------------------------------------------------------------------
// Vector3 free functions
// ---------------------------------------------------------------------------

[[nodiscard]] constexpr float dot(Vector3 a, Vector3 b) noexcept {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

[[nodiscard]] constexpr Vector3 cross(Vector3 a, Vector3 b) noexcept {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}

[[nodiscard]] constexpr float length_squared(Vector3 v) noexcept {
    return dot(v, v);
}

[[nodiscard]] constexpr float length(Vector3 v) noexcept {
    return std::sqrt(length_squared(v));
}

[[nodiscard]] constexpr Vector3 normalize(Vector3 v) noexcept {
    const float len = length(v);
    return len > 0.0f ? v / len : Vector3{};
}

// ---------------------------------------------------------------------------
// Vector4 free functions
// ---------------------------------------------------------------------------

[[nodiscard]] constexpr float dot(Vector4 a, Vector4 b) noexcept {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

[[nodiscard]] constexpr float length_squared(Vector4 v) noexcept {
    return dot(v, v);
}

[[nodiscard]] constexpr float length(Vector4 v) noexcept {
    return std::sqrt(length_squared(v));
}

[[nodiscard]] constexpr Vector4 normalize(Vector4 v) noexcept {
    const float len = length(v);
    return len > 0.0f ? v / len : Vector4{0.0f, 0.0f, 0.0f, 0.0f};
}

// ---------------------------------------------------------------------------
// Conversion helpers
// ---------------------------------------------------------------------------

[[nodiscard]] constexpr Vector4 to_vec4(Vector3 v, float w = 1.0f) noexcept {
    return {v.x, v.y, v.z, w};
}

[[nodiscard]] constexpr Vector3 xyz(Vector4 v) noexcept {
    return {v.x, v.y, v.z};
}

} // namespace ascii3d
