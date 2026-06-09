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

## Benchmarks uitleesprestaties

- Meet de doorvoersnelheid van het uitlezen van de virtuele **(1) 2bit**, **(2) dict** en **(3) fai** bestanden via FUSE.
- Meet de prestaties van het **cachen van een grote 2bit file naar fastafs** (conversie/ingest).
- Gebruik `benchmarks/run_view_benchmarks.sh` als startpunt; uitbreiden met de ontbrekende bestandstypen.

## Geheugenlek in mount-lifecycle (`init_ffs2f`, `src/fuse.cpp`)
- valgrind op `fastafs mount -f <plain-archief> <mnt>` meldt bij afsluiten:
  `definitely lost: 32 bytes in 1 blocks` (168 bytes incl. indirect, loss record 7/7).
- Stacktrace: `operator new` → `fastafs::init_ffs2f(unsigned int, bool)` →
  `parse_args(...)` → `fuse(...)` → `main`.
- Oorzaak: in `parse_args` (`src/fuse.cpp:725-726`) worden twee caches gealloceerd
  (`fi->cache = fi->f->init_ffs2f(padding, masking)` en
  `fi->cache_p0 = fi->f->init_ffs2f(0, true)`) en in de `fuse_instance` bewaard,
  maar nooit ge-`delete`d bij het afsluiten van de mount. Idem mogelijk voor
  `fi->f` / `fi->u2b` zelf.
- Fix: de `fuse_instance` netjes opruimen na `fuse_main()` (of `unique_ptr`/RAII),
  zodat `init_ffs2f`-allocaties (en de daarin geneste vectoren — vandaar de 136
  indirecte bytes) vrijkomen. Daarna valgrind opnieuw draaien ter verificatie.

## `ls *.fa` op het mountpoint geeft een error
- Ontdekt tijdens handmatig testen van een **plain** (ongecomprimeerd) mount.
  Lezen werkt prima (`less` / `cat` op het virtuele `.fa` geeft correcte
  sequentie), maar een `ls *.fa` in het mountpoint geeft een foutmelding.
- Vermoeden: het probleem zit niet in de read-path maar in `getattr`/lookup
  voor het glob-geëxpandeerde `.fa`-pad (zie `do_getattr` in `src/fuse.cpp`,
  rond regel 110-145), of in `do_readdir` (`src/fuse.cpp:171`). Reads
  (`do_read`) werken immers wel.
- TODO: exacte foutmelding van `ls` vastleggen (errno) en herleiden naar de
  betreffende FUSE-callback; daarna een minimale reproductie toevoegen.

## `database::add` naar `const char *` (`include/database.hpp:23`)
- `add(char *)` muteert de pointer niet; net als `get` kan het `const char *` worden.
- Dan vervallen de `(char*)`-casts in `test/database/test_database.cpp`
  (regels 52, 57, 73, 76, 106) en is de API consistent met `get`.
