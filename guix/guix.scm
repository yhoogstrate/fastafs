;; guix package --install-from-file=/home/youri/src/fastafs/guix.scm
;; https://guix.gnu.org/blog/2018/a-packaging-tutorial-for-guix/

(use-modules (guix packages)
             (guix download)
             (guix git-download)
             (guix build-system gnu)
             (guix build-system cmake)
             (guix licenses)
             (gnu packages boost)
             (gnu packages compression)
             (gnu packages tls)
             (gnu packages linux))

(package
  (name "fastafs")
  (version "1.7.0")
  (source (origin
            (method url-fetch)
            ; (uri (string-append "https://github.com/yhoogstrate/fastafs/archive/a39eddbf810d7a828d33d6dbe8c913bbffd58948.tar.gz"))
            (uri (string-append "file:///home/youri/.local/src/fastafs.tar.gz"))
            (sha256
             (base32
              "1njzvaxy1nq4202ispphyxddihq1x1cmfzbl8zmkqiwa028k540c"))))
  (build-system cmake-build-system)
    (arguments
      `(#:build-type "debug"
        #:tests? #f) ; skip tests that fail because test data is not in build path
        )
  (inputs
         `(("boost" ,boost)
           ("zlib" ,zlib)
           ("openssl" ,openssl)
           ("fuse" ,fuse)
         ))
  (synopsis "fastafs")
  (description
   "fastafs: toolkit for file system virtualisation of random access compressed FASTA, FAI, DICT & TWOBIT files")
  (home-page "https://github.com/yhoogstrate/fastafs")
  (license gpl2+))
