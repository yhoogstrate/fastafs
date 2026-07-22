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

## `.2bit` mount van fourbit/fivebit sequences geeft stille datacorruptie
- `view_ucsc2bit_chunk()` en `ucsc2bit_filesize()` controleren niet of een sequence
  daadwerkelijk twobit-gecodeerd is. Bij een archief met IUPAC (fourbit) of proteïne
  (fivebit) sequences decodeert `view_fasta_chunk()` de karakters correct terug naar
  IUPAC/aminozuur-tekens, maar `twobit_byte::set(char*)` kent alleen `A, C, G, T, U, N`.
- **Debug build**: `throw std::invalid_argument` bij het eerste onbekende karakter → crash.
- **Release build**: de `default:`-tak zit achter `#if DEBUG`; onbekende karakters worden
  stilzwijgend genegeerd en de betreffende bits blijven ongedefinieerd → garbage output.
- Fix: guard toevoegen die controleert of alle sequences `is_twobit()` zijn, ofwel in
  `ucsc2bit_filesize()` / `view_ucsc2bit_chunk()`, ofwel al bij het mounten in `fuse.cpp`
  (parse_args). Overwegen: foutmelding bij `fastafs mount`, of IUPAC/proteïne-sequences
  uitsluiten van de `.2bit` virtualfile.

## Perf: volledige index wordt per 2bit-read herbouwd (`view_ucsc2bit_chunk`)
- `fastafs::view_ucsc2bit_chunk()` (`src/fastafs.cpp:1032`) roept halverwege
  `this->init_ffs2f(0, false)` aan (`src/fastafs.cpp:1131`) en doet aan het eind — en
  bij elk van de ~12 vroege returns — weer `delete cache;`.
- `init_ffs2f()` bouwt de **complete index van het hele archief** opnieuw op: per
  sequence een `ffs2f_init_seq` met vectoren van `n_blocks + 1` en `m_blocks + 1`
  elementen, waarbij elke blokgrens van nucleotide- naar FASTA-bestandspositie wordt
  omgerekend (`src/fastafs.cpp:130-149`). Dat is een allocatie plus een volledige pass
  over alle N/M-blokken van het genoom — **per read-call** van 4KB (debug) of 256KB
  (release). Bij een soft-masked hg38 (miljoenen M-blokken) is dat tientallen MB
  alloceren, vullen en weggooien, voor elke read.
- Raakt twee paden:
  - FUSE: `do_read()` op het virtuele `.2bit` (`src/fuse.cpp:345`).
  - CLI: `fastafs view -2` loopt in `src/main.cpp:236-241` per chunk → herbouwt de
    index per chunk.
- De FUSE-instance heeft al twee kant-en-klare caches liggen (`ffi->cache` en
  `ffi->cache_p0`, gebouwd bij mount in `src/fuse.cpp:731-732`), maar
  `view_ucsc2bit_chunk` kan er niet bij: het is een `fastafs`-methode zonder
  cache-parameter.
- **Fix**: de index één keer bouwen en bewaren, bijv. als lui geïnitialiseerd lid van
  `fastafs` (`ffs2f_init *cache_ucsc2bit`, vrijgegeven in de destructor), zodat zowel
  FUSE als de CLI-lus hem hergebruiken. Kleine, lokale wijziging: de `init_ffs2f`-aanroep
  vervangen en de `delete cache;`-regels laten vallen. Let op de interactie met het
  `init_ffs2f`-geheugenlek hierboven.
- **Blijft daarna staan** (tweede laag, meer werk): `view_ucsc2bit_chunk` loopt bij elke
  read nog steeds vanaf byte 0 door de headerstructuur. De `for`-lussen over
  `n_starts`/`m_starts` (`src/fastafs.cpp:1160`, `1198`) draaien altijd volledig door om
  `pos_limit` op te tellen, ook als er niets geschreven wordt → O(alle blokken) per read.
  Vraagt een offset-tabel per sequence zodat je direct naar het juiste blok springt; pas
  zinvol als de `init_ffs2f`-aanroep eruit is.

