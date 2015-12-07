#!/bin/bash

#command -v convert >/dev/null 2>&1 || { echo >&2 "imagemagick n'est pas installé, il est alors impossible de générer la police demandée. Essayer 'sudo apt-get install imagemagick'"; exit 1; }
command -v gm >/dev/null 2>&1 || { echo >&2 "graphics magick n'est pas installé, il est alors impossible de générer la police demandée. Essayer 'sudo apt-get install graphicsmagick'"; exit 1; }

FONT_SIZE=12
FONT_NAME=Latin-Modern-Mono-12-Regular
FONT_WIDTH=29
FONT_HEIGHT=30

ASCII_START=32
ASCII_END=126

TMP_DIR_NAME=tmp-font
TMP_DIR_PATH=`pwd`

FONT_FILE_C_NAME=font.c
FONT_FILES_PATH=`pwd`

mkdir tmp-font

if [ -d "${TMP_DIR_NAME}" ]
then
       	echo "Création du répertoire temporaire ${TMP_DIR_PATH}/${TMP_DIR_NAME}"
else
       	echo "Erreur lors de la création du répertoire temporaire ${TMP_DIR_PATH}/${TMP_DIR_NAME}"
	exit 1
fi

cat <<< "
FontTable * initFont() {

    FontTable *font = (FontTable *)kAlloc(sizeof (FontTable));
    font->values = (char **)kAlloc(FONT_TABLE_SIZE * sizeof (char *)); // ASCII Size Table

    for (int i = 0; i < FONT_TABLE_SIZE; ++i) {
        font->values[i] = char_font_63_bits;
    }

" > ${TMP_DIR_PATH}/${TMP_DIR_NAME}/font-func.c

for ((i = $ASCII_START; i <= $ASCII_END; i++)); do
	n=`printf "\x$(printf %x $i)"`
#	 convert -resize ${FONT_WIDTH}x${FONT_HEIGHT}\! -font $FONT_NAME -pointsize $FONT_SIZE label:$n ${TMP_DIR_PATH}/${TMP_DIR_NAME}/$i.xbm && \
#        convert -font $FONT_NAME -pointsize $FONT_SIZE label:$n ${TMP_DIR_PATH}/${TMP_DIR_NAME}/$i.xbm && \
	gm convert -antialias -font $FONT_NAME -pointsize $FONT_SIZE label:$n ${TMP_DIR_PATH}/${TMP_DIR_NAME}/$i.xbm && \
	cat ${TMP_DIR_PATH}/${TMP_DIR_NAME}/$i.xbm | sed "s/#define /#define char_font_/" | sed "s/static char /static char char_font_/" >> ${FONT_FILES_PATH}/${FONT_FILE_C_NAME} && \
	echo "    font->values[${i}] = char_font_${i}_bits; // $n" >> ${TMP_DIR_PATH}/${TMP_DIR_NAME}/font-func.c
	echo "    font->widths[${i}] = char_font_${i}_width;" >> ${TMP_DIR_PATH}/${TMP_DIR_NAME}/font-func.c
        echo "    font->heights[${i}] = char_font_${i}_height;" >> ${TMP_DIR_PATH}/${TMP_DIR_NAME}/font-func.c
done

cat ${TMP_DIR_PATH}/${TMP_DIR_NAME}/font-func.c >> ${FONT_FILES_PATH}/${FONT_FILE_C_NAME} && \
echo "Création du fichier ${FONT_FILES_PATH}/${FONT_FILE_C_NAME}"

read -p "Supprimer le dossier temporaire ? (Y/n) : " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
    exit 1
fi

rm -f ${TMP_DIR_PATH}/${TMP_DIR_NAME}/*.xbm
rm -f ${TMP_DIR_PATH}/${TMP_DIR_NAME}/font-func.c
rmdir ${TMP_DIR_PATH}/${TMP_DIR_NAME}
