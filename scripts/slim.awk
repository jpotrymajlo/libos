# SLIM jest AWK-owym przekodownikiem pomi/edzy r/o/znymi systemami
# reprezentowania polskich liter.

# Poniedzia’ek, 12 lutego 1996 r., wersja 0.55

BEGIN {
 V["s"]=1;  V["S"]=1; V["/"]=1 # notacja ciachowa (,,slaszowa'')
 V["l"]=2;  V["L"]=2           # CP852 (Latin 2)
 V["i"]=3;  V["I"]=3           # ISO-8859-2 (ISO Latin2)
 V["m"]=4;  V["M"]=4           # Mazovia
 V["w"]=5;  V["W"]=5           # Windows CP1250
 V["c"]=6;  V["C"]=6           # CSK
 V["d"]=7;  V["D"]=7           # DHN
 V["e"]=8;  V["E"]=8           # EC (Cork Encoding)
 V["a"]=9;  V["A"]=9           # Apple Macintosh
 V["b"]=10; V["B"]=10          # belkot
 V["o"]=11; V["O"]=11          # TeX-ohida
 jest_robota=(V[S]!=V[R])
 while (getline < TBL > 0) if (/^[^ ]/) {
  konwersja_wieloznakowa=(length($V[S])>1) || (length($V[R])>1)
  gsub(/\\/,"\\\\",$V[S]) # specyfika AWK-a
  TRANS[$V[S]]=$V[R]
 }

}

/\000/ {print $0; N0++}

{
 if (jest_robota) {
  if (konwersja_wieloznakowa) { # notacja ciachowa jest nieco specyficzna
   if (V[R]==1) {gsub("\\\\/","\000"); gsub("/","//")} # ,,ukrycie'' \/
   if (V[S]==1) gsub("//","\000")                      # ,,ukrycie'' //
   for (i in TRANS) gsub(i,TRANS[i])
   if (V[R]==1) {gsub("\000","\\/")}                   # odtworzenie \/
   if (V[S]==1) gsub("\000","/")                       # odtworzenie //
  } else {
   s=""
   for (i=1; i<=length($0); ++i) {
    c=substr($0,i,1); s=s (c in TRANS ? TRANS[c] : c)
   }
   $0=s
  }
 }
 if (OFI!="") {print > OFI} else print
}

END {
 if (N0>0) print "UWAGA: znak(i) o kodzie zero (" N0 ")"
# for (i in TRANS) print i, ":", TRANS[i]

}
