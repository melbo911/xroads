#!/bin/bash
xp=$(dirname "$_")
if [ -f $xp/xroads.app ];then
   echo "renaming xroads.app into xroads ..."
   mv $xp/xroads.app $xp/xroads
   echo "clearing quarantine flag ..."
   xattr -c "$xp/xroads"
   echo "done."
   sleep 5
fi


sleep 10
