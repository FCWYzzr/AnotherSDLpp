//
// Created by FCWY on 25-3-20.
//

#ifndef SDL_PIXEL_HPP
#define SDL_PIXEL_HPP
#include <SDL3/SDL_pixels.h>
#include "SDL_stdinc.hpp"
#include "SDL_error.hpp"
#include "SDL_endian.hpp"

namespace SDL {
    namespace alpha {
        template<typename T=std::uint8_t>
        constexpr T opaque = 255;

        template<std::floating_point T=float>
        constexpr T opaque<T> = 1.0;

        template<typename T=uint8_t>
        constexpr T transparent = 0;
    }

    enum class PixelType {
        UNKNOWN     =   SDL_PIXELTYPE_UNKNOWN,
        INDEX1      =   SDL_PIXELTYPE_INDEX1,
        INDEX4      =   SDL_PIXELTYPE_INDEX4,
        INDEX8      =   SDL_PIXELTYPE_INDEX8,
        PACKED8     =   SDL_PIXELTYPE_PACKED8,
        PACKED16    =   SDL_PIXELTYPE_PACKED16,
        PACKED32    =   SDL_PIXELTYPE_PACKED32,
        ARRAYU8     =   SDL_PIXELTYPE_ARRAYU8,
        ARRAYU16    =   SDL_PIXELTYPE_ARRAYU16,
        ARRAYU32    =   SDL_PIXELTYPE_ARRAYU32,
        ARRAYF16    =   SDL_PIXELTYPE_ARRAYF16,
        ARRAYF32    =   SDL_PIXELTYPE_ARRAYF32,
        INDEX2      =   SDL_PIXELTYPE_INDEX2
    };

    constexpr SDL_PixelType legacy(const PixelType type) noexcept{
        return static_cast<SDL_PixelType>(type);
    }

    enum class BitmapOrder {
        NONE = SDL_BITMAPORDER_NONE,
        BE   = SDL_BITMAPORDER_4321,
        LE   = SDL_BITMAPORDER_1234
    };

    constexpr SDL_BitmapOrder legacy(const BitmapOrder order) noexcept {
        return static_cast<SDL_BitmapOrder>(order);
    }

    enum class PackedOrder {
        NONE = SDL_PACKEDORDER_NONE,
        XRGB = SDL_PACKEDORDER_XRGB,
        RGBX = SDL_PACKEDORDER_RGBX,
        ARGB = SDL_PACKEDORDER_ARGB,
        RGBA = SDL_PACKEDORDER_RGBA,
        XBGR = SDL_PACKEDORDER_XBGR,
        BGRX = SDL_PACKEDORDER_BGRX,
        ABGR = SDL_PACKEDORDER_ABGR,
        BGRA = SDL_PACKEDORDER_BGRA
    };

    constexpr SDL_PackedOrder legacy(const PackedOrder order) noexcept {
        return static_cast<SDL_PackedOrder>(order);
    }


    enum class ArrayOrder {
        NONE = SDL_ARRAYORDER_NONE,
        RGB  = SDL_ARRAYORDER_RGB,
        RGBA = SDL_ARRAYORDER_RGBA,
        ARGB = SDL_ARRAYORDER_ARGB,
        BGR  = SDL_ARRAYORDER_BGR,
        GBRA = SDL_ARRAYORDER_BGRA,
        ABGR = SDL_ARRAYORDER_ABGR
    };

    constexpr SDL_ArrayOrder legacy(const ArrayOrder order) noexcept {
        return static_cast<SDL_ArrayOrder>(order);
    }


    enum class PackedLayout {
        NONE = SDL_PACKEDLAYOUT_NONE,
        L332 = SDL_PACKEDLAYOUT_332,
        L4444 = SDL_PACKEDLAYOUT_4444,
        L1555 = SDL_PACKEDLAYOUT_1555,
        L5551 = SDL_PACKEDLAYOUT_5551,
        L565 = SDL_PACKEDLAYOUT_565,
        L8888 = SDL_PACKEDLAYOUT_8888,
        L2101010 = SDL_PACKEDLAYOUT_2101010,
        L1010102 = SDL_PACKEDLAYOUT_1010102
    };

