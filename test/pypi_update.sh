#!/bin/sh
# update mcush to pypi
# account name: pengshulin
# twine must be installed before
# Peng Shulin <trees_peng@163.com>
#

python setup.py sdist

VER=`python -c 'import mcush; print mcush.__version__'`
TAR=dist/mcush-$VER.tar.gz

test -f $TAR && twine upload $TAR


