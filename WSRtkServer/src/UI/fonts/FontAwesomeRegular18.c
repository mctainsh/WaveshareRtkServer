/*******************************************************************************
 * Size: 18 px
 * Bpp: 1
 * Opts: --bpp 1 --size 18 --no-compress --font Font Awesome 6 Free-Solid-900.otf --range 62206,62153,63424,63423 --format lvgl -o FontAwesomeRegular18.c
 ******************************************************************************/

 #include "lvgl.h"


#ifndef FONTAWESOMEREGULAR18
#define FONTAWESOMEREGULAR18 1
#endif

#if FONTAWESOMEREGULAR18

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+F2C9 "" */
    0x1e, 0xf, 0xc7, 0x39, 0x86, 0x61, 0x98, 0x66,
    0x19, 0x96, 0x6d, 0x9b, 0x66, 0xdb, 0xb3, 0xde,
    0xf7, 0xbc, 0xcd, 0x86, 0x7f, 0x87, 0x80,

    /* U+F2FE "" */
    0x0, 0x60, 0x0, 0x1c, 0x0, 0x7, 0x0, 0x3,
    0xc0, 0xf, 0xfc, 0x7, 0xff, 0x81, 0xff, 0xe0,
    0x3f, 0xf0, 0x3c, 0xcf, 0x1f, 0x33, 0xe7, 0xcc,
    0xf9, 0xff, 0xfe, 0x3f, 0xff, 0x3f, 0x3, 0xff,
    0xe1, 0xff, 0xff, 0xff, 0x7f, 0xff, 0x80,

    /* U+F7BF "" */
    0x1, 0x0, 0x0, 0xe1, 0x0, 0x6c, 0xe0, 0x31,
    0xfc, 0x18, 0x3f, 0x83, 0x1f, 0xc0, 0x6f, 0xe0,
    0xf, 0xf8, 0x7b, 0xfb, 0x3f, 0xfc, 0x67, 0xfe,
    0xc, 0xff, 0xc6, 0x1f, 0x9b, 0x3, 0xf3, 0x83,
    0x7c, 0x40, 0xcf, 0x0, 0x1, 0xc0, 0x0, 0x20,
    0x0,

    /* U+F7C0 "" */
    0x1, 0xe0, 0x0, 0x7f, 0x0, 0x1, 0xe0, 0x0,
    0x1c, 0x1, 0xe3, 0x80, 0x7e, 0x60, 0x3, 0xdd,
    0x0, 0x33, 0xe0, 0x6, 0xfc, 0x61, 0xbf, 0x98,
    0x6f, 0xf8, 0x0, 0x7e, 0x0, 0x1f, 0xc0, 0x7,
    0xf8, 0x0, 0xff, 0x0, 0x1f, 0xe0, 0x0, 0xf0,
    0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 180, .box_w = 10, .box_h = 18, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 23, .adv_w = 288, .box_w = 18, .box_h = 17, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 62, .adv_w = 288, .box_w = 18, .box_h = 18, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 103, .adv_w = 288, .box_w = 18, .box_h = 18, .ofs_x = 0, .ofs_y = -2}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0x35, 0x4f6, 0x4f7
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 62153, .range_length = 1272, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 4, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};

extern const lv_font_t lv_font_montserrat_18;


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t FontAwesomeRegular18 = {
#else
lv_font_t FontAwesomeRegular18 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 18,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = 0,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = &lv_font_montserrat_18,
#endif
    .user_data = NULL,
};



#endif /*#if FONTAWESOMEREGULAR18*/