    constexpr SDL_PackedLayout legacy(const PackedLayout layout) noexcept {
        return static_cast<SDL_PackedLayout>(layout);
    }

    namespace pixel {
        using SDL::fourcc;
    }

    enum class PixelFormat {
        INDEX1LSB                      = SDL_PIXELFORMAT_INDEX1LSB,
        INDEX1MSB                      = SDL_PIXELFORMAT_INDEX1MSB,
        INDEX2LSB                      = SDL_PIXELFORMAT_INDEX2LSB,
        INDEX2MSB                      = SDL_PIXELFORMAT_INDEX2MSB,
        INDEX4LSB                      = SDL_PIXELFORMAT_INDEX4LSB,
        INDEX4MSB                      = SDL_PIXELFORMAT_INDEX4MSB,
        INDEX8                         = SDL_PIXELFORMAT_INDEX8,
        RGB332                         = SDL_PIXELFORMAT_RGB332,
        XRGB4444                       = SDL_PIXELFORMAT_XRGB4444,
        XBGR4444                       = SDL_PIXELFORMAT_XBGR4444,
        XRGB1555                       = SDL_PIXELFORMAT_XRGB1555,
        XBGR1555                       = SDL_PIXELFORMAT_XBGR1555,
        ARGB4444                       = SDL_PIXELFORMAT_ARGB4444,
        RGBA4444                       = SDL_PIXELFORMAT_RGBA4444,
        ABGR4444                       = SDL_PIXELFORMAT_ABGR4444,
        BGRA4444                       = SDL_PIXELFORMAT_BGRA4444,
        ARGB1555                       = SDL_PIXELFORMAT_ARGB1555,
        RGBA5551                       = SDL_PIXELFORMAT_RGBA5551,
        ABGR1555                       = SDL_PIXELFORMAT_ABGR1555,
        BGRA5551                       = SDL_PIXELFORMAT_BGRA5551,
        RGB565                         = SDL_PIXELFORMAT_RGB565,
        BGR565                         = SDL_PIXELFORMAT_BGR565,
        RGB24                          = SDL_PIXELFORMAT_RGB24,
        BGR24                          = SDL_PIXELFORMAT_BGR24,
        XRGB8888                       = SDL_PIXELFORMAT_XRGB8888,
        RGBX8888                       = SDL_PIXELFORMAT_RGBX8888,
        XBGR8888                       = SDL_PIXELFORMAT_XBGR8888,
        BGRX8888                       = SDL_PIXELFORMAT_BGRX8888,
        ARGB8888                       = SDL_PIXELFORMAT_ARGB8888,
        RGBA8888                       = SDL_PIXELFORMAT_RGBA8888,
        ABGR8888                       = SDL_PIXELFORMAT_ABGR8888,
        BGRA8888                       = SDL_PIXELFORMAT_BGRA8888,
        XRGB2101010                    = SDL_PIXELFORMAT_XRGB2101010,
        XBGR2101010                    = SDL_PIXELFORMAT_XBGR2101010,
        ARGB2101010                    = SDL_PIXELFORMAT_ARGB2101010,
        ABGR2101010                    = SDL_PIXELFORMAT_ABGR2101010,
        RGB48                          = SDL_PIXELFORMAT_RGB48,
        BGR48                          = SDL_PIXELFORMAT_BGR48,
        RGBA64                         = SDL_PIXELFORMAT_RGBA64,
        ARGB64                         = SDL_PIXELFORMAT_ARGB64,
        BGRA64                         = SDL_PIXELFORMAT_BGRA64,
        ABGR64                         = SDL_PIXELFORMAT_ABGR64,
        RGB48_FLOAT                    = SDL_PIXELFORMAT_RGB48_FLOAT,
        BGR48_FLOAT                    = SDL_PIXELFORMAT_BGR48_FLOAT,
        RGBA64_FLOAT                   = SDL_PIXELFORMAT_RGBA64_FLOAT,
        ARGB64_FLOAT                   = SDL_PIXELFORMAT_ARGB64_FLOAT,
        BGRA64_FLOAT                   = SDL_PIXELFORMAT_BGRA64_FLOAT,
        ABGR64_FLOAT                   = SDL_PIXELFORMAT_ABGR64_FLOAT,
        RGB96_FLOAT                    = SDL_PIXELFORMAT_RGB96_FLOAT,
        BGR96_FLOAT                    = SDL_PIXELFORMAT_BGR96_FLOAT,
        RGBA128_FLOAT                  = SDL_PIXELFORMAT_RGBA128_FLOAT,
        ARGB128_FLOAT                  = SDL_PIXELFORMAT_ARGB128_FLOAT,
        BGRA128_FLOAT                  = SDL_PIXELFORMAT_BGRA128_FLOAT,
        ABGR128_FLOAT                  = SDL_PIXELFORMAT_ABGR128_FLOAT,
        YV12                           = SDL_PIXELFORMAT_YV12,
        IYUV                           = SDL_PIXELFORMAT_IYUV,
        YUY2                           = SDL_PIXELFORMAT_YUY2,
        UYVY                           = SDL_PIXELFORMAT_UYVY,
        YVYU                           = SDL_PIXELFORMAT_YVYU,
        NV12                           = SDL_PIXELFORMAT_NV12,
        NV21                           = SDL_PIXELFORMAT_NV21,
        P010                           = SDL_PIXELFORMAT_P010,
        EXTERNAL_OES                   = SDL_PIXELFORMAT_EXTERNAL_OES,
        RGBA32                         = SDL_PIXELFORMAT_RGBA32,
        ARGB32                         = SDL_PIXELFORMAT_ARGB32,
        BGRA32                         = SDL_PIXELFORMAT_BGRA32,
        ABGR32                         = SDL_PIXELFORMAT_ABGR32,
        RGBX32                         = SDL_PIXELFORMAT_RGBX32,
        XRGB32                         = SDL_PIXELFORMAT_XRGB32,
        BGRX32                         = SDL_PIXELFORMAT_BGRX32,
        XBGR32                         = SDL_PIXELFORMAT_XBGR32
    };

