#include "dcm2pnm.hpp"

#define BUILD_DCM2PNM_AS_DCMJ2PNM
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define SHORTCOL 4
#define LONGCOL 20

#define BUILD_DCM2PNM_AS_DCMJ2PNM
#define WITH_LIBPNG

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/dcmdata/dctk.h"          /* for various dcmdata headers */
#include "dcmtk/dcmdata/dcuid.h"         /* for dcmtk version name */
#include "dcmtk/dcmdata/dcrledrg.h"      /* for DcmRLEDecoderRegistration */
#include "dcmtk/dcmimgle/dcmimage.h"     /* for DicomImage */
#include "dcmtk/dcmimgle/digsdfn.h"      /* for DiGSDFunction */
#include "dcmtk/dcmimgle/diciefn.h"      /* for DiCIELABFunction */
#include "dcmtk/dcmimage/diregist.h"     /* include to support color images */
#include "dcmtk/ofstd/ofstd.h"           /* for OFStandard */
#include "dcmtk/dcmimage/dipipng.h"      /* for dcmimage PNG plugin */
#include "dcmtk/dcmdata/cmdlnarg.h"      /* for prepareCmdLineArgs */
#include "dcmtk/ofstd/ofconapp.h"        /* for OFConsoleApplication */
#include "dcmtk/ofstd/ofcmdln.h"         /* for OFCommandLine */
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmdata/dcistrma.h"
#include "dcmtk/dcmdata/dcistrmb.h"
#include "dcmtk/dcmjpls/djdecode.h"
#include "dcmtk/dcmimage/dipitiff.h"
#include "dcmtk/dcmjpeg/djdecode.h"      /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/dipijpeg.h"      /* for dcmimage JPEG plugin */

/* output file types */
enum E_FileType
{
    EFT_RawPNM,
    EFT_8bitPNM,
    EFT_16bitPNM,
    EFT_NbitPNM,
    EFT_BMP,
    EFT_8bitBMP,
    EFT_24bitBMP,
    EFT_32bitBMP,
    EFT_JPEG,
    EFT_TIFF,
    EFT_PNG
#ifdef PASTEL_COLOR_OUTPUT
   ,EFT_PastelPNM
#endif
};

DCM2PNM::DCM2PNM() {
}

DCM2PNM::~DCM2PNM() {
}

/*
MONOCHROME2 MONOCHROME1 PALETTE COLOR RGB YBR_FULL YBR_FULL_422 YBR_PARTIAL_422 YBR_RCT YBR_ICT
http://dicom.nema.org/medical/dicom/2019a/output/chtml/part05/sect_8.2.html

About open_memstream https://iotone.ir/shop/public/upload/article/5b9f487cb4536.pdf
*/

char *DCM2PNM::getError() {
    return &errorString[0];
}

void DCM2PNM::error(string str) {
    this->errorString = str;
}

void DCM2PNM::warning(string str) {
    std::cout << str << std::endl;
}

