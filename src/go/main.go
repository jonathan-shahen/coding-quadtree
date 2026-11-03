package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"strings"
)

func main() {
	earlyQuit := flag.Bool("q", false, "a bool")
	flag.Parse()

	if len(flag.Args()) != 1 {
		fmt.Println("Missing csv_file")
		flag.PrintDefaults()
		os.Exit(1)
	}
	csvFile := flag.Args()[0]

	quadTree := QuadTree{}

	numNodes, err := quadTree.load(csvFile)
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

	if err := quadTree.analyze(); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

	fmt.Printf("Loaded %d nodes from %s\n", numNodes, csvFile)

	if *earlyQuit == true {
		fmt.Println("EXITING: early exit parameter found")
		os.Exit(0)
	}

	reader := bufio.NewReader(os.Stdin)
	for {
		fmt.Print("\nEnter lat,lon,radius,limit (or 'quit'): ")

		line, err := reader.ReadString('\n')
		if err != nil {
			break
		}

		line = strings.TrimSpace(line)

		if line == "quit" {
			break
		}

		var lat, lon, radius float32
		var limit int

		_, err = fmt.Sscanf(line, "%f,%f,%f,%d", &lat, &lon, &radius, &limit)
		if err != nil {
			fmt.Println("Invalid input:", err)
			continue
		}

		result, err := quadTree.find(lat, lon, radius, limit)

		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		fmt.Println(result.json())
	}
}
