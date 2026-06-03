# TODO

## `view_faidx_chunk` return type (`src/fastafs.cpp:1560`)
- Verander return type van `uint32_t` naar `size_t`. Dat matcht `std::string::copy()`
  (geeft al `size_t`) en de `buffer_size`-parameter; de narrowing-cast in de `return`
  vervalt. Consistent met `view_dict_chunk` (al `size_t`).
- Bredere opschoning: de `view_*_chunk`-methodes mengen `uint32_t` en `size_t`.
  Idealiter alle naar `size_t` trekken.
- Latente bug (los van het type): `contents.size() - file_offset` is een unsigned
  aftrekking; bij `file_offset > contents.size()` (lezen voorbij EOF) underflowt dit
  en `contents.copy(..., file_offset)` gooit `std::out_of_range`. Guard toevoegen:
  `if(file_offset >= (off_t)contents.size()) return 0;`.

## `database::add` naar `const char *` (`include/database.hpp:23`)
- `add(char *)` muteert de pointer niet; net als `get` kan het `const char *` worden.
- Dan vervallen de `(char*)`-casts in `test/database/test_database.cpp`
  (regels 52, 57, 73, 76, 106) en is de API consistent met `get`.
