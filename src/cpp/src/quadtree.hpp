#pragma once
// #define DEBUG

#define QUAD_TREE__NODE_POINTS_LIMIT 100
#define QUAD_TREE__NODE_RESERVE 1'000'000
#define QUAD_TREE__NODE_MIN_AREA 10

#ifdef DEBUG
#define QUAD_TREE__NODE_POINTS_LIMIT 3
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <format>
#include <sstream>
#include <vector>
#include <chrono>
#include <list>

#include <tracy/Tracy.hpp>

#include "search_results.hpp"
#include "geo_boundary.hpp"
#include "utils.hpp"

namespace QuadTree
{
    struct Point
    {
        int id;
        float lat;
        float lon;
    };

    class QuadTreeNode
    {
    public:
        GeoBoundary boundary;

        /** LEAF NODE: Stores id/lat/lon for each point */
        std::vector<Point> points;

        /** INTERNAL NODE: these values represent the 4 (quad) child nodes */
        int nw = -1;
        int ne = -1;
        int sw = -1;
        int se = -1;

    public:
        QuadTreeNode(GeoBoundary boundary) : boundary(boundary) {}

        /** Returns TRUE if this node is a LEAF node (i.e. contains IDs) */
        inline bool is_leaf() const
        {
            return this->nw == -1;
        }

        /** Returns TRUE if this node is a LEAF node (i.e. contains IDs) */
        inline int num_points() const
        {
            return this->points.size();
        }
    };

    class QuadTree
    {
    private:
        std::vector<QuadTreeNode> _nodes;
        int _root_index = 0;
        long long _count_points = 0;
        long long _count_subdivides = 0;
        long long _count_overflows = 0;

    public:
        QuadTree(std::string csvfilepath);
        SearchResults find(float lat, float lon, float radius, int limit) const;

        /** Performs an analysis of the QuadTree and prints the results to std::cout */
        void analyze() const;

    private:
        bool try_load_tree(std::string csvfile);
        void build_tree(std::string csvfile);

        /** Returns a pointer to the root node. */
        QuadTreeNode *root_node()
        {
            return &_nodes[_root_index];
        }

        /**
         * Insert a new point into the quadtree
         *
         * @param node Pointer to node to try to insert into
         * @param id Int ID to the point
         * @param lat Latitude
         * @param lon Longitude
         */
        bool insert(QuadTreeNode *node, int id, float lat, float lon);
        inline bool insert(QuadTreeNode *node, Point point)
        {
            return insert(node, point.id, point.lat, point.lon);
        }

        void subdivide(QuadTreeNode *node);
    };

}
