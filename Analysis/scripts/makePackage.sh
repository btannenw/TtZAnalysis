#!/bin/bash

#   This script creates an application package.
#   
#   The package will contain the executable
#   and all CMSSW libraries of the local release.
#   That means, the application package can be run
#   from a clean CMSSW environment (same version)
#   or from within an installation while CMSSW
#   is compiled/cleaned or modified
#
#   It behaves exactly like the source executable
#   as far as command parsing, output dirs, ...
#   are concerned.



workdir=`pwd`
executable=$1
option="" # "all" is experimental
outputfile=$2
if [[ "${outputfile}" == "" ]]
then
	outputfile=$workdir/indep_$executable
fi

if [[ "${executable}" == "" ]]
then
	echo "USAGE: ${0} <executable> <options>"
	exit 1
fi

execpath=`which $1`
if [[ "${execpath}" == "" ]]
then
	echo "executable not found in PATH"
	exit 2
fi
	
tmpdir=`mktemp -d`
trap "rm -rf $tmpdir; exit" SIGHUP SIGINT SIGTERM
cd $tmpdir
mkdir -p work
tmpwork=$tmpdir/work
BINDIR=$tmpwork/bin/
LOCLIB=$tmpwork/lib/

mkdir -p $BINDIR
mkdir -p $LOCLIB

cp $execpath $BINDIR/


CMSLIBS=$CMSSW_BASE/lib/$SCRAM_ARCH/
EXTLIBS=${CMSSW_BASE}/external/${SCRAM_ARCH}/lib/

function copylibs(){

    linklibs=""
    libdir=$LOCLIB
    mkdir -p $libdir
    if [[ "${option}" == "all" ]]
    then
       allibs=` ldd $execpath | grep so | sed -e '/^[^\t]/ d' | sed -e 's/\t//'| sed -e 's/.*=..//'| sed -e 's/ (0.*)//' `
       for f in $allibs
	   do
			  cp $f $libdir
	   done
    else
       cp ${CMSLIBS}lib*.so $libdir
    fi

    cp $EXTLIBS/libunfold.so $libdir 

}

copylibs


# create script to run:
outscript=$tmpwork/run.sh
touch $outscript
chmod +x $outscript

cat > $outscript << EOT

#!/bin/bash
binpath=\$1
shift
current=\$1
shift
cd \$current
echo running packaged: $executable "\$@"
export LD_LIBRARY_PATH=\$binpath/work/lib:$LD_LIBRARY_PATH
\$binpath/work/bin/$executable "\$@"

EOT

# tar the whole thing

tar czf pack.tar.gz work

# create package

cat > scriptheader.sh.in << EOT

#!/bin/bash

current=\`pwd\`
dir=\`mktemp -d\`
trap "rm -rf \$dir; exit" SIGHUP SIGINT SIGTERM
cd \$dir

function untar_payload()
{
	match=\$(grep --text --line-number '^PAYLOAD:\$' \$current/\$0 | cut -d ':' -f 1)
	payload_start=\$((match + 1))
	tail -n +\$payload_start \$current/\$0 | tar -xzf -
	
}

untar_payload
\$dir/work/run.sh \$dir \$current "\$@"
rm -rf \$dir

exit 0

EOT

#output file is already defined in workdir
cat scriptheader.sh.in >> $outputfile
echo "PAYLOAD:" >> $outputfile

cat pack.tar.gz >> $outputfile

chmod +x $outputfile


rm -rf $tmpdir

