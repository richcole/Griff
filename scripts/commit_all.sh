if [ -z "$1" ] || [ "$2" ] ; then
  echo "Usage: bash commit_all.sh <message>"
  exit -1
fi

MSG=$1

for x in cass_browser PIG rj_cgi rj_caml scripts misc ; do
  pushd $x
  cvs commit -m"$MSG"
  popd
done