void *DCM2PNM::convert(int *outSize, char *fileData, int fileSize, unsigned int opt_frame, int outputFormat,
                        unsigned int opt_quality, int opt_useClip, int opt_left, int opt_top, unsigned int opt_width,
                        unsigned int opt_height, int opt_scaleType, unsigned int opt_scale_width, unsigned int opt_scale_height) {
    E_FileReadMode      opt_readMode = ERM_autoDetect;    /* default: fileformat or dataset */
    E_TransferSyntax    opt_transferSyntax = EXS_Unknown; /* default: xfer syntax recognition */

    unsigned long       opt_compatibilityMode = CIF_MayDetachPixelData | CIF_TakeOverExternalDataset;
    OFBool              opt_ignoreVoiLutDepth = OFFalse;  /* default: do not ignore VOI LUT bit depth */
                                                          /* default: pixel data may detached if no longer needed */
    OFCmdUnsignedInt    opt_frameCount = 1;               /* default: one frame */

    int                 opt_convertToGrayscale = 0;       /* default: color or grayscale */
    int                 opt_changePolarity = 0;           /* default: normal polarity */
    int                 opt_useAspectRatio = 1;           /* default: use aspect ratio for scaling */
    OFCmdUnsignedInt    opt_useInterpolation = 1;         /* default: use interpolation method '1' for scaling */
    //int                 opt_useClip = 0;                  /* default: don't clip */
    //OFCmdSignedInt      opt_left = 0, opt_top = 0;        /* clip region (origin) */
    //OFCmdUnsignedInt    opt_width = 0, opt_height = 0;    /* clip region (extension) */
    int                 opt_rotateDegree = 0;             /* default: no rotation */
    int                 opt_flipType = 0;                 /* default: no flipping */
    //int                 opt_scaleType = 0;                /* default: no scaling */
                        /* 1 = X-factor, 2 = Y-factor, 3=X-size, 4=Y-size */
    OFCmdFloat          opt_scale_factor = 1.0;
    OFCmdUnsignedInt    opt_scale_size = 1;

    //OFCmdUnsignedInt    opt_scale_width = 0;
    //OFCmdUnsignedInt    opt_scale_height = 0;

    //1 - задать вручную, то нужно указать параметр opt_windowParameter
    //3 - автоматическое определение - это в крайнем случае, если не получится выдрать первый параметр из метаданных

    int                 opt_windowType = 3;               /* default: calc min max */
                        /* 1=Wi, 2=Wl, 3=Wm, 4=Wh, 5=Ww, 6=Wn, 7=Wr */
    OFCmdUnsignedInt    opt_windowParameter = 0;
    OFCmdUnsignedInt    opt_roiLeft = 0, opt_roiTop = 0, opt_roiWidth = 0, opt_roiHeight = 0;
    OFCmdFloat          opt_windowCenter = 0.0, opt_windowWidth = 0.0;

    EF_VoiLutFunction   opt_voiFunction = EFV_Default;
    ES_PresentationLut  opt_presShape = ESP_Default;
    OFString            opt_displayFile;

    DiPNGInterlace      opt_interlace = E_pngInterlaceAdam7;
    DiPNGMetainfo       opt_metainfo  = E_pngFileMetainfo;

    // JPEG parameters
    //OFCmdUnsignedInt    opt_quality = 80;                 /* default: 90% JPEG quality */
    E_SubSampling       opt_sampling = ESS_422;           /* default: 4:2:2 sub-sampling */
    E_DecompressionColorSpaceConversion opt_decompCSconversion = EDC_photometricInterpretation;

    int                 opt_suppressOutput = 0;           /* default: create output */
    E_FileType          opt_fileType = EFT_JPEG;        /* default: JPEG */

    if (outputFormat == 1) {
        opt_fileType = EFT_JPEG;
    } else if (outputFormat == 2) {
        opt_fileType = EFT_PNG;
    } else if (outputFormat == 3) {
        opt_fileType = EFT_8bitBMP;
    }
                                                          /* (binary for file output and ASCII for stdout) */
    OFCmdUnsignedInt    opt_fileBits = 0;                 /* default: 0 */

    FILE *stream = nullptr;
    char *bResult = nullptr;
    size_t dataLength = 0;

    DcmRLEDecoderRegistration::registerCodecs();
    DJDecoderRegistration::registerCodecs(opt_decompCSconversion);

    DcmInputBufferStream inputStream;
    inputStream.setBuffer(fileData, fileSize);
    inputStream.setEos();

    DcmFileFormat *dfile = new DcmFileFormat();
    OFCondition cond = dfile->read(inputStream, opt_transferSyntax, EGL_withoutGL, DCM_MaxReadLength);

    if (cond.bad()) {
        error("Stream read error");
        return nullptr;
    }

    DcmDataset *dataset = dfile->getDataset();
    E_TransferSyntax xfer = dataset->getOriginalXfer();

    Sint32 frameCount;
    if (dataset->findAndGetSint32(DCM_NumberOfFrames, frameCount).bad()) frameCount = 1;

    //std::cout << "frame count is: " << frameCount << std::endl;
    //std::cout << "opt_frame is: " << opt_frame << std::endl;
    //std::cout << "opt_frameCount is: " << opt_frameCount << std::endl;

    if ((opt_frameCount == 0) || ((opt_frame == 1) && (opt_frameCount == OFstatic_cast(Uint32, frameCount)))) {
        // since we process all frames anyway, decompress the complete pixel data (if required)
        opt_compatibilityMode |= CIF_DecompressCompletePixelData;
    }

    if (frameCount > 1) {
        // use partial read access to pixel data (only in case of multiple frames)
       opt_compatibilityMode |= CIF_UsePartialAccessToPixelData;
    }

    DicomImage *di = new DicomImage(dfile, xfer, opt_compatibilityMode, opt_frame - 1, opt_frameCount);
    if (di == NULL) {
        error("Out of memory");
        return nullptr;
    }

    //std::cout << "frame count after is: " << opt_frameCount << std::endl;

    if (di->getStatus() != EIS_Normal) {
        error(DicomImage::getString(di->getStatus()));
        return nullptr;
    }

    //std::cout << "status: " << DicomImage::getString(di->getStatus()) << std::endl;

    if (!opt_suppressOutput) {
        /* try to select frame */
        if (opt_frame != di->getFirstFrame() + 1) {
            error("cannot select frame, invalid frame number");
            return nullptr;
        }

        /* convert to grayscale if necessary */
        if ((opt_convertToGrayscale) && (!di->isMonochrome())) {
             DicomImage *newimage = di->createMonochromeImage();
             if (newimage == NULL) {
                error("Out of memory or cannot convert to monochrome image");
                return nullptr;
             } else if (newimage->getStatus() != EIS_Normal) {
                error(DicomImage::getString(newimage->getStatus()));
                return nullptr;
             } else {
                delete di;
                di = newimage;
             }
        }

         //di->hideAllOverlays();

        /* process VOI parameters */
        switch (opt_windowType) {
            case 1: /* use the n-th VOI window from the image file */
                if ((opt_windowParameter < 1) || (opt_windowParameter > di->getWindowCount())) {
                    error("cannot select VOI window");
                    return nullptr;
                }
                if (!di->setWindow(opt_windowParameter - 1)) warning ("cannot select VOI window");
                break;
            case 2: /* use the n-th VOI look up table from the image file */
                if ((opt_windowParameter < 1) || (opt_windowParameter > di->getVoiLutCount())) {
                    error("cannot select VOI LUT");
                    return nullptr;
                }
                if (!di->setVoiLut(opt_windowParameter - 1, opt_ignoreVoiLutDepth ? ELM_IgnoreValue : ELM_UseValue)) warning("cannot select VOI LUT");
                break;
            case 3: /* Compute VOI window using min-max algorithm */
                if (!di->setMinMaxWindow(0))
                    warning("cannot compute min/max VOI window");
                break;
            case 4: /* Compute VOI window using Histogram algorithm, ignoring n percent */
                if (!di->setHistogramWindow(OFstatic_cast(double, opt_windowParameter)/100.0))
                    warning("cannot compute histogram VOI window");
                break;
            case 5: /* Compute VOI window using center and width */
                if (!di->setWindow(opt_windowCenter, opt_windowWidth)) warning("cannot set VOI window to specified values");
                break;
            case 6: /* Compute VOI window using min-max algorithm ignoring extremes */
                if (!di->setMinMaxWindow(1)) warning("cannot compute min/max VOI window");
                break;
            case 7: /* Compute region of interest VOI window */
                if (!di->setRoiWindow(opt_roiLeft, opt_roiTop, opt_roiWidth, opt_roiHeight))  warning( "cannot compute region of interest VOI window");
                break;
            default: /* no VOI windowing */
                if (di->isMonochrome()) {
                    if (!di->setNoVoiTransformation()) warning("cannot ignore VOI window");
                }
                break;
        }
        /* VOI LUT function */
        if (opt_voiFunction != EFV_Default) {
            if (!di->setVoiLutFunction(opt_voiFunction)) warning("cannot set VOI LUT function");
        }

        /* process presentation LUT parameters */
        if (opt_presShape != ESP_Default) {
            if (!di->setPresentationLutShape(opt_presShape)) warning("cannot set presentation LUT shape");
        }

        /* change polarity */
        if (opt_changePolarity) {
            if (!di->setPolarity(EPP_Reverse)) warning("cannot set polarity");
        }

        /* perform clipping */
        if (opt_useClip && (opt_scaleType == 0)) {
             DicomImage *newimage = di->createClippedImage(opt_left, opt_top, opt_width, opt_height);
             if (newimage == NULL) {
                 error("clipping failed");
                 return nullptr;
             } else if (newimage->getStatus() != EIS_Normal) {
                 error(DicomImage::getString(newimage->getStatus()));
                 return nullptr;
             } else {
                 delete di;
                 di = newimage;
             }
        }

        /* perform rotation */
        if (opt_rotateDegree > 0) {
            if (!di->rotateImage(opt_rotateDegree))  warning("cannot rotate image");
        }

        /* perform flipping */
        if (opt_flipType > 0) {
            switch (opt_flipType) {
                case 1:
                    if (!di->flipImage(1, 0))
                        warning("cannot flip image");
                    break;
                case 2:
                    if (!di->flipImage(0, 1)) warning("cannot flip image");
                    break;
                case 3:
                    if (!di->flipImage(1, 1)) warning("cannot flip image");
                    break;
                default:
                    break;
            }
        }

        if (opt_scaleType > 0) {
            DicomImage *newimage;
            switch (opt_scaleType) {
                case 1:
                    if (opt_useClip)
                        newimage = di->createScaledImage(opt_left, opt_top, opt_width, opt_height, opt_scale_factor, 0.0,
                            OFstatic_cast(int, opt_useInterpolation), opt_useAspectRatio);
                    else
                        newimage = di->createScaledImage(opt_scale_factor, 0.0, OFstatic_cast(int, opt_useInterpolation),
                            opt_useAspectRatio);
                    break;
                case 2:
                    if (opt_useClip)
                        newimage = di->createScaledImage(opt_left, opt_top, opt_width, opt_height, 0.0, opt_scale_factor,
                            OFstatic_cast(int, opt_useInterpolation), opt_useAspectRatio);
                    else
                        newimage = di->createScaledImage(0.0, opt_scale_factor, OFstatic_cast(int, opt_useInterpolation),
                            opt_useAspectRatio);
                    break;
                case 3:
                    if (opt_useClip)
                        newimage = di->createScaledImage(opt_left, opt_top, opt_width, opt_height, opt_scale_size, 0,
                            OFstatic_cast(int, opt_useInterpolation), opt_useAspectRatio);
                    else
                        newimage = di->createScaledImage(opt_scale_size, 0, OFstatic_cast(int, opt_useInterpolation),
                            opt_useAspectRatio);
                    break;
                case 4:
                    if (opt_useClip)
                        newimage = di->createScaledImage(opt_left, opt_top, opt_width, opt_height, 0, opt_scale_size,
                            OFstatic_cast(int, opt_useInterpolation), opt_useAspectRatio);
                    else
                        newimage = di->createScaledImage(0, opt_scale_size, OFstatic_cast(int, opt_useInterpolation),
                            opt_useAspectRatio);
                    break;
                case 5:{
                    if (opt_useClip) {
                    newimage = di->createScaledImage(opt_left, opt_top, opt_width, opt_height, static_cast<unsigned long>(opt_scale_width), static_cast<unsigned long>(opt_scale_height),
                                                OFstatic_cast(int, 0), opt_useAspectRatio);
                    } else {
                        newimage = di->createScaledImage(static_cast<unsigned long>(opt_scale_width), static_cast<unsigned long>(opt_scale_height),
                                        OFstatic_cast(int, opt_useInterpolation), opt_useAspectRatio);
                    }
                }
                break;
                default:
                    newimage = NULL;
                    break;
            }

            if (newimage == NULL) {
                error("Out of memory or cannot scale image");
                return nullptr;
            } else if (newimage->getStatus() != EIS_Normal)  {
                error(DicomImage::getString(newimage->getStatus()));
                return nullptr;
            } else {
                delete di;
                di = newimage;
            }
        }

        /* write selected frame to mem buffer */
        int result = 0;
        stream = open_memstream(&bResult, &dataLength);

        if (stream == NULL) {
            error("Stream is null");
            return nullptr;
        }

        unsigned int frame = opt_frame - 1;

        unsigned int fcount = OFstatic_cast(unsigned int, ((opt_frameCount > 0) && (opt_frameCount <= di->getFrameCount())) ? opt_frameCount : di->getFrameCount());

        //std::cout << "fcount: " << fcount << " di frameCount: " << di->getFrameCount() << std::endl;

        {
            switch (opt_fileType)
            {
                case EFT_RawPNM:
                    result = di->writeRawPPM(stream, 8, 0);
                    fclose(stream);
                    break;
                case EFT_8bitPNM:
                    result = di->writePPM(stream, 8, 0);
                    fclose(stream);
                    break;
                case EFT_16bitPNM:
                    result = di->writePPM(stream, 16, 0);
                    fclose(stream);
                    break;
                case EFT_NbitPNM:
                    result = di->writePPM(stream, OFstatic_cast(int, opt_fileBits), 0);
                    fclose(stream);
                    break;
                case EFT_BMP:
                    result = di->writeBMP(stream, 0, 0);
                    fclose(stream);
                    break;
                case EFT_8bitBMP:
                    result = di->writeBMP(stream, 8, 0);
                    fclose(stream);
                    break;
                case EFT_24bitBMP:
                    result = di->writeBMP(stream, 24, 0);
                    fclose(stream);
                    break;
                case EFT_32bitBMP:
                    result = di->writeBMP(stream, 32, 0);
                    fclose(stream);
                    break;
                case EFT_JPEG:{
                        DiJPEGPlugin plugin;
                        plugin.setQuality(OFstatic_cast(unsigned int, opt_quality));
                        plugin.setSampling(opt_sampling);
                        result = di->writePluginFormat(&plugin, stream, 0);
                    }
                    break;
                case EFT_PNG: {
                        DiPNGPlugin pngPlugin;
                        pngPlugin.setInterlaceType(opt_interlace);
                        pngPlugin.setMetainfoType(opt_metainfo);
                        result = di->writePluginFormat(&pngPlugin, stream, 0);
                        fclose(stream);
                    }
                    break;
                default:
                      result = di->writePPM(stream, 8, 0);
                      fclose(stream);
                    break;
            }

            if (!result) {
                error("Cannot write frame");
                return nullptr;
            }
        }
    }

    delete di;

    DcmRLEDecoderRegistration::cleanup();
    DJDecoderRegistration::cleanup();

    *outSize = static_cast<int>(dataLength);

    return bResult;
}
