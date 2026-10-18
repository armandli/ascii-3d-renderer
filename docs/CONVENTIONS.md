* vector convention use column vector
* handedness left handed, camera looking down +Z, visible points Z > 0, clip w = Z
* use radians, with fov_degrees the exception
* screen space x [0,W) goes right and y [0,H) goes down, sample at pixel center
* depth smaller is closer, view z, clear value is +infinity, depth test passes on strict < (equal depth is rejected)
* winding is clockwise in view space, normal is cross(v2 - v1, v3 - v1)
* terminal cell aspect 2x, aspect = (width / cell_aspect) / height , make it configurable
* float tolerance 1e-5f
