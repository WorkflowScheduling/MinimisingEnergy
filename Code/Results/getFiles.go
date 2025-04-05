package main

import (
	"fmt"
	"os"
)

func main() {
	files, err := os.ReadDir("./Dataset/Parsed/")
	if err != nil {
		panic(err)
	}

	for _, file := range files {
		fmt.Print("\"" + file.Name() + "\",")
	}
}
