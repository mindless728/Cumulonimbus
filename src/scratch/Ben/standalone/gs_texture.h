#ifndef GS_TEXTURE_H_
#define GS_TEXTURE_H_
    #define TEXTURE_MODE_AND            0x1
    #define TEXTURE_MODE_OR             0x2
    #define TEXTURE_MODE_XOR            0x3
    #define TEXTURE_MODE_OVERWRITE      0x4
    typedef struct _gs_texture {
        uint8_t width;
        uint8_t height;
        uint16_t num_textures;
        uint16_t ***data;
    } gs_texture_t;

    void draw_pixel( uint32_t x, uint32_t y, pixel_t color );
    void draw_double_pixel( uint32_t x, uint32_t y, pixel_t color1, pixel_t color2 );
    void draw_texture( uint32_t x, uint32_t y, uint16_t texture_num );
#endif
