#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

struct Point {
    int x, y, z;
    Point(int a, int b, int c) : x(a), y(b), z(c) {}
};

class Octree {
private:
    Octree* children[8] = { nullptr };
    std::vector<Point> points;
    Point bottomLeft;
    double h;
    int nPoints;

    int getOctant(const Point& p) const {
        int mx = bottomLeft.x + h / 2;
        int my = bottomLeft.y + h / 2;
        int mz = bottomLeft.z + h / 2;
        return (p.x >= mx) + 2 * (p.y >= my) + 4 * (p.z >= mz);
    }
void print_tree_helper(int depth) {
    std::string indent(depth * 2, '-');

    std::cout << indent << "Nodo: Pos(" 
              << bottomLeft.x << ", " 
              << bottomLeft.y << ", " 
              << bottomLeft.z << "), Lado: " 
              << h << "\n";

    for (const Point& p : points) {
        std::cout << indent << "  Punto: (" 
                  << p.x << ", " 
                  << p.y << ", " 
                  << p.z << ")\n";
    }

    for (int i = 0; i < 8; ++i) {
        if (children[i]) {
            children[i]->print_tree_helper(depth + 1);
        }
    }
}


    double distance(const Point& a, const Point& b) const {
        return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) +
            (a.z - b.z) * (a.z - b.z));
    }

public:
    Octree(Point bottomLeft, double h)
        : bottomLeft(bottomLeft), h(h), nPoints(0) {}

    ~Octree() {
        for (auto& child : children) {
            delete child;
        }
    }
    void print_tree() {
        print_tree_helper(0);
    }

    void insert(const Point& p) {
        if (nPoints < 1) {
            points.push_back(p);
            nPoints++;
            return;
        }

        int octant = getOctant(p);
        if (!children[octant]) {
            double half = h / 2.0;
            Point newBottomLeft = bottomLeft;
            if (octant & 1)
                newBottomLeft.x += half;
            if (octant & 2)
                newBottomLeft.y += half;
            if (octant & 4)
                newBottomLeft.z += half;
            children[octant] = new Octree(newBottomLeft, half);
        }
        children[octant]->insert(p);
    }
void find_closest_helper_brute(const Point& target, double radius, double& bestDist, Point& bestPoint, bool& found) {
    for (const Point& p : points) {
        double d = distance(p, target);
        if (d <= radius && d < bestDist) {
            bestDist = d;
            bestPoint = p;
            found = true;
        }
    }

    for (int i = 0; i < 8; ++i) {
        if (children[i]) {
            children[i]->find_closest_helper_brute(target, radius, bestDist, bestPoint, found);
        }
    }
}


Point find_closest(const Point& target, double radius) {
    double bestDist = std::numeric_limits<double>::max();
    Point bestPoint = target;
    bool found = false;

    find_closest_helper_brute(target, radius, bestDist, bestPoint, found);

    if (!found) {
        std::cerr << "⚠️ No se encontró ningún punto dentro del radio de " << radius << std::endl;
    }

    return bestPoint;
}




    void print_origin() {
        std::cout << "Origin: (" << bottomLeft.x << "," << bottomLeft.y << ","
            << bottomLeft.z << ")\n";
    }

    void print_size() { std::cout << "Size of side root: " << h << std::endl; }
};

std::vector<Point> readCSV(const std::string& filename) {
    std::vector<Point> points;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
        return points;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string x_str, y_str, z_str;
        if (std::getline(ss, x_str, ',') && std::getline(ss, y_str, ',') &&
            std::getline(ss, z_str, ',')) {
            try {
                int x = std::stoi(x_str);
                int y = std::stoi(y_str);
                int z = std::stoi(z_str);
                points.emplace_back(x, y, z);
            }
            catch (const std::exception& e) {
                std::cerr << "Error al procesar la línea: " << line << " -> "
                    << e.what() << std::endl;
            }
        }
    }
    file.close();
    return points;
}

int main() {
    std::vector<Point> points = readCSV("points2.csv");
    if (points.empty()) {
        std::cerr << "No se encontraron puntos en el archivo.\n";
        return 1;
    }

    int minX = std::numeric_limits<int>::max();
    int minY = std::numeric_limits<int>::max();
    int minZ = std::numeric_limits<int>::max();
    int maxX = std::numeric_limits<int>::min();
    int maxY = std::numeric_limits<int>::min();
    int maxZ = std::numeric_limits<int>::min();

    for (const auto& p : points) {
        minX = std::min(minX, p.x);
        minY = std::min(minY, p.y);
        minZ = std::min(minZ, p.z);
        maxX = std::max(maxX, p.x);
        maxY = std::max(maxY, p.y);
        maxZ = std::max(maxZ, p.z);
    }

    Point bottomLeft(minX, minY, minZ);
    double h = std::max({ maxX - minX, maxY - minY, maxZ - minZ });

    std::cout << "Bottom Left: (" << bottomLeft.x << ", " << bottomLeft.y << ", "
        << bottomLeft.z << ")\n";
    std::cout << "Side Length (h): " << h << std::endl;

    Octree octree(bottomLeft, h);
    for (const auto& p : points) {
        octree.insert(p);
    }

    Point w2(0, 0, 0);
    Point w(18, 28, 275);
    Point w3(0,0,0);
    //octree.print_tree();
    w3 = octree.find_closest(w2, 82.8);

    std::cout << "closest point is: " << w3.x << " " << w3.y << " " << w3.z
        << std::endl;
    return 0;
}