    constexpr SDL_PixelFormat legacy(PixelFormat format) noexcept {
        return static_cast<SDL_PixelFormat>(format);
    }

    inline const char* to_string(const PixelFormat format) noexcept {
        return SDL_GetPixelFormatName(legacy(format));
    }

    inline std::tuple<int, Uint32, Uint32, Uint32, Uint32>
        masks(const PixelFormat format) noexcept {
        auto ret = std::tuple<int, Uint32, Uint32, Uint32, Uint32>{};
        if (!SDL_GetMasksForPixelFormat(
            legacy(format),
            &std::get<0>(ret),
            &std::get<1>(ret),
            &std::get<2>(ret),
            &std::get<3>(ret),
            &std::get<4>(ret)
        ))
            throw Error{};
        return ret;
    }

    inline PixelFormat from_masks(int bpp, Uint32 r, Uint32 g, Uint32 b, Uint32 a) noexcept {
        return static_cast<PixelFormat>(SDL_GetPixelFormatForMasks(bpp, r, g, b, a));
    }

    template<typename OrderT>
    requires std::same_as<OrderT, PackedOrder>
        || std::same_as<OrderT, BitmapOrder>
        || std::same_as<OrderT, ArrayOrder>
    constexpr PixelFormat define_pixel_format(
        PixelType type,
        OrderT order,
        PackedLayout layout,
        unsigned bits,
        unsigned bytes
    ) {
        return static_cast<PixelFormat>(SDL_DEFINE_PIXELFORMAT(
            static_cast<unsigned>(type),
            static_cast<unsigned>(order),
            static_cast<unsigned>(layout),
            bits, bytes
        ));
    }

    constexpr bool flag(const PixelFormat format) noexcept {
        return SDL_PIXELFLAG(legacy(format));
    }

    constexpr PixelType type(const PixelFormat format) noexcept {
        return static_cast<PixelType>(SDL_PIXELTYPE(legacy(format)));
    }

    constexpr int order(const PixelFormat format) noexcept {
        return SDL_PIXELORDER(legacy(format));
    }

    constexpr PackedLayout layout(const PixelFormat format) noexcept {
        return static_cast<PackedLayout>(SDL_PIXELTYPE(legacy(format)));
    }

    constexpr int bits_per_pixel(const PixelFormat format) noexcept {
        return SDL_BITSPERPIXEL(legacy(format));
    }

