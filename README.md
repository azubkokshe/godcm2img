# godcm2img

go wrapper for dcmj2pnm (DCMTK)

## Features
* Simple API
* Convert to JPEG, PNG and BMP

## Dependencies
* [dcmtk](https://github.com/DCMTK/dcmtk)

## Quick start
### How to install godcm2img?

1. [Install dcmtk to your system](https://github.com/DCMTK/dcmtk/blob/master/INSTALL)
2. Get package
```bash
go get https://github.com/azubkokshe/godcm2img
```

### How to convert dicom?
```go
dcmConverter := dcm2img.New()
defer dcmConverter.Free()

resp, err = dcmConverter.GetJPEG(&fileData, dcm2img.ConvertProperties{
    FrameNumber: req.FrameNumber,
    Quality:     req.Quality,
    UseClip:     isUseClip,
    ClipLeft:    req.RegionLeft,
    ClipTop:     req.RegionTop,
    ClipWidth:   req.RegionWidth,
    ClipHeight:  req.RegionHeight,
    UseScale:    isUseScale,
    ScaleWidth:  req.Columns,
    ScaleHeight: req.Rows,
})
```

