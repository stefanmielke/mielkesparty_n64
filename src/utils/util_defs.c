#include "util_defs.h"

bool is_intersecting(Rect a, Rect b) {
    return
        a.pos.x < b.pos.x + b.size.width &&
        a.pos.x + a.size.width > b.pos.x &&
        a.pos.y < b.pos.y + b.size.height &&
        a.pos.y + a.size.height > b.pos.y;
}

bool contains(Rect inner, Rect outer) {
    return
        inner.pos.x >= outer.pos.x &&
        inner.pos.x + inner.size.width <= outer.pos.x + outer.size.width &&
        inner.pos.y >= outer.pos.y &&
        inner.pos.y + inner.size.height <= outer.pos.y + outer.size.height;
}
