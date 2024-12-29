#include "direct_text_render.hpp"
#include "kernel/libk_decls.h"
#include "string.h"
void direct_text_render::__advance() noexcept { if(__cursor_pos.x < __fb_col_cap()) __cursor_pos.x++; else endl(); }
void direct_text_render::__write_one(char c) { if(c == '\n') endl(); else if(c) { __render(c, __fb_ptr, __cursor_pos); __advance(); } }
void direct_text_render::cls() { if(__fb_ptr) for(point p {0, 0}; p.y < __fb_row_cap(); p.y++) for(p.x = 0; p.x < __fb_col_cap(); p.x++) __render.fill(__render.bg_color(), __fb_ptr, p); }
void direct_text_render::endl() { __cursor_pos.x = 0; if(__cursor_pos.y < __fb_row_cap())  __cursor_pos.y++; else __cursor_pos.y = 0; }
void direct_text_render::print_text(const char *text) { if(__fb_ptr) for(size_t i = 0; i < strnlen(text, __fb_col_cap() * __fb_row_cap()); i++) __write_one(text[i]); }
void direct_text_render::print_text(std::string const &text) { if(__fb_ptr) for(size_t i = 0; i < text.size(); i++) __write_one(text[i]); }
void direct_text_render::print_line(const char *text) { print_text(text); endl(); }
void direct_text_render::print_line(std::string const &text) { print_text(text); endl(); }
