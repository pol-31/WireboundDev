#ifndef WIREBOUNDDEV_SRC_TEXT_TEXT_H_
#define WIREBOUNDDEV_SRC_TEXT_TEXT_H_

#include "../Sprite.h"
#include "TextRenderer.h"

/// here we don't care about is it mask or not - TextRenderer is responsible
/// for that; so we only set color or brightness

class Text final : public Sprite {
 public:
  Text() = delete;
  Text(TextRenderer& text_renderer,
       const Texture& tex, TexCoords tex_coords)
      : Sprite(tex, tex_coords),
        text_renderer_(text_renderer) {}

  /// instance uniquely maps onto TextRenderer textures (by now
  /// there's no need to have two instances mapping onto the same coords)
  Text(const Text&) = delete;
  Text& operator=(const Text&) = delete;

  Text(Text&& other) = default;
  Text& operator=(Text&& other) = delete; // member-ref

  ~Text() override {
    Invalidate();
  }

 private:
  void Invalidate() {
    text_renderer_.Invalidate(tex_coords_);
  }

  TextRenderer& text_renderer_;
};

class StaticText final : public StaticSprite {
 public:
  StaticText() = delete;
  StaticText(TextRenderer& text_renderer,
       const Texture& tex, TexCoords tex_coords,
       const std::array<float, 8>& position)
      : StaticSprite(tex, tex_coords, position),
        text_renderer_(text_renderer) {}

  /// instance uniquely maps onto TextRenderer textures (by now
  /// there's no need to have two instances mapping onto the same coords)
  StaticText(const StaticText&) = delete;
  StaticText& operator=(const StaticText&) = delete;

  StaticText(StaticText&& other) = default;
  StaticText& operator=(StaticText&& other) = delete; // member-ref

  ~StaticText() override {
    Invalidate();
  }

 private:
  void Invalidate() {
    text_renderer_.Invalidate(tex_coords_);
  }

  TextRenderer& text_renderer_;
};

#endif //WIREBOUNDDEV_SRC_TEXT_TEXT_H_
