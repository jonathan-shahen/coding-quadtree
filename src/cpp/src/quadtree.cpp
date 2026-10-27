#include "quadtree.hpp"

namespace QuadTree
{
    QuadTree::QuadTree(std::string csvfile)
    {
        // Try to load a tree that matches the
        if (!this->try_load_tree(csvfile))
        {
            this->build_tree(csvfile);
        }
    }

    SearchResults QuadTree::find(float lat, float lon, float radius, int limit) const
    {
        // implementation
        std::cout << std::format("TODO: find({},{},{},{})\n", lat, lon, radius, limit);
        return SearchResults();
    }

    bool QuadTree::try_load_tree(std::string csvfile)
    {
        // TODO

        if (true)
        {
            return false;
        }

        std::cout << "Loading QuadTree from: " << csvfile << '\n';

        return false;
    }

    void QuadTree::build_tree(std::string csvfile)
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "Building QuadTree from: " << csvfile << '\n';
        // Reserve an initial capacity to reduce number of allocs
        _nodes.reserve(QUAD_TREE__NODE_RESERVE);

        // Build the default root node
        _nodes.push_back(QuadTreeNode(GeoBoundary(-90.0, 90.0, -180.0, 180.0)));
        _root_index = 0;

        std::ifstream file;
        char buf[1024 * 10 + 1];

        file.rdbuf()->pubsetbuf(buf, sizeof buf);
        file.open(csvfile);

        int row = 0;
        for (std::string line; getline(file, line);)
        {
            // Skip the header row
            if (row == 0)
            {
                row++;
                continue;
            }
            std::stringstream line_stream(line);
            std::string token;

            // Must have atleast 5 characters for a valid line
            if (line.length() < 5)
                throw std::runtime_error("Invalid CSV Line: " + line);

            // CSV Header: id,lat,lon
            if (!getline(line_stream, token, ','))
                throw std::runtime_error("Invalid CSV Line: " + line);
            int id = std::stod(token);
            if (!getline(line_stream, token, ','))
                throw std::runtime_error("Invalid CSV Line: " + line);
            float lat = std::stof(token);
            if (!getline(line_stream, token, ','))
                throw std::runtime_error("Invalid CSV Line: " + line);
            float lon = std::stof(token);

#ifdef DEBUG
            if (row % 1000 == 0)
            {
                std::cout << std::format("PROGRESS (row {}) id/lat/lon: {}/{}/{}\n", row, id, lat, lon);
            }
#endif

            this->insert(root_node(), id, lat, lon);
            _count_points++;

            row++;
        }

        auto t2 = std::chrono::high_resolution_clock::now();
        auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        std::cout << "    Build Time: " << time_span.count() << " seconds.\n";
        std::cout << "    Subdivides: " << _count_subdivides << "\n";
        std::cout << "        Points: " << _count_points << "\n";
    }

    /**
     *
     * @return TRUE if inserted, FALSE if not inserted
     */
    bool QuadTree::insert(QuadTreeNode *node, int id, float lat, float lon)
    {
        // SKIP: if doesn't fit in the boundary
        if (!node->boundary.inside(lat, lon))
            return false;

        Point point{id, lat, lon};

        // INSERT: into a LEAF node if there is capacity OR cannot split
        // NOTE: we must exceed capacity if the minimum node area is reached
        if (node->is_leaf() &&
            (node->num_points() < QUAD_TREE__NODE_POINTS_LIMIT || node->boundary.area() <= QUAD_TREE__NODE_MIN_AREA))
        {
            node->points.push_back(point);
            return true;
        }

        // If leaf node, then must subdivide to continue
        if (node->is_leaf())
        {
            this->subdivide(node);
        }

        if (insert(&_nodes[node->nw], point))
            return true;
        if (insert(&_nodes[node->ne], point))
            return true;
        if (insert(&_nodes[node->sw], point))
            return true;
        if (insert(&_nodes[node->se], point))
            return true;

        return false;
    }

    void QuadTree::subdivide(QuadTreeNode *node)
    {
        _count_subdivides++;

        // Setup each quadrant
        // North West
        _nodes.push_back(QuadTreeNode(node->boundary.nw()));
        node->nw = _nodes.size() - 1;
        // North East
        _nodes.push_back(QuadTreeNode(node->boundary.ne()));
        node->ne = _nodes.size() - 1;
        // South West
        _nodes.push_back(QuadTreeNode(node->boundary.sw()));
        node->sw = _nodes.size() - 1;
        // South East
        _nodes.push_back(QuadTreeNode(node->boundary.se()));
        node->se = _nodes.size() - 1;

        // Move the points into the quadrants
        Point point;

        while (!node->points.empty())
        {
            point = node->points.back();
            node->points.pop_back();

            if (insert(&_nodes[node->nw], point))
                continue;
            if (insert(&_nodes[node->ne], point))
                continue;
            if (insert(&_nodes[node->sw], point))
                continue;
            if (insert(&_nodes[node->se], point))
                continue;
            throw std::runtime_error("Point has no place to go: " + point.id);
        }
    }
}