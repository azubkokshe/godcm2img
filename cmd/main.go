package main

import (
	"errors"
	"fmt"
	dcm2img "github.com/azubkokshe/godcm2img"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"strconv"
	"time"
)

func conv(path string, name string, count int64) {
	foo := dcm2img.New()
	defer foo.Free()
	res, err := foo.Convert(path, 1, 1)
	if err != nil {
		panic(err)
	} else {
		ioutil.WriteFile("./result/"+strconv.FormatInt(int64(count), 10)+".jpeg", res, 0666)
	}
}

func main() {
	count := 0
	var allSize int64
	t1 := time.Now()
	err := filepath.Walk("/home/alexandr/DICOM/COVID-19-AR",
		func(path string, info os.FileInfo, err error) error {
			if err != nil {
				return err
			}
			if !info.IsDir() {
				allSize += info.Size()
				conv(path, info.Name(), int64(count))
				count++
				if count > 10000 {
					return errors.New("!")
				}
			}
			return nil
		})

	fmt.Println("Elapsed:", time.Since(t1))
	fmt.Println("AllSize:", allSize)
	fmt.Println("all count", count)

	if err != nil {
		log.Println(err)
	}
}
