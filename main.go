package main

import (
	"fmt"
	"github.com/azubkokshe/godcm2img/godcm2img"
	"io/ioutil"
	"time"
)

func main() {
	t1 := time.Now()
	foo := godcm2img.New()
	defer foo.Free()
	res, err := foo.Convert("4.dcm", 1, 1)
	if err != nil {
		ioutil.WriteFile("out3.jpeg", res, 0666)
	} else {
		panic(err)
	}
	fmt.Println(time.Since(t1))
}

