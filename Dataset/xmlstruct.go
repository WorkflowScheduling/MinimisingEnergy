package main

import (
	"encoding/xml"
)

type Adag struct {
	XMLName    xml.Name `xml:"adag"`
	JobCount   string   `xml:"jobCount,attr"`
	ChildCount string   `xml:"childCount,attr"`
	Jobs       []Job    `xml:"job"`
	Childs     []Child  `xml:"child"`
}

type Job struct {
	XMLName xml.Name `xml:"job"`
	Id      string   `xml:"id,attr"`
	Runtime string   `xml:"runtime,attr"`
	UsesArr []Uses   `xml:"uses"`
}

type Child struct {
	XMLName xml.Name `xml:"child"`
	Ref     string   `xml:"ref,attr"`
	Parents []Parent `xml:"parent"`
}

type Uses struct {
	XMLName xml.Name `xml:"uses"`
	File    string   `xml:"file,attr"`
	Link    string   `xml:"link,attr"`
	Data    int64    `xml:"size,attr"`
}

type Parent struct {
	XMLName xml.Name `xml:"parent"`
	Ref     string   `xml:"ref,attr"`
}

type Dag struct {
	WorstCaseTime []float64
	AdjList       [][]uint64
}
