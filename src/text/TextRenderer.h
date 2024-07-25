#ifndef WIREBOUNDDEV_SRC_TEXT_TEXTRENDERER_H_
#define WIREBOUNDDEV_SRC_TEXT_TEXTRENDERER_H_

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>
#include <stdexcept>

#include "../../assets/text/BitmapsInfo.h"
#include "../GlWrappers.h"
#include "../Aabb.h"

class Text;
class StaticText;

class Bitmap {
 public:
  Bitmap(std::string_view path, const bitmaps::BitmapInfo& bmp_info)
      : tex_(path), info_(bmp_info) {
  }
 private:
  Texture tex_;
  bitmaps::BitmapInfo info_;
};


/// render text onto its internal textures (few caching) and give tex coords,
/// so output is just texture id and texture coord like default sprite.
/// The only difference is dtor, where we Invalidate() -
/// - we're not going to use it anymore

// TODO: rename on TextCacher
class TextRenderer {
 public:
  TextRenderer() {
    Init();
  }

  ~TextRenderer() {
    DeInit();
  }

  // TODO: make __current_localization__ global


  //TODO: is it relevant?
  /// returns id (not idx) of rectangle (on int overflow we start from beginning.)
  /// there's too low chances so two rectangles has the same id.
  /// We return only id, not object for safe invalidating, so then if app
  /// ask to draw, we check id and if it's abset - we render it again (on the same id?
  /// - if on the same - we'll never get id-collisiong what would be nice)


  struct Text {
    // TODO: what about "chars_in_row" to automatically set \n?
    int id;
    std::string_view str;
    // in not initialized - empty; draw only via TextRenderer::Render() to check
    // are tex_coords valid at this point
    TexCoords tex_coords;
    // TODO: to reduce validity checks we can add timer based on FPS
  };

  /// for InventorySnap (we provide where to render and texture)
  TexCoords Render(int line_id, const Texture& tex,
                   float left, float right, float top) {
    tex.Bind();
    // we don't remember str even for InventorySnap but simply
    // rewrite if user switch inventory tab
    //TODO:
    // render word, try to append on the same row, if not - advance to next
    // if we got overflow - throw and exception (we neven tolerate overflow
    // and in Release we should guarantee it)
  }

  /// render from localization file (for the first time)
  Text Render(int line) {
    auto str = GetLocalizedLine();
    Text text(str, line); // line as id (indeed it's unique)
    RenderImpl(text);
    return text;
  }

  // is class_Text just another one useless redundant abstraction? - NO, it's ok:
  void Example() {
    int sword_global_id = details::kTextOffsetWeapon + details::weapon::kSwordId;
    std::string_view sword_line = GetLine(sword_id);
    Weapon sword = ParseWeapon(sword_line);
    text_renderer.Render(sword.description_id);
  }

  // TODO: we can move from top to down and after reaching the bottom we start
  //  from some offset like 0.3f, then after reaching the top we also start
  //  from offset 0.3f, but from the top, so it would be 0.7f
  //  ---
  //  A.K. bottom-top skyline switching packing.


  [[nodiscard]] inline bool IsRendered(const Text& text) const {
    return small_text_cache_.IsRendered() || large_text_cache_.IsRendered();
  }

  /// if already rendered to tex_cache_
  void Render(Text text, TexPosition tex_pos) {
    if (!IsRendered(text)) {
      auto rect = RenderToTemp(text);
      if (text.type == Text::Type::kSmall) {
        small_text_cache_.Cache(text);
        small_text_cache_.BindTex();
      } else {
        large_text_cache_.Cache(text);
        large_text_cache_.BindTex();
      }

    }
    text.tex_coords.Bind();
    tex_pos.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }

 private:
  enum class TexType {
    kAscii = 0,
    kLangSpec = 1,
    kTtfGlyph = 2,
    kTemp = 3
  };

  /// useful to calculate them only once
  struct TtfFontData {
    float ttf_scale{0.0f};
    int glyph_height{0};
    int y_adjust{0};
  };

  Aabb RenderToTemp(const Text& text) {
    //TODO: copy from WireboundTextBaker
  }

