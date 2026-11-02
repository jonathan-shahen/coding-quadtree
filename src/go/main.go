package main

import (
	"flag"
	"fmt"
	"os"
)

func main() {
	earlyQuit := *flag.Bool("q", false, "a bool")
	flag.Parse()

	if len(flag.Args()) != 1 {
		fmt.Println("Missing csv_file")
		flag.PrintDefaults()
	}
	csvFile := flag.Args()[0]

	quadTree := QuadTree{}

	numNodes, err := quadTree.loadCsv(csvFile)
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

	if err := quadTree.analyze(); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

	fmt.Printf("Loaded %d nodes from %s\n", numNodes, csvFile)

	if earlyQuit == true {
		fmt.Println("EXITING: early exit parameter found")
		os.Exit(0)
	}

}
