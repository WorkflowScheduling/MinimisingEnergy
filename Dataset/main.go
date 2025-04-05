package main

import (
	"fmt"
	"io/fs"
	"os"
)

func RecursiveDir(dir fs.DirEntry) {
	files, err := os.ReadDir("./WorkFlowBenchmark/" + dir.Name() + "/")
	if err != nil {
		panic(err)
	}
	for _, file := range files {
		parsed := ReadFromFile("./WorkFlowBenchmark/" + dir.Name() + "/" + file.Name())
		dag := ConvertToDag(parsed)
		tempName := []rune(file.Name())
		tempName = tempName[:len(tempName)-4]
		WriteToFile("./Parsed/"+string(tempName)+".txt", dag)
		fmt.Println("Processed File :" + file.Name())
	}
}

func main() {
	files, err := os.ReadDir("./WorkFlowBenchmark/")
	if err != nil {
		panic(err)
	}

	for _, file := range files {
		RecursiveDir(file)
	}
}
