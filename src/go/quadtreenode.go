package main

const NODE_POINTS_LIMIT = 100

type Point struct {
	id  int64
	lat float32
	lon float32
}

const InvalidNode = -1

type QuadTreeNode struct {
	boundary GeoBoundary
	points   []Point
	nw       int
	ne       int
	sw       int
	se       int
}

func NewQuadTreeNode(boundary GeoBoundary) QuadTreeNode {
	return QuadTreeNode{
		boundary: boundary,
		points:   make([]Point, 0, NODE_POINTS_LIMIT),
		nw:       InvalidNode,
		ne:       InvalidNode,
		sw:       InvalidNode,
		se:       InvalidNode,
	}
}

func (node *QuadTreeNode) leaf() bool {
	return node.nw == -1
}
