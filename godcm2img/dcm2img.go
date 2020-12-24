package godcm2img

// #cgo LDFLAGS: -L. -ldcm2pnm
// #include "wrapper.h"
import "C"
import (
	"errors"
	"unsafe"
)

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

func (p DCM2PNM) Convert(fileName string, frameNumber int, outputFormat int) ([]byte, error) {
	dataLength := C.int(-1)
	data := C.Convert(p.ptr, &dataLength, C.int(frameNumber), C.int(outputFormat))
	if dataLength != -1 {
		return C.GoBytes(data, dataLength), nil
	} else {
		return nil, errors.New(C.GetError())
	}
}
