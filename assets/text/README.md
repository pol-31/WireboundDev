after generating the data by gperf, we renamed filename, removed class declaration,
add include guard, add namespace, replace const char* by std::string_view, etc...
But we also:
- replaced return type "const struct Phrase*" by int, because we need only line num;
- add constexpr qualifier.

