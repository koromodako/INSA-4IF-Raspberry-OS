#!/bin/bash

# ImageMagick
#command -v convert >/dev/null 2>&1 || { echo >&2 "imagemagick n'est pas installé, il est alors impossible de générer la police demandée. Essayer 'sudo apt-get install imagemagick'"; exit 1; }

# Graphics Magick
command -v gm >/dev/null 2>&1 || { echo >&2 "graphics magick n'est pas installé, il est alors impossible de générer la police demandée. Essayer 'sudo apt-get install graphicsmagick'"; exit 1; }

FONT_SIZE=12
FONT_NAME=Latin-Modern-Mono-12-Regular

ASCII_START=32
ASCII_END=126

TMP_DIR_NAME=tmp-font
TMP_DIR_PATH=`pwd`

FONT_FILE_NAME=font_spec.h
FONT_FILES_PATH=`pwd`

mkdir tmp-font

if [ -d "${TMP_DIR_NAME}" ]
then
       	echo "Création du répertoire temporaire ${TMP_DIR_PATH}/${TMP_DIR_NAME}"
else
       	echo "Erreur lors de la création du répertoire temporaire ${TMP_DIR_PATH}/${TMP_DIR_NAME}"
	exit 1
fi

echo "#ifndef FONT_SPEC_H_
#define FONT_SPEC_H_
" > ${FONT_FILES_PATH}/${FONT_FILE_NAME}

for ((i = $ASCII_START; i <= $ASCII_END; i++)); do
	n=`printf "\x$(printf %x $i)"`
        # Use Image Magick : convert -font $FONT_NAME -pointsize $FONT_SIZE label:$n ${TMP_DIR_PATH}/${TMP_DIR_NAME}/$i.xbm && \
	gm convert -antialias -font $FONT_NAME -pointsize $FONT_SIZE label:$n ${TMP_DIR_PATH}/${TMP_DIR_NAME}/$i.xbm && \
	cat ${TMP_DIR_PATH}/${TMP_DIR_NAME}/$i.xbm | sed "s/#define /#define char_font_/" | sed "s/static char /static char char_font_/" >> ${FONT_FILES_PATH}/${FONT_FILE_NAME}
done

echo "
#endif /* FONT_SPEC_H_ */" >> ${FONT_FILES_PATH}/${FONT_FILE_NAME}

echo "Création du fichier ${FONT_FILES_PATH}/${FONT_FILE_NAME}"

read -p "Supprimer le dossier temporaire ? (Y/n) : " -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
    exit 1
fi

rm -f ${TMP_DIR_PATH}/${TMP_DIR_NAME}/*.xbm
rmdir ${TMP_DIR_PATH}/${TMP_DIR_NAME}
