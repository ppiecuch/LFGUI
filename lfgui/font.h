#ifndef LFGUI_FONT_H
#define LFGUI_FONT_H

#include <vector>
#include <fstream>
#include <memory>
#include <exception>

namespace lfgui
{

class font
{
public:
    struct bitmap
    {
        int x0=0;
        int x1=0;
        int y0=0;
        int y1=0;
        unsigned char* data=0;
        int width()const{return x1-x0;}
        int height()const{return y1-y0;}

        bitmap(){}

        bitmap(const bitmap& o)
        {
            if(!o.valid())
            {
                x0=0;
                x1=0;
                y0=0;
                y1=0;
                data=0;
                return;
            }
            data=(unsigned char*)malloc(o.width()*o.height());
            x0=o.x0;
            x1=o.x1;
            y0=o.y0;
            y1=o.y1;
            memcpy(data,o.data,o.width()*o.height());
        }

        bitmap& operator=(const bitmap& o)
        {
            if(!o.valid())
            {
                x0=0;
                x1=0;
                y0=0;
                y1=0;
                data=0;
                return *this;
            }
            data=(unsigned char*)malloc(o.width()*o.height());
            x0=o.x0;
            x1=o.x1;
            y0=o.y0;
            y1=o.y1;
            memcpy(data,o.data,o.width()*o.height());
            return *this;
        }

        bitmap(bitmap&& o)
        {
            if(!o.valid())
            {
                x0=0;
                x1=0;
                y0=0;
                y1=0;
                data=0;
                return;
            }
            data=o.data;
            x0=o.x0;
            x1=o.x1;
            y0=o.y0;
            y1=o.y1;
            o.data=0;
        }

        bitmap& operator=(bitmap&& o)
        {
            if(!o.valid())
            {
                x0=0;
                x1=0;
                y0=0;
                y1=0;
                data=0;
                return *this;
            }
            data=o.data;
            x0=o.x0;
            x1=o.x1;
            y0=o.y0;
            y1=o.y1;
            o.data=0;
            return *this;
        }

        ~bitmap()
        {
            if(data)
                free(data);
            data=0;
        }

        bool valid()const{return width();}
    };
private:
    stbtt_fontinfo stbtt_font;
    std::vector<uint8_t> ttf_buffer;
    std::vector<std::vector<bitmap>> glyph_cache;
public:
    font(const std::string& filename)
    {
        std::streampos size;

        std::ifstream file(filename,std::ios::in|std::ios::binary|std::ios::ate);
        if(!file.is_open())
            throw std::logic_error("LFGUI Error: Can't load font file \""+filename+"\".");

        size=file.tellg();
        ttf_buffer.resize(size);
        file.seekg(0,std::ios::beg);
        file.read((char*)ttf_buffer.data(),size);
        file.close();

        stbtt_InitFont(&stbtt_font,ttf_buffer.data(),stbtt_GetFontOffsetForIndex(ttf_buffer.data(),0));
    }

    bitmap get_glyph(unsigned int character,int font_size)
    {
        bitmap b;
        int width;
        int height;
        float s=stbtt_ScaleForPixelHeight(&stbtt_font,font_size);
        stbtt_GetCodepointBitmapBox(&stbtt_font,character,s,s,&b.x0,&b.y0,&b.x1,&b.y1);
//std::cout<<b.x0<<' '<<b.y0<<' '<<b.x1<<' '<<b.y1<<std::endl;
        b.data=stbtt_GetCodepointBitmap(&stbtt_font,s,s,character,&width,&height,0,0);
        return b;
    }

    bitmap get_glyph_cached(unsigned int character,size_t font_size)
    {
        if(glyph_cache.size()<font_size)
            glyph_cache.resize(font_size+1);

        if(character<=glyph_cache[font_size].size())
        {
            if(glyph_cache[font_size][character].width()==0)
                goto create_glyph;
            return glyph_cache[font_size][character];
        }
        glyph_cache[font_size].resize(character+1);
create_glyph:
        glyph_cache[font_size][character]=get_glyph(character,font_size);
        return glyph_cache[font_size][character];
    }

    int text_length(const std::string& text,int font_size)
    {
        int w=0;
        for(unsigned char c:text)
        {
            if(c==' ')
                w+=font_size/3;
            else
                w+=get_glyph_cached(c,font_size).width()+1;
        }
        return w;
    }

    int text_length(const std::string& text,int font_size,size_t start_character,size_t end_character)
    {
        int w=0;
        for(size_t i=start_character;i<end_character;i++)
        {
            if(text[i]==' ')
                w+=font_size/3;
            else
                w+=get_glyph_cached((unsigned char)text[i],font_size).width()+1;    // an additional pixel space between each character
        }
        return w;
    }

    static font& default_font()
    {
        static font f("FreeSans.ttf");
        return f;
    }
};
//static const stbtt_fontinfo& font()
/*static void font()
{
    unsigned char *bitmap;
    int w,h,i,j;
    int c='a';
    int s=20;

    auto file=fopen("FreeSans.ttf","rb");
    fread(ttf_buffer.data(),1,1<<25,file);

    stbtt_InitFont(&font, ttf_buffer.data(), stbtt_GetFontOffsetForIndex(ttf_buffer.data(),0));
    bitmap = stbtt_GetCodepointBitmap(&font, 0,stbtt_ScaleForPixelHeight(&font, s), c, &w, &h, 0,0);

    for (j=0; j < h; ++j)
    {
       for (i=0; i < w; ++i)
          putchar(" .:ioVM@"[bitmap[j*w+i]>>5]);
       putchar('\n');
    }
}*/

}   // namespace lfgui

#endif // LFGUI_FONT_H
