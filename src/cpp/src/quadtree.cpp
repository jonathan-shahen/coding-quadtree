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
        ZoneScopedN("Quadtree Find");
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

        std::cout << "Building QuadTree from: " << csvfile << '\n';

        return false;
    }

    /**
     *
     * @return TRUE if inserted, FALSE if not inserted
     */
    bool QuadTree::insert(QuadTreeNode *node, int id, float lat, float lon)
    {
        ZoneScopedN("Quadtree Insert");
        // SKIP: if doesn't fit in the boundary
        if (!node->boundary.inside(lat, lon))
            return false;

        Point point{id, lat, lon};

        // INSERT: into a LEAF node if there is capacity
        if (node->is_leaf() && node->num_points() < QUAD_TREE__NODE_POINTS_LIMIT)
        {
            node->points.push_back(point);
            return true;
        }

        // Mall Problem: Prevent Infinite Depth Tree
        // NOTE: we must exceed capacity if the minimum node area is reached
        if (node->is_leaf() && node->boundary.area() <= QUAD_TREE__NODE_MIN_AREA)
        {
            _count_overflows++;
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
        ZoneScopedN("Quadtree subdivide");
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

    void QuadTree::build_tree(std::string csvfile)
    {
        ZoneScopedN("Quadtree Build Tree");
        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "=================================================\n";
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
        std::cout << std::format("    Build Time: {} seconds.\n", commaify(time_span.count()));
        std::cout << std::format("     Overflows: {}\n", commaify(_count_overflows));
        std::cout << std::format("    Subdivides: {}\n", commaify(_count_subdivides));
        std::cout << std::format("         Nodes: {}\n", commaify(_nodes.size()));
        std::cout << std::format("        Points: {}\n", commaify(_count_points));
        std::cout << "=================================================\n";
    }

    void QuadTree::analyze() const
    {
        ZoneScopedN("Quadtree Analyze");
        auto t1 = std::chrono::high_resolution_clock::now();
        int count_empty_leaves = 0;
        constexpr std::size_t max_point_counts = 15;
        std::list<int> top_point_counts(max_point_counts + 1, 0);

        for (std::size_t i = 0; i < _nodes.size(); ++i)
        {
            auto const &node = _nodes[i];
            if (node.is_leaf() && node.points.size() == 0)
            {
                count_empty_leaves++;
                continue;
            }

            if (node.points.size() > _nodes[top_point_counts.back()].points.size())
            {
                top_point_counts.push_back(i);
                top_point_counts.sort([this](const auto &a, const auto &b)
                                      { return _nodes[a].points.size() > _nodes[b].points.size(); });

                if (top_point_counts.size() > max_point_counts)
                {
                    top_point_counts.pop_back();
                }
            }
        }

        std::cout << "=================================================\n";
        std::cout << "Analyze of QuadTree\n";
        auto t2 = std::chrono::high_resolution_clock::now();
        auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        std::cout << std::format("       Analysis Time: {} seconds.\n", commaify(time_span.count()));
        std::cout << std::format("               Nodes: {}\n", commaify(_nodes.size()));
        std::cout << std::format("               Empty: {}\n", commaify(count_empty_leaves));

        std::cout << std::format("    Top Point Counts:\n");
        std::size_t i = 1;
        for (const auto &node_i : top_point_counts)
        {
            auto const &node = _nodes[node_i];
            std::cout << std::format("                     {:0>2}. Node {: >{}}; Points: {}; Area: {}; {}\n",
                                     i, node_i, std::to_string(_nodes.size()).length(),
                                     commaify(node.points.size()),
                                     commaify(node.boundary.area()),
                                     node.boundary.to_string()
                                    );
            i++;
        }
        std::cout << "=================================================\n";
    }
}