  class SmallTextCache {
   public:
    const TexCoords& GetCoords(const Text& text) {
      BindTex();
      auto found = std::lower_bound(
          cached_data_.begin(), cached_data_.end(), text.id,
          [](const CachedInstance& instance, int id) {
            return instance.id < id;
          });
      if (found !=  cached_data_.end()) {
        return found->tex_coords;
      }
      return Render(text);
    }

    void BindTex() {
      tex_.Bind();
    }

    bool IsRendered(Text& text) {
      return epoch_ <= text.epoch || IsStillAlive(text);
    }

   private:
    class Skyline {
     public:
      Skyline() {
        skylines_.push_back({});
      }

      // TODO: we can move from top to down and after reaching the bottom we start
      //  from some offset like 0.3f, then after reaching the top we also start
      //  from offset 0.3f, but from the top, so it would be 0.7f
      //  ---
      //  A.K. bottom-top skyline switching packing.

      /// returns left bottom corner
      glm::vec2 Push(TexCoords tex_coords) {
        float width = tex_coords.Width();
        float height = tex_coords.Height();
        auto found = std::lower_bound(
            skylines_.begin(), skylines_.end(), width,
            [height](const Instance& skyline, float width) {
              // TODO: how to add spread_constraint?
              //  (just choose different element - code below doesn't affected
              //  by position as well as sorting order)
              return skyline.width < width &&
                     skyline.y_bottom + height < 1.0f;
            });
        if (found == skylines_.end()) {
          Reset();
          found = skylines_.begin();
        }

        // TODO: we've forgot about wasting of the space for better fitting...

        // TODO:
        //  - check is width < 2.0f
        //  - if there's no such width, we can just find the highest and
        //    put on top of everything

        glm::vec2 pos{found->x_left, found->y_bottom};

        std::vector<Instance> temp_skylines;
        if (width < found->width &&
            width + details::kMinSkylineLen < found->width) {
          /// here we init temp_skylines with all elements except modified;
          /// also init two new/modified elements: smaller, greater
          /// (keep sorted order)

          /// if we can merge with previous (based on height pf the previous)
          Instance smaller, greater;
          if (found != skylines_.begin()
              && (found - 1)->y_bottom == (found->y_bottom + height)) {
            smaller = *found;
            smaller.width = found->width - width;
            smaller.x_left += width;
            greater = *(found - 1);
            greater.width += width;
            if (smaller.width > greater.width) {
              std::swap(smaller, greater);
            }
            /// remove 2 (merged prev and split current)
            temp_skylines.resize(skylines_.size() - 2);
            auto where_to_copy = temp_skylines.begin();
            /// skip (found) and (found-1)
            std::copy(skylines_.begin(), found - 1, where_to_copy);
            where_to_copy += std::distance(skylines_.begin(), found - 1);
            std::copy(found + 1, skylines_.end(), where_to_copy);
          } else {
            /// here we split one skyline into two, so add one
            skylines_.resize(skylines_.size() + 1);
            smaller = *found;
            smaller.width -= width;
            smaller.x_left += width;
            greater = *found;
            greater.width = width;
            greater.y_bottom += height;
            if (smaller.width > greater.width) {
              std::swap(smaller, greater);
            }
            /// remove 1 current (because width has changed)
            temp_skylines.resize(skylines_.size() - 1);
            auto where_to_copy = temp_skylines.begin();
            std::copy(skylines_.begin(), found, where_to_copy);
            where_to_copy += std::distance(skylines_.begin(), found);
            std::copy(found + 1, skylines_.end(), where_to_copy);
          }

          /// copy: all before both -> smaller -> between smaller & greater
          ///   -> greater -> all after both
          auto before_smaller = std::lower_bound(
              temp_skylines.begin(), temp_skylines.end(), smaller,
              [](const Instance& inst, const Instance& smaller) {
                return inst.width < smaller.width;
              });
          // TODO: if !NDEBUG
          if (before_smaller == temp_skylines.end()) {
            throw std::runtime_error("unreachable condition");
          }

          auto before_greater = std::lower_bound(
              temp_skylines.begin(), temp_skylines.end(), greater,
              [](const Instance& inst, const Instance& greater) {
                return inst.width < greater.width;
              });
          // TODO: if !NDEBUG
          if (before_smaller == temp_skylines.end()) {
            throw std::runtime_error("unreachable condition");
          }

          auto where_to_copy = skylines_.begin();
          std::copy(temp_skylines.begin(), before_smaller, where_to_copy);

          where_to_copy += std::distance(temp_skylines.begin(), before_smaller);
          *where_to_copy = smaller;
          where_to_copy += 1;

          std::copy(before_smaller, before_greater, where_to_copy);

          where_to_copy += std::distance(temp_skylines.begin(), before_greater);
          *where_to_copy = greater;
          where_to_copy += 1;

          std::copy(before_greater, temp_skylines.end(), where_to_copy);
        } else {
          // lower_bound, so there they're EQUAL (case greater_than impossible)
          // order depends on width, which we don't change here, so that's all
          found->y_bottom += height;
        }
        return pos;
      }

