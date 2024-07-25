#ifndef WIREBOUNDDEV_ASSETS_TEXT_PHRASESHASHTEXT_H_
#define WIREBOUNDDEV_ASSETS_TEXT_PHRASESHASHTEXT_H_

/* C++ code produced by gperf version 3.1 */
/* Command-line: gperf -e'$' -t -LC++ -H Hash -N GetPhraseLine -Z PhrasesHash -CEIG -W kWordlist /home/pavlo/Desktop/prerender/en_prerendered_text_temp.cpp  */
/* Computed positions: -k'1,8,$' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#include <array>
#include <cstring>
#include <string_view>

namespace hash_text {

struct Phrase{
  std::string_view name;
  int line{-1};
};

enum {
  TOTAL_KEYWORDS = 86,
  MIN_WORD_LENGTH = 2,
  MAX_WORD_LENGTH = 99,
  MIN_HASH_VALUE = 2,
  MAX_HASH_VALUE = 136
};

/* maximum key range = 135, duplicates = 0 */

inline constexpr std::array<Phrase, 137> kWordlist = {
    Phrase{""}, Phrase{""},
    Phrase{"No",77},
    Phrase{"Use",52},
    Phrase{"Move",37},
    Phrase{"right",41},
    Phrase{"Escape",58},
    Phrase{"Monitor",14},
    Phrase{"LMB",47},
    Phrase{"back",39},
    Phrase{"Block",80},
    Phrase{"Motion Blur",20},
    Phrase{"Ok",75},
    Phrase{"Language",9},
    Phrase{"grab",42},
    Phrase{"Shift",50},
    Phrase{"attack",43},
    Phrase{"Shadows",30},
    Phrase{"SpaceBar",48},
    Phrase{"Fire",81},
    Phrase{"Shock",84},
    Phrase{"action",71},
    Phrase{"Stamina",74},
    Phrase{"key",70},
    Phrase{""},
    Phrase{"Button Layout Preset",34},
    Phrase{"reload",68},
    Phrase{""},
    Phrase{"Sky",28},
    Phrase{""},
    Phrase{"Inventory Pause",4},
    Phrase{"Liquid",29},
    Phrase{"Special Jumping Sounds",33},
    Phrase{"FPS Mark",10},
    Phrase{"left",40},
    Phrase{"throw",44},
    Phrase{"Damage",79},
    Phrase{"Screen resolution",15},
    Phrase{"iiiiiMaster volume",3},
    Phrase{"MSAA",24},
    Phrase{"aaaaaBrightness",2},
    Phrase{"Accept",78},
    Phrase{"Display Mode",16},
    Phrase{"Activate",53},
    Phrase{"inventory",59},
    Phrase{"Dialogue Volume",31},
    Phrase{"Hold to Sit",36},
    Phrase{"forward",38},
    Phrase{"lay",63},
    Phrase{"quickload",61},
    Phrase{"SFX (effects) Volume",32},
    Phrase{"Frame Rate Limitation",17},
    Phrase{"console",62},
    Phrase{"SFX",26},
    Phrase{"Dynamic Auto Camera",11},
    Phrase{"toggle walk/run",57},
    Phrase{"Hold to Run",35},
    Phrase{"aqwertyuiopasdfghjklzxcvbnm",5},
    Phrase{"CapsLock",51},
    Phrase{"Hold",55},
    Phrase{"Particle System",19},
    Phrase{"Poison",83},
    Phrase{"quick-tools wheel",66},
    Phrase{"map",56},
    Phrase{"Cold",82},
    Phrase{"Gamma",23},
    Phrase{"vision mode",64},
    Phrase{"--- here all keys (jump, run, walk, etc...) ---",69},
    Phrase{"Invert X-axis",12},
    Phrase{"heal",67},
    Phrase{"Post Processing",21},
    Phrase{"abcdefghijklmnopqrstuvwzyx",8},
    Phrase{"Quality",18},
    Phrase{"custom action",65},
    Phrase{"Ctrl",49},
    Phrase{"Vegetation",27},
    Phrase{"Health",72},
    Phrase{""},
    Phrase{"Yes",76},
    Phrase{"quicksave",60},
    Phrase{""},
    Phrase{"(important phrases for prerendering in font#2)",0},
    Phrase{"HP",73},
    Phrase{"RMB",46},
    Phrase{"WASD",45},
    Phrase{""},
    Phrase{"V-Sync",22},
    Phrase{""}, Phrase{""},
    Phrase{"Jump",54},
    Phrase{""},
    Phrase{"QWERTYUIOPASDFGHJKLZXCVBNM",6},
    Phrase{"General",25},
    Phrase{""}, Phrase{""}, Phrase{""}, Phrase{""}, Phrase{""}, Phrase{""}, Phrase{""},Phrase{""},
    Phrase{"pppppSensitivity",1},
    Phrase{""},
    Phrase{"Invert Y-axis",13},
    Phrase{""},Phrase{""},Phrase{""},Phrase{""},Phrase{""},Phrase{""},Phrase{""},Phrase{""},Phrase{""},
    Phrase{""},Phrase{""},Phrase{""},Phrase{""},Phrase{""},Phrase{""},Phrase{""},Phrase{""},Phrase{""},
    Phrase{""},Phrase{""},Phrase{""},Phrase{""},Phrase{""},Phrase{""},Phrase{""},
    Phrase{"// here may be more other __prerendered with different fonts, but <=2 is the most common and useful",85},
    Phrase{""},Phrase{""},Phrase{""},Phrase{""},Phrase{""},Phrase{""},
    Phrase{"ABCDEFGHIJKLMNOPQRSTUVWZYX",7}
};

