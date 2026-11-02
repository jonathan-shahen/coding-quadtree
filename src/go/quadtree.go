package main

type QuadTree struct {
	nodes            []QuadTreeNode
	root_index       uint64
	count_points     uint64
	count_subdivides uint64
	count_overflows  uint64
}

func (tree *QuadTree) loadCsv(csvFile string) (int64, error) {

	return 0, nil
}

func (tree *QuadTree) analyze() error {

	return nil
}
