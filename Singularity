Bootstrap: yum
OSVersion: 7
MirrorURL: http://mirror.centos.org/centos-%{OSVERSION}/%{OSVERSION}/os/$basearch/
Include: yum tar

%setup
# Copy any necessary files

%environment

    # Use bash as default shell
    SHELL=/bin/bash

    # Add paths
    PATH="/usr/local/w2rap-contigger/bin:$PATH"

    # Export paths
    export PATH

export PATH=/usr/local/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/lib64:$LD_LIBRARY_PATH
%post
    # Make gpfs folder to hold mount
    mkdir /gpfs
    mkdir /shared
    mkdir /local

    # Create and move to build directory
    mkdir /root/build && cd /root/build
   


# run OS updates
yum update -y  && yum  upgrade -y

#Dependencies for GCC 5.5.5
yum install gmp-devel mpfr-devel libmpc-devel wget which zlib zlib-devel gcc gcc-c++ gzip file  git make -y
NOW=`date`
echo "export NOW=\"${NOW}\"" >> $SINGULARITY_ENVIRONMENT

#Building Gcc 5.5 from source
cd /root/build
wget ftp://ftp.mirrorservice.org/sites/sourceware.org/pub/gcc/releases/gcc-5.5.0/gcc-5.5.0.tar.gz
tar -zxvf gcc-5.5.0.tar.gz
mkdir -p gcc-5.5.0-build
cd gcc-5.5.0-build
../gcc-5.5.0/configure --enable-languages=c,c++ --disable-multilib
make  -j 4
make install



 #Install cmake
export PATH=/usr/local/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/lib64:$LD_LIBRARY_PATH
which gcc
gcc --version
cd /usr/local/
wget https://cmake.org/files/v3.4/cmake-3.4.1-Linux-x86_64.tar.gz
tar xvf cmake-3.4.1-Linux-x86_64.tar.gz
export PATH="`pwd`/cmake-3.4.1-Linux-x86_64/bin:$PATH"
rm -rf cmake-3.4.1-Linux-x86_64.tar.gz

# Install w2rap-contigger

git clone https://github.com/gonzalogacc/w2rap-contigger.git
cd w2rap-contigger/
cmake -D CMAKE_CXX_COMPILER=g++ .
make -j 4



%labels
    Author A.Sayed@uea.ac.uk
    Version v0.0.1

%help
    This is a w2wrap-contig  container build using singularity 3.1.1 and GCC 5.5.0 and now testing on singularity hub. Not sure why it is working
  
