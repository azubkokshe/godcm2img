package dcm2img

//#cgo linux LDFLAGS: -pthread -ldcmdata -lz -loflog -lofstd -ldcmjpeg -ldcmimgle -ldcmimage -lpng -lijg12 -lijg16 -lijg8
//#cgo darwin LDFLAGS:  -pthread -ldcmdata -lz -loflog -lofstd -ldcmjpeg -ldcmimgle -ldcmimage -lpng -lijg12 -lijg16 -lijg8
//#include "wrapper.h"
//#include <stdio.h>
//#include <stdlib.h>
import "C"
import (
	"errors"
	"unsafe"
)

//#cgo LDFLAGS: -pthread -ldcmdata -lz -loflog -lofstd -ldcmjpeg -ldcmimgle -ldcmimage
//#cgo LDFLAGS: -L/usr/lib -liconv -pthread -ldcmdata -lz -loflog -lofstd -ldcmjpeg -ldcmimgle -ldcmimage -lpng -lijg12 -lijg16 -lijg8

type DCM2PNM struct {
	ptr unsafe.Pointer
}

func New() DCM2PNM {
	var dcm2pnm DCM2PNM
	dcm2pnm.ptr = C.New()
	return dcm2pnm
}

func (p DCM2PNM) Free() {
	C.Destroy(p.ptr)
}

func (p DCM2PNM) GetJPEG(fileData *[]byte, frameNumber int) ([]byte, error) {
	return p.convert(fileData, frameNumber, 1)
}

func (p DCM2PNM) GetPNG(fileData *[]byte, frameNumber int) ([]byte, error) {
	return p.convert(fileData, frameNumber, 2)
}

func (p DCM2PNM) GetBMP(fileData *[]byte, frameNumber int) ([]byte, error) {
	return p.convert(fileData, frameNumber, 3)
}

func (p DCM2PNM) convert(fileData *[]byte, frameNumber int, outputFormat int) ([]byte, error) {
	dataLength := C.int(-1)
	cData := C.CBytes(*fileData)

	data := C.Convert(p.ptr, &dataLength, (*C.char)(cData), C.int(len(*fileData)), C.uint(frameNumber), C.int(outputFormat))
	defer C.free(unsafe.Pointer(data))
	defer C.free(cData)
	if dataLength > 0 && data != nil {
		return C.GoBytes(data, dataLength), nil
	} else {
		return nil, errors.New(C.GoString(C.GetError(p.ptr)))
	}
}
