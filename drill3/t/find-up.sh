#!/bin/bash

num=0

if ["$1" == ""]
then
   echo "No argument provided" > /dev/stderr
   exit 2
else
   if test -e "$1"
   then
      echo "./$1"
      exit 0
elif test -e "$2"
   then
      echo "./$2"
      exit 0
   else
   #   stderr > /dev/null
      while [ ! "$(pwd)" -ef / ]
      do

	 if test -e "$1"
	 then
	    for (( i=0; i<$num; i++ ))
	    do
	       echo -n "../"
  	    done
            echo "$1"
            exit 0
         elif test -e "$2"
            then
	       for (( i=0; i<$num; i++ ))
	       do
	          echo -n "../"
		  done
		  echo "$2"
		  exit 0
	 else
            cd ..
	    num=$(($num + 1))
	 fi

      done
      if test "$(pwd)" -ef / && test ! -e "$1"
      then
      exit 1
      elif test "$(pwd)" -ef / && test ! -e "$2"
      then
      exit 1
      fi
   fi
fi
