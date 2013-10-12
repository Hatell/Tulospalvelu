Tulospalvelu
============

Riippuvuudet
------------

 - **Qt 4**
 - git://gitorious.org/qt/qtserialport.git

Ristikäännös
------------

Windows buildia varten on luotu muutama scripti helpottamaan ristikäännöksen
tekemistä. Ristikäännös on toteutettu käyttämällä mingw-w64 pakettia.
Scriptit löytyvät hakemistosta mingw-w64 ja niitä käytetään seuraavasti.

 1. Kopioi hakemisto mingw-w64 koneellesi.
 2. Siirry hakemistoon ja aja scriptit 1-init.sh, 2-prebuild.sh ja 3-build.sh
 3. Kerää tarvittavat dll:t ja exe ajamalla scripti 4-make\_copy.sh /polku/kohde/hakemistoon

