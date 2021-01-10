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

type DCM2PNM struct {
	ptr unsafe.Pointer
}

type ConvertProperties struct {
	FrameNumber int
	Quality     int
	UseClip     bool
	ClipLeft    int
	ClipTop     int
	ClipWidth   int
	ClipHeight  int
	UseScale    bool
	ScaleWidth  int
	ScaleHeight int
}

func New() DCM2PNM {
	var dcm2pnm DCM2PNM
	dcm2pnm.ptr = C.New()
	return dcm2pnm
}

func (p DCM2PNM) Free() {
	C.Destroy(p.ptr)
}

func (p DCM2PNM) GetJPEG(fileData *[]byte, prop ConvertProperties) ([]byte, error) {
	return p.convert(fileData, prop.FrameNumber, 1, prop.Quality, prop.UseClip, prop.ClipLeft, prop.ClipTop,
		prop.ClipWidth, prop.ClipHeight, prop.UseScale, prop.ScaleWidth, prop.ScaleHeight)
}

func (p DCM2PNM) GetPNG(fileData *[]byte, prop ConvertProperties) ([]byte, error) {
	return p.convert(fileData, prop.FrameNumber, 2, prop.Quality, prop.UseClip, prop.ClipLeft, prop.ClipTop,
		prop.ClipWidth, prop.ClipHeight, prop.UseScale, prop.ScaleWidth, prop.ScaleHeight)
}

func (p DCM2PNM) GetBMP(fileData *[]byte, prop ConvertProperties) ([]byte, error) {
	return p.convert(fileData, prop.FrameNumber, 3, prop.Quality, prop.UseClip, prop.ClipLeft, prop.ClipTop,
		prop.ClipWidth, prop.ClipHeight, prop.UseScale, prop.ScaleWidth, prop.ScaleHeight)
}

func (p DCM2PNM) convert(fileData *[]byte, frameNumber int, outputFormat int, quality int, useClip bool, left int, top int, width int, height int,
	useScale bool, scaleWidth int, scaleHeight int) ([]byte, error) {
	dataLength := C.int(-1)
	cData := C.CBytes(*fileData)

	isUseClip := 0
	if useClip == true {
		isUseClip = 1
	}

	isUseScale := 0
	if useScale == true {
		isUseScale = 5
	}

	data := C.Convert(p.ptr, &dataLength, (*C.char)(cData), C.int(len(*fileData)), C.uint(frameNumber), C.int(outputFormat), C.uint(quality),
		C.int(isUseClip), C.int(left), C.int(top), C.uint(width), C.uint(height), C.int(isUseScale),
		C.uint(scaleWidth), C.uint(scaleHeight))

	defer C.free(unsafe.Pointer(data))
	defer C.free(cData)

	if dataLength > 0 && data != nil {
		return C.GoBytes(data, dataLength), nil
	} else {
		return nil, errors.New(C.GoString(C.GetError(p.ptr)))
	}
}
