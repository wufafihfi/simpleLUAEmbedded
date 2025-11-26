#include "LsMath.h"

namespace LsMath {
    bool isPointInPolygon(Vector2d P, const std::vector<Vector2d>& polygon) {
        int n = polygon.size();
        if (n < 3) return false; // 多边形至少要有3个顶点

        bool inside = false;

        for (int i = 0; i < n; i++) {
            Vector2d P1 = polygon[i];
            Vector2d P2 = polygon[(i + 1) % n];

            // 检查射线是否与多边形的边相交
            if ((P1.y > P.y) != (P2.y > P.y)) { // 点P在P1和P2的y轴之间
                // 计算交点的x坐标
                double xIntersection = (P.y - P1.y) * (P2.x - P1.x) / (P2.y - P1.y) + P1.x;

                // 如果交点的x坐标大于点P的x坐标，则说明射线穿过该线段
                if (xIntersection > P.x) {
                    inside = !inside; // 切换inside状态
                }
            }
        }

        return inside;
    }
}