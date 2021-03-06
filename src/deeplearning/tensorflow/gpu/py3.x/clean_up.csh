#!/bin/tcsh

if ( -d $PWD/py_distro ) then 
   deactivate
   cd $PWD/user_ops ; make clean ; cd ../ 
   rm -rf $PWD/py_distro
   if ( -z "$OLD_PYTHONHOME" ) then 
      echo "PYTHONHOME was unset. Please correct if incorrect"
      unset PYTHONHOME
   else
      echo "PYTHONHOME set to $OLD_PYTHONHOME. Please correct if incorrect"
      setenv PYTHONHOME $OLD_PYTHONHOME
   endif
endif

unset LD_PRELOAD
unset pyflow
unset TF_HOME 
unset TF_INSTALL_DIR
unset PNETCDF_INSTALL_DIR
unset TF_MPI_ENABLE
unset FAKE_SYSTEM_LIBS