    constexpr int bytes_per_pixel(const PixelFormat format) noexcept {
        return SDL_BITSPERPIXEL(legacy(format));
    }

    constexpr bool is_indexed(const PixelFormat format) noexcept {
        return SDL_ISPIXELFORMAT_INDEXED(legacy(format));
    }

    constexpr bool is_packed(const PixelFormat format) noexcept {
        return SDL_ISPIXELFORMAT_PACKED(legacy(format));
    }

    constexpr bool is_array(const PixelFormat format) noexcept {
        return SDL_ISPIXELFORMAT_ARRAY(legacy(format));
    }

    constexpr bool is_10bit(const PixelFormat format) noexcept {
        return SDL_ISPIXELFORMAT_10BIT(legacy(format));
    }

    constexpr bool is_float(const PixelFormat format) noexcept {
        return SDL_ISPIXELFORMAT_FLOAT(legacy(format));
    }

    constexpr bool is_alpha(const PixelFormat format) noexcept {
        return SDL_ISPIXELFORMAT_ALPHA(legacy(format));
    }

    constexpr bool is_fourcc(const PixelFormat format) noexcept {
        return SDL_ISPIXELFORMAT_FOURCC(legacy(format));
    }

    enum class ColorType {
        UNKNOWN = SDL_COLOR_TYPE_UNKNOWN,
        RGB = SDL_COLOR_TYPE_RGB,
        YCBCR = SDL_COLOR_TYPE_YCBCR
    };

    enum class ColorRange {
        UNKNOWN = SDL_COLOR_RANGE_UNKNOWN,
        LIMITED = SDL_COLOR_RANGE_LIMITED,
        FULL    = SDL_COLOR_RANGE_FULL
    };

    enum class ColorPrimaries {
        UNKNOWN                        = SDL_COLOR_PRIMARIES_UNKNOWN,
        BT709                          = SDL_COLOR_PRIMARIES_BT709,
        UNSPECIFIED                    = SDL_COLOR_PRIMARIES_UNSPECIFIED,
        BT470M                         = SDL_COLOR_PRIMARIES_BT470M,
        BT470BG                        = SDL_COLOR_PRIMARIES_BT470BG,
        BT601                          = SDL_COLOR_PRIMARIES_BT601,
        SMPTE240                       = SDL_COLOR_PRIMARIES_SMPTE240,
        GENERIC_FILM                   = SDL_COLOR_PRIMARIES_GENERIC_FILM,
        BT2020                         = SDL_COLOR_PRIMARIES_BT2020,
        XYZ                            = SDL_COLOR_PRIMARIES_XYZ,
        SMPTE431                       = SDL_COLOR_PRIMARIES_SMPTE431,
        SMPTE432                       = SDL_COLOR_PRIMARIES_SMPTE432,
        EBU3213                        = SDL_COLOR_PRIMARIES_EBU3213
    };

    enum class TransferCharacteristics {
        UNKNOWN                        = SDL_TRANSFER_CHARACTERISTICS_UNKNOWN,
        BT709                          = SDL_TRANSFER_CHARACTERISTICS_BT709,
        UNSPECIFIED                    = SDL_TRANSFER_CHARACTERISTICS_UNSPECIFIED,
        GAMMA22                        = SDL_TRANSFER_CHARACTERISTICS_GAMMA22,
        GAMMA28                        = SDL_TRANSFER_CHARACTERISTICS_GAMMA28,
        BT601                          = SDL_TRANSFER_CHARACTERISTICS_BT601,
        SMPTE240                       = SDL_TRANSFER_CHARACTERISTICS_SMPTE240,
        LINEAR                         = SDL_TRANSFER_CHARACTERISTICS_LINEAR,
        LOG100                         = SDL_TRANSFER_CHARACTERISTICS_LOG100,
        LOG100_SQRT10                  = SDL_TRANSFER_CHARACTERISTICS_LOG100_SQRT10,
        IEC61966                       = SDL_TRANSFER_CHARACTERISTICS_IEC61966,
        BT1361                         = SDL_TRANSFER_CHARACTERISTICS_BT1361,
        SRGB                           = SDL_TRANSFER_CHARACTERISTICS_SRGB,
        BT2020_10BIT                   = SDL_TRANSFER_CHARACTERISTICS_BT2020_10BIT,
        BT2020_12BIT                   = SDL_TRANSFER_CHARACTERISTICS_BT2020_12BIT,
        PQ                             = SDL_TRANSFER_CHARACTERISTICS_PQ,
        SMPTE428                       = SDL_TRANSFER_CHARACTERISTICS_SMPTE428,
        HLG                            = SDL_TRANSFER_CHARACTERISTICS_HLG
    };

