#include <atlimage.h> // Active Template Library IMAGE
#include <combaseapi.h> // Component Object Model BASE API
#include <gdiplusimaging.h> // Graphics Device Interface PLUS IMAGING
#include <objbase.h> // OBJect BASE
#include <shlguid.h> // SHelL Globally Unique IDentifier
#include <shobjidl_core.h> // SHell OBJect Interface Definition Language CORE
#include <thumbcache.h> // THUMBnail CACHE
#include <windef.h> // WINdows DEFinitions
#include <winerror.h> // WINdows Error

#include <iostream>
#include <cstdlib>

#define MAXIMUM_FILEPATH_LENGTH 32767
#define MAXIMUM_THUMBNAIL_SIZE_PIXELS 2160

int main(int argc, char* argv[]) {
  wchar_t kSrcFilepath[MAXIMUM_FILEPATH_LENGTH];
  std::mbstowcs(kSrcFilepath, argv[1], MAXIMUM_FILEPATH_LENGTH);
  wchar_t kDstFilepath[MAXIMUM_FILEPATH_LENGTH];
  std::mbstowcs(kDstFilepath, argv[2], MAXIMUM_FILEPATH_LENGTH);

  HRESULT kResult; // https://learn.microsoft.com/zh-tw/windows/win32/seccrypto/common-hresult-values

  kResult = CoInitializeEx(NULL, // https://learn.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-coinitializeex
                           COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE); // https://learn.microsoft.com/en-us/windows/win32/api/objbase/ne-objbase-coinit
  if (FAILED(kResult)) { // https://learn.microsoft.com/en-us/windows/win32/api/winerror/nf-winerror-failed
    std::cout << "CoInitializeEx failed with error code " << kResult << std::endl;
    return kResult;
  }

  IShellItem* pItem; // https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-ishellitem
  kResult = SHCreateItemFromParsingName(kSrcFilepath, // https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-shcreateitemfromparsingname
                                        NULL,
                                        IID_PPV_ARGS(&pItem)); // https://learn.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-iid_ppv_args
  if (FAILED(kResult)) {
    std::cout << "SHCreateItemFromParsingName failed with error code " << kResult << std::endl;
    return kResult;
  }

  IThumbnailProvider* pThumbnailProvider; // https://learn.microsoft.com/en-us/windows/win32/api/thumbcache/nn-thumbcache-ithumbnailprovider
  kResult = pItem->BindToHandler(NULL,
                                 BHID_ThumbnailHandler, // https://learn.microsoft.com/zh-tw/windows/win32/api/shobjidl_core/nf-shobjidl_core-ishellitem-bindtohandler
                                 IID_PPV_ARGS(&pThumbnailProvider));
  if (FAILED(kResult)) {
    std::cout << "IShellItem::BindToHandler failed with error code " << kResult << std::endl;
    return kResult;
  }

  HBITMAP kBitmap; // https://learn.microsoft.com/zh-tw/windows/win32/winprog/windows-data-types
  WTS_ALPHATYPE kAlphaType; // https://learn.microsoft.com/en-us/windows/win32/api/thumbcache/nf-thumbcache-ithumbnailprovider-getthumbnail
  kResult = pThumbnailProvider->GetThumbnail(MAXIMUM_THUMBNAIL_SIZE_PIXELS, &kBitmap, &kAlphaType);
  if (FAILED(kResult)) {
    std::cout << "IThumbnailProvider::GetThumbnail failed with error code " << kResult << std::endl;
    return kResult;
  }

  // TODO confirm order
  pThumbnailProvider->Release();
  pItem->Release();
  CoUninitialize();



  CImage kImage; // https://learn.microsoft.com/en-us/cpp/atl-mfc-shared/reference/cimage-class?view=msvc-170
  kImage.Attach(kBitmap);
  kResult = kImage.Save(kDstFilepath,
                        Gdiplus::ImageFormatJPEG); // https://learn.microsoft.com/zh-tw/windows/win32/gdiplus/-gdiplus-constant-image-file-format-constants
  if (FAILED(kResult)) {
    std::cout << "CImage::Save failed with error code " << kResult << std::endl;
    return kResult;
  }

  return 0;
}