      void Reset() {
        skylines_.clear();
        skylines_.push_back({}); // always at least one
      }

     private:
      struct Instance {
        float y_bottom{0.0f};
        float x_left{0.0f};
        float width{0.0f}; // we might store x_right, but this is more useful
      };

      /// always sorted in asc order by widths
      std::vector<Instance> skylines_;
    };

    struct CachedInstance {
      int id{0};
      Aabb aabb;
    };

    /// if we've updated epoch, but user ask for text from previous epoch AND
    /// text hadn't been Invalidate()-d yet
    bool IsStillAlive(Text& text) {
      auto found = std::lower_bound(
          cached_data_.begin(), cached_data_.end(), text.id,
          [](const CachedInstance& inst, int id) {
            return inst.id < id;
          });
      if (found == cached_data_.end()) {
        return false;
      }
      Restore(text);
      return true;
    }

    void Restore(Text& text) {
      NOT_IMPLEMENTED;
      glBindFramebuffer();
      //TODO: do we really need framebuffer for glBlitFramebuffer?..
    }

    const TexCoords& Render(const Text& text) {
      /// overflow handling on its own, so we don't know about it, BUT
      /// after overflow pack_skyline_ starts to rewrite data from bottom again.
      /// That means we always check was there on new pos some old rectangles and
      /// if yes - we invalidate them
      auto updated_rect = pack_skyline_.Push(rect);
      // rect drawn from -1;1 (left-top corner) in temp texture, while
      // updated_rect transformed to position in main texture
      Invalidate(updated_rect.top, updated_rect.bottom);
      return pos;
    }

    void Invalidate(float top, float bottom) {
      /// intersects by height, i.e. need to be removed
      auto it_intersects = std::remove_if(
          cached_data_ptrs_.begin(), cached_data_ptrs_.end(),
          [top, bottom](const CachedInstance* instance) {
            return instance->aabb.IntersectsH(top, bottom);
          });
      // int should be enough
      auto removed_num = static_cast<int>(
          std::distance(it_intersects, cached_data_ptrs_.end()));
      std::vector<int> ids_to_remove(removed_num);
      std::transform(it_intersects, cached_data_ptrs_.end(),
                     ids_to_remove.begin(),
                     [](const CachedInstance* instance) -> int {
                       return instance->id;
                     });
      for (auto it = it_intersects; it != cached_data_ptrs_.end(); ++it) {
        ids_to_remove.push_back((*it)->id);
      }

      cached_data_ptrs_.erase(it_intersects, cached_data_ptrs_.end());

      /// remove instances from cached_data_ based on collected IDs
      for (int id : ids_to_remove) {
        auto it = std::remove_if(
            cached_data_.begin(), cached_data_.end(),
            [id](const CachedInstance& instance) {
              return instance.id == id;
            });
        cached_data_.erase(it, cached_data_.end());
      }
    }

    Skyline pack_skyline_;

    Texture tex_;

    /// because Text_instances don't know about working of packing algorithm,
    /// we need no know was certain text invalidated or not.
    /// Sometimes we've set epoch+1 in advance (see SmallTextCache::Render()),
    /// so the only "cache-miss" here is "less_than" epoch
    /// (type int should be enough, but we will see).
    /// If text epoch != 0 it guarantees it was rendered al least once
    int epoch_ {1};

    // sorted by id (fast every-frame lookup for rendering)
    std::vector<CachedInstance> cached_data_;
    // sorted by height (for modifying in case of first-time render to cache)
    std::vector<CachedInstance*> cached_data_ptrs_;
  };

