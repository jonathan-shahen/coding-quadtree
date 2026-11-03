package main

import (
	"fmt"
	"strings"
)

type SearchResults struct {
	foundIds []int64
	extraIds []int64
}

func (results *SearchResults) json() string {
	return fmt.Sprintf("{\"ids\":[%s], \"extra\":[%s]}",
		strings.Replace(fmt.Sprint(results.foundIds), " ", ",", -1),
		strings.Replace(fmt.Sprint(results.extraIds), " ", ",", -1),
	)
}