    enum class MatrixCoefficients {
        IDENTITY                       = SDL_MATRIX_COEFFICIENTS_IDENTITY,
        BT709                          = SDL_MATRIX_COEFFICIENTS_BT709,
        UNSPECIFIED                    = SDL_MATRIX_COEFFICIENTS_UNSPECIFIED,
        FCC                            = SDL_MATRIX_COEFFICIENTS_FCC,
        BT470BG                        = SDL_MATRIX_COEFFICIENTS_BT470BG,
        BT601                          = SDL_MATRIX_COEFFICIENTS_BT601,
        SMPTE240                       = SDL_MATRIX_COEFFICIENTS_SMPTE240,
        YCGCO                          = SDL_MATRIX_COEFFICIENTS_YCGCO,
        BT2020_NCL                     = SDL_MATRIX_COEFFICIENTS_BT2020_NCL,
        BT2020_CL                      = SDL_MATRIX_COEFFICIENTS_BT2020_CL,
        SMPTE2085                      = SDL_MATRIX_COEFFICIENTS_SMPTE2085,
        CHROMA_DERIVED_NCL             = SDL_MATRIX_COEFFICIENTS_CHROMA_DERIVED_NCL,
        CHROMA_DERIVED_CL              = SDL_MATRIX_COEFFICIENTS_CHROMA_DERIVED_CL,
        ICTCP                          = SDL_MATRIX_COEFFICIENTS_ICTCP
    };

    enum class ChromaLocation {
        NONE=  SDL_CHROMA_LOCATION_NONE,
        LEFT = SDL_CHROMA_LOCATION_LEFT,
        CENTER = SDL_CHROMA_LOCATION_CENTER,
        TOPLEFT = SDL_CHROMA_LOCATION_TOPLEFT
    };

    enum class ColorSpace {
        UNKNOWN                        = SDL_COLORSPACE_UNKNOWN,
        SRGB                           = SDL_COLORSPACE_SRGB,
        SRGB_LINEAR                    = SDL_COLORSPACE_SRGB_LINEAR,
        HDR10                          = SDL_COLORSPACE_HDR10,
        JPEG                           = SDL_COLORSPACE_JPEG,
        BT601_LIMITED                  = SDL_COLORSPACE_BT601_LIMITED,
        BT601_FULL                     = SDL_COLORSPACE_BT601_FULL,
        BT709_LIMITED                  = SDL_COLORSPACE_BT709_LIMITED,
        BT709_FULL                     = SDL_COLORSPACE_BT709_FULL,
        BT2020_LIMITED                 = SDL_COLORSPACE_BT2020_LIMITED,
        BT2020_FULL                    = SDL_COLORSPACE_BT2020_FULL,
        RGB_DEFAULT                    = SDL_COLORSPACE_RGB_DEFAULT
    };

    constexpr ColorSpace define_color_space(
        ColorType type,
        ColorRange range,
        ColorPrimaries primaries,
        TransferCharacteristics transfer,
        MatrixCoefficients matrix,
        ChromaLocation chroma
        ) {
        return static_cast<ColorSpace>(SDL_DEFINE_COLORSPACE(
            static_cast<int>(type),
            static_cast<int>(range),
            static_cast<int>(primaries),
            static_cast<int>(transfer),
            static_cast<int>(matrix),
            static_cast<int>(chroma)
        ));
    }

    constexpr SDL_Colorspace legacy(ColorSpace color_space) {
        return static_cast<SDL_Colorspace>(color_space);
    }

    constexpr ColorType type(ColorSpace space) {
        return static_cast<ColorType>(SDL_COLORSPACETYPE(legacy(space)));
    }

    constexpr ColorRange range(ColorSpace space) {
        return static_cast<ColorRange>(SDL_COLORSPACERANGE(legacy(space)));
    }