  /// for large text area: we have two such areas in case of "book" in game,
  /// where we have two pages OR in case of long single text column (like
  /// license & agreements) where we can slide flawlessly
  class LargeTextCache {
   public:
    const TexCoords& GetCoords(const Text& text) {
      BindTex();
      // TODO: we don't need ALL space, but rectangle with certain size
      if (text.id == left_id_) {
        return {-1.0f, 0.0f, 1.0f, -1.0f};
      } else if (text.id == right_id_) {
        return {0.0f, 1.0f, 1.0f, -1.0f};
      }
      return Render(text);
    }

    void BindTex() {
      tex_.Bind();
    }

    [[nodiscard]] inline bool IsRendered(const Text& text) const {
      return text.id == left_id_ || text.id == right_id_;
    }

   private:
    const TexCoords& Render(const Text& text) {
      left_ = !left_;
      NOT_IMPLEMENTED;
      if (left_) {
        return {-1.0f, 0.0f, 1.0f, -1.0f};
      } else {
        return {0.0f, 1.0f, 1.0f, -1.0f};
      }
    }

    Texture tex_;
    bool left_{false};
    int left_id_{0};
    int right_id_{0};
  };

  void Init();
  void InitTextures();
  void InitFramebuffers();
  void InitBuffers();

  void DeInit();

  void ResetLocale();
  void ResetFramebuffers();

  void CheckSetup();

  static void SetupGreyscaleTexture(
      GLuint tex_id, int tex_width, int tex_height);

  static void SetupFramebuffer(GLuint fbo_id, GLuint tex_id);

  /// it modifies framebuffer by attaching tex_id
  static void ClearFramebufferTexture(GLuint fb_id, GLuint tex_id);

  void ReadFontFile(std::string_view path,
                    unsigned char data_buffer[]) {
    std::ifstream font_file(path.data(), std::ios::binary);
    if (!font_file) {
      throw std::runtime_error(std::format("Failed to open file: {}", path));
    }
    font_file.read(reinterpret_cast<char*>(data_buffer),
                   details::kFontBufferSize);
    if (font_file.gcount() == 0 && font_file.fail()) {
      throw std::runtime_error(std::format("Failed to read file: {}", path));
    }
  }

  Bitmap bmp_ascii_;
  Bitmap bmp_loc_; // including path to .ttf

  /// useful data calculated once and shared across all glyphs
  TtfFont ttf_loc_;

  std::unique_ptr<unsigned char[]> font_buffer_;
  std::unique_ptr<unsigned char[]> char_buffer_;

  /// if glyph absent in bitmaps, stb_truetype.h render glyph from .ttf here,
  /// then we rerender it to phrases texture
  Texture tex_char_;

  /// before rendering text onto tex_cache_ or InventorySnap we always render
  /// to tex_temp_, because we need to find out its bounding size in order
  /// to pack tightly.
  Texture tex_temp_;

  SmallTextCache small_text_cache_;
  LargeTextCache large_text_cache_;

  /** We render text to 2 targets:
   * - InventorySnap;
   * - Caching.
   *
   * In case of InventorySnap we provide texture id and point top/left,
   * where to render (or even top/left/right in case of InventorySpan::Chat).
   * There's no Invalidate() - InventorySnap automatically handle it and it
   * just fully rewrite OLD by NEW.
   * 1) all what player might want to recheck/clarify fast - so literally SNAPS.
   * 2) we snap only phone view (half-nodepad), so we never snap Map or Archive.
   * 3) common span sections: chat(quest), stats/effect,
   *     some small Instruction/Description
   *
   * In case of Caching TextRenderer provide its own texture and here we
   * render only sections with a lot of text or what is need to read only once.
   * We don't have Invalidate() too and on overflow (we have one pointer-advance
   * or packing-rect structure) we start rewriting on top of the oldest data
   * (data at the top of the caching texture) and invalidate only them - so
   * we take new_text::y_height -> search for all rects intersecting this height
   * and remove them. We done invalidate ALL in case of last important data was
   * written just at the bottom and we need to keep it alive some time.
   * HERE we need packing algorithm, because there we don't know about the text
   * that we want to render, so.. SKYLINE?
   * */

  GLuint vao_{0};
  GLuint vbo_{0};

  Shader shader_;
};

#endif //WIREBOUNDDEV_SRC_TEXT_TEXTRENDERER_H_
