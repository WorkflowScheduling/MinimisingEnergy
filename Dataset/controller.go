package main

import (
	"encoding/xml"
	"fmt"
	"io"
	"os"
	"strconv"
)

func ReadFromFile(path string) Adag {
	xmlFile, err := os.Open(path)
	if err != nil {
		panic("Error Opening File: " + path)
	}
	defer xmlFile.Close()

	byteValue, _ := io.ReadAll(xmlFile)

	var adag Adag

	err = xml.Unmarshal(byteValue, &adag)

	if err != nil {
		panic("Error Parsing xml from : " + path)
	}
	return adag
}

func ConvertToDag(adag Adag) Dag {
	var dag Dag
	jobCount, _ := strconv.Atoi(adag.JobCount)

	dag.WorstCaseTime = make([]float64, jobCount)
	dag.AdjList = make([][]uint64, jobCount)

	idToindex := map[string]int{}
	idToJob := map[string]Job{}
	outputToIndex := map[string]int{}

	for i, val := range adag.Jobs {
		idToindex[val.Id] = i
		idToJob[val.Id] = val
		dag.WorstCaseTime[i], _ = strconv.ParseFloat(val.Runtime, 64)
		dag.AdjList[i] = make([]uint64, jobCount)
		for _, use := range val.UsesArr {
			if use.Link == "output" {
				outputToIndex[use.File] = i
			}
		}
	}

	for i, val := range adag.Jobs {
		for _, use := range val.UsesArr {
			if parid, ok := outputToIndex[use.File]; ok && use.Link == "input" && use.Data > 0 {
				dag.AdjList[parid][i] = uint64(use.Data)
			}
		}
	}

	for _, val := range adag.Childs {
		curr := idToindex[val.Ref]
		for _, par := range val.Parents {
			parid := idToindex[par.Ref]
			if dag.AdjList[parid][curr] == 0 {
				dag.AdjList[parid][curr] = 1
			}
		}
	}

	return dag
}

func Recurvise(dag *Dag, i int) float64 {
	var ans float64
	ans = 0
	for i, val := range dag.AdjList[i] {
		if val > 0 {
			temp := Recurvise(dag, i)
			if temp > ans {
				ans = temp
			}
		}
	}
	return dag.WorstCaseTime[i] + ans
}

func GetTotalWc(dag *Dag) float64 {
	numOfNodes := len(dag.AdjList)
	indegree := make([]int, numOfNodes)

	for _, val := range dag.AdjList {
		for i, edge := range val {
			if edge > 0 {
				indegree[i]++
			}
		}
	}
	var ans float64
	ans = 0
	for i, val := range indegree {
		if val == 0 {
			temp := Recurvise(dag, i)
			if temp > ans {
				ans = temp
			}
		}
	}

	return ans
}

func WriteToFile(path string, dag Dag) {
	file, err := os.Create(path)

	if err != nil {
		panic("Error Creating File : " + path)
	}

	defer file.Close()

	randomFloatNumber := 0.9
	totalWc := GetTotalWc(&dag)
	DeadlineWc := (1.5) * totalWc
	file.Write([]byte(fmt.Sprint(len(dag.AdjList)) + " " + fmt.Sprint(randomFloatNumber) + " " + fmt.Sprint(DeadlineWc) + "\n"))

	for i, val := range dag.AdjList {
		st := fmt.Sprint(dag.WorstCaseTime[i])
		count := 0
		for _, list := range val {
			if list > 0 {
				count++
			}
		}
		st += " " + fmt.Sprint(count)
		for i, list := range val {
			if list > 0 {
				st += " " + fmt.Sprint(i) + " " + fmt.Sprint(list)
			}
		}
		st += "\n"
		_, err = file.Write([]byte(st))
		if err != nil {
			panic("Error Writing to file : " + path + " " + st)
		}
	}
}
