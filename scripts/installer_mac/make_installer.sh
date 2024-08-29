#!/bin/bash

# Documentation for pkgbuild and productbuild: https://developer.apple.com/library/archive/documentation/DeveloperTools/Reference/DistributionDefinitionRef/Chapters/Distribution_XML_Ref.html

# preflight check
PRODUCT=$1 # product name
INDIR=$2 # directory where the binaries to be packaged are
RESOURCES_DIR=$3 # directory where readme, icons, etc are
TARGET_DIR=$4 # directory to which installer will be placed
VERSION=$5 # version number
SIGNING_ID=$6

TMPDIR="./installer-tmp"
mkdir -p $TMPDIR

echo "MAKE from $INDIR $RESOURCES_DIR into $TARGET_DIR with $VERSION"

VST3="${PRODUCT}.vst3"
AU="${PRODUCT}.component"
CLAP="${PRODUCT}.clap"

PRODUCTFILE=`echo $PRODUCT | tr ' ' '-' | tr '[:upper:]' '[:lower:]'`
echo "PRODUCTFILE is ${PRODUCTFILE}"

if [ "$VERSION" == "" ]; then
    echo "You must specify the version you are packaging!"
    echo "eg: ./make_installer.sh 1.0.6b4"
    exit 1
fi

OUTPUT_BASE_FILENAME="${PRODUCTFILE}-$VERSION-macOs"


build_flavor()
{
    flavor=$1
    flavorprod=$2
    ident=$3
    loc=$4
    scripts=$5

    echo --- BUILDING ${PRODUCTFILE}_${flavor}.pkg from "$flavorprod" ---

    workdir=$TMPDIR/$flavor
    mkdir -p $workdir

    # In the past we would pkgbuild --analyze first to make a plist file, but we are perfectly fine with the
    # defaults, so we skip that step here. http://thegreyblog.blogspot.com/2014/06/os-x-creating-packages-from-command_2.html
    # was pretty handy in figuring that out and man pkgbuild convinced us to not do it, as did testing.
    #
    # The defaults only work if a component is a sole entry in a staging directory though, so synthesize that
    # by moving the product to a tmp dir

    cp -r "$INDIR/$flavor/$flavorprod" "$workdir"
    ls -l $workdir

    # Package the sucker. This will be wrapped by productbuild, so no need to sign here
    pkgbuild --root $workdir --identifier $ident --version $VERSION --install-location "$loc" "$TMPDIR/${PRODUCTFILE}_${flavor}.pkg" $sca || exit 1

    rm -rf $workdir
}


if [[ -d $INDIR/"VST3"/$VST3 ]]; then
    build_flavor "VST3" "$VST3" "com.tote-bag-labs.${PRODUCTFILE}.vst3.pkg" "/Library/Audio/Plug-Ins/VST3"
fi

if [[ -d $INDIR/"AU"/$AU ]]; then
    build_flavor "AU" "$AU" "com.tote-bag-labs.${PRODUCTFILE}.component.pkg" "/Library/Audio/Plug-Ins/Components"
fi

if [[ -d $INDIR/"CLAP"/$CLAP ]]; then
    build_flavor "CLAP" "$CLAP" "com.tote-bag-labs.${PRODUCTFILE}.clap.pkg" "/Library/Audio/Plug-Ins/CLAP"
fi


echo --- Sub Packages Created ---
ls "${TMPDIR}"

# create distribution.xml

if [[ -d $INDIR/"VST3"/$VST3 ]]; then
	VST3_PKG_REF="<pkg-ref id=\"com.tote-bag-labs.${PRODUCTFILE}.vst3.pkg\"/>"
	VST3_CHOICE="<line choice=\"com.tote-bag-labs.${PRODUCTFILE}.vst3.pkg\"/>"
	VST3_CHOICE_DEF="<choice id=\"com.tote-bag-labs.${PRODUCTFILE}.vst3.pkg\" visible=\"true\" start_selected=\"true\" title=\"${PRODUCT} VST3\"><pkg-ref id=\"com.tote-bag-labs.${PRODUCTFILE}.vst3.pkg\"/></choice><pkg-ref id=\"com.tote-bag-labs.${PRODUCTFILE}.vst3.pkg\" version=\"${VERSION}\" onConclusion=\"none\">${PRODUCTFILE}_VST3.pkg</pkg-ref>"
