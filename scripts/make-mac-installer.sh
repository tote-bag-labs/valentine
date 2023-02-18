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
    build_flavor "VST3" "$VST3" "com.ToteBagLabs.${PRODUCTFILE}.vst3.pkg" "/Library/Audio/Plug-Ins/VST3"
fi

if [[ -d $INDIR/"AU"/$AU ]]; then
    build_flavor "AU" "$AU" "com.ToteBagLabs.${PRODUCTFILE}.component.pkg" "/Library/Audio/Plug-Ins/Components"
fi


echo --- Sub Packages Created ---
ls "${TMPDIR}"

# create distribution.xml

if [[ -d $INDIR/"VST3"/$VST3 ]]; then
	VST3_PKG_REF="<pkg-ref id=\"com.ToteBagLabs.${PRODUCTFILE}.vst3.pkg\"/>"
	VST3_CHOICE="<line choice=\"com.ToteBagLabs.${PRODUCTFILE}.vst3.pkg\"/>"
	VST3_CHOICE_DEF="<choice id=\"com.ToteBagLabs.${PRODUCTFILE}.vst3.pkg\" visible=\"true\" start_selected=\"true\" title=\"${PRODUCT} VST3\"><pkg-ref id=\"com.ToteBagLabs.${PRODUCTFILE}.vst3.pkg\"/></choice><pkg-ref id=\"com.ToteBagLabs.${PRODUCTFILE}.vst3.pkg\" version=\"${VERSION}\" onConclusion=\"none\">${PRODUCTFILE}_VST3.pkg</pkg-ref>"
fi
if [[ -d $INDIR/"AU"/$AU ]]; then
	AU_PKG_REF="<pkg-ref id=\"com.ToteBagLabs.${PRODUCTFILE}.component.pkg\"/>"
	AU_CHOICE="<line choice=\"com.ToteBagLabs.${PRODUCTFILE}.component.pkg\"/>"
	AU_CHOICE_DEF="<choice id=\"com.ToteBagLabs.${PRODUCTFILE}.component.pkg\" visible=\"true\" start_selected=\"true\" title=\"${PRODUCT} Audio Unit\"><pkg-ref id=\"com.ToteBagLabs.${PRODUCTFILE}.component.pkg\"/></choice><pkg-ref id=\"com.ToteBagLabs.${PRODUCTFILE}.component.pkg\" version=\"${VERSION}\" onConclusion=\"none\">${PRODUCTFILE}_AU.pkg</pkg-ref>"
fi

cat > $TMPDIR/distribution.xml << XMLEND
<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="1">
    <title>${PRODUCT} ${VERSION}</title>
    <license file="License.txt" />
    <readme file="Readme.rtf" />
    ${VST3_PKG_REF}
    ${AU_PKG_REF}
    <options require-scripts="false" customize="always" />
    <choices-outline>
        ${VST3_CHOICE}
        ${AU_CHOICE}
    </choices-outline>
    ${VST3_CHOICE_DEF}
    ${AU_CHOICE_DEF}
</installer-gui-script>
XMLEND

# build installation bundle
echo "Building installation bundle"

# copy resources into temp dir
mkdir -p "${TMPDIR}/${RESOURCES_DIR}"
cp -r "${RESOURCES_DIR}/Readme.rtf" "${TMPDIR}/${RESOURCES_DIR}"
cp -r "${RESOURCES_DIR}/License.txt" "${TMPDIR}/${RESOURCES_DIR}"

pushd ${TMPDIR}

echo productbuild --sign "$SIGNING_ID" --distribution "distribution.xml" --package-path "." --resources "${RESOURCES_DIR}" "$OUTPUT_BASE_FILENAME.pkg"
productbuild --sign "$SIGNING_ID" --distribution "distribution.xml" --package-path "." --resources ${RESOURCES_DIR} "$OUTPUT_BASE_FILENAME.pkg"

echo --- Main Package Created ---

popd

mkdir -p "${TMPDIR}/${PRODUCTFILE}"
mv "${TMPDIR}/${OUTPUT_BASE_FILENAME}.pkg" "${TMPDIR}/${PRODUCTFILE}"

# create the DMG
if [[ -f "${TARGET_DIR}/$OUTPUT_BASE_FILENAME.dmg" ]]; then
    rm "${TARGET_DIR}/$OUTPUT_BASE_FILENAME.dmg"
fi
hdiutil create /tmp/tmp.dmg -ov -volname "$OUTPUT_BASE_FILENAME" -fs HFS+ -srcfolder "${TMPDIR}/${PRODUCTFILE}/"
hdiutil convert /tmp/tmp.dmg -format UDZO -o "${TARGET_DIR}/$OUTPUT_BASE_FILENAME.dmg"