    constexpr ColorPrimaries primaries(ColorSpace space) {
        return static_cast<ColorPrimaries>(SDL_COLORSPACEPRIMARIES(legacy(space)));
    }

    constexpr ChromaLocation chroma(ColorSpace space) {
        return static_cast<ChromaLocation>(SDL_COLORSPACECHROMA(legacy(space)));
    }

    constexpr TransferCharacteristics transfer_characteristics(ColorSpace space) {
        return static_cast<TransferCharacteristics>(SDL_COLORSPACETRANSFER(legacy(space)));
    }

    constexpr MatrixCoefficients matrix_coefficients(ColorSpace space) {
        return static_cast<MatrixCoefficients>(SDL_COLORSPACEMATRIX(legacy(space)));
    }

    constexpr bool is_matrix_bt601(ColorSpace space) {
        return SDL_ISCOLORSPACE_MATRIX_BT601(legacy(space));
    }

    constexpr bool is_matrix_bt709(ColorSpace space) {
        return SDL_ISCOLORSPACE_MATRIX_BT709(legacy(space));
    }

    constexpr bool is_matrix_bt2020_ncl(ColorSpace space) {
        return SDL_ISCOLORSPACE_MATRIX_BT2020_NCL(legacy(space));
    }

    constexpr bool is_range_limited(ColorSpace space) {
        return SDL_ISCOLORSPACE_LIMITED_RANGE(legacy(space));
    }

    constexpr bool is_range_full(ColorSpace space) {
        return SDL_ISCOLORSPACE_FULL_RANGE(legacy(space));
    }

    template<typename T=Uint8>
    struct Color: SDL_Color{};

    template<>
    struct Color<float>: SDL_FColor {};

    using PixelFormatDetails = SDL_PixelFormatDetails;

    inline const PixelFormatDetails* detail(const PixelFormat format) noexcept {
        return SDL_GetPixelFormatDetails(legacy(format));
    }


    struct Palette{
        SDL_Palette*
            handle;

        explicit Palette(int ncolors): handle{SDL_CreatePalette(ncolors)} {
            if (!handle)
                throw Error{};
        }

        ~Palette() noexcept {
            SDL_DestroyPalette(handle);
        }

        auto colors() {
            return std::span{reinterpret_cast<Color<>*>(handle->colors), static_cast<size_t>(handle->ncolors)};
        }
        auto colors() const {
            return std::span{reinterpret_cast<const Color<>*>(handle->colors), static_cast<size_t>(handle->ncolors)};
        }

        // ReSharper disable once CppMemberFunctionMayBeConst
        void set_ranges(int offset, std::span<Color<>> span) {
            if (!SDL_SetPaletteColors(handle, span.data(), offset, span.size()))
                throw Error{};
        }
    };

    inline Uint32 map_rgb(const PixelFormatDetails& detail, const Palette& palette, Uint8 r, Uint8 g, Uint8 b) noexcept {
        return SDL_MapRGB(&detail, palette.handle, r, g, b);
    }

    inline Uint32 map_rgba(const PixelFormatDetails& detail, const Palette& palette, Uint8 r, Uint8 g, Uint8 b, Uint8 a) noexcept {
        return SDL_MapRGBA(&detail, palette.handle, r, g, b, a);
    }

    inline std::tuple<Uint8, Uint8, Uint8>
        get_rgb(Uint8 mapped, const PixelFormatDetails& detail, const Palette& palette) {
        auto ret = std::tuple<Uint8, Uint8, Uint8>{};
        SDL_GetRGB(mapped, &detail, palette.handle, &std::get<0>(ret), &std::get<1>(ret), &std::get<2>(ret));
    }

    inline std::tuple<Uint8, Uint8, Uint8, Uint8>
        get_rgba(Uint8 mapped, const PixelFormatDetails& detail, const Palette& palette) {
        auto ret = std::tuple<Uint8, Uint8, Uint8, Uint8>{};
        SDL_GetRGBA(mapped, &detail, palette.handle, &std::get<0>(ret), &std::get<1>(ret), &std::get<2>(ret), &std::get<3>(ret));
    }



}


#endif //SDL_PIXEL_HPP