inline constexpr std::array<unsigned char, 256> asso_values = {
    137, 137, 137, 137, 137, 137, 137, 137, 137, 137,
    137, 137, 137, 137, 137, 137, 137, 137, 137, 137,
    137, 137, 137, 137, 137, 137, 137, 137, 137, 137,
    137, 137,   0, 137, 137, 137, 137, 137, 137, 137,
    15,  15, 137, 137, 137,  10, 137,   0, 137, 137,
    137, 137, 137, 137, 137, 137, 137, 137, 137, 137,
    137, 137, 137, 137, 137,  35,   0,  40,  30,   0,
    15,  55,  35,  15,  25, 137,   5,   0,   0,   5,
    45,  50,  80,  10, 137,   0,  35,  50,  40,  75,
    137, 137, 137, 137, 137, 137, 137,   5,   0,  45,
    20,   0,  20,  10,  35,  20, 137,   5,  30,   0,
    10,   0,  60,  15,   0,   0,   0,  25,  55,  30,
    5,  15, 137, 137, 137, 137, 137, 137, 137, 137,
    137, 137, 137, 137, 137, 137, 137, 137, 137, 137,
    137, 137, 137, 137, 137, 137, 137, 137, 137, 137,
    137, 137, 137, 137, 137, 137, 137, 137, 137, 137,
    137, 137, 137, 137, 137, 137, 137, 137, 137, 137,
    137, 137, 137, 137, 137, 137, 137, 137, 137, 137,
    137, 137, 137, 137, 137, 137, 137, 137, 137, 137,
    137, 137, 137, 137, 137, 137, 137, 137, 137, 137,
    137, 137, 137, 137, 137, 137, 137, 137, 137, 137,
    137, 137, 137, 137, 137, 137, 137, 137, 137, 137,
    137, 137, 137, 137, 137, 137, 137, 137, 137, 137,
    137, 137, 137, 137, 137, 137, 137, 137, 137, 137,
    137, 137, 137, 137, 137, 137, 137, 137, 137, 137,
    137, 137, 137, 137, 137, 137
};

static consteval unsigned int Hash(std::string_view str) {
  unsigned int hval = str.length();
  switch (hval) {
    default:
      hval += asso_values[static_cast<unsigned char>(str[7])];
      [[fallthrough]];
    case 7:
      [[fallthrough]];
    case 6:
      [[fallthrough]];
    case 5:
      [[fallthrough]];
    case 4:
      [[fallthrough]];
    case 3:
      [[fallthrough]];
    case 2:
      [[fallthrough]];
    case 1:
      hval += asso_values[static_cast<unsigned char>(str[0])];
      break;
  }
  return hval + asso_values[static_cast<unsigned char>(str.back())];
}

/// line as an id
consteval int GetPhraseId(
    std::string_view str) {
  std::size_t len = str.size();
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
    auto key = Hash(str);
    if (key <= MAX_HASH_VALUE) {
      if (str == kWordlist[key].name) {
        return kWordlist[key].line;
      }
    }
  }
  /// STATIC_UNREACHABLE (idk how to implement this macros...) TODO:
  /// If you here or simply got
  /// "Constexpr variable 'idx2' must be initialized by a constant expression"
  /// OR
  /// "control reached end of constexpr function"
  /// That means you provided invalid phrase (should be in kWordlist)

  // intentionally no return statement (see explanation above)
}

} // namespace hash_text

#endif //WIREBOUNDDEV_ASSETS_TEXT_PHRASESHASHTEXT_H_