## Perf: faidx en dict worden per read/stat opnieuw opgebouwd
- `fastafs::view_faidx_chunk()` (`src/fastafs.cpp:1584`) roept `get_faidx(padding)` aan,
  dat bij **elke** read de volledige faidx-string opnieuw opbouwt: een lus over alle
  sequences met string-concatenatie en `std::to_string` per regel
  (`src/fastafs.cpp:1540`). Er wordt vervolgens een venster van `buffer_size` bytes
  uitgekopieerd en de rest weggegooid.
- `get_faidx()` opent bovendien een `std::ifstream` op het archief die alleen wordt
  gebruikt om te checken of het bestand te openen is, en direct weer wordt gesloten —
  een open/close-syscallpaar per read, zonder dat er iets uit gelezen wordt.
- Hetzelfde gebeurt in `do_getattr` (`src/fuse.cpp:138` en `160`): daar wordt de hele
  faidx opgebouwd puur om `.size()` op te vragen. Elke `stat()` op het virtuele
  `.fa.fai`-bestand betaalt de volledige opbouw.
- `fastafs::view_dict_chunk()` (`src/fastafs.cpp:1311`) heeft dezelfde twee problemen:
  een ongebruikte `ifstream` open/close per read, plus per read een lus over alle
  sequences met `md5_digest_to_hash()` en `std::to_string()` — ook voor de sequences die
  buiten het gevraagde venster vallen.
- Beide bestanden zijn volledig bepaald door het archief plus `padding`, en veranderen
  nooit tijdens een mount.
- **Fix**: de faidx- en dict-inhoud één keer als `std::string` opbouwen (bij mount, of
  lui bij eerste gebruik, gecached in het `fastafs`-object of in de `fuse_instance`) en
  per read alleen nog `.copy(buffer, n, offset)` doen. `do_getattr` gebruikt dan de
  `.size()` van diezelfde gecachede string. De ongebruikte `ifstream`-opens kunnen weg.
- Idem voor `ucsc2bit::get_faidx()` / `ucsc2bit::view_faidx_chunk()`
  (`src/ucsc2bit.cpp:448`, `477`), die exact hetzelfde patroon volgen.
- Zie ook het `view_faidx_chunk`-item bovenaan dit bestand: de EOF-underflow-guard kan
  in dezelfde beurt mee.

## `get_fastafs_processes` opschoning (`src/lsfastafs.cpp`)
- De NULL-`FILE*`-deref bij een mislukte `fopen("/proc/mounts")` is al gefixt
  (guard + `return out;`). Resterende punten:
- **`do`/`while`-lus met `fscanf` is fragiel** (`src/lsfastafs.cpp:50-80`): bij een
  misvormde regel kan `fscanf` een waarde teruggeven die noch `6` noch `EOF` is zonder
  het probleemtoken te consumeren → risico op een blijvende lus. Herschrijven naar een
  `while`-conditie op `== 6` / stoppen op `EOF`, met positieve logica — conform de
  stijlgids in CLAUDE.md (die dit `do/while`-`fscanf`-patroon expliciet afraadt).
- **xattr-buffers niet expliciet ge-null-termineerd** (`src/lsfastafs.cpp:68-73`):
  `getxattr` zet geen afsluitende `\0`. Werkt nu alleen omdat de fuse-kant zelf een
  trailing null meeschrijft (`do_getxattr` retourneert `len+1`). Een sequentienaam mag
  tot 255 tekens zijn; met null erbij (256) > de meegegeven grootte 255 → truncatie
  zonder null → `std::string(char*)` leest voorbij de buffer. Fix: `sizeof(buf)` meegeven
  én zelf termineren op de door `getxattr` geretourneerde lengte.
- **Dode code**: `dict_fn` (`src/lsfastafs.cpp:67`) wordt berekend maar nergens gebruikt.
- **Klein**: de build-comment op regel 14 (`g++ ... src/lsfastafs.cpp`) klopt niet meer;
  de vertaaleenheid hangt af van `utils`/`config`.

## `database::add` naar `const char *` (`include/database.hpp:23`)
- `add(char *)` muteert de pointer niet; net als `get` kan het `const char *` worden.
- Dan vervallen de `(char*)`-casts in `test/database/test_database.cpp`
  (regels 52, 57, 73, 76, 106) en is de API consistent met `get`.
