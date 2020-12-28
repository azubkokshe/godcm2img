package main

import (
	"fmt"
	dcm2img "github.com/azubkokshe/godcm2img"
	"io/ioutil"
	"strconv"

	"log"
	"os"
	"path/filepath"

	"time"
)

func conv(path string, name string, count int64) {
	foo := dcm2img.New()
	defer foo.Free()
	fileData, err := ioutil.ReadFile(path)
	if err != nil {
		panic(err)
	}
	log.Println("read file size", len(fileData), path)
	res, err := foo.GetPNG(&fileData, 1)
	if err != nil {
		panic(err)
	} else {
		ioutil.WriteFile("./result/"+strconv.FormatInt(int64(count), 10)+".png", res, 0666)
	}
}

func main() {
	count := 0
	var allSize int64
	t1 := time.Now()
	for i := 0; i < 1; i++ {
		log.Println(i)
		err := filepath.Walk("/Users/mac/Documents/Projects/KAZDREAM/GO/dicom",
			func(path string, info os.FileInfo, err error) error {
				if err != nil {
					return err
				}
				if !info.IsDir() {
					allSize += info.Size()
					conv(path, info.Name(), int64(count))
				}
				return nil
			})

		if err != nil {
			log.Println(err)
		}

		time.Sleep(5 * time.Millisecond)
	}
	fmt.Println("Elapsed:", time.Since(t1))
	//time.Sleep(100 * time.Second)
	fmt.Println("AllSize:", allSize)
	fmt.Println("all count", count)
}