fi
if [[ -d $INDIR/"AU"/$AU ]]; then
	AU_PKG_REF="<pkg-ref id=\"com.tote-bag-labs.${PRODUCTFILE}.component.pkg\"/>"
	AU_CHOICE="<line choice=\"com.tote-bag-labs.${PRODUCTFILE}.component.pkg\"/>"
	AU_CHOICE_DEF="<choice id=\"com.tote-bag-labs.${PRODUCTFILE}.component.pkg\" visible=\"true\" start_selected=\"true\" title=\"${PRODUCT} Audio Unit\"><pkg-ref id=\"com.tote-bag-labs.${PRODUCTFILE}.component.pkg\"/></choice><pkg-ref id=\"com.tote-bag-labs.${PRODUCTFILE}.component.pkg\" version=\"${VERSION}\" onConclusion=\"none\">${PRODUCTFILE}_AU.pkg</pkg-ref>"
fi
if [[ -d $INDIR/"CLAP"/$CLAP ]]; then
	CLAP_PKG_REF="<pkg-ref id=\"com.tote-bag-labs.${PRODUCTFILE}.clap.pkg\"/>"
	CLAP_CHOICE="<line choice=\"com.tote-bag-labs.${PRODUCTFILE}.clap.pkg\"/>"
	CLAP_CHOICE_DEF="<choice id=\"com.tote-bag-labs.${PRODUCTFILE}.component.pkg\" visible=\"true\" start_selected=\"true\" title=\"${PRODUCT} CLAP\"><pkg-ref id=\"com.tote-bag-labs.${PRODUCTFILE}.clap.pkg\"/></choice><pkg-ref id=\"com.tote-bag-labs.${PRODUCTFILE}.clap.pkg\" version=\"${VERSION}\" onConclusion=\"none\">${PRODUCTFILE}_CLAP.pkg</pkg-ref>"
fi

cat > $TMPDIR/distribution.xml << XMLEND
<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="1">
    <title>${PRODUCT} ${VERSION}</title>
    <license file="License.txt" />
    <readme file="Readme.rtf" />
    ${VST3_PKG_REF}
    ${AU_PKG_REF}
    ${CLAP_PKG_REF}
    <options require-scripts="false" customize="always" />
    <choices-outline>
        ${VST3_CHOICE}
        ${AU_CHOICE}
        ${CLAP_CHOICE}
    </choices-outline>
    ${VST3_CHOICE_DEF}
    ${AU_CHOICE_DEF}
    ${CLAP_CHOICE_DEF}
</installer-gui-script>
XMLEND

# build installation bundle
echo "Building installation bundle"

# copy resources into temp dir
mkdir -p "${TMPDIR}/${RESOURCES_DIR}"
cp -r "${RESOURCES_DIR}/Readme.rtf" "${TMPDIR}/${RESOURCES_DIR}"
cp -r "${RESOURCES_DIR}/License.txt" "${TMPDIR}/${RESOURCES_DIR}"

pushd ${TMPDIR}

if [[ ! -z "$SIGNING_ID" ]]; then
    echo productbuild --sign "$SIGNING_ID" --distribution "distribution.xml" --package-path "." --resources "${RESOURCES_DIR}" "$OUTPUT_BASE_FILENAME.pkg"
    productbuild --sign "$SIGNING_ID" --distribution "distribution.xml" --package-path "." --resources ${RESOURCES_DIR} "$OUTPUT_BASE_FILENAME.pkg"
else
    echo productbuild ---distribution "distribution.xml" --package-path "." --resources "${RESOURCES_DIR}" "$OUTPUT_BASE_FILENAME.pkg"
    productbuild --distribution "distribution.xml" --package-path "." --resources "${RESOURCES_DIR}" "$OUTPUT_BASE_FILENAME.pkg"
fi

echo --- Main Package Created ---

popd

# move completed package to output directory
mv "${TMPDIR}/${OUTPUT_BASE_FILENAME}.pkg" "${TARGET_DIR}"